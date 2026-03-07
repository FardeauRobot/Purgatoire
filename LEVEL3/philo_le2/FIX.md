# FIX.md — Philosophers Debug Report

## Reading the Helgrind Output

Before diving into the bugs, let's learn to read what Helgrind is telling you.

### Error 1: "Exiting thread still holds 1 lock"

```
Thread #2: Exiting thread still holds 1 lock
   at 0x4910A66: start_thread (pthread_create.c:593)
```

**What it means:** A philosopher thread returned from `ft_routine_guests()` while it
still had a `pthread_mutex_lock()` active — it never called the matching
`pthread_mutex_unlock()`. Thread #2 is the first spawned thread (philosopher 1,
index 0).

**How to find the culprit:** Thread #2 maps to the first `pthread_create` call in
`ft_guests_arr` (start.c:59). The philosopher broke out of the `while(1)` loop
(because `ft_end_check` returned `STANDARD_ERROR`), but at that point it was
blocked on a fork mutex inside `ft_fork_appropriation`. When the reaper sets
`ended = 1`, the philosopher eventually gets the mutex, sees the end flag, and
returns — but it still holds the fork mutex it just acquired.

### Error 2: "pthread_mutex_destroy of a locked mutex"

```
Thread #1: pthread_mutex_destroy of a locked mutex
   at 0x401E1C: ft_philo_clean (error.c:28)
   by 0x401259: main (main.c:23)
```

**What it means:** `main()` calls `ft_philo_clean()` which calls
`pthread_mutex_destroy` on every fork mutex. But one of those fork mutexes is
still locked (because of Error 1 above — the thread exited without unlocking).
You can't destroy a locked mutex.

### Error 3: "pthread_mutex_destroy failed with EBUSY"

```
Thread #1's call to pthread_mutex_destroy failed
   with error code 16 (EBUSY: Device or resource busy)
```

**What it means:** This is the consequence of Error 2. The `pthread_mutex_destroy`
call returned `EBUSY` because the mutex is still locked. Your code ignores this
return value, so the mutex leaks.

### The "died" printed twice

```
1228 1 died
1231 1 died
```

The reaper calls `ft_status_change(&philo->guests[i], DEAD)`, which internally
calls `ft_action_print`. But in `ft_action_print`, the `ft_end_check` at the top
sees `ended == 1` (set by the reaper just before), and since `status == DEAD`,
it prints "died". Then the routine of another thread (or the same reaper loop
iteration) also hits `ft_action_print` and prints it again. The race exists
because `ft_status_change` sets `ended` before calling print, so multiple code
paths can enter the "dead" print branch.

---

## Bug #1 — Thread exits while holding a fork mutex

### Where

`ft_fork_appropriation()` in actions.c, lines 17-35.

### What happens

A philosopher calls `pthread_mutex_lock(guest->forks[LEFT])` (or RIGHT).
While blocked waiting for the lock, the reaper sets `ended = 1`.
Eventually the philosopher acquires the lock, but the `ft_end_check` at the TOP
of `ft_fork_appropriation` already ran (before the lock call), so it doesn't
catch the end flag. The philosopher eventually breaks out of the main loop
via a later `ft_end_check`, but **never unlocks the fork it just acquired**.

### The cleanup in `ft_fork_handler` is insufficient

```c
if (ft_end_check(guest->data) != SUCCESS)
{
    if (guest->held_fork[LEFT] == 1)
        pthread_mutex_unlock(guest->forks[LEFT]);
    if (guest->held_fork[RIGHT] == 1)
        pthread_mutex_unlock(guest->forks[RIGHT]);
    return (STANDARD_ERROR);
}
```

This only runs at the **start** of `ft_fork_handler`, before any fork is taken.
After `ft_fork_appropriation` acquires a fork and sets `held_fork`, there is no
cleanup if `ft_end_check` subsequently fails. The routine returns `NULL` and the
fork stays locked.

### Fix

After each `pthread_mutex_lock` on a fork, immediately re-check `ft_end_check`.
If the simulation ended while you were waiting, unlock what you just locked and
return an error:

```c
pthread_mutex_lock(guest->forks[LEFT]);
guest->held_fork[LEFT] = 1;
if (ft_end_check(guest->data) != SUCCESS)
{
    pthread_mutex_unlock(guest->forks[LEFT]);
    guest->held_fork[LEFT] = 0;
    return (STANDARD_ERROR);
}
ft_status_change(guest, FORK);
// same for RIGHT...
```

