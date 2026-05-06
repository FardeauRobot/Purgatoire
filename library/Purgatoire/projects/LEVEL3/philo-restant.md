# RESTANT.md — Philosopher Project Completion Checklist

## 📋 Overview

This document outlines what remains to be done to complete the **Philosophers** project from 42 school. It categorizes work into **Critical Issues**, **Missing Implementations**, **Code Flaws**, and **Test Cases** based on analysis of your current code.

---

## 🔴 CRITICAL ISSUES (Must Fix)

### 1. **No Actual Dining Philosopher Logic**
**Current State:** The `ft_routine()` function is hardcoded with fake states that execute sequentially.

**What's Missing:**
- No timestamp tracking for when philosophers eat
- No death detection based on `time_to_die`
- No meal counting logic
- No real synchronization between philosophers

**Impact:** The program does nothing meaningful.

---

### 2. **No Synchronization/Mutexes for Forks**
**Current State:** No mutex initialization or fork management.

**What's Missing:**
```c
// Add to t_philo structure
pthread_mutex_t	*forks;  // Array of mutexes for each fork
pthread_mutex_t	print_lock;  // Protect printf output
pthread_mutex_t	death_lock;  // Protect death flag checking
```

**Impact:** Multiple philosophers can pick up the same fork simultaneously → race conditions.

---

### 3. **Threads Never Join**
**Current State:** Threads are created with `pthread_create()`, but there's no `pthread_join()`.

