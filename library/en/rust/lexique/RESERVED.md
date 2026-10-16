# Reserved and Weak Keywords: Words Rust Keeps Back

> **TL;DR.** **Reserved** keywords do nothing today, but you can't use them as names, because the language might need them later (or used them long ago). **Weak** keywords only have a special meaning in certain positions, and are ordinary identifiers everywhere else. This is the counterpart of C++'s [`LEGACY_KEYWORDS`](../../cpp/lexique/LEGACY_KEYWORDS.md).

Related: [`INDEX.md`](INDEX.md) · [`TRAIT_DYN.md`](TRAIT_DYN.md)

---

## 1. Reserved keywords

| Keyword | Status | Why it's reserved |
|---|---|---|
| `abstract` | unused | Kept from early designs; there are no abstract classes. Use a `trait` |
| `become` | unused | Planned for **guaranteed tail calls** (`become f(x)` instead of `return f(x)`) |
| `box` | unused | Once meant heap allocation (`box 5`), which was removed. Use `Box::new(5)` |
| `do` | unused | There's no `do…while`. Use `loop { …; if !c { break } }` |
| `final` | unused | No inheritance, so nothing to seal |
| `gen` | reserved in **2024** | For generators (`gen { yield x }`), still being designed |
| `macro` | unused on stable | "Macros 2.0" (`macro m() {}`), still unstable |
| `override` | unused | Trait impls don't need to announce that they override |
| `priv` | unused | Private is the default; `pub` is the only thing you ever write |
| `try` | reserved in **2018** | For `try { … }` blocks (unstable). The `?` operator is what you use today |
| `typeof` | unused | Would give the type of an expression, like `decltype`. Not planned |
| `unsized` | unused | Early syntax for `?Sized`; the bound is what's used |
| `virtual` | unused | No `virtual`. Dynamic dispatch is `dyn Trait` |
| `yield` | unused | For generators and coroutines, together with `gen` |

> ⚠️ The C++ reflexes `virtual`, `override`, `final` and `abstract` all compile to *"expected identifier, found reserved keyword"*. That error means you're modelling inheritance. Step back and think about `trait` or `enum`.

### Using one as a name anyway: raw identifiers

```rust
fn r#try(x: i32) -> i32 { x + 1 }     // r# lets any keyword be used as an identifier

fn main() {
    let r#type = "raw";               // handy for FFI or serialisation field names
    println!("{} {}", r#try(1), r#type);
}
```

---

## 2. Weak keywords

Special only in context. You can still name a variable `union` or `raw`.

| Keyword | Special when… | Page |
|---|---|---|
| `union` | It starts an item: `union U { a: u32, b: f32 }`. Reading a field is `unsafe` | [`UNSAFE_EXTERN`](UNSAFE_EXTERN.md) · ⏳ `UNION.md` *(coming soon)* |
| `'static` | Used as a lifetime: `&'static str` | [`CONST_STATIC`](CONST_STATIC.md) §5 |
| `macro_rules` | Followed by `!`: `macro_rules! name { … }` defines a declarative macro | ⏳ `MACROS` *(coming soon)* |
| `raw` | In `&raw const x` / `&raw mut x`, which make a raw pointer without a reference | [`UNSAFE_EXTERN`](UNSAFE_EXTERN.md) §2 |
| `safe` | On an item inside an `unsafe extern` block (2024) | [`UNSAFE_EXTERN`](UNSAFE_EXTERN.md) §3 |

```rust
union IntOrFloat { i: u32, f: f32 }     // a C union: no tag

fn main() {
    let union = 3;                      // weak keyword: fine as a variable name
    let u = IntOrFloat { f: 1.0 };
    let bits = unsafe { u.i };          // reading a union field is unsafe
    println!("{union} {bits:#x}");      // 0x3f800000: the IEEE-754 bits of 1.0
}
```

---

## 3. Keywords by edition

| Edition | Added |
|---|---|
| 2015 | The base set. `async`, `await`, `dyn` and `try` weren't keywords yet |
| 2018 | `async`, `await`, `dyn` become strict; `try` reserved |
| 2021 | No new keywords; closures capture individual fields |
| 2024 | `gen` reserved; `unsafe extern`, `safe` and `unsafe` attributes (`#[unsafe(no_mangle)]`) |

Editions are picked per crate in `Cargo.toml`, and a 2015 crate still links with a 2024 crate. See [`CARGO`](../notions/tooling/CARGO.md).
