# 🦀 Rust — Two Doors

Everything Rust in this vault, written for someone who already knows **C and C++98**. Every page answers the same question: *"I know how C++ does this. What does Rust do instead, and why?"*

```
        "How does ownership work?"                      "What does `dyn` mean?"
                    │                                            │
                    ▼                                            ▼
        ┌─────────────────────┐                      ┌─────────────────────┐
        │   🧠 NOTIONS        │                      │   🔑 LEXIQUE        │
        │  concepts by theme  │◄────── zooms ────────│  one page per       │
        │  ownership, traits… │                      │  Rust keyword       │
        └─────────────────────┘                      └─────────────────────┘
```

| Door | Hub | When to open it |
|---|---|---|
| 🧠 [`notions/`](notions/INDEX.md) | Philosophy, ownership, borrowing, lifetimes, traits, errors, Cargo | You want to **understand a concept** in depth. |
| 🔑 [`lexique/`](lexique/INDEX.md) | Every strict Rust keyword + the [`GLOSSAIRE`](lexique/GLOSSAIRE.md) | A **word** is in your way. Look it up, then move on. |

> 🌟 First time here? Read [`notions/PHILOSOPHY.md`](notions/PHILOSOPHY.md), then [`OWNERSHIP`](notions/fundamentals/OWNERSHIP.md) → [`BORROWING`](notions/fundamentals/BORROWING.md) → [`LIFETIMES`](notions/fundamentals/LIFETIMES.md), in that order. Everything else in Rust sits on those three.

---

## 🧭 Suggested reading order

| Step | Read | Why now |
|---|---|---|
| 1 | [`PHILOSOPHY`](notions/PHILOSOPHY.md) | The mindset. Without it the borrow checker feels like an enemy. |
| 2 | [`CARGO`](notions/tooling/CARGO.md) | So you can run every snippet yourself (`cargo new sandbox`). |
| 3 | [`TYPES`](notions/fundamentals/TYPES.md) · [`LET_MUT`](lexique/LET_MUT.md) · [`FN`](lexique/FN.md) · [`CONTROL_FLOW`](lexique/CONTROL_FLOW.md) | The syntax you'll use on every line. |
| 4 | [`OWNERSHIP`](notions/fundamentals/OWNERSHIP.md) → [`BORROWING`](notions/fundamentals/BORROWING.md) → [`LIFETIMES`](notions/fundamentals/LIFETIMES.md) | The part of Rust with no C++ equivalent. |
| 5 | [`STRUCT`](lexique/STRUCT.md) · [`ENUM`](lexique/ENUM.md) · [`MATCH`](lexique/MATCH.md) · [`IMPL_SELF`](lexique/IMPL_SELF.md) | How you model data, without classes. |
| 6 | [`TRAITS_GENERICS`](notions/traits/TRAITS_GENERICS.md) · [`TRAIT_DYN`](lexique/TRAIT_DYN.md) | Rust's answer to templates *and* virtual. |
| 7 | [`ERROR_HANDLING`](notions/errors/ERROR_HANDLING.md) | Rust has no exceptions. This is what it uses instead. |

---

## ⚖️ C++98 ↔ Rust at a glance

| You write in C++98… | In Rust you write… | Page |
|---|---|---|
| `int x = 5;` (mutable) | `let mut x = 5;` (immutable unless `mut`) | [`LET_MUT`](lexique/LET_MUT.md) |
| `new` / `delete`, the Orthodox Canonical Form | Ownership + `Drop`; there's no OCF to write | [`OWNERSHIP`](notions/fundamentals/OWNERSHIP.md) |
| `const T&` / `T&` | `&T` / `&mut T`, checked at compile time | [`BORROWING`](notions/fundamentals/BORROWING.md) |
| `class` with methods | `struct` + `impl` | [`IMPL_SELF`](lexique/IMPL_SELF.md) |
| `virtual` + inheritance | `trait` + `dyn Trait` (no inheritance of data) | [`TRAIT_DYN`](lexique/TRAIT_DYN.md) |
| `template<typename T>` | Generics with trait bounds | [`TRAITS_GENERICS`](notions/traits/TRAITS_GENERICS.md) |
| `switch` | `match` (exhaustive, destructuring) | [`MATCH`](lexique/MATCH.md) |
| `try` / `catch` / `throw` | `Result<T, E>` + `?` | [`ERROR_HANDLING`](notions/errors/ERROR_HANDLING.md) |
| `NULL` | `Option<T>` | [`ENUM`](lexique/ENUM.md) |
| `namespace`, headers | `mod`, `use`, `pub` | [`MOD_USE_PUB`](lexique/MOD_USE_PUB.md) |
| Makefile + `-Wall -Wextra -Werror` | `cargo build` + `cargo clippy` | [`CARGO`](notions/tooling/CARGO.md) |
| valgrind | Mostly unnecessary: safe Rust can't leak by accident or use-after-free | [`OWNERSHIP`](notions/fundamentals/OWNERSHIP.md) |
