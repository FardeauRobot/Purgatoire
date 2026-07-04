# `volatile` — "The Compiler May Not Optimize Reads/Writes Away"

> **TL;DR.** `volatile` tells the compiler: *"every read and write to this variable must hit memory in source order — don't cache it in a register, don't reorder, don't elide."* It is for **memory-mapped hardware** and **signal handlers**, not for threads.

Related: [`CONST.md`](CONST.md) · [`MUTABLE.md`](MUTABLE.md)

---

## 1. The problem `volatile` solves

The compiler is allowed to optimize like this:

```cpp
int status;
while (status == 0) {
    /* spin */
}
```

becomes, after optimization:

```
   load  r1, [status]   ; load once
.L:                     ;
   test  r1, r1         ; check the register over and over
   jz    .L
```

If `status` is updated by a signal handler, an interrupt, or a memory-mapped device, the compiler doesn't know — and the loop never terminates.

```
                        ┌─────────────────────────┐
   compiler thinks:     │ status doesn't change   │
                        │ between iterations      │
                        └────────────┬────────────┘
                                     │
                                     ▼
                        register-cached forever → infinite loop
```

`volatile int status;` forces:

```
   .L:
   load  r1, [status]   ; reload from memory every iteration
   test  r1, r1
   jz    .L
```

---

## 2. What `volatile` actually means

A `volatile` access is a **side effect**, just like I/O. The compiler:

- Must perform the read or write at the source-program point.
- Must not combine multiple `volatile` accesses into one.
- Must not eliminate accesses whose result is unused.
- Must keep `volatile` accesses in source order **with respect to other `volatile` accesses**.

It does **not**:

- Synchronize between threads.
- Establish a memory barrier on multicore hardware (`x86`, `ARM`, etc. each have their own ordering rules).
- Make accesses atomic.

---

## 3. Where `volatile` is correct

### 3.1 Memory-mapped I/O (embedded)

```cpp
volatile uint32_t *uart_status = (uint32_t*)0x4000'1000;
volatile uint32_t *uart_data   = (uint32_t*)0x4000'1004;

void send(char c) {
    while ((*uart_status & TX_READY) == 0) {}   // each read MUST hit the device
    *uart_data = c;
}
```

```
        CPU                            device
         │                              │
   load [uart_status]  ──── bus ───►   register reflects current state
         │                              │
   test bit, loop                       │
         │                              │
   store [uart_data] ───── bus ────►   buffered byte, transmitted
```

Without `volatile`, the compiler may:
- Read `uart_status` once and assume it never changes → busy-wait forever.
- Drop the write to `uart_data` if it sees no further use of the variable.

### 3.2 Signal handlers

```cpp
volatile sig_atomic_t stop = 0;

void handler(int) { stop = 1; }

int main() {
    signal(SIGINT, handler);
    while (!stop) { /* work */ }
    return 0;
}
```

`sig_atomic_t` is the only type the C++ standard guarantees a signal handler can read/write portably. `volatile` ensures the main loop re-reads `stop` from memory.

### 3.3 `setjmp` / `longjmp` locals

A local that is written between `setjmp` and `longjmp` and read after `longjmp` must be `volatile`, otherwise the value is undefined.

---

## 4. Where `volatile` is NOT a fix

### 4.1 Multithreading

`volatile` provides **zero** synchronization on a modern multicore CPU.

```cpp
// BROKEN
volatile bool ready = false;
volatile int  data  = 0;

// thread A
data = 42;
ready = true;

// thread B
while (!ready) {}
use(data);   // may still see data == 0
```

The CPU and compiler can reorder `data = 42` and `ready = true` to each other from B's point of view. `volatile` does not stop reordering across non-volatile reads/writes.

```
       core A                 cache coherence              core B
   ┌──────────────┐                                    ┌──────────────┐
   │ data = 42    │                                    │ load ready   │
   │ ready = true │                                    │ load data    │
   └───────┬──────┘                                    └──────────────┘
           │
   the writes can become visible
   to core B in either order
   on most architectures
```

The right tool is `std::atomic<T>` (C++11+) or a mutex. C++98 has neither portably — use `pthread_mutex_t`.

### 4.2 Marking non-thread-safe code "thread-safe"