Also, ensure the main routine loop always cleans up held forks before returning:

```c
while (1)
{
    if (ft_eat(guest) != SUCCESS)
        break;
    // ...
}
// Cleanup before thread exit:
if (guest->held_fork[LEFT])
    pthread_mutex_unlock(guest->forks[LEFT]);
if (guest->held_fork[RIGHT])
    pthread_mutex_unlock(guest->forks[RIGHT]);
return (NULL);
```

---

## Bug #2 — Starvation (philosophers 3 and 5 never eat)

### Where

`ft_fork_appropriation()` in actions.c + `ft_routine_guests()` in start.c.

### What happens

With 5 philosophers and the `t_id % 2` even/odd fork ordering:

| Philo | First fork grabbed | Second fork grabbed |
|-------|--------------------|---------------------|
| 1 (odd)  | LEFT = m_fork[0]  | RIGHT = m_fork[4] |
| 2 (even) | RIGHT = m_fork[0] | LEFT = m_fork[1]  |
| 3 (odd)  | LEFT = m_fork[2]  | RIGHT = m_fork[1] |
| 4 (even) | RIGHT = m_fork[2] | LEFT = m_fork[3]  |
| 5 (odd)  | LEFT = m_fork[4]  | RIGHT = m_fork[3] |

Philo 2 and 3 both contend on m_fork[0] and m_fork[1]/m_fork[2].
Philo 4 and 5 both contend on m_fork[2] and m_fork[3].

Because `pthread_mutex_lock` has **no fairness guarantee**, the philosopher who
just released and instantly re-locks the mutex tends to win over the one that
has been waiting. This is called **thread starvation**.

### Two sub-causes

**A) `usleep(3000)` is way too small.**
3000 microseconds = 3 milliseconds. `time_to_eat` = 200 ms. The stagger is
1.5% of the eating time — meaningless, especially under helgrind which slows
everything down. Even philosophers finish their first meal and re-grab forks
before odd philosophers have even started.

**B) `ft_think` has the right idea but isn't enough alone.**
Your think delay formula `(time_to_eat * 2) - time_to_sleep` is correct for
odd numbers of philosophers, but it can't compensate if the initial stagger
is wrong and philosophers are already out of sync.

### Fix

**Initial stagger** — delay even philosophers, not odd (since odd grab LEFT
first, they should start first to establish the alternation):

```c
if (guest->t_id % 2 == 0)
    usleep(guest->data->time_to_eat * 1000); // full time_to_eat in usec
```

Or even more simply, use the **resource ordering** approach instead of even/odd.
Always lock the lower-addressed mutex first:

```c
if (guest->forks[LEFT] < guest->forks[RIGHT])
{
    pthread_mutex_lock(guest->forks[LEFT]);
    pthread_mutex_lock(guest->forks[RIGHT]);
}
else
{
    pthread_mutex_lock(guest->forks[RIGHT]);
    pthread_mutex_lock(guest->forks[LEFT]);
}
```

This prevents deadlock AND distributes contention more evenly.

**Think time for odd philosopher counts** — your formula is correct, keep it:

```c
think_time = (time_to_eat * 2) - time_to_sleep;
```

---

## Bug #3 — "died" printed twice

### Where

`ft_reaper()` in reaper.c + `ft_action_print()` in output.c.

### What happens

1. Reaper detects death, sets `philo->ended = 1`
2. Reaper calls `ft_status_change(guest, DEAD)` which calls `ft_action_print`
3. Inside `ft_action_print`, `ft_end_check` returns `STANDARD_ERROR` (because
   `ended == 1`), and since `status == DEAD`, it prints "died" → **first print**
4. Meanwhile, another thread (or the same code path on a subsequent call) also
   enters `ft_action_print`, sees `ended == 1` and `status == DEAD`, and prints
   "died" again → **second print**

The core issue: `ft_action_print` will print "died" for **any** thread whose
status is `DEAD` whenever `ended` is set. If two threads enter `ft_action_print`
and both see `guest->status == DEAD`, you get two prints.

### Fix

Use a dedicated flag to ensure "died" is printed exactly once:

```c
// In reaper, after setting ended = 1:
pthread_mutex_lock(&philo->m_print);
printf("%lld %d died\n",
    ft_get_time(MILLISECONDS) - philo->start_time,
    philo->guests[i].t_id);
pthread_mutex_unlock(&philo->m_print);
return (NULL);  // don't call ft_status_change for DEAD
```

