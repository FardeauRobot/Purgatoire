# `for` / `while` / `do` / `break` / `continue` — Loops

> **TL;DR.** Three loop shapes: `while` (test first), `do…while` (test after — body runs at least once), `for` (init + test + step in one line). `break` exits the loop; `continue` jumps to the next iteration. All of them compile down to the same thing: a conditional jump backwards.

Related: [`IF_ELSE.md`](IF_ELSE.md) · [`SWITCH.md`](SWITCH.md) · [`RETURN_GOTO.md`](RETURN_GOTO.md)

---

## 1. The three shapes

```cpp
while (cond) { body; }            // 0..n times

do { body; } while (cond);        // 1..n times — note the ;

for (init; cond; step) { body; }  // the counting idiom
```

```
   while:            do…while:          for:
   ┌─► cond ─false─►exit   ┌─► body        init
   │     │true             │     │           │
   │   body                │   cond ─false─►exit ◄── cond ─false─┐
   └─────┘                 └─true┘           │true               │
                                           body → step ──────────┘
```

**Choosing:** counting over a known range → `for`. Waiting on a condition → `while`. "Run once, then maybe again" (menus, retry prompts) → `do…while`.

## 2. The canonical C++98 `for`

```cpp
for (int i = 0; i < n; ++i)                 // index loop
for (std::string::size_type i = 0; i < s.size(); ++i)  // string-safe
for (std::vector<int>::iterator it = v.begin(); it != v.end(); ++it)  // STL
```

- The loop variable declared in `init` **dies with the loop** — its scope is the loop.
- Iterators compare with `!=`, not `<` — not every iterator supports `<`.
- No range-`for` (`for (x : v)`) — that's C++11. Spot it, flag it, rewrite it.

## 3. `break` and `continue`

```cpp
for (int i = 0; i < n; ++i) {
    if (skip(i))  continue;   // → jump to ++i
    if (found(i)) break;      // → jump past the loop
    process(i);
}
```

- Both act on the **innermost** enclosing loop only. No labeled break in C++ — exiting nested loops cleanly is a job for a function + `return`, or a flag.
- `break` inside a `switch` inside a loop exits the **switch**, not the loop. Classic trap.

## 4. Traps

**Infinite loop by typo:**

```cpp
for (unsigned i = n; i >= 0; --i)   // 💥 unsigned is ALWAYS >= 0
```

**Off-by-one:** `<` vs `<=`. Idiom: half-open ranges `[0, n)` — start at 0, test with `<`, and size, count, and index all agree.

**Mutating a container while iterating it** — invalidates iterators. See [`VECTOR.md`](../notions/containers/VECTOR.md) invalidation rules.

**`while (cin >> x)`** is the correct read-loop — it tests the *stream state* after the read, not before:

```cpp
int x;
while (std::cin >> x)   // stops on EOF or bad input
    total += x;
```

## 5. Tips

- Prefer `++i` to `i++` in loops — for iterators, `i++` copies. Free habit, occasional win.
- Hoist invariants: don't call `v.size()`-equivalents that are expensive inside the condition.
- A loop body longer than a screen wants to become a function.