Adding `volatile` does nothing for atomicity, race conditions, or visibility ordering between threads. It is a cargo-cult anti-pattern.

---

## 5. Syntax — same rules as `const`

Read the type right-to-left:

```cpp
volatile int  x;             // a volatile int
int volatile  y;             // identical
volatile int *p;             // pointer to a volatile int
int *volatile q;             // volatile pointer to a (regular) int
const volatile int *r;       // pointer to a const volatile int
                             // (e.g., a hardware status register
                             //  that you must reread but can't write)
```

`const volatile` is a real and useful combination — perfect for read-only hardware registers that change without your code's involvement.

---

## 6. Member-function `volatile`

Mirror of [`const`](CONST.md) — promises the function can be called on `volatile` objects:

```cpp
class Sensor {
    int _value;
public:
    int read() volatile { return _value; }     // can be called on volatile Sensor
};

volatile Sensor s;
s.read();         // OK
```

Almost never useful in 42-level code; common in driver code.

---

## 7. Hardware view — what `volatile` actually compiles to

`volatile` is **not a memory barrier instruction**. The compiler still emits a plain load/store; it just can't elide or reorder it relative to other `volatile` accesses.

```
    int n = *p;          int n = *vp;        (vp is volatile int*)
    int m = *p;          int m = *vp;
    
    optimizer:           optimizer:
    one load             two loads — required
```

On x86, this lines up with the cache-coherent memory model: the load instruction *will* see a value that was previously written by another core, but ordering between unrelated locations is not guaranteed without fence instructions (`mfence`, `lfence`, `sfence`).

---

## 8. Tips & tricks

### 8.1 Drivers — wrap registers in a struct

```cpp
struct UartRegs {
    volatile uint32_t status;
    volatile uint32_t data;
    volatile uint32_t control;
};

UartRegs *uart = reinterpret_cast<UartRegs*>(0x4000'1000);
uart->data = 'A';
```

Cleaner than naked pointers, and `volatile` propagates to every member access.

### 8.2 Don't use `volatile` for "I want the value reread for debugging"

Compile with `-O0` instead. `volatile` ships in production code; "it makes my breakpoint work" is not a real reason.

### 8.3 `volatile` and `const_cast`

You can `const_cast<volatile T*>(p)` and `const_cast<T*>(vp)` — same rules, different qualifier. Do not use this casually; you're stripping a contract.

### 8.4 Atomics replace it for threads — completely

```cpp
// C++11+, the right tool:
std::atomic<bool> ready(false);
std::atomic<int>  data(0);
```

Atomics give you both the "don't optimize away" guarantee **and** memory ordering. `volatile` gives only the first.

---

## 9. The 42 reality

For 42 CPP modules you almost never need `volatile`. It surfaces in:

- Embedded coursework (not 42 CPP).
- Talking to OS-level signal handlers (not 42 CPP).
- Cargo-culted code you should rewrite without it.

But you **will** see it in interview questions and senior-level codebases — be ready to explain *exactly* what it does and what it doesn't.

---

## 10. Visual summary

```
                ┌─────────────────────────────────────┐
                │   "every access hits memory"        │
                │       (compiler can't optimize      │
                │        reads/writes away)           │
                └────────────────────┬────────────────┘
                                     │
              ┌──────────────────────┼──────────────────────┐
              ▼                      ▼                      ▼
   memory-mapped I/O        signal handlers        setjmp/longjmp locals
   (UART, GPIO,             (sig_atomic_t flag)    (preserve across long jump)
    DMA registers)
              │                      │                      │
              └─────── all single-threaded patterns ────────┘
                                     │
                                     ▼
                           NOT a synchronization tool.
                            Use std::atomic / mutex
                            for cross-thread visibility.
```

---

## 11. Practice

1. Will `volatile int x; x = 1; x = 2; x = 3;` produce three stores in the assembly? *(Yes — the compiler must emit each side effect.)*
2. Why does a spin-wait loop on a non-volatile flag potentially hang at `-O2`? *(The compiler hoists the load out of the loop, caching the initial value in a register forever.)*
3. Why is `volatile std::atomic<int>` redundant on x86_64? *(The atomic already provides every guarantee `volatile` does, plus more — ordering, atomicity. `volatile` adds no useful semantics on top.)*
