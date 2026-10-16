# 🔢 Types: Scalars, Strings, Arrays and Slices

> **TL;DR.** Every integer type states its exact width (`i32`, `u8`, `usize`…); there's no platform-dependent `int` or `long`. Overflow **panics in debug builds** and wraps in release. Text is always UTF-8 and comes in two forms: `String` (owned, growable, on the heap) and `&str` (a borrowed view). Arrays `[T; N]` have a fixed size; slices `&[T]` are views with a length.

Related: [`../../lexique/TYPE_AS.md`](../../lexique/TYPE_AS.md) · [`../../lexique/BOOL.md`](../../lexique/BOOL.md) · [`BORROWING.md`](BORROWING.md) · C++ side: [`STRING.md`](../../../cpp/notions/fundamentals/STRING.md)

---

## 1. Scalars

| Rust | C equivalent | Note |
|---|---|---|
| `i8` `i16` `i32` `i64` `i128` | `int8_t` … `int64_t` | `i32` is the default for integer literals |
| `u8` `u16` `u32` `u64` `u128` | `uint8_t` … | `u8` is a byte, **not** a character |
| `isize` / `usize` | `ssize_t` / `size_t` | Pointer-sized. **Indices and lengths are always `usize`** |
| `f32` / `f64` | `float` / `double` | `f64` is the default for float literals |
| `bool` | `bool` | See [`BOOL`](../../lexique/BOOL.md) |
| `char` | *(none)* | **4 bytes**, one Unicode scalar value. `'é'` and `'🦀'` are single `char`s |
| `()` | `void` | The unit type: a real value of size 0 |

```rust
fn main() {
    let big = 1_000_000u64;           // _ separators, type suffix
    let hex = 0xff;
    let bin = 0b1010;
    let byte = b'A';                  // u8 = 65
    let crab = '🦀';
    println!("{big} {hex} {bin} {byte} {crab} {}", std::mem::size_of::<char>());
    println!("{} {}", i32::MAX, u8::MIN);
}
```

---

## 2. Integer overflow: never silent UB

In C, signed overflow is UB. In Rust it's always defined, and you choose what happens:

```rust
fn main() {
    let x: u8 = 250;
    println!("{:?}", x.checked_add(10));    // None: detect it
    println!("{}", x.wrapping_add(10));     // 4: modular arithmetic, on purpose
    println!("{}", x.saturating_add(10));   // 255: clamp
    println!("{:?}", x.overflowing_add(10));// (4, true): the value plus a flag
    // let y = x + 10;   // debug build: panic "attempt to add with overflow"
    //                   // release build: wraps to 4
}
```

---

## 3. Strings: `String` vs `&str`

```
 let s: String = String::from("héllo");

   stack (String)            heap (UTF-8 bytes)
 ┌──────────┐              ┌──┬──┬──┬──┬──┬──┐
 │ ptr ─────┼─────────────►│h │é    │l │l │o │   6 bytes, 5 chars
 │ len  6   │              └──┴──┴──┴──┴──┴──┘
 │ cap  6   │                    ▲
 └──────────┘                    │
 let w: &str = &s[3..];  ────────┘   &str = (ptr, len): a borrowed view
```

| | `String` | `&str` |
|---|---|---|
| C++ analogue | `std::string` | `const char *` + length (C++17 `string_view`) |
| Owns its memory? | Yes, on the heap | No, it's a borrow |
| Growable? | Yes (`push_str`) | No |
| Literals | — | `"abc"` is a `&'static str` |
| As a parameter | Rarely | ✅ Take `&str`; a `&String` converts automatically |

```rust
fn greet(name: &str) -> String {          // borrow in, owned out
    format!("hello, {name}")              // format! = sprintf, returns a String
}

fn main() {
    let mut s = String::from("rust");
    s.push_str("acean");
    s.push('!');
    let lit: &str = "ferris";
    println!("{} / {}", greet(&s), greet(lit));
    println!("{} bytes, {} chars", "héllo".len(), "héllo".chars().count()); // 6 bytes, 5 chars
    let owned: String = lit.to_string();  // &str → String (allocates)
    let view: &str = &owned;              // String → &str (free)
    println!("{view}");
}
```

### No indexing with `s[i]`

```rust,compile_fail
fn main() {
    let s = String::from("héllo");
    let c = s[1];   // error[E0277]: the type `str` cannot be indexed by `{integer}`
}
```

UTF-8 characters take 1 to 4 bytes, so "the i-th character" isn't an O(1) operation, and Rust won't pretend it is. Use `s.chars().nth(i)`, `s.as_bytes()[i]` for bytes, or a byte range `&s[0..1]` (which panics if it cuts a character in half).

---

## 4. Arrays, `Vec`, slices

```rust
fn main() {
    let arr: [i32; 4] = [1, 2, 3, 4];     // fixed size, on the stack: like int arr[4]
    let zeros = [0u8; 16];                // 16 zero bytes
    let mut v: Vec<i32> = Vec::new();     // growable, on the heap: like std::vector
    v.push(10);
    v.extend([20, 30]);
    let v2 = vec![1, 2, 3];               // the vec! macro

    let slice: &[i32] = &arr[1..3];       // a view: [2, 3]
    println!("{arr:?} {} {v:?} {v2:?} {slice:?}", zeros.len());
    println!("{:?} {:?}", v.get(99), v.first()); // None, Some(10): no crash
    // v[99];                             // panics: index out of bounds (never a silent overrun)
}
```

| | `[T; N]` | `Vec<T>` | `&[T]` |
|---|---|---|---|
| Size | Fixed at compile time | Grows at runtime | Whatever it borrows |
| Where | Inline (stack or inside a struct) | Header on the stack, data on the heap | Pointer + length |
| C / C++ | `T arr[N]` | `std::vector<T>` | `(T *, size_t)` |

**Bounds checks:** `v[i]` checks the index and **panics** when it's out of range. You get a clean crash with a message instead of a silent heap overflow. Iterators (`for x in &v`) don't need the check, so loop that way.

---

## 5. Tuples

```rust
fn min_max(v: &[i32]) -> (i32, i32) {
    let mut lo = v[0];
    let mut hi = v[0];
    for &x in v { lo = lo.min(x); hi = hi.max(x); }
    (lo, hi)                              // several return values, no out-pointers
}

fn main() {
    let (lo, hi) = min_max(&[3, 9, 1]);
    let t = (1, "two", 3.0);
    println!("{lo} {hi} {} {}", t.1, t.2);
}
```

This replaces C's `void min_max(int *v, int *lo, int *hi)` with its output pointers.
