# `true`, `false`: A Real Boolean

> **TL;DR.** `bool` is its own 1-byte type with exactly two values. It's **not** an integer: no `if (n)`, no `while (ptr)`, no `bool` + `int` arithmetic. Conversions are one-way and explicit: `b as i32` works, `n as bool` doesn't.

Related: [`CONTROL_FLOW.md`](CONTROL_FLOW.md) · [`TYPE_AS.md`](TYPE_AS.md)

---

## 1. C/C++ bridge

| C / C++98 | Rust |
|---|---|
| C: `int`, any nonzero is true | `bool`: only `true` / `false` |
| `if (count)` | `if count != 0` |
| `if (!ptr)` | `if opt.is_none()` (there's no null) |
| `bool b = 42;` → true | ❌ `let b: bool = 42;` doesn't compile |
| `int n = true;` → 1 | `let n = true as i32;` → 1 |
| `int n = (bool)x;` | `let b = x != 0;` |

---

## 2. Operators

```rust
fn main() {
    let a = true;
    let b = false;
    println!("{} {} {}", a && b, a || b, !a);     // short-circuit, like C
    println!("{} {} {}", a & b, a | b, a ^ b);    // non-short-circuit, and XOR
    println!("{}", 3 > 2 && "a" < "b");
    println!("{}", a as u8 + b as u8);             // 1: explicit casts
    let flag: bool = "true".parse().unwrap();      // from a string
    println!("{flag} {}", flag.then_some(7).unwrap_or(0));
}
```

`!` is logical NOT for `bool` **and** bitwise NOT for integers (C's `~`). Rust has no `~` operator.

---

## 3. Useful methods

```rust
fn main() {
    let adult = 20 >= 18;
    let label = adult.then(|| "adult");   // Option<&str>: Some if true
    println!("{label:?}");
    let b = true;
    println!("{}", std::mem::size_of::<bool>()); // 1
    println!("{}", u8::from(b));                 // lossless conversion to an int
}
```

---

## 4. Under the hood

`bool` is one byte and must hold **exactly** `0x00` or `0x01`. Any other bit pattern is UB, which you can only produce with `unsafe`. The compiler uses the 254 other values as a **niche**, so `Option<bool>` is still a single byte. See [`ENUM`](ENUM.md) §4.
