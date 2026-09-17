# 🦀 The Philosophy of Rust

> **TL;DR.** Rust makes a bet: most memory bugs in C and C++ come from *who owns this memory, and who else is looking at it right now?* If the compiler can answer that question for every value, you get C-level speed **without** a garbage collector **and** without use-after-free, double free, data races or dangling pointers. The price is that you have to convince the compiler, up front.

Related: [`OWNERSHIP`](fundamentals/OWNERSHIP.md) · [`BORROWING`](fundamentals/BORROWING.md) · [`../lexique/UNSAFE_EXTERN.md`](../lexique/UNSAFE_EXTERN.md)

---

## 1. Why it exists

Rust started in 2006 as a side project by Graydon Hoare at Mozilla. Mozilla sponsored it from 2009, and 1.0 shipped in May 2015. The problem it attacked was Firefox: millions of lines of C++, and security bugs that kept coming from the same few categories.

About **70% of serious security bugs** in large C/C++ codebases (the figure Microsoft and Google's Chromium team both reported) are memory-safety bugs:

| Bug class | You've met it in… | What Rust does |
|---|---|---|
| Use-after-free | `delete p; p->x;` | Can't compile: the value has been moved or dropped |
| Double free | Copy a class with a raw pointer and no OCF | A value has one owner, so it's dropped exactly once |
| Dangling reference | Returning `&local` | Rejected by lifetime checking |
| Buffer overflow | `tab[i]` with `i` out of range | Indexing is bounds-checked and panics |
| Data race | Philosophers without a mutex | Rejected: `Send`/`Sync` + borrowing rules |
| Null dereference | `ptr->x` on `NULL` | There's no null. Use `Option<T>` and you must check it |
| Uninitialised read | `int x; printf("%d", x);` | Can't compile: every variable must be initialised before use |

What *isn't* prevented: logic bugs, deadlocks, integer overflow in release builds (it wraps; it's checked in debug), leaks through `Rc` cycles or `std::mem::forget`, and anything inside an `unsafe` block you got wrong.

---

## 2. The four pillars

```
   ┌──────────────────────┐  ┌──────────────────────┐  ┌──────────────────────┐  ┌──────────────────────┐
   │ 1. OWNERSHIP         │  │ 2. ZERO-COST         │  │ 3. EXPLICIT          │  │ 4. THE COMPILER      │
   │    one owner,        │  │    abstractions      │  │    over implicit     │  │    IS YOUR MENTOR    │
   │    freed at scope end│  │    compile to C speed│  │    no hidden magic   │  │    fight it early    │
   └──────────────────────┘  └──────────────────────┘  └──────────────────────┘  └──────────────────────┘
```

### Pillar 1: Ownership replaces both `delete` and the garbage collector

In C you `free` by hand. Java and Go have a GC that frees for you at runtime. C++ has RAII: the destructor frees at the end of scope. Rust takes RAII and makes it **mandatory and checked**. Every value has exactly one owner, and when the owner goes out of scope the value is dropped. The compiler tracks this statically, so there's no runtime cost.

This is the Orthodox Canonical Form problem solved at the language level: you never write a copy constructor to avoid a double free, because the default isn't copying at all, it's *moving*.

### Pillar 2: Zero-cost abstractions

Bjarne Stroustrup's rule, which Rust inherits: *"What you don't use, you don't pay for. What you do use, you couldn't hand-code any better."* An iterator chain like `v.iter().map(f).filter(g).sum()` compiles to the same loop you would have written in C. Generics are monomorphised like C++ templates, so there's no boxing and no vtable unless you ask for one with `dyn`.

### Pillar 3: Explicit over implicit

C++ does a lot behind your back: implicit conversions, implicit copy constructors, implicit `this`, exceptions that can come out of any call. Rust makes you write it:

| Implicit in C++ | Explicit in Rust |
|---|---|
| `int` → `long`, `double` → `int` | `x as i64`. No implicit numeric conversion at all |
| Copy constructor called silently | `.clone()` spelled out; only trivial `Copy` types copy implicitly |
| Any function may throw | Failure is in the return type: `Result<T, E>` |
| `this` is implicit | `self` is the first parameter, written out |
| Mutable by default | Immutable by default: `mut` to opt in |

### Pillar 4: The compiler is a mentor, not a gatekeeper

`-Wall -Wextra -Werror` is the 42 way of saying *"let the compiler catch it."* Rust takes that as far as it goes. Its errors are long, have error codes (`rustc --explain E0382`), and usually suggest the fix.

The classic arc for a C++ programmer:

```
 week 1: "the borrow checker hates me"
 week 3: "oh, that WAS a bug"
 month 2: you start designing so that ownership is obvious. Your C++ gets better too.
```

---

## 3. Safe and unsafe Rust

Rust is really two languages in one:

```
 ┌─────────────────────────────────────────────┐
 │  SAFE RUST: the compiler proves memory      │
 │  safety. 99% of your code.                  │
 │   ┌─────────────────────────────────────┐   │
 │   │ unsafe { ... }: YOU promise the     │   │
 │   │ invariants hold. Raw pointers, FFI, │   │
 │   │ hardware. Small and audited.        │   │
 │   └─────────────────────────────────────┘   │
 └─────────────────────────────────────────────┘
```

`Vec`, `String` and `HashMap` are built with `unsafe` inside and expose a safe API outside. That's the pattern: put the danger in a small box, check it hard, and wrap it. See [`UNSAFE_EXTERN.md`](../lexique/UNSAFE_EXTERN.md).

---

## 4. What Rust is used for

- **Systems**: parts of the Linux kernel (drivers, since 6.1), Android, the Windows kernel, Firefox.
- **Infra and cloud**: AWS Firecracker (the microVMs behind Lambda), Cloudflare's proxies, Discord's backend.
- **CLI tools**: `ripgrep`, `fd`, `bat`, `uv`. Rust is a good fit for small, fast binaries.
- **WebAssembly**: Rust is one of the best-supported languages for compiling to Wasm.
- **Embedded**: `no_std` Rust runs on microcontrollers with no OS.

---

## 5. The mindset shift, in one table

| C++ reflex | Rust reflex |
|---|---|
| "Who deletes this?" | "Who **owns** this?" The owner drops it; nobody else needs to think about it. |
| "Pass by `const&` to avoid a copy" | Pass `&T`. Same idea, but the compiler checks it can't dangle. |
| "Make a class hierarchy" | Make an `enum` if the set of cases is closed, a `trait` if it's open. |
| "Throw an exception" | Return `Result`, propagate it with `?`. |
| "Check for `NULL`" | The type is `Option<T>`, so you can't forget. |
| "Silence the warning" | Read the error. It's usually right. |

> 🔁 Re-read this page after [`BORROWING.md`](fundamentals/BORROWING.md). Pillar 4 will make a lot more sense by then.
