# `enum`: Tagged Unions, and the End of NULL

> **TL;DR.** A Rust `enum` is a C `enum` whose variants can **carry data**. It's a tagged union, and the compiler checks the tag for you. `Option<T>` and `Result<T, E>` are plain enums in the standard library, and together they replace both `NULL` and exceptions.

Related: [`MATCH.md`](MATCH.md) · [`STRUCT.md`](STRUCT.md) · [`../notions/errors/ERROR_HANDLING.md`](../notions/errors/ERROR_HANDLING.md)

---

## 1. C/C++ bridge

The C way to say "this is either a circle or a rectangle":

```c
enum kind { CIRCLE, RECT };
struct shape {
    enum kind kind;                     // the tag, which YOU must keep in sync
    union { double r; struct { double w, h; } rect; } u;
};
// read s.u.r while kind == RECT → garbage, no error
```

The Rust way:

```rust
enum Shape {
    Circle(f64),
    Rect { w: f64, h: f64 },
}

fn area(s: &Shape) -> f64 {
    match s {                         // you can't reach the data without checking the tag
        Shape::Circle(r) => 3.14159 * r * r,
        Shape::Rect { w, h } => w * h,
    }
}

fn main() {
    println!("{}", area(&Shape::Rect { w: 2.0, h: 3.0 }));
}
```

| C / C++98 | Rust |
|---|---|
| `enum` = named integers, converts to `int` silently | Its own type; `as i32` only for field-less enums |
| Tag + `union`, kept in sync by hand | One type; the compiler keeps the tag and you *must* check it |
| `NULL` | `Option<T>`: `Some(v)` or `None` |
| Exceptions or an error code | `Result<T, E>`: `Ok(v)` or `Err(e)` |
| A class hierarchy for a closed set of cases | Usually an `enum` + `match` |

---

## 2. Three kinds of variant, all in one enum

```rust
#[derive(Debug)]
enum Message {
    Quit,                       // unit
    Move { x: i32, y: i32 },    // struct-like
    Write(String),              // tuple-like
}

impl Message {                  // enums get impl blocks too
    fn is_quit(&self) -> bool { matches!(self, Message::Quit) }
}

fn main() {
    let msgs = vec![Message::Write("hi".into()), Message::Move { x: 1, y: 2 }, Message::Quit];
    for m in &msgs {
        println!("{m:?} quit? {}", m.is_quit());
    }
}
```

C-like enums still work, and can have explicit values:

```rust
#[derive(Debug, Clone, Copy, PartialEq)]
enum Level { Debug = 0, Info = 1, Warning = 2, Error = 3 }  // Harl, CPP01

fn main() {
    let l = Level::Warning;
    println!("{:?} = {}", l, l as i32);
}
```

---

## 3. `Option<T>`: the billion-dollar fix

Tony Hoare called the null reference his "billion-dollar mistake". Rust doesn't have one. A value that might be missing has type `Option<T>`:

```rust
fn find(v: &[i32], target: i32) -> Option<usize> {
    for (i, x) in v.iter().enumerate() {
        if *x == target { return Some(i); }
    }
    None
}

fn main() {
    let v = [4, 8, 15];
    match find(&v, 8) {
        Some(i) => println!("found at {i}"),
        None => println!("absent"),
    }
    let i = find(&v, 99).unwrap_or(usize::MAX); // provide a default
    println!("{i}");
}
```

`Option<usize>` and `usize` are **different types**. You can't do arithmetic on an `Option` without unwrapping it first, so "forgot to check for NULL" can't compile.

---

## 4. Under the hood

```
 Shape::Rect { w, h }        size = tag + largest variant (+ padding)
 ┌──────┬─────────┬─────────┐
 │ tag  │    w    │    h    │   8 + 8 + 8 = 24 bytes
 └──────┴─────────┴─────────┘
```

**Niche optimisation:** when a type has impossible bit patterns, Rust stores the tag in them. A reference can never be null, so `Option<&T>` is **exactly one pointer** wide and `None` is stored as `0`. That's the C null pointer, except the type system makes you check it.

```rust
fn main() {
    use std::mem::size_of;
    println!("{} {}", size_of::<&u8>(), size_of::<Option<&u8>>());       // 8 8
    println!("{} {}", size_of::<Box<u8>>(), size_of::<Option<Box<u8>>>()); // 8 8
}
```

---

## 5. Traps

| Symptom | Cause |
|---|---|
| `non-exhaustive patterns` | A `match` is missing a variant. That's the feature working |
| `expected i32, found Option<i32>` | Unwrap it: `match`, `if let`, `?`, `unwrap_or` |
| Writing `Shape::Circle` over and over | `use Shape::*;` inside the function, or `Self::Circle` inside `impl` |
| Calling `.unwrap()` everywhere | You've brought NULL back as a crash. See [`ERROR_HANDLING`](../notions/errors/ERROR_HANDLING.md) |
