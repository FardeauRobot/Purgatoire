# `switch` / `case` / `default` / `break` — Multi-Way Branching

> **TL;DR.** `switch (expr)` jumps to the `case` label whose constant matches `expr`. Execution then runs *through* every following statement until it hits `break` or the end of the block — that's **fall-through**, and it's the one thing that bites everyone. `default:` is the fallback when nothing matched.

Related: [`ENUM.md`](ENUM.md) · [`BOOL.md`](BOOL.md) · [`MEMBER_FUNCTION_POINTERS.md`](../advanced/MEMBER_FUNCTION_POINTERS.md) (the alternative pattern)

---

## 1. The shape

```cpp
switch (level) {
    case 0:
        std::cout << "DEBUG"   << std::endl;
        break;
    case 1:
        std::cout << "INFO"    << std::endl;
        break;
    case 2:
        std::cout << "WARNING" << std::endl;
        break;
    case 3:
        std::cout << "ERROR"   << std::endl;
        break;
    default:
        std::cout << "unknown" << std::endl;
}
```

Read it as: *"evaluate `level` once, then jump to the matching `case`."* Without `break`, execution continues into the next case — there is no implicit barrier between cases.

---

## 2. What `expr` can be

The expression in `switch (expr)` must be of **integral type** (or implicitly convertible to one):

- `int`, `short`, `long`, `char`, `bool`
- `enum` values (they convert to `int`)
- anything with a user-defined conversion to an integral type

**Not allowed in C++98:**

- `std::string` — no integral conversion. This is why the Harl exercise uses parallel arrays of strings + a `for` loop, not a `switch`.
- `float` / `double` — equality on floats is meaningless.
- Class types without an integral conversion.

```cpp
switch (std::string("hi")) { ... }     // ❌ won't compile
```

The `case` labels themselves must be **integral constant expressions** — values the compiler can compute at compile time. Variables don't work:

```cpp
int x = 3;
switch (n) {
    case x: ...        // ❌ x is not a constant
    case 3: ...        // ✅
    case 1 + 2: ...    // ✅ constant arithmetic
}
```

---

## 3. Fall-through — the defining feature

`switch` is a glorified `goto`. Once execution lands on a matching `case`, it runs forward through *every statement* until either `break`, `return`, `throw`, or the closing `}`.

```cpp
switch (n) {
    case 1:
        std::cout << "one ";        // n==1 prints: "one two three "
    case 2:
        std::cout << "two ";        // n==2 prints: "two three "
    case 3:
        std::cout << "three ";      // n==3 prints: "three "
}
```

This is **intentional** in C++ — sometimes you want it:

```cpp
switch (level) {
    case 0:  debug();      // fall through
    case 1:  info();       // fall through
    case 2:  warning();    // fall through
    case 3:  error();      break;
    default: complainUnknown();
}
```

That's the **Harl filter** pattern (CPP01 ex06): "complain at this level **and every level above it**." Fall-through gives you cascading behavior for free.

But the same feature causes the most common bug in C — forgetting a `break` so two cases silently merge. Modern compilers warn (`-Wimplicit-fallthrough` in C++17+), but C++98 gives you nothing. **Discipline is the only safety net.**

---

## 4. `default` — the fallback

