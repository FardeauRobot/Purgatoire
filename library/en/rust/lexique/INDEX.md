# 🔑 LEXIQUE — Every Rust Keyword, One Click Away

One page per keyword (or tightly-related group): what it does, how it maps to C/C++98, what the compiler does with it, and the traps. **All strict Rust keywords are listed below.** Find yours in the A→Z table, click, read, and get back to work.

> Looking for a *concept* rather than a keyword (ownership, borrow, trait object, monomorphisation…)? That's the [`GLOSSAIRE.md`](GLOSSAIRE.md).
> The zoomed-out topic pages live in [`../notions/`](../notions/INDEX.md).

---

## 🔤 A→Z — strict keywords

| Keyword | Page | | Keyword | Page |
|---|---|---|---|---|
| `as` | [`TYPE_AS.md`](TYPE_AS.md) | | `loop` | [`CONTROL_FLOW.md`](CONTROL_FLOW.md) |
| `async` | ⏳ `ASYNC_AWAIT.md` *(coming soon)* | | `match` | [`MATCH.md`](MATCH.md) |
| `await` | ⏳ `ASYNC_AWAIT.md` *(coming soon)* | | `mod` | [`MOD_USE_PUB.md`](MOD_USE_PUB.md) |
| `break` | [`CONTROL_FLOW.md`](CONTROL_FLOW.md) | | `move` | [`MOVE.md`](MOVE.md) |
| `const` | [`CONST_STATIC.md`](CONST_STATIC.md) | | `mut` | [`LET_MUT.md`](LET_MUT.md) |
| `continue` | [`CONTROL_FLOW.md`](CONTROL_FLOW.md) | | `pub` | [`MOD_USE_PUB.md`](MOD_USE_PUB.md) |
| `crate` | [`MOD_USE_PUB.md`](MOD_USE_PUB.md) | | `ref` | [`LET_MUT.md`](LET_MUT.md) |
| `dyn` | [`TRAIT_DYN.md`](TRAIT_DYN.md) | | `return` | [`FN.md`](FN.md) |
| `else` | [`CONTROL_FLOW.md`](CONTROL_FLOW.md) | | `self` | [`IMPL_SELF.md`](IMPL_SELF.md) |
| `enum` | [`ENUM.md`](ENUM.md) | | `Self` | [`IMPL_SELF.md`](IMPL_SELF.md) |
| `extern` | [`UNSAFE_EXTERN.md`](UNSAFE_EXTERN.md) | | `static` | [`CONST_STATIC.md`](CONST_STATIC.md) |
| `false` | [`BOOL.md`](BOOL.md) | | `struct` | [`STRUCT.md`](STRUCT.md) |
| `fn` | [`FN.md`](FN.md) | | `super` | [`MOD_USE_PUB.md`](MOD_USE_PUB.md) |
| `for` | [`CONTROL_FLOW.md`](CONTROL_FLOW.md) | | `trait` | [`TRAIT_DYN.md`](TRAIT_DYN.md) |
| `if` | [`CONTROL_FLOW.md`](CONTROL_FLOW.md) | | `true` | [`BOOL.md`](BOOL.md) |
| `impl` | [`IMPL_SELF.md`](IMPL_SELF.md) | | `type` | [`TYPE_AS.md`](TYPE_AS.md) |
| `in` | [`CONTROL_FLOW.md`](CONTROL_FLOW.md) | | `unsafe` | [`UNSAFE_EXTERN.md`](UNSAFE_EXTERN.md) |
| `let` | [`LET_MUT.md`](LET_MUT.md) | | `use` | [`MOD_USE_PUB.md`](MOD_USE_PUB.md) |
| `where` | [`TRAIT_DYN.md`](TRAIT_DYN.md) | | `while` | [`CONTROL_FLOW.md`](CONTROL_FLOW.md) |

**Reserved** (they do nothing yet but you can't use them as names): `abstract` `become` `box` `do` `final` `gen` `macro` `override` `priv` `try` `typeof` `unsized` `virtual` `yield`. **Weak** (keywords only in certain positions): `union` `'static` `macro_rules` `raw` `safe`. All of them are covered in [`RESERVED.md`](RESERVED.md).

> ⚠️ `virtual`, `override` and `final` are reserved but **mean nothing**. If you reach for them you're thinking in C++ inheritance. Read [`TRAIT_DYN.md`](TRAIT_DYN.md).

---

## 🗂️ By theme

| Theme | Pages |
|---|---|
| **Bindings & values** | [`LET_MUT`](LET_MUT.md) · [`BOOL`](BOOL.md) · [`CONST_STATIC`](CONST_STATIC.md) · [`TYPE_AS`](TYPE_AS.md) |
| **Control flow** | [`CONTROL_FLOW`](CONTROL_FLOW.md) · [`MATCH`](MATCH.md) · [`FN`](FN.md) |
| **Data modelling** | [`STRUCT`](STRUCT.md) · [`ENUM`](ENUM.md) · [`IMPL_SELF`](IMPL_SELF.md) |
| **Polymorphism** | [`TRAIT_DYN`](TRAIT_DYN.md) |
| **Ownership** | [`MOVE`](MOVE.md) · `ref` in [`LET_MUT`](LET_MUT.md) |
| **Modules & visibility** | [`MOD_USE_PUB`](MOD_USE_PUB.md) |
| **Escape hatches** | [`UNSAFE_EXTERN`](UNSAFE_EXTERN.md) |
| **Not (yet) keywords** | [`RESERVED`](RESERVED.md) |

---

## 🗺️ Keyword ↔ notion crossroads

| Reading the notion… | Zoom into these keywords |
|---|---|
| [`fundamentals/OWNERSHIP.md`](../notions/fundamentals/OWNERSHIP.md) | [`LET_MUT`](LET_MUT.md) · [`MOVE`](MOVE.md) · [`STRUCT`](STRUCT.md) |
| [`fundamentals/BORROWING.md`](../notions/fundamentals/BORROWING.md) | [`LET_MUT`](LET_MUT.md) (`ref`, `mut`) · [`IMPL_SELF`](IMPL_SELF.md) (`&self` / `&mut self`) |
| [`fundamentals/LIFETIMES.md`](../notions/fundamentals/LIFETIMES.md) | [`FN`](FN.md) · [`CONST_STATIC`](CONST_STATIC.md) (`'static`) |
| [`fundamentals/TYPES.md`](../notions/fundamentals/TYPES.md) | [`TYPE_AS`](TYPE_AS.md) · [`BOOL`](BOOL.md) |
| [`traits/TRAITS_GENERICS.md`](../notions/traits/TRAITS_GENERICS.md) | [`TRAIT_DYN`](TRAIT_DYN.md) · [`IMPL_SELF`](IMPL_SELF.md) |
| [`errors/ERROR_HANDLING.md`](../notions/errors/ERROR_HANDLING.md) | [`ENUM`](ENUM.md) · [`MATCH`](MATCH.md) · [`FN`](FN.md) |
| [`tooling/CARGO.md`](../notions/tooling/CARGO.md) | [`MOD_USE_PUB`](MOD_USE_PUB.md) · [`UNSAFE_EXTERN`](UNSAFE_EXTERN.md) |

---

## 📐 What each page covers

Every keyword page has the same skeleton, so you always know where to look:

1. **TL;DR**: one sentence to anchor the rest.
2. **C/C++ bridge**: the closest thing you already know, and where the analogy breaks.
3. **What it does**: the language-level meaning, with snippets that compile (edition 2024).
4. **Under the hood**: what the compiler emits, where the data lives.
5. **Traps**: the compiler errors you'll actually hit, and what they mean.
