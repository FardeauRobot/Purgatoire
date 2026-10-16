# `return` / `goto` — Leaving Early

> **TL;DR.** `return` ends the current function, hands a value back to the caller, and — crucially in C++ — **destructors of all locals run** on the way out. `goto` jumps to a label in the same function; it still exists in C++98, and you will essentially never write one.

Related: [`LOOPS.md`](LOOPS.md) · [`TRY_CATCH_THROW.md`](TRY_CATCH_THROW.md) · [`VOID.md`](VOID.md)

---

## 1. `return`

```cpp
int abs(int x) {
    if (x < 0)
        return -x;      // early return — done, exit now
    return x;
}

void log(const std::string& s) {
    if (s.empty())
        return;         // void functions: bare return
    std::cout << s << std::endl;
}
```

What actually happens at `return`:

```
   return expr;
      │
      ├─ 1. expr is evaluated (maybe copied into the return slot)
      ├─ 2. destructors of locals run, reverse order of creation
      └─ 3. control jumps back to the caller
```

Step 2 is the C++ difference: RAII means an early `return` **cannot leak** stack-managed resources. This is why the "single exit point" dogma from C matters less here.

## 2. Early-return style (guard clauses)

```cpp
int process(File& f) {
    if (!f.isOpen())   return ERR_CLOSED;   // guards first,
    if (f.empty())     return 0;            // flat and readable
    // happy path, unindented
    return doWork(f);
}
```

## 3. Traps

**Returning a reference/pointer to a local** — the local dies at `}`, the caller gets a corpse:

```cpp
std::string& bad() {
    std::string s = "hi";
    return s;            // 💥 dangling reference — UB
}
```

**Missing return on a path** — UB, not an error by default. `-Werror` + `-Wall` turns it fatal. The [`switch`](SWITCH.md)-over-enum with all cases + no default helps `-Wswitch` prove coverage.

**`return` in `main`**: `return 0;` = success. Reaching `main`'s `}` without a return is legal (implicit `return 0`) — but 42 style writes it explicitly.

## 4. `goto` — the museum piece

```cpp
    if (err1) goto cleanup;
    ...
cleanup:
    close(fd);
```

That's the **C** idiom for centralized cleanup. In C++ the destructor does this job automatically — RAII replaced `goto cleanup`. Remaining legal-but-rare use: breaking out of deeply nested loops, and even then a function + `return` is cleaner.

Rules if you ever meet one: same function only, cannot jump over the initialization of an object with a constructor (compiler error), and it's forbidden by most style guides including everything 42-adjacent.

> **Mental model:** `return` = exit through the front door, destructors shake your hand on the way out. `goto` = climb out a window; C++ mostly bricked them up.
