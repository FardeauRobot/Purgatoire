# `let`, `mut`, `ref`: Bindings, Immutable by Default

> **TL;DR.** `let` creates a *binding*: a name for a value. It's **immutable unless you write `mut`**, which is the opposite of C++, where you write `const` to opt out of mutation. `ref` makes a pattern bind by reference instead of by move. You'll rarely need it.

Related: [`../notions/fundamentals/OWNERSHIP.md`](../notions/fundamentals/OWNERSHIP.md) · [`CONST_STATIC.md`](CONST_STATIC.md) · [`MATCH.md`](MATCH.md)

---

## 1. C/C++ bridge

| C++98 | Rust | Note |
|---|---|---|
| `const int x = 5;` | `let x = 5;` | Immutable is the default |
| `int x = 5;` | `let mut x = 5;` | Mutation is opt-in |
| `int x;` (garbage) | `let x: i32;` then assign before use | Reading it before assignment doesn't compile |
| `const int N = 10;` (compile-time) | `const N: i32 = 10;` | That's `const`, not `let`. See [`CONST_STATIC`](CONST_STATIC.md) |

---

## 2. `let`: type inference, not `auto` roulette

```rust
fn main() {
    let a = 5;          // i32: the default integer type
    let b = 2.5;        // f64: the default float type
    let c: u8 = 255;    // annotation when you want a specific type
    let d = 10u64;      // or a literal suffix
    let (x, y) = (1, 2); // let takes a PATTERN, here a tuple
    println!("{a} {b} {c} {d} {x} {y}");
}
```

Inference runs across the whole function, not just the line (Hindley–Milner style). A `let v = Vec::new();` gets its element type from the first `v.push(3u8)` further down.

### Initialise before use, always

```rust
fn main() {
    let x: i32;
    let cond = true;
    if cond { x = 1; } else { x = 2; }  // deferred init is fine...
    println!("{x}");                    // ...as long as every path assigns
}
```

---

## 3. `mut`

```rust,compile_fail
fn main() {
    let count = 0;
    count += 1; // error[E0384]: cannot assign twice to immutable variable `count`
}
```

```rust
fn main() {
    let mut count = 0;
    count += 1;
    println!("{count}");
}
```

`mut` belongs to the **binding**, not the type. Moving a value into a `mut` binding makes it mutable:

```rust
fn main() {
    let v = vec![1, 2];
    let mut w = v;   // same Vec, new owner, now mutable
    w.push(3);
    println!("{w:?}");
}
```

The same word shows up in reference types: `&mut T` is a *mutable borrow*. That's a different position with a related meaning. See [`BORROWING`](../notions/fundamentals/BORROWING.md).

---

## 4. Shadowing: `let` again

```rust
fn main() {
    let input = "42";
    let input: i32 = input.parse().unwrap(); // new binding, new type, same name
    let input = input * 2;
    println!("{input}");
}
```

This is **not** mutation. Each `let` creates a fresh variable, and the old one is hidden. It's idiomatic for "same thing, next stage of processing", and it saves you from names like `input_str` / `input_num`. C++98 only lets you do this in an inner scope.

---

## 5. `ref`: bind by reference in a pattern

Patterns normally *move* the matched value into the new binding. `ref` borrows it instead:

```rust
fn main() {
    let name = Some(String::from("Ferris"));
    match name {
        Some(ref n) => println!("hi {n}"), // n: &String, name is NOT moved
        None => {}
    }
    println!("{name:?}"); // still usable
}
```

Since the 2018 edition, **match ergonomics** do this for you when you match on a reference, and that's the form you'll see in modern code:

```rust
fn main() {
    let name = Some(String::from("Ferris"));
    if let Some(n) = &name {      // matching on &Option<String> → n: &String
        println!("hi {n}");
    }
    println!("{name:?}");
}
```

> `ref` is on the left of `=`, in the pattern. `&` is on the right, in the expression. `let ref x = y;` and `let x = &y;` do the same thing.

---

## 6. Traps

| Error | Meaning | Fix |
|---|---|---|
| `E0384` cannot assign twice to immutable variable | Forgot `mut` | Add `mut`, or use shadowing if you're really making a new value |
| `E0381` used binding isn't initialized | A path doesn't assign it | Assign on every branch |
| warning: variable does not need to be mutable | `mut` you never used | Remove it. Rust's version of `-Wunused` |
| `unused variable: x` | Rust's `-Wunused` | Prefix with `_x` if it's intentional |