`default:` is the label hit when **no** `case` matched. It can appear anywhere in the switch (the compiler doesn't care), but by convention it goes last.

```cpp
switch (c) {
    case 'y': case 'Y': accept(); break;
    case 'n': case 'N': reject(); break;
    default:            askAgain();
}
```

If you omit `default` and nothing matches, the entire switch is a no-op.

Note: multiple `case` labels can stack on one statement (`case 'y': case 'Y':` above). Each label is just a jump target; the statement runs once when reached.

---

## 5. Implementation / hardware view — jump tables

A `switch` over **dense, contiguous** integer values usually compiles to a **jump table**: an array of code addresses indexed by the switch value. This is faster than a chain of `if`s because it's `O(1)`, not `O(n)`:

```
  switch (level)         ──┐
                            │   table lookup
                            ▼
              level →   [ &case_0, &case_1, &case_2, &case_3 ]
                            │
                            └──▶ jmp *table[level]
```

For **sparse** values (`case 1: case 7: case 100:`) or non-contiguous ranges, the compiler may fall back to a binary-search tree of comparisons, or even a chain of `if/else` if there are only a few cases. You can't force one strategy or the other — that's the optimizer's call.

Either way, `switch` is at least as fast as the equivalent `if/else` chain, and often faster. *Premature* optimization, sure — but it's a real win on hot paths.

---

## 6. Variable scope inside a switch

The whole switch body is **one scope**. Cases don't introduce new scopes by themselves. This means you can't declare a variable in one case and have it die before the next:

```cpp
switch (n) {
    case 1:
        int x = 10;            // ⚠ this declaration is visible in case 2!
        std::cout << x;
        break;
    case 2:
        std::cout << x;        // x exists but was never initialized for this path
        break;
}
```

This is a real footgun. The compiler may even **reject** the code if you skip past a non-trivial initialization. The fix: **wrap a case in braces** to give it its own scope:

```cpp
switch (n) {
    case 1: {
        int x = 10;
        std::cout << x;
        break;
    }
    case 2:
        std::cout << "no x here";
        break;
}
```

Treat the `{ }` around a multi-statement case as a habit — it costs nothing and avoids the trap.

---

## 7. `switch` vs `if/else` vs dispatch tables

Three tools, three sweet spots:

| Pattern | When to reach for it |
|---|---|
| `if/else` | Two or three cases; complex conditions (not just equality); ranges, predicates, mixed types. |
| `switch` | Many cases on a single integral/enum value; fall-through is desirable or harmless. |
| dispatch table | Mapping a value (any type) to a *function*. See [`MEMBER_FUNCTION_POINTERS.md`](../advanced/MEMBER_FUNCTION_POINTERS.md). |

The Harl exercise is interesting because **all three** apply at different points:

- **ex05** uses a dispatch table (string → method).
- **ex06** uses a dispatch table to *find* the level index, then a `switch` to drive the fall-through cascade.

---

## 8. Tips & tricks

### Pair `switch` with `enum`

The cleanest use of `switch` is over an `enum`. The compiler can check that you handled every enumerator — that's what your Makefile's `-Wswitch` flag does:

```cpp
enum Level { DEBUG, INFO, WARNING, ERROR };

switch (lv) {
    case DEBUG:   ...; break;
    case INFO:    ...; break;
    case WARNING: ...; break;
    // case ERROR missing → -Wswitch warns
}
```

Add `default:` only if you actually want a catch-all; omitting it makes `-Wswitch` louder.

### Explicit `// fallthrough` comments

When you intend to fall through, **say so in a comment**. Future-you (or your eval) will assume a missing `break` is a bug otherwise:

```cpp
case 0: debug();   // fallthrough
case 1: info();    break;
```

### `break` only breaks the nearest `switch` (or loop)

A `break` inside a `switch` inside a `for` exits the `switch`, **not** the loop. If you need to leave the loop from inside a case, use a flag or `return`/`goto`:

```cpp
for (int i = 0; i < n; ++i) {
    switch (a[i]) {
        case 0: break;           // exits the switch, NOT the for
        case 1: ...; break;
    }
    // execution continues here after the switch's break
}
```

---

## 9. Common errors

| Mistake | Result | Fix |
|---|---|---|
| Forgotten `break` | Two cases silently merge | Add `break;` or `// fallthrough` comment |
| Variable declared in a case without `{}` | Compile error or stray scope leak | Wrap the case in `{ }` |
| `switch (std::string)` | "switch quantity not an integer" | Use parallel arrays + loop or `if/else` |
| Non-constant `case` label | "expression is not an integral constant" | Use a literal or `const` integral |
| Duplicate `case` values | "duplicate case value" | Each integer can label at most one case |
| Missing `default` with `-Wswitch` on an enum | Warning about unhandled enumerator | Either add the case or add `default:` |

---

## 10. Visual summary

```
    switch (expr)
        │
        ▼  one evaluation
   ┌────────────────┐
   │ jump table /   │
   │ comparisons    │
   └────────────────┘
        │
        ├──▶ case A:  stmt; stmt;  ──▶ break? ─yes─▶ ┐
        │                              │             │
        │                              no            │
        │                              ▼             │
        ├──▶ case B:  stmt; stmt;  ──▶ break? ─yes─▶ │  (fall-through)
        │                                            │
        ├──▶ ...                                     │
        │                                            │
        └──▶ default: stmt;                          │
                                                     ▼
                                              exit switch
```

The arrows going *down* into the next case (when you forget `break`) are the fall-through paths. The arrow going *right* out of `break` is the only safe exit.

---

## 11. Practice questions

1. What's printed by `switch (1) { case 1: cout << "a"; case 2: cout << "b"; default: cout << "c"; }`?
2. Why doesn't `switch (s)` compile when `s` is a `std::string`?
3. You write `switch (lv)` over an `enum Level { DEBUG, INFO, WARNING, ERROR };` and handle only three. With `-Wswitch`, what happens?
4. Inside `for (...) switch (...) { case 0: break; }`, does the `break` exit the `for`?
5. Why does C++ require braces around a case that declares a local variable?

<details>
<summary>Answers</summary>

1. `"abc"` — no `break`s, so once `case 1` is hit, execution falls through every statement.
2. `std::string` has no implicit conversion to an integral type; `switch` only accepts integral expressions.
3. The compiler warns about the unhandled enumerator. With `-Werror` (which your Makefile sets), it becomes an error — the program won't compile until you handle it or add `default:`.
4. No — `break` exits the nearest enclosing `switch` or loop. The `switch` is nearer, so the `for` continues.
5. The entire `switch` body shares one scope. A bare declaration can be "jumped over" by control entering at a later case, leaving the variable in an undefined state. Braces give the case its own scope, so the variable can't leak to other cases.

</details>