**Issue in [start.c](src/core/start.c#L33):**
```c
void	ft_start(t_philo *philo)
{
	ft_guests_arr(philo);
	// Missing: Wait for all threads to finish
}
```

**Required:**
```c
void	ft_start(t_philo *philo)
{
	ft_guests_arr(philo);
	
	// Wait for all threads to complete
	int i = 0;
	while (i < philo->nb_philo)
	{
		pthread_join(philo->guests[i].thread, NULL);
		i++;
	}
}
```

**Impact:** Program exits immediately without running any philosopher logic.

---

### 4. **Off-by-One Error in Thread Creation**
**Issue in [start.c](src/core/start.c#L38):**
```c
while (++i <= philo->nb_philo)  // ❌ WRONG: creates nb_philo + 1 threads
```

**Should be:**
```c
while (++i < philo->nb_philo)   // ✅ CORRECT: creates exactly nb_philo threads
```

**Impact:** One extra thread is created, breaking the philosopher count.

---

### 5. **No Timestamp Tracking**
**Current State:** No way to track when philosophers last ate or when they should die.

**Missing in [structures.h](includes/structures.h#L47):**
```c
typedef struct s_guest
{
	int			t_id;
	pthread_t	thread;
	int		    nb_meals;
	int		    time_last_meal;      // ❌ Should be time_t or long
	t_state		status;
	time_t		last_eat_time;       // ❌ MISSING: Track last meal timestamp
}	t_guest;

typedef struct s_philo
{
	// ... existing fields ...
	time_t		simulation_start;    // ❌ MISSING: Track when simulation started
	int		    someone_died;        // ❌ MISSING: Flag to stop all threads
}	t_philo;
```

**Impact:** Can't detect if a philosopher has starved to death.

---

### 6. **No Output Timestamp**
**Issue in [output.c](src/utils/output.c#L37):**
```c
void	ft_action_print(int t_id, t_state state)
{
	// ❌ Missing timestamp
	printf(GREEN" %d is eating\n"RESET, t_id);
}
```

**Should output:**
```
timestamp_ms philosopher_id action
```

**Example:**
```
1234 1 has taken a fork
1235 1 has taken a fork
1236 1 is eating
```

---

## ⚠️ MISSING IMPLEMENTATIONS

### 1. **Mutex Initialization & Destruction**
**Location:** [src/core/start.c](src/core/start.c)

Need to add:
```c
void	ft_init_mutexes(t_philo *philo)
{
	int i;
	
	philo->forks = malloc(sizeof(pthread_mutex_t) * philo->nb_philo);
	if (!philo->forks)
		ft_exit(philo, ERR_MALLOC, "Failed to allocate fork mutexes", ERR_MALLOC);
	
	i = -1;
	while (++i < philo->nb_philo)
		pthread_mutex_init(&philo->forks[i], NULL);
	
	pthread_mutex_init(&philo->print_lock, NULL);
	pthread_mutex_init(&philo->death_lock, NULL);
}

void	ft_destroy_mutexes(t_philo *philo)
{
	int i;
	
	i = -1;
	while (++i < philo->nb_philo)
		pthread_mutex_destroy(&philo->forks[i]);
	pthread_mutex_destroy(&philo->print_lock);
	pthread_mutex_destroy(&philo->death_lock);
	free(philo->forks);
}
```

---

### 2. **Reaper Thread (Death Monitor)**
**Current State:** [reaper.c](src/utils/reaper.c) is completely empty.

**What it should do:**
- Run as a separate thread that monitors all philosophers
- Check if any philosopher hasn't eaten within `time_to_die` milliseconds
- Print death message and set a stop flag
- Wake up all waiting threads so they exit cleanly

```c
void	*ft_reaper(void *ptr)
{
	t_philo	*philo;
	int		i;
	time_t	current_time;
	
	philo = (t_philo *)ptr;
	while (1)
	{
		i = -1;
		while (++i < philo->nb_philo)
		{
			pthread_mutex_lock(&philo->death_lock);
			current_time = ft_get_time_ms() - philo->simulation_start;
			if (current_time - philo->guests[i].last_eat_time > philo->time_to_die)
			{
				philo->someone_died = 1;
				pthread_mutex_unlock(&philo->death_lock);
				// Print death and return
				return (NULL);
			}
			pthread_mutex_unlock(&philo->death_lock);
		}
		usleep(100);
	}
	return (NULL);
}
```

---

### 3. **Time Utilities**
**Missing:** Functions to handle millisecond timing.

```c
// Add to src/utils/
time_t	ft_get_time_ms(void)
{
	struct timeval	tv;
	
	gettimeofday(&tv, NULL);
	return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

void	ft_sleep_ms(time_t ms)
{
	time_t	start;
	
	start = ft_get_time_ms();
	while (ft_get_time_ms() - start < ms)
		usleep(100);
}
```

---

### 4. **Protected Print Function**
**Current Issue:** Multiple threads printing simultaneously causes race conditions.

Need:
```c
void	ft_protected_print(t_philo *philo, int t_id, const char *msg)
{
	pthread_mutex_lock(&philo->print_lock);
	printf("%ld %d %s\n", 
		ft_get_time_ms() - philo->simulation_start, 
		t_id, 
		msg);
	pthread_mutex_unlock(&philo->print_lock);
}
```

---

## 🐛 CODE FLAWS

### Issue 1: Wrong Data Type for Timestamp
**File:** [structures.h](includes/structures.h#L50)

```c
int		time_last_meal;  // ❌ Should be time_t or long
```

Timestamps overflow on `int`. Change to:
```c
time_t	last_eat_time;
```

---

### Issue 2: Missing Function Prototypes
**File:** [defines.h](includes/defines.h)

Missing prototypes:
```c
void	*ft_reaper(void *ptr);
void	ft_init_mutexes(t_philo *philo);
void	ft_destroy_mutexes(t_philo *philo);
time_t	ft_get_time_ms(void);
void	ft_sleep_ms(time_t ms);
int		ft_isnum(char *str);
```

---

### Issue 3: No Memory Cleanup on Error
**File:** [main.c](src/main.c)

```c
int	main(int argc, char **argv)
{
	t_philo	philo;

	ft_bzero(&philo, sizeof(t_philo));
	if (ft_parsing(&philo, argc, argv) != SUCCESS)
		return (ERRN_PARSING);  // ❌ Need to free allocated resources
	ft_start(&philo);
	ft_destroy_mutexes(&philo);  // ❌ Missing
	return (EXIT_SUCCESS);
}
```

---

### Issue 4: No Shared Death Flag Check
**Impact:** Philosophers never know when to stop running.

Every philosopher should check `philo->someone_died` regularly:
```c
while (!philo->someone_died)
{
	// eat, think, sleep
}
```

---

## 📝 EDGE CASE TESTS

### Test 1: Single Philosopher (Deadlock)
```bash
./philo 1 800 200 200
```
**Expected:** Philosopher picks up 1 fork, waits 800ms, dies alone.
**What should happen:** Timestamp reports death at ~800ms.

---

### Test 2: Two Philosophers (Classic Deadlock)
```bash
./philo 2 600 200 200
```
**Expected:** Both should eat at least once before dying at 600ms.
**Verify:** Output shows both eating and both dying.

---

### Test 3: Three Philosophers (Should work)
```bash
./philo 3 500 100 100
```
**Expected:** All should survive and cycle eat/sleep/think.
**Duration:** Run for 5+ seconds without crashes.

---

### Test 4: Large Number (250 philosophers - Max)
```bash
./philo 250 800 200 200
```
**Issues to watch:**
- Memory usage (250 mutexes + threads)
- No segmentation faults
- Proper cleanup

---

### Test 5: With Meal Limit
```bash
./philo 5 800 200 200 7
```
**Expected:** After each philosopher eats 7 times, program exits cleanly.

---

### Test 6: With One More Meal (Program Hangs)
```bash
./philo 5 800 200 200 10000
```
**Expected:** Eventually someone dies before reaching 10,000 meals.

---

### Test 7: Very Fast Deaths
```bash
./philo 4 10 5 5
```
**Expected:** Philosophers die quickly (within ~10ms).
**Verify:** No segfaults, clean death reporting.

---

### Test 8: Very Long Timings
```bash
./philo 2 100000 50000 50000
```
**Expected:** No timeout, proper tracking of large millisecond values.

---

### Test 9: Invalid Arguments
```bash
./philo              # No args
./philo 5            # Too few
./philo 5 800 200 200 5 extra  # Too many
./philo -5 800 200 200  # Negative
./philo 251 800 200 200  # Too many philosophers
./philo 5 abc 200 200  # Non-numeric
```

---

### Test 10: Zero/Boundary Values
```bash
./philo 0 800 200 200   # Zero philosophers (should error)
./philo 5 0 200 200     # Zero time_to_die (should error or instant death)
./philo 5 800 0 200     # Zero eat_time
```

---

## 🚀 IMPLEMENTATION ROADMAP

### Phase 1: Data Structures & Initialization
- [ ] Add timestamp fields to `t_guest` and `t_philo`
- [ ] Initialize mutexes in `ft_init_mutexes()`
- [ ] Create `ft_get_time_ms()` utility
- [ ] Fix off-by-one error in thread creation

### Phase 2: Core Philosopher Logic
- [ ] Implement real `ft_routine()` with eat/think/sleep cycle
- [ ] Add fork pickup/putdown with mutex protection
- [ ] Implement death detection
- [ ] Add protected output with timestamps

### Phase 3: Monitoring & Cleanup
- [ ] Implement `ft_reaper()` thread
- [ ] Add `ft_destroy_mutexes()` cleanup
- [ ] Ensure proper thread joining
- [ ] Add meal counting logic

### Phase 4: Testing & Polish
- [ ] Run all edge case tests
- [ ] Fix race conditions
- [ ] Handle memory leaks
- [ ] Validate against 42 norm

---

## ✅ VALIDATION CHECKLIST

Before submission verify:

- [ ] No data races (use ThreadSanitizer: `gcc -fsanitize=thread`)
- [ ] No memory leaks (use Valgrind or LeakSanitizer)
- [ ] Philosophers don't eat with only 1 fork
- [ ] Death is detected within a reasonable margin
- [ ] Output timestamps are monotonically increasing
- [ ] Single philosopher dies alone
- [ ] Two philosophers can't both eat infinitely
- [ ] All threads properly joined and cleaned up
- [ ] No undefined behavior
- [ ] Complies with 42 norm

---

## 📚 Key Resources from FUNCTIONS.md

Your project already documents:
- ✅ `memset()` / `ft_bzero()` — Zeroing memory
- ✅ `printf()` — Output (but needs timestamps!)
- ✅ `malloc()` / `free()` — Memory management
- ✅ `pthread_create()` — Thread creation (but missing join!)
- ✅ `pthread_mutex_*` — Protection (not yet used!)
- ✅ `gettimeofday()` — Get current time
- ✅ `usleep()` — Sleep in microseconds

**Priority:** Study mutex operations and `gettimeofday()` if unfamiliar.

---

## 🎯 Summary

Your project is **30% complete**:
- ✅ Argument parsing
- ✅ Error handling framework
- ✅ Thread creation scaffolding
- ❌ Actual philosopher logic (broken routine)
- ❌ Synchronization (no mutexes)
- ❌ Death detection (no reaper)
- ❌ Proper timing (no tracking)
- ❌ Thread joining (crashes on exit)

**Estimate to completion:** 4-6 hours of focused work following the roadmap above.
