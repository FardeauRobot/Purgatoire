# `if` / `else` — Conditional Branching

> **TL;DR.** `if (condition)` runs the next statement/block when the condition converts to `true`. `else` catches the other path. Chain with `else if`. The condition is **any expression convertible to `bool`** — which is where the bugs hide.

Related: [`SWITCH.md`](SWITCH.md) · [`BOOL.md`](BOOL.md) · [`LOOPS.md`](LOOPS.md)

---

## 1. The shape

```cpp
if (hp <= 0) {
    std::cout << "dead" << std::endl;
} else if (hp < 20) {
    std::cout << "critical" << std::endl;
} else {
    std::cout << "fine" << std::endl;
}
```

```
        condition ──true──►  branch A
            │
          false
            ▼
        else if ──true──►  branch B
            │
          false
            ▼
          else  ─────────►  branch C
```

Exactly **one** branch runs. Conditions are tested top-down; the first `true` wins.

## 2. What counts as true

Any non-zero arithmetic value, any non-null pointer. `0`, `0.0`, and null pointers are `false`.

```cpp
int *p = ft_find(x);
if (p)          // idiomatic: "p is not null"
    use(*p);
```

## 3. The classic traps

**`=` instead of `==`** — assignment inside a condition compiles and is almost always a bug:

```cpp
if (x = 5)      // 💥 assigns 5, always true
if (x == 5)     // ✅ compares
```
`-Wall` catches it (`-Wparentheses`). Werror makes it fatal — one reason 42 mandates the flags.

**Dangling `else`** — `else` binds to the **nearest** `if`, not the one your indentation suggests:

```cpp
if (a)
    if (b) f();
else g();       // 💥 belongs to `if (b)` despite the indent
```
Cure: always braces when nesting.

**Missing braces + added line**:

```cpp
if (err)
    log(err);
    return 1;   // 💥 runs unconditionally — indentation lies
```

## 4. The ternary `?:`

An `if/else` that is an **expression** — it produces a value. Fine in C++ (only banned by the C norminette).

```cpp
int max = (a > b) ? a : b;
std::cout << (ok ? "yes" : "no");
```

Use it for a *choice of value*, never for a *choice of action* — side effects in a ternary read terribly.

## 5. Tips

- Test the error/edge case first and return early — keeps the happy path unindented.
- Comparing a variable to a constant? `if (x == 42)`. Multiple constants on one variable? Consider [`switch`](SWITCH.md).
- A long `else if` chain dispatching on *type* is a smell — that's what [`virtual`](VIRTUAL.md) is for.
