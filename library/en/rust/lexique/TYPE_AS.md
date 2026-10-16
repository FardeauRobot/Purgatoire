# `type`, `as`: Aliases and Explicit Casts

> **TL;DR.** `type Name = Existing;` creates an **alias**, the same as `typedef`, not a new type. `x as T` is Rust's only built-in cast, and it works on primitives and raw pointers. There are **no implicit numeric conversions at all**, so `as` shows up in places where C would have converted silently.

Related: [`../notions/fundamentals/TYPES.md`](../notions/fundamentals/TYPES.md) · [`STRUCT.md`](STRUCT.md) (newtypes)

---

## 1. `type`: an alias, like `typedef`

```rust
type Kilometers = u32;                      // an alias: interchangeable with u32
type Grid = Vec<Vec<char>>;
type Res<T> = Result<T, String>;            // aliases can be generic

fn parse(s: &str) -> Res<u32> {
    s.parse().map_err(|e| format!("{e}"))
}

fn main() {
    let d: Kilometers = 5;
    let x: u32 = d + 1;                     // no error: it's the same type
    let g: Grid = vec![vec!['.'; 3]; 2];
    println!("{x} {:?} {}", parse("12"), g.len());
}
```

| C / C++98 | Rust |
|---|---|
| `typedef unsigned int km;` | `type Km = u32;` |
| `typedef struct s_node t_node;` (the 42 C style) | Unnecessary: `struct Node` is already a name |
| A *distinct* type | A **newtype**: `struct Km(u32);`. See [`STRUCT`](STRUCT.md) |

`type` also appears **inside traits** as an *associated type*: `type Item;` in `Iterator`. See [`TRAITS_GENERICS`](../notions/traits/TRAITS_GENERICS.md).

---

## 2. No implicit conversions

```rust,compile_fail
fn main() {
    let a: i32 = 5;
    let b: i64 = a;          // error[E0308]: expected `i64`, found `i32`
    let c: f64 = a * 1.5;    // error: cannot multiply `i32` by `{float}`
}
```

C would widen `a` silently and promote to `double`. Rust makes you choose, because some of those conversions lose information.

---

## 3. `as`: the one cast operator

```rust
fn main() {
    let a: i32 = 300;
    println!("{}", a as i64);        // 300: widening, lossless
    println!("{}", a as u8);         // 44: TRUNCATION (300 mod 256), no warning
    println!("{}", -1i32 as u32);    // 4294967295: two's-complement reinterpretation
    println!("{}", 3.99f64 as i32);  // 3: truncates toward zero
    println!("{}", 1e20f64 as i32);  // 2147483647: SATURATES (in C this is UB!)
    println!("{}", f64::NAN as i32); // 0
    println!("{}", 'A' as u8);       // 65
    println!("{}", 97u8 as char);    // 'a' (only u8 → char is allowed)
    println!("{}", true as i32);     // 1
}
```

| Rust `as` | The closest C++98 cast |
|---|---|
| Numeric ↔ numeric | `static_cast`, but float → int **saturates** instead of UB |
| Raw pointer ↔ raw pointer | `reinterpret_cast` |
| Pointer ↔ integer (`p as usize`) | `reinterpret_cast<uintptr_t>` |
| Removing `const` | `ptr as *mut T`, the `const_cast` equivalent (raw pointers only) |
| `dynamic_cast` | None. Use `enum` + `match`, or `Any::downcast_ref` |

---

## 4. Prefer the checked conversions

`as` can silently truncate. For conversions that can fail, use the conversion traits:

```rust
fn main() {
    let big: i64 = 300;
    let small: Result<u8, _> = u8::try_from(big);   // Err: it doesn't fit
    let fine: i64 = i64::from(42i32);               // From: only lossless conversions exist
    let also: i64 = 42i32.into();                   // the same, through Into
    println!("{small:?} {fine} {also}");
}
```

| Want | Use |
|---|---|
| A lossless widening | `From` / `.into()` |
| A conversion that may not fit | `TryFrom` / `.try_into()` → `Result` |
| Deliberate truncation or wrapping | `as`, and add a comment saying so |

This connects to CPP06 `ScalarConverter`: in C++ you check the range by hand before `static_cast`. In Rust, `try_from` does that check for you.
