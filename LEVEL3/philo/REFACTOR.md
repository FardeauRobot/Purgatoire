# REFACTOR — Suggested improvements

---

## 1. `ft_fork_handler` should be `static` (actions.c)

It is only used internally within `actions.c` and has no prototype in any
header. The 42 norm requires file-local functions to be declared `static`.

```c
// Before
int	ft_fork_handler(t_guest *guest, t_fork side)

// After
static int	ft_fork_handler(t_guest *guest, t_fork side)
```

---

## 2. `ft_philo_fill` — if/else chain (parsing.c)

The function uses a series of `if` instead of `else if`. The final block:

```c
if (i == 5)
{
    ...
    philo->needed_meals = (int)tmp;
}
else
    philo->needed_meals = 0;  // <-- runs for EVERY i != 5
```

This resets `needed_meals` to 0 on every iteration except the last. It works
by accident (i=5 is the last iteration when argc=6, and 0 is the correct
default when argc=5), but it is misleading and fragile.

**Fix:** use `else if` throughout and remove the trailing `else`:

```c
static void	ft_philo_fill(t_philo *philo, long long tmp, int i)
{
    if (i == 1)
    {
        ...
    }
    else if (i == 2)
        philo->time_to_die = (time_t)tmp;
    else if (i == 3)
        philo->time_to_eat = (time_t)tmp;
    else if (i == 4)
        philo->time_to_sleep = (time_t)tmp;
    else if (i == 5)
    {
        ...
        philo->needed_meals = (int)tmp;
    }
}
```

---

## 3. `ft_philo_clean` misses per-guest mutexes (error.c)

`m_last_meal` and `m_status` for each guest are initialized in `ft_guests_arr`
but never destroyed. Add a loop in `ft_philo_clean`:

```c
i = -1;
while (++i < philo->nb_philo)
{
    pthread_mutex_destroy(&philo->guests[i].m_last_meal);
    pthread_mutex_destroy(&philo->guests[i].m_status);
}
```

---

## 4. `ft_action_print` reads `guest->status` without mutex (output.c)

`ft_status_change` correctly locks `m_status` when **writing**, but
`ft_action_print` reads `guest->status` without locking `m_status`. The reaper
could write `DEAD` via `ft_status_change` at the same time, creating a data
race. Wrap the reads in `ft_action_print` with `m_status` lock/unlock.

---

## 5. Death message should be printed by the reaper (reaper.c)

Currently the reaper sets `DEAD` status and the `is_dead` flag, but the death
message (`"X died"`) is only printed if the dead philosopher's thread happens
to call `ft_action_print`. This is unreliable and can violate the 10 ms print
requirement.

**Fix:** have the reaper print the death message directly (under `m_print`
lock) right after detecting death.

---

## 6. `ft_routine` should check action return values (start.c)

After death is detected, `ft_eat` / `ft_sleep` / `ft_think` return `1`, but
`ft_routine` ignores those return values and continues to the next action
before re-checking the while condition. This wastes cycles:

```c
// Before
while (!ft_meal(guest->data))
{
    ft_eat(guest);
    ft_sleep(guest);
    ft_think(guest);
}

// After — breaks immediately when an action detects death
while (!ft_meal(guest->data))
{
    if (ft_eat(guest) || ft_sleep(guest) || ft_think(guest))
        break ;
}
```

---

## 7. `ft_exit` can destroy mutexes while threads are running (error.c)

`ft_exit` calls `ft_philo_clean` then `exit()`. If called from
`ft_guests_arr` after some threads have already been created (e.g.
`pthread_create` fails for thread N), the already-running threads still hold
references to the mutexes being destroyed. This is undefined behaviour.

**Fix:** before calling `ft_philo_clean`, join (or cancel) all threads that
were successfully created.

---

## 8. Duplicate prototypes in defines.h

`ft_status_change` and `ft_dead_check` appear under both `// ACTIONS.C` and
`// UTILS.C` comment sections. Remove the duplicates (they live in `utils.c`).

---

## 9. Dead code / debug leftovers

| File | Item |
|------|------|
| `philo.h` | `#include <stdio.h>` with `// TODO : A DELETE` |
| `start.c` | `// TODO : PROTEGER INIT MUTEX` |
| `output.c` | Color macros in `printf` (subject requires plain output) |
| `start_utils.c` | File is empty — can be removed |
| `output.c` | `ft_print_main_philo` — debug function, not needed |

---

## 10. Unused variable in `ft_routine` (start.c)

```c
int i;   // declared but never used
```

Remove it to satisfy `-Werror -Wunused-variable`.