Or add an `already_printed_death` flag protected by `m_print` to guarantee
only one "died" message ever goes through.

---

## Bug #4 — `ft_philo_clean` doesn't destroy per-guest mutexes properly

### Where

`ft_philo_clean()` in error.c.

### What happens

`ft_philo_clean` calls `pthread_mutex_destroy` on fork mutexes and guest
mutexes, but:

1. It doesn't check if a mutex is currently locked (Error 2 from helgrind)
2. If `ft_exit` is called mid-initialization (e.g., a `pthread_create` fails
   on guest index 3), it tries to destroy mutexes for guests 0–4, but guests
   3 and 4 were never initialized

### Fix

Track how many guests were successfully initialized and only destroy those:

```c
void ft_philo_clean(t_philo *philo)
{
    int i;

    i = -1;
    while (++i < philo->nb_philo)
    {
        pthread_mutex_destroy(&philo->guests[i].m_status);
        pthread_mutex_destroy(&philo->guests[i].m_last_meal);
        pthread_mutex_destroy(&philo->guests[i].m_nb_meal);
        pthread_mutex_destroy(&philo->m_fork[i]);
    }
    pthread_mutex_destroy(&philo->m_print);
    pthread_mutex_destroy(&philo->m_ended);
    pthread_mutex_destroy(&philo->m_full);
    if (philo->m_fork)
        free(philo->m_fork);
}
```

And ensure all threads have exited (via `pthread_join`) **before** calling
`ft_philo_clean`. Currently `main()` calls `ft_start` (which joins threads)
and then `ft_philo_clean`, which is correct — but if a thread exits holding
a lock, the destroy will fail. Fixing Bug #1 fixes this transitively.

---

## Bug #5 — `ft_end_check` race in `ft_fork_appropriation`

### Where

`ft_fork_appropriation()` in actions.c, line 18.

### What happens

```c
int ft_fork_appropriation(t_guest *guest)
{
    if (ft_end_check(guest->data) != SUCCESS)  // ← check happens HERE
        return (STANDARD_ERROR);
    if (guest->status == THINKING)
    {
        pthread_mutex_lock(guest->forks[...]);  // ← blocks HERE for a long time
```

Between the `ft_end_check` and the `pthread_mutex_lock`, the simulation may end.
The philosopher then blocks on the lock indefinitely (or until the thread
holding it exits, leaving the mutex in an undefined state).

Even worse: after acquiring the lock, there's no re-check of the end condition.
The philosopher proceeds to eat from a dead simulation.

### Fix

Always re-check after acquiring each fork:

```c
pthread_mutex_lock(guest->forks[LEFT]);
guest->held_fork[LEFT] = 1;
if (ft_end_check(guest->data) != SUCCESS)
{
    pthread_mutex_unlock(guest->forks[LEFT]);
    guest->held_fork[LEFT] = 0;
    return (STANDARD_ERROR);
}
```

---

## Summary — The lifecycle a fix should achieve

```
1. Even philos start eating immediately
2. Odd philos wait ~(time_to_eat / 2) then start eating
3. After eating: unlock both forks, then sleep
4. After sleeping: think for (2 * time_to_eat - time_to_sleep) if nb_philo is odd
5. After thinking: try to grab forks again
6. After each mutex_lock: re-check ft_end_check, unlock if ended
7. On thread exit: always ensure held forks are released
8. Reaper prints "died" exactly once, directly under m_print
9. ft_philo_clean only destroys mutexes that were initialized and are unlocked
```

## Quick Helgrind Cheat Sheet

| Helgrind message | Meaning | Usual cause |
|---|---|---|
| `Exiting thread still holds N lock(s)` | Thread returned/exited with a locked mutex | Missing `pthread_mutex_unlock` on error path |
| `pthread_mutex_destroy of a locked mutex` | You tried to destroy a mutex someone still holds | Destroying before all threads finished, or thread leaked a lock |
| `Possible data race` | Two threads access same memory, at least one writes, no mutex | Missing mutex protection around shared variable |
| `Lock order violation` | Thread A locks M1→M2, Thread B locks M2→M1 | Inconsistent lock ordering → potential deadlock |
| `EBUSY` on destroy | The mutex is still locked | Consequence of a leaked lock |
