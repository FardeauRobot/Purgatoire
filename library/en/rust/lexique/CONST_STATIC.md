# `const`, `static`: Compile-Time Values and Globals

> **TL;DR.** `const` is a **compile-time value** that gets copied into every place it's used, like a typed `#define`. `static` is **one fixed memory location** that lives for the whole program, like a C global. A `static mut` can only be touched inside `unsafe`. Rust makes mutable globals painful on purpose.

Related: [`LET_MUT.md`](LET_MUT.md) · [`UNSAFE_EXTERN.md`](UNSAFE_EXTERN.md) · [`../notions/fundamentals/LIFETIMES.md`](../notions/fundamentals/LIFETIMES.md)

---

## 1. C/C++ bridge

| C / C++98 | Rust | Note |
|---|---|---|
| `#define MAX 42` | `const MAX: u32 = 42;` | Typed and scoped |
| `const int N = 10;` (file scope) | `const N: i32 = 10;` | |
| `static int counter;` (a global) | `static COUNTER: AtomicU32 = ...;` | Thread-safe by construction |
| `int g_count;` (a mutable global) | `static mut G: i32 = 0;` + `unsafe` | Discouraged |
| `const char *msg = "hi";` | `static MSG: &str = "hi";` | |
| `static` local (keeps its value between calls) | A `static` inside the function | Same |
| `static` on a function (file-private) | Just don't write `pub` | Another meaning of C's `static` that Rust doesn't use |
| A `const` member function | `&self` | See [`IMPL_SELF`](IMPL_SELF.md). Not the same thing at all |

The type annotation is **mandatory** for both: `const X = 5;` doesn't compile.

---

## 2. `const`

```rust
const SECONDS_PER_DAY: u64 = 24 * 60 * 60;      // evaluated at compile time
const PRIMES: [u32; 5] = [2, 3, 5, 7, 11];

const fn cube(x: u64) -> u64 { x * x * x }      // callable at compile time
const BIG: u64 = cube(1000);

struct Grid;
impl Grid { const WIDTH: usize = 80; }          // an associated const

fn main() {
    let buf = [0u8; Grid::WIDTH];               // array sizes need a const
    println!("{SECONDS_PER_DAY} {} {BIG} {}", PRIMES[4], buf.len());
}
```

A `const` has **no single address**. Each use is a fresh copy, which is why a `const` can't be mutable, and why `&CONST` in two places may give two different pointers.

`const fn` is like C++11's `constexpr`: the same function can run at compile time or at run time.

---

## 3. `static`

```rust
use std::sync::atomic::{AtomicU32, Ordering};

static GREETING: &str = "hello";                 // one location, 'static lifetime
static CALLS: AtomicU32 = AtomicU32::new(0);     // a thread-safe mutable global

fn tracked() {
    CALLS.fetch_add(1, Ordering::Relaxed);       // no unsafe needed: atomics are Sync
}

fn main() {
    tracked();
    tracked();
    println!("{GREETING}: {} calls", CALLS.load(Ordering::Relaxed));
}
```

A `static` must be `Sync` (safe to share between threads), because any thread can reach it. That's why a plain `static X: Cell<i32>` doesn't compile, while an atomic or a `Mutex` does.

---

## 4. `static mut`: the escape hatch

```rust
static mut COUNTER: u32 = 0;

fn bump() {
    unsafe { COUNTER += 1; }   // you promise: no other thread is doing this right now
}

fn main() {
    bump();
    bump();
    let n = unsafe { COUNTER };   // copy the value out; don't take a reference
    println!("{n}");
}
```

Since the 2024 edition, taking a **reference** to a `static mut` (`&COUNTER`, and so `println!("{}", COUNTER)`, which borrows) is an error by default. Every access is a potential data race, and the compiler can't check it. Use an atomic, a `Mutex`, or `std::sync::OnceLock` instead.

---

## 5. `'static`: the lifetime, not the keyword

`'static` means *"valid for the whole program"*. String literals are `&'static str` because they live in the binary's read-only data section. A `T: 'static` bound means *"T contains no borrowed data that could expire"*. It does **not** mean T is a static variable. See [`LIFETIMES`](../notions/fundamentals/LIFETIMES.md).

---

## 6. Which one?

```
 Do I need ONE address that persists (a counter, a table, a lock)?
   ├─ no  → const
   └─ yes → static
             └─ mutable? → an atomic / Mutex / OnceLock inside the static
                           (static mut only for FFI or embedded, with care)
```
