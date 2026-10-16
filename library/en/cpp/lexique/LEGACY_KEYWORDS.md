# `auto` / `register` / `asm` / `export` — The C++98 Fossils

> **TL;DR.** Four keywords that exist in C++98 but that you will never usefully write. They matter for one reason: **`auto` means something completely different in C++98 than in modern C++**, and evaluators love that question.

Related: [`STATIC.md`](STATIC.md) · [`EXTERN.md`](EXTERN.md) · [`TYPEDEF.md`](TYPEDEF.md)

---

## 1. `auto` — the trap question

In **C++98**, `auto` is a *storage-class specifier* meaning "automatic storage duration" — i.e. a plain stack local. Which is already the default, making it 100% redundant:

```cpp
auto int x = 5;   // C++98: same as `int x = 5;` — legal, pointless
```

In **C++11+**, the keyword was recycled for type deduction:

```cpp
auto x = 5;       // C++11: compiler deduces int — FORBIDDEN at 42
```

> **The eval answer:** "`auto` exists in C++98 but only as a storage specifier; type-deduction `auto` is C++11, so it's banned by the subject." Same C++98/C++11 recycling story as [`USING.md`](USING.md) aliases.

## 2. `register` — a hint nobody listens to

```cpp
register int i;   // "please keep i in a CPU register"
```

Modern compilers ignore the hint entirely — their register allocators are better than yours. Only visible effect in C++98: you can't take the address of a `register` variable... except compilers barely enforce that either. Deprecated in C++11, removed in C++17.

## 3. `asm` — inline assembly

```cpp
asm("nop");       // inject raw assembly
```

Standard in name only — syntax and behavior are implementation-defined (`asm volatile (...)` GCC-style, etc.). Kernel and driver territory. At 42: never.

## 4. `export` — the keyword that never worked

Meant to let template definitions live in `.cpp` files instead of headers. Exactly **one** compiler frontend ever implemented it (EDG); everyone else refused. Removed in C++11. Templates stay in headers — see [`TEMPLATE.md`](TEMPLATE.md) for why (monomorphization needs the source).

---

## Scorecard

| Keyword | C++98 meaning | Fate | Ever write it? |
|---|---|---|---|
| `auto` | automatic storage (default anyway) | recycled in C++11 for type deduction | ❌ — but know the trap |
| `register` | register hint | removed C++17 | ❌ |
| `asm` | inline assembly | still around, implementation-defined | ❌ |
| `export` | out-of-header templates | never implemented, removed C++11 | ❌ |
