# Philosophers — Allowed Functions Reference

> A deep-dive into every function you're allowed to use in the 42 Philosophers
> project, plus tips & tricks for building a clean, race-free solution.

---

## Table of Contents

1. [memset](#1-memset)
2. [printf](#2-printf)
3. [malloc](#3-malloc)
4. [free](#4-free)
5. [write](#5-write)
6. [usleep](#6-usleep)
7. [gettimeofday](#7-gettimeofday)
8. [pthread_create](#8-pthread_create)
9. [pthread_detach](#9-pthread_detach)
10. [pthread_join](#10-pthread_join)
11. [pthread_mutex_init](#11-pthread_mutex_init)
12. [pthread_mutex_destroy](#12-pthread_mutex_destroy)
13. [pthread_mutex_lock](#13-pthread_mutex_lock)
14. [pthread_mutex_unlock](#14-pthread_mutex_unlock)
15. [Tips & Tricks for a Beautiful Philosopher](#15-tips--tricks-for-a-beautiful-philosopher)

---

## 1. `memset`

### Prototype

```c
#include <string.h>

void *memset(void *s, int c, size_t n);
```

### What It Does

`memset` fills the first **n** bytes of the memory area pointed to by `s` with
the constant byte `c` (converted to an `unsigned char`).

### How It Works Internally

The CPU iterates over the memory block starting at address `s` and writes the
byte value `(unsigned char)c` into each successive byte. Modern implementations
use optimised SIMD instructions (SSE/AVX on x86) to write 16–64 bytes at once
when the block is large enough, falling back to a simple byte loop for small or
unaligned regions.

Because `memset` operates on **bytes**, passing an `int` value like `1` will
**not** set each `int`-sized element to `1` — it will set each *byte* to `0x01`,
resulting in `0x01010101` for a 4-byte int.

### Edge Cases & Pitfalls

| Scenario | Behaviour |
|---|---|
| `n == 0` | No bytes are written; `s` is returned unchanged. |
| `s == NULL` | **Undefined behaviour** — will segfault on most systems. |
| `c` outside 0–255 | Only the low 8 bits are used (`c & 0xFF`). |
| Overlapping use | Not applicable — there's only one pointer. |

### Best Practice for Philosophers

Use `memset` to zero-initialise your structures right after `malloc`:

```c
t_data *data = malloc(sizeof(t_data));
if (!data)
    return (NULL);
memset(data, 0, sizeof(t_data));
```

This guarantees every field starts at a known state and avoids reading
uninitialised memory.

---

## 2. `printf`

### Prototype

```c
#include <stdio.h>

int printf(const char *format, ...);
```

### What It Does

`printf` formats a string according to `format` and writes the result to
**stdout** (file descriptor 1). It returns the number of characters printed, or
a negative value on error.

### How It Works Internally

1. **Parsing** — `printf` walks the format string, copying literal characters
   into an internal buffer and stopping at each `%` conversion specifier.
2. **Conversion** — For each specifier (`%d`, `%s`, `%ld`, …), it pulls the
   next variadic argument from the stack/registers (via `va_arg`), converts it
   to a string representation, and appends it to the buffer.
3. **Flushing** — The buffer (typically 4096 bytes, managed by `stdio`) is
   flushed to the kernel via `write()` when it's full, when a `\n` is
   encountered (line-buffered mode for terminals), or when the stream is
   explicitly flushed.

The kernel then copies the data from user-space into the terminal driver's
buffer, which eventually renders the characters on screen.

### Edge Cases & Pitfalls

| Scenario | Behaviour |
|---|---|
| `format == NULL` | **Undefined behaviour** — segfault on most systems. |
| Format/argument mismatch | **Undefined behaviour** — e.g. `printf("%d", "hello")`. |
| Return value < 0 | Write error (disk full, broken pipe, etc.). |
| Thread safety | `printf` holds an internal lock on `stdout`, so calls from multiple threads won't *corrupt* the buffer, but lines from different threads **can interleave**. |

### Best Practice for Philosophers

`printf` is technically thread-safe (the `stdio` lock prevents buffer
corruption), but **messages from different threads can interleave** when printed
in rapid succession. This is the main reason philosopher logs sometimes appear
out of order.

**Recommended approach:**

- Protect your print calls with a dedicated **write mutex** so that the
  timestamp fetch + the print happen atomically:

```c
void	philo_log(t_philo *philo, char *msg)
{
    long	now;

    pthread_mutex_lock(&philo->data->write_mutex);
    now = get_timestamp_ms() - philo->data->start_time;
    printf("%ld %d %s\n", now, philo->id, msg);
    pthread_mutex_unlock(&philo->data->write_mutex);
}
```

- **Check the death flag** inside the lock before printing, so you never print
  after a philosopher has died.

---

## 3. `malloc`

### Prototype

```c
#include <stdlib.h>

void *malloc(size_t size);
```

### What It Does

`malloc` allocates `size` bytes of heap memory and returns a pointer to the
beginning of the allocated block. The memory is **not initialised** (it contains
whatever garbage was left there).

### How It Works Internally

1. **User-space allocator** — `malloc` maintains internal data structures
   (free-lists, bins, arenas) to manage previously freed chunks. If a
   suitably-sized chunk is available, it's returned immediately.
2. **Kernel request** — If no suitable chunk exists, the allocator calls
   `sbrk()` (to extend the heap) or `mmap()` (for large allocations, typically
   ≥ 128 KB) to request pages from the kernel.
3. **Virtual memory** — The kernel maps virtual pages to physical frames
   lazily (on first access via page faults). So `malloc` can "succeed" even if
   physical RAM is nearly full — the OOM killer only fires when the pages are
   actually touched.
4. **Metadata** — The allocator stores a small header (typically 8–16 bytes)
   *before* the returned pointer, containing the block size and allocation
   status. This is why writing before the returned pointer corrupts the heap.

### Edge Cases & Pitfalls

| Scenario | Behaviour |
|---|---|
| `size == 0` | Implementation-defined: may return `NULL` or a unique pointer that must not be dereferenced. |
| Returns `NULL` | Allocation failed — **always check**. |
| Memory not zeroed | Contains garbage; use `memset` or `calloc` to zero. |
| Overflow in size calculation | E.g. `malloc(n * sizeof(int))` where `n * sizeof(int)` wraps around — allocates a tiny block, leading to buffer overflow. |

### Best Practice for Philosophers

```c
t_philo *philos = malloc(sizeof(t_philo) * nb_philos);
if (!philos)
    return (exit_error(data, "malloc", strerror(errno)), NULL);
memset(philos, 0, sizeof(t_philo) * nb_philos);
```

Always check the return value. Always zero the memory. Always free it later.

---

## 4. `free`

### Prototype

```c
#include <stdlib.h>

void free(void *ptr);
```

### What It Does

`free` releases the memory block pointed to by `ptr`, which must have been
returned by a previous call to `malloc`, `calloc`, or `realloc`. After `free`,
accessing `ptr` is **undefined behaviour**.

### How It Works Internally

1. The allocator reads the metadata header just before `ptr` to determine the
   block size.
2. The block is marked as free and inserted into the appropriate free-list/bin.
3. **Coalescing** — Adjacent free blocks may be merged to reduce fragmentation.
4. For large `mmap`'d blocks, `free` calls `munmap()` to return pages directly
   to the kernel.
5. For smaller blocks, the memory generally stays in the process's address
   space (the heap doesn't shrink) — it's simply re-used by future `malloc`
   calls.

### Edge Cases & Pitfalls

| Scenario | Behaviour |
|---|---|
| `ptr == NULL` | **No-op** — perfectly safe. |
| Double free | **Undefined behaviour** — heap corruption, crashes, exploits. |
| `ptr` not from `malloc` | **Undefined behaviour** — e.g. freeing a stack variable. |
| Use after free | **Undefined behaviour** — the memory might be reused. |

### Best Practice for Philosophers

- **Set pointers to `NULL` after freeing** to catch accidental reuse:

```c
free(data->philos);
data->philos = NULL;
```

- Write a single `cleanup` function that frees everything in reverse allocation
  order, and call it from both the normal exit path and the error path.
- Never free inside a thread that doesn't own the memory — let the main thread
  handle all cleanup after `pthread_join`.

---

## 5. `write`

### Prototype

```c
#include <unistd.h>

ssize_t write(int fd, const void *buf, size_t count);
```

### What It Does

`write` writes up to `count` bytes from buffer `buf` to the file descriptor
`fd`. Returns the number of bytes actually written, or `-1` on error (with
`errno` set).

### How It Works Internally

1. **System call** — `write` is a thin wrapper around the `write` syscall
   (`syscall(SYS_write, fd, buf, count)`). It traps into kernel mode.
2. **Kernel buffer** — The kernel copies data from user-space into the
   appropriate kernel buffer (pipe buffer, socket buffer, terminal driver
   buffer, page cache for files).
3. **Short writes** — The kernel may write fewer bytes than requested (e.g.
   pipe buffer full, signal interruption). The return value tells you how many
   bytes were actually written.
4. **Blocking** — By default, `write` blocks until at least one byte is
   written. For terminals and pipes, the kernel wakes the process when buffer
   space becomes available.

### Edge Cases & Pitfalls

| Scenario | Behaviour |
|---|---|
| `fd` invalid | Returns `-1`, `errno == EBADF`. |
| `buf == NULL` | Returns `-1`, `errno == EFAULT`. |
| `count == 0` | Returns `0` (no bytes written). |
| Signal interruption | Returns `-1`, `errno == EINTR` — retry the call. |
| Short write | Returned value < `count` — must loop to write remaining bytes. |

### Best Practice for Philosophers

You likely won't call `write` directly if you're using `printf`, but if you
need a truly atomic log line (no `stdio` buffering layer), `write` to fd 1
with a pre-formatted string is an option:

```c
char buf[128];
int len = snprintf(buf, sizeof(buf), "%ld %d is eating\n", ts, id);
write(1, buf, len);
```

Note: even `write` calls are not truly atomic above `PIPE_BUF` (4096 bytes on
Linux), but philosopher log lines are always short.

---

## 6. `usleep`

### Prototype

```c
#include <unistd.h>

int usleep(useconds_t usec);
```

### What It Does

`usleep` suspends the calling thread for **at least** `usec` microseconds.
Returns `0` on success, `-1` on error.

### How It Works Internally

1. The thread calls into the kernel, which removes it from the CPU run queue
   and sets a timer.
2. The thread is placed in a **sleep queue** — it consumes zero CPU time while
   sleeping.
3. When the timer fires (via a hardware/software interrupt), the kernel moves
   the thread back to the run queue.
4. The thread doesn't resume *immediately* — it must wait for the scheduler to
   assign it a CPU, which adds **scheduling latency** (typically 1–10 ms on
   Linux with the default scheduler).

### Edge Cases & Pitfalls

| Scenario | Behaviour |
|---|---|
| `usec == 0` | Returns immediately (yields the CPU). |
| `usec > 1000000` | Behaviour is unspecified on some systems (POSIX says the argument should be < 1 000 000). Use `sleep()` or `nanosleep()` for longer durations. |
| Actual sleep > requested | **Always** — `usleep(1000)` may sleep 1–15 ms due to scheduler granularity. |
| Signal interruption | May return early with `-1` and `errno == EINTR`. |

### Best Practice for Philosophers

**Never rely on a single `usleep` call for precise timing.** The golden pattern
is a **busy-wait loop** that re-checks the elapsed time:

```c
void	precise_sleep(long duration_ms, t_data *data)
{
    long	start;

    start = get_timestamp_ms();
    while (!someone_died(data))
    {
        if (get_timestamp_ms() - start >= duration_ms)
            break ;
        usleep(200);  // sleep in small increments (100–500 µs)
    }
}
```

This gives you **~0.2 ms precision** while still yielding the CPU. The small
`usleep` prevents busy-spinning from consuming 100% of a core.

**Why 200 µs?** It's a sweet spot:
- Small enough for < 1 ms accuracy
- Large enough that the thread yields and doesn't hog the CPU
- Adjust down (100) for stricter timing or up (500) if CPU usage matters more

---

## 7. `gettimeofday`

### Prototype

```c
#include <sys/time.h>

int gettimeofday(struct timeval *tv, struct timezone *tz);
```

### What It Does

`gettimeofday` fills `tv` with the current wall-clock time as seconds and
microseconds since the **Unix Epoch** (January 1, 1970 00:00:00 UTC). The `tz`
argument is obsolete and should be `NULL`.

### How It Works Internally

1. On modern Linux, `gettimeofday` is implemented as a **vDSO** (virtual
   dynamic shared object) — it reads a shared memory page that the kernel keeps
   updated, so it **doesn't need an actual syscall**. This makes it extremely
   fast (~20–50 ns).
2. The time source is typically the **TSC** (Time Stamp Counter) on x86, a
   per-CPU hardware counter running at a fixed frequency, calibrated against
   the HPET or PIT at boot.
3. The kernel periodically updates the vDSO page with NTP-adjusted offsets so
   the time stays accurate.

### The `timeval` Structure

```c
struct timeval {
    time_t      tv_sec;   // seconds since epoch
    suseconds_t tv_usec;  // microseconds (0–999999)
};
```

### Edge Cases & Pitfalls

| Scenario | Behaviour |
|---|---|
| `tv == NULL` | **Undefined behaviour** / segfault. |
| NTP adjustments | Time can **jump forward or backward** — not monotonic. For a short-lived project like philo this is negligible, but be aware. |
| Return value | Almost always `0`; errors are extremely rare. |
| `tv_usec` range | Always 0–999999. Don't forget to handle the seconds rollover when computing deltas. |

### Best Practice for Philosophers

Convert to a single millisecond timestamp for easy arithmetic:

```c
long	get_timestamp_ms(void)
{
    struct timeval	tv;

    gettimeofday(&tv, NULL);
    return ((long)tv.tv_sec * 1000L + (long)tv.tv_usec / 1000L);
}
```

Call this once at the start (`data->start_time = get_timestamp_ms()`) and
compute all subsequent timestamps as offsets: `now - data->start_time`.

---

## 8. `pthread_create`

### Prototype

```c
#include <pthread.h>

int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                   void *(*start_routine)(void *), void *arg);
```

### What It Does

Creates a new thread of execution. The new thread starts by calling
`start_routine(arg)`. On success, the thread ID is stored in `*thread` and `0`
is returned. On error, a non-zero error code is returned.

### How It Works Internally

1. **`clone()` syscall** — On Linux, `pthread_create` calls `clone()` with
   flags like `CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_THREAD`, creating a
   new kernel task that shares the same virtual address space, file descriptors,
   and signal handlers as the parent.
2. **Stack allocation** — The C library allocates a new stack for the thread
   (default size: 8 MB on Linux, mostly uncommitted virtual memory thanks to
   lazy allocation). A guard page is placed at the bottom to detect stack
   overflow.
3. **Scheduling** — The new thread is added to the kernel's run queue. On a
   multi-core system, it may start running immediately on another core — there
   is **no guarantee** about which thread (parent or child) runs first.
4. **Thread-local storage** — Each thread gets its own `errno`, stack, and TLS
   segment, but **all heap memory and global variables are shared**.

### Edge Cases & Pitfalls

| Scenario | Behaviour |
|---|---|
| `thread == NULL` | **Undefined behaviour**. |
| System thread limit reached | Returns `EAGAIN`. |
| `start_routine == NULL` | **Undefined behaviour**. |
| `attr == NULL` | Uses default attributes (joinable, default stack size). |
| Thread starts before `pthread_create` returns | Possible — don't access `*thread` from the new thread until synchronisation. |

### Best Practice for Philosophers

```c
int	i = 0;
while (i < data->nb_philos)
{
    if (pthread_create(&philos[i].thread, NULL, philosopher_routine, &philos[i]))
        return (exit_error(data, "pthread_create", strerror(errno)));
    i++;
}
```

**Key points:**
- Pass a pointer to each philosopher's own struct — **never** pass `&i`
  directly (it will change before the thread reads it).
- Always check the return value.
- `attr` can be `NULL` for default (joinable) threads.

---

## 9. `pthread_detach`

### Prototype

```c
#include <pthread.h>

int pthread_detach(pthread_t thread);
```

### What It Does

Marks the thread `thread` as **detached**. When a detached thread terminates,
its resources (stack, thread descriptor) are automatically released without
requiring another thread to call `pthread_join`.

### How It Works Internally

1. Sets a flag in the thread's internal control block indicating it's detached.
2. If the thread has already terminated, its resources are freed immediately.
3. If it's still running, the resources will be freed when it eventually exits.
4. A detached thread is like a "fire and forget" — no other thread can wait for
   it or retrieve its return value.

### Edge Cases & Pitfalls

| Scenario | Behaviour |
|---|---|
| Thread already joined | **Undefined behaviour**. |
| Thread already detached | **Undefined behaviour**. |
| Thread doesn't exist | **Undefined behaviour**. |
| Calling `pthread_join` on a detached thread | **Undefined behaviour**. |

### Best Practice for Philosophers

In the mandatory part of Philosophers, you generally **should not use
`pthread_detach`**. You want to `pthread_join` all philosopher threads to
ensure they've all finished before you free shared resources.

However, `pthread_detach` is useful for a **monitor/death-checker thread**
that you don't need to join:

```c
pthread_t monitor;
pthread_create(&monitor, NULL, death_monitor, data);
pthread_detach(monitor);
// The monitor thread will clean up after itself when it exits
```

> ⚠️ If you detach a thread, make sure it doesn't access any memory that might
> be freed by the main thread. The detached thread has no synchronisation point.

---

## 10. `pthread_join`

### Prototype

```c
#include <pthread.h>

int pthread_join(pthread_t thread, void **retval);
```

### What It Does

Blocks the calling thread until the specified `thread` terminates. If `retval`
is not `NULL`, the thread's return value (from `return` or `pthread_exit`) is
stored in `*retval`.

### How It Works Internally

1. The calling thread checks if the target thread has already exited.
2. If not, it goes to sleep on a **futex** (fast user-space mutex) associated
   with the target thread.
3. When the target thread exits, the kernel wakes all threads waiting on its
   futex.
4. The target thread's stack and resources are then freed.

The join operation serves two purposes:
- **Synchronisation** — you know the thread is done.
- **Resource reclamation** — without join (and without detach), the thread's
  resources leak (like a zombie process).

### Edge Cases & Pitfalls

| Scenario | Behaviour |
|---|---|
| Thread is detached | **Undefined behaviour**. |
| Thread already joined | **Undefined behaviour**. |
| `thread` is the calling thread | **Deadlock** (waits for itself forever). Returns `EDEADLK` on some implementations. |
| `retval == NULL` | Return value is discarded — perfectly fine. |
| Multiple threads join the same thread | **Undefined behaviour** — only one thread should join. |

### Best Practice for Philosophers

Join all philosopher threads after signalling them to stop:

```c
int	i = 0;
while (i < data->nb_philos)
{
    pthread_join(philos[i].thread, NULL);
    i++;
}
// NOW it's safe to free resources and destroy mutexes
```

**Golden rule:** Never free shared data or destroy mutexes until **all** threads
have been joined.

---

## 11. `pthread_mutex_init`

### Prototype

```c
#include <pthread.h>

int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr);
```

### What It Does

Initialises the mutex pointed to by `mutex` with the attributes specified by
`attr`. If `attr` is `NULL`, default attributes are used. Returns `0` on
success.

### How It Works Internally

1. Sets the mutex's internal state to "unlocked" (typically writes `0` to an
   atomic integer).
2. Initialises bookkeeping fields (owner thread, recursion count, wait queue).
3. With default attributes, the mutex is of type `PTHREAD_MUTEX_DEFAULT` — fast
   but with **undefined behaviour** on double-lock by the same thread.

The mutex is essentially a wrapper around a **futex** on Linux:
- The lock state is an integer in user-space (fast path: no syscall needed).
- The wait queue is managed by the kernel (slow path: when contention occurs).

### Edge Cases & Pitfalls

| Scenario | Behaviour |
|---|---|
| `mutex == NULL` | **Undefined behaviour**. |
| Initialising an already-initialised mutex | **Undefined behaviour**. |
| Using `PTHREAD_MUTEX_INITIALIZER` instead | Valid for statically-allocated mutexes; no need to call `init`. |
| Forgetting to call `init` | **Undefined behaviour** — the mutex contains garbage. |

### Best Practice for Philosophers

Initialise all mutexes in a dedicated setup function and check each return
value:

```c
int	init_forks(t_data *data)
{
    int	i;

    i = 0;
    while (i < data->nb_philos)
    {
        if (pthread_mutex_init(&data->forks[i], NULL) != 0)
            return (FAILURE);
        i++;
    }
    return (SUCCESS);
}
```

If any initialisation fails, destroy the ones already initialised before
returning.

---

## 12. `pthread_mutex_destroy`

### Prototype

```c
#include <pthread.h>

int pthread_mutex_destroy(pthread_mutex_t *mutex);
```

### What It Does

Destroys the mutex `mutex`, releasing any resources it holds. After destruction,
the mutex is uninitialised and must be re-initialised before reuse.

### How It Works Internally

1. Checks that the mutex is unlocked and no thread is waiting on it.
2. Marks the mutex as destroyed (sets internal state to an invalid value).
3. On Linux with default mutexes, this is essentially a no-op in terms of
   resource freeing (no kernel object to release), but it's important for
   correctness and portability.

### Edge Cases & Pitfalls

| Scenario | Behaviour |
|---|---|
| Destroying a locked mutex | **Undefined behaviour**. |
| Destroying a mutex threads are waiting on | **Undefined behaviour**. |
| Destroying an uninitialised mutex | **Undefined behaviour**. |
| Double destroy | **Undefined behaviour**. |

### Best Practice for Philosophers

Destroy mutexes **only after all threads have been joined**:

```c
void	cleanup(t_data *data)
{
    int	i;

    // First: join all threads (already done)
    // Then: destroy mutexes
    i = 0;
    while (i < data->nb_philos)
    {
        pthread_mutex_destroy(&data->forks[i]);
        i++;
    }
    pthread_mutex_destroy(&data->write_mutex);
    pthread_mutex_destroy(&data->meal_mutex);
    free(data->philos);
    free(data->forks);
}
```

---

## 13. `pthread_mutex_lock`

### Prototype

```c
#include <pthread.h>

int pthread_mutex_lock(pthread_mutex_t *mutex);
```

### What It Does

Locks the mutex. If the mutex is already locked by another thread, the calling
thread **blocks** until the mutex becomes available.

### How It Works Internally

1. **Fast path (uncontended)** — An atomic compare-and-swap (`cmpxchg` on x86)
   tries to change the lock state from 0 (unlocked) to 1 (locked). If it
   succeeds, the function returns immediately — **no syscall, ~25 ns**.
2. **Slow path (contended)** — If the CAS fails (another thread holds the
   lock), the thread may **spin** briefly (adaptive spinning) hoping the lock
   is released quickly. If spinning fails, it calls `futex(FUTEX_WAIT)` to
   sleep in the kernel until woken.
3. **Wake-up** — When the lock holder unlocks, `futex(FUTEX_WAKE)` wakes one
   waiting thread, which retries the CAS.

### Edge Cases & Pitfalls

| Scenario | Behaviour |
|---|---|
| Locking an already-held mutex (same thread) | **Deadlock** with default mutex type. Use `PTHREAD_MUTEX_ERRORCHECK` during debugging to get `EDEADLK` instead. |
| `mutex` not initialised | **Undefined behaviour**. |
| `mutex == NULL` | **Undefined behaviour** / segfault. |
| Thread cancelled while holding lock | Lock remains locked — other threads deadlock. Use cleanup handlers. |

### Best Practice for Philosophers

**Lock ordering is everything.** To prevent deadlocks with forks:

```c
// Each philosopher picks up the lower-numbered fork first
void	take_forks(t_philo *philo)
{
    int	first;
    int	second;

    if (philo->left_fork < philo->right_fork)
    {
        first = philo->left_fork;
        second = philo->right_fork;
    }
    else
    {
        first = philo->right_fork;
        second = philo->left_fork;
    }
    pthread_mutex_lock(&philo->data->forks[first]);
    philo_log(philo, "has taken a fork");
    pthread_mutex_lock(&philo->data->forks[second]);
    philo_log(philo, "has taken a fork");
}
```

Alternatively, the classic approach: **even-numbered philosophers pick left
first, odd pick right first.** Either way, the goal is to break the circular
wait condition.

---

## 14. `pthread_mutex_unlock`

### Prototype

```c
#include <pthread.h>

int pthread_mutex_unlock(pthread_mutex_t *mutex);
```

### What It Does

Unlocks the mutex. If threads are waiting on the mutex, one of them is woken up
and acquires the lock.

### How It Works Internally

1. **Fast path (no waiters)** — An atomic store sets the lock state to 0.
   Returns immediately, ~10 ns.
2. **Slow path (waiters)** — After setting the state to 0, calls
   `futex(FUTEX_WAKE, 1)` to wake one waiting thread.
3. The woken thread's `pthread_mutex_lock` retries the CAS and (usually)
   succeeds.

### Edge Cases & Pitfalls

| Scenario | Behaviour |
|---|---|
| Unlocking a mutex not held by this thread | **Undefined behaviour** with default type. `PTHREAD_MUTEX_ERRORCHECK` returns `EPERM`. |
| Unlocking an unlocked mutex | **Undefined behaviour**. |
| `mutex` not initialised | **Undefined behaviour**. |

### Best Practice for Philosophers

Always pair lock/unlock. Keep the **critical section** (code between lock and
unlock) as **short as possible**:

```c
// GOOD — lock only what's necessary
pthread_mutex_lock(&data->meal_mutex);
philo->last_meal_time = get_timestamp_ms();
philo->meals_eaten++;
pthread_mutex_unlock(&data->meal_mutex);

// BAD — holding the lock during I/O
pthread_mutex_lock(&data->meal_mutex);
philo->last_meal_time = get_timestamp_ms();
printf("%ld %d is eating\n", ts, id);  // slow I/O under lock!
usleep(data->time_to_eat * 1000);      // sleeping under lock!!
pthread_mutex_unlock(&data->meal_mutex);
```

---

## 15. Tips & Tricks for a Beautiful Philosopher

### Architecture

```
includes/
├── philo.h          # Main header — includes everything
├── defines.h        # Macros, return codes, limits
├── structures.h     # All typedefs and structs
└── errors.h         # Error message constants

src/
├── main.c           # Entry point: parse → init → run → cleanup
├── parsing/
│   ├── parse_args.c # Validate and convert arguments
│   └── validate.c   # Range checks (positive, non-zero, etc.)
├── core/
│   ├── init.c       # Allocate and initialise data, philos, mutexes
│   ├── routine.c    # The philosopher's life cycle (think/eat/sleep)
│   ├── monitor.c    # Death checker + optional meal counter
│   └── actions.c    # take_forks, eat, sleep_philo, think
└── utils/
    ├── time.c       # get_timestamp_ms, precise_sleep
    ├── error.c      # print_error, exit_error
    └── cleanup.c    # Join threads, destroy mutexes, free memory
```

### Data Flow

```
main()
 ├─ parse_args()          → fills data->nb_philos, time_to_die, etc.
 ├─ init_data()           → malloc philos[], forks[], init mutexes
 ├─ data->start_time = get_timestamp_ms()
 ├─ create_threads()      → pthread_create for each philosopher
 ├─ monitor_routine()     → runs in main thread (or a dedicated thread)
 │   └─ loops checking: has anyone died? has everyone eaten enough?
 ├─ join_threads()        → pthread_join all philosophers
 └─ cleanup()             → destroy mutexes, free memory
```

### The Philosopher Routine

```c
void	*philosopher_routine(void *arg)
{
    t_philo	*philo = (t_philo *)arg;

    // Stagger start: even philos wait briefly to avoid initial contention
    if (philo->id % 2 == 0)
        precise_sleep(1, philo->data);

    while (!someone_died(philo->data))
    {
        think(philo);
        take_forks(philo);      // lock two mutexes
        eat(philo);             // update last_meal_time, sleep time_to_eat
        release_forks(philo);   // unlock two mutexes
        sleep_philo(philo);     // sleep time_to_sleep
    }
    return (NULL);
}
```

### Struct Design

```c
typedef struct s_data
{
    int             nb_philos;
    long            time_to_die;
    long            time_to_eat;
    long            time_to_sleep;
    int             must_eat;       // -1 if not specified
    long            start_time;
    int             someone_died;
    int             all_ate;
    pthread_mutex_t write_mutex;
    pthread_mutex_t meal_mutex;     // protects last_meal_time & meals_eaten
    pthread_mutex_t dead_mutex;     // protects someone_died flag
    pthread_mutex_t *forks;
    t_philo         *philos;
}   t_data;

typedef struct s_philo
{
    int             id;             // 1-indexed
    int             left_fork;      // index into data->forks
    int             right_fork;
    long            last_meal_time;
    int             meals_eaten;
    pthread_t       thread;
    t_data          *data;          // back-pointer to shared data
}   t_philo;
```

### Preventing Data Races

| Shared Variable | Protected By |
|---|---|
| `someone_died` | `dead_mutex` |
| `last_meal_time`, `meals_eaten` | `meal_mutex` |
| `stdout` (printf) | `write_mutex` |
| Each fork | Its own `forks[i]` mutex |

**Read-side protection matters too!** Reading `someone_died` without the mutex
is a data race even if it's "just a flag". Use the mutex:

```c
int	someone_died(t_data *data)
{
    int	result;

    pthread_mutex_lock(&data->dead_mutex);
    result = data->someone_died;
    pthread_mutex_unlock(&data->dead_mutex);
    return (result);
}
```

### Preventing Deadlocks

Deadlocks happen when threads hold locks in different orders. Solutions:

1. **Ordered locking** — Always lock the lower-numbered fork first.
2. **Even/odd stagger** — Even philosophers pick left first, odd pick right.
   This is the simplest and most common approach:

```c
void	take_forks(t_philo *philo)
{
    if (philo->id % 2 == 0)
    {
        pthread_mutex_lock(&philo->data->forks[philo->right_fork]);
        philo_log(philo, "has taken a fork");
        pthread_mutex_lock(&philo->data->forks[philo->left_fork]);
        philo_log(philo, "has taken a fork");
    }
    else
    {
        pthread_mutex_lock(&philo->data->forks[philo->left_fork]);
        philo_log(philo, "has taken a fork");
        pthread_mutex_lock(&philo->data->forks[philo->right_fork]);
        philo_log(philo, "has taken a fork");
    }
}
```

### The Monitor Pattern

Run the death checker in the **main thread** after creating all philosopher
threads:

```c
void	monitor(t_data *data)
{
    int	i;

    while (1)
    {
        i = 0;
        while (i < data->nb_philos)
        {
            pthread_mutex_lock(&data->meal_mutex);
            if (get_timestamp_ms() - data->philos[i].last_meal_time
                > data->time_to_die)
            {
                pthread_mutex_unlock(&data->meal_mutex);
                philo_log(&data->philos[i], "died");
                pthread_mutex_lock(&data->dead_mutex);
                data->someone_died = 1;
                pthread_mutex_unlock(&data->dead_mutex);
                return ;
            }
            pthread_mutex_unlock(&data->meal_mutex);
            i++;
        }
        usleep(1000); // check every ~1ms
    }
}
```

### The One-Philosopher Edge Case

With only 1 philosopher, there's only 1 fork — they can never eat. Handle it:

```c
if (data->nb_philos == 1)
{
    philo_log(&philos[0], "has taken a fork");
    precise_sleep(data->time_to_die, data);
    philo_log(&philos[0], "died");
    return ;
}
```

### Timing Precision Checklist

- [ ] Use `get_timestamp_ms()` everywhere — never raw `gettimeofday`.
- [ ] Use `precise_sleep()` with small `usleep` increments — never a single
      `usleep(time_to_eat * 1000)`.
- [ ] Stagger even/odd philosophers at start to reduce initial contention.
- [ ] Print timestamp *inside* the write-lock to avoid ordering issues.
- [ ] Death must be detected within **10 ms** of `time_to_die` elapsing.

### Testing Commands

```bash
# Should never die
./philo 5 800 200 200
./philo 4 410 200 200

# Should die
./philo 4 310 200 100

# Should stop after each eats 7 times
./philo 5 800 200 200 7

# Edge case: one philosopher
./philo 1 800 200 200

# Stress test
./philo 200 800 200 200
```

### Debugging Tips

1. **Use `helgrind`** — Valgrind's thread error detector:
   ```bash
   valgrind --tool=helgrind ./philo 5 800 200 200
   ```

2. **Use `ThreadSanitizer`** — Compile with:
   ```bash
   cc -fsanitize=thread -g -O1 ...
   ```

3. **Add debug logging** (remove before submission):
   ```c
   #ifdef DEBUG
   # define LOG(fmt, ...) fprintf(stderr, "[DBG] " fmt "\n", ##__VA_ARGS__)
   #else
   # define LOG(fmt, ...)
   #endif
   ```

4. **Check for leaks** (after all threads are joined):
   ```bash
   valgrind --leak-check=full ./philo 5 800 200 200 7
   ```

---

> *"The unexamined mutex is not worth locking."* — Socrates, probably
