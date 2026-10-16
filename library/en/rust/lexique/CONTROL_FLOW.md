# `if` `else` `loop` `while` `for` `in` `break` `continue`: Control Flow

> **TL;DR.** The same words as C, with three differences: conditions must be `bool` (no `if (ptr)`), `if` and `loop` are **expressions** that produce values, and `for` only walks **iterators**. There's no C-style `for (i = 0; i < n; i++)`.

Related: [`MATCH.md`](MATCH.md) · [`BOOL.md`](BOOL.md) · [`FN.md`](FN.md)

---

## 1. At a glance

| C / C++98 | Rust |
|---|---|
| `if (x > 0) {...}` | `if x > 0 {...}`: no parentheses, braces **mandatory** |
| `if (ptr)` / `if (n)` | ❌ You have to write `if n != 0`. Conditions must be `bool` |
| `c ? a : b` | `if c { a } else { b }` |
| `while (1)` / `for (;;)` | `loop { }` |
| `for (int i = 0; i < n; i++)` | `for i in 0..n` |
| `for (i = n - 1; i >= 0; i--)` | `for i in (0..n).rev()` |
| `do { } while (c);` | `loop { ...; if !c { break; } }` |
| `goto` out of nested loops | Loop labels: `break 'outer;` |

---

## 2. `if` / `else` is an expression

```rust
fn main() {
    let n = 7;
    let parity = if n % 2 == 0 { "even" } else { "odd" }; // both arms must have the same type
    println!("{n} is {parity}");

    if n < 0 {
        println!("negative");
    } else if n == 0 {
        println!("zero");
    } else {
        println!("positive");
    }
}
```

```rust,compile_fail
fn main() {
    let n = 3;
    if n { println!("C habit"); } // error[E0308]: expected `bool`, found integer
}
```

---

## 3. `loop`: infinite, and it can return a value

```rust
fn main() {
    let mut tries = 0;
    let found = loop {
        tries += 1;
        if tries * tries > 50 {
            break tries;        // `break value` → that's the value of the `loop` expression
        }
    };
    println!("{found}");
}
```

Only `loop` can `break` with a value. A `while` or `for` might run zero times, so it would have nothing to give back.

Prefer `loop` over `while true`. The compiler knows `loop` only exits through `break`, which helps both initialisation checking and the `!` type.

---

## 4. `while`

```rust
fn main() {
    let mut n = 27u64;
    let mut steps = 0;
    while n != 1 {
        n = if n % 2 == 0 { n / 2 } else { 3 * n + 1 };
        steps += 1;
    }
    println!("collatz: {steps} steps");
}
```

`while let` loops for as long as a pattern keeps matching. See [`MATCH.md`](MATCH.md).

---

## 5. `for … in …`: iterators only

```rust
fn main() {
    for i in 0..3 { print!("{i} "); }        // 0 1 2   (half-open, like i < n)
    for i in 0..=3 { print!("{i} "); }       // 0 1 2 3 (inclusive)
    for i in (0..10).step_by(3) { print!("{i} "); }
    println!();

    let v = vec![10, 20, 30];
    for x in &v { print!("{x} "); }           // borrow: v still usable after
    for (i, x) in v.iter().enumerate() { print!("[{i}]={x} "); }
    println!();
    for x in v { print!("{x} "); }            // by value: v is MOVED into the loop
    println!();
}
```

| You write | Iterates over | `v` afterwards |
|---|---|---|
| `for x in &v` | `&T` | still usable |
| `for x in &mut v` | `&mut T` (you can modify the elements) | still usable |
| `for x in v` | `T` (takes ownership) | **moved**, gone |

`for` uses iterators, so it's as fast as indexing and has no bounds checks inside the loop. `for i in 0..v.len() { v[i] }` works, but it checks bounds on every access and it's not idiomatic.

---

## 6. `break` / `continue` and labels

```rust
fn main() {
    'outer: for y in 0..5 {
        for x in 0..5 {
            if x == y { continue 'outer; } // jump to the next y
            if y == 4 { break 'outer; }    // leave both loops: no goto needed
            print!("({x},{y}) ");
        }
    }
    println!();
}
```

Labels start with an apostrophe, like lifetimes. They're a different thing that shares the same syntax.

---

## 7. Traps

| Symptom | Cause |
|---|---|
| `expected bool, found integer` | `if n` / `while n`. Compare explicitly |
| `borrow of moved value: v` after a `for` | You looped `for x in v`. Loop over `&v` |
| `if` without `else` used as a value | `if c { 1 }` has type `()` in its missing branch, so it doesn't type-check |
| `mismatched types` between the `if` arms | Both arms must produce the same type |
