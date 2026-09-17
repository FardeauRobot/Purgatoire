# `match`, `if let`, `while let`, `let … else`: Pattern Matching

> **TL;DR.** `match` is `switch` done right: it works on **any** type, it **destructures** what it matches, it's an **expression**, it has **no fall-through**, and it must be **exhaustive**. Forget a case and it doesn't compile. `if let`, `while let` and `let … else` are the one-pattern shortcuts.

Related: [`ENUM.md`](ENUM.md) · [`LET_MUT.md`](LET_MUT.md) · [`../notions/errors/ERROR_HANDLING.md`](../notions/errors/ERROR_HANDLING.md)

---

## 1. C/C++ bridge

| `switch` in C++98 | `match` in Rust |
|---|---|
| Integers and enums only | Any type: tuples, structs, strings, ranges, references… |
| Falls through without `break` | Never falls through. Use `a \| b` to share an arm |
| `default:` is optional; a missing case is silent (or a `-Wswitch` warning) | Must be exhaustive; `_` is the catch-all |
| A statement | An expression: `let x = match …` |
| `case 1: case 2: case 3:` | `1..=3 =>` |

The `-Wswitch` flag in this repo's Makefiles gives you a weak version of Rust's exhaustiveness check. In Rust it's always on, and it's an error.

---

## 2. The shapes of a pattern

```rust
#[derive(Debug)]
enum Shape { Circle { r: f64 }, Rect { w: f64, h: f64 }, Dot }

fn describe(n: i32) -> &'static str {
    match n {
        0 => "zero",
        1 | 2 => "small",                 // or
        3..=9 => "digit",                 // inclusive range
        x if x < 0 => "negative",         // guard: an extra `if` condition
        _ => "big",                       // catch-all
    }
}

fn area(s: &Shape) -> f64 {
    match s {
        Shape::Circle { r } => 3.14159 * r * r,       // destructure a struct-like variant
        Shape::Rect { w, h } if w == h => w * w,      // guard on the destructured fields
        Shape::Rect { w, h } => w * h,
        Shape::Dot => 0.0,
    }
}

fn main() {
    println!("{} {} {}", describe(2), describe(-4), describe(77));
    let shapes = [Shape::Circle { r: 1.0 }, Shape::Rect { w: 2.0, h: 3.0 }, Shape::Dot];
    for s in &shapes { println!("{s:?} → {}", area(s)); }

    let pair = (3, -3);
    match pair {
        (0, y) => println!("on the y axis at {y}"),
        (x, y) if x + y == 0 => println!("on the anti-diagonal"),
        (x, _) => println!("x = {x}, y ignored"),
    }

    let point = (1, 2, 3);
    let (a, .., c) = point;                // `..` skips the rest
    let n @ 1..=5 = a else { return };     // `@` binds and tests at once
    println!("{a} {c} {n}");
}
```

| Pattern | Matches |
|---|---|
| `42`, `'c'`, `"text"` | A literal |
| `x` | Anything, and binds it to `x` |
| `_` | Anything, binds nothing |
| `a \| b` | Either one |
| `1..=5` | An inclusive range |
| `(a, b)`, `[first, .., last]` | A tuple or a slice |
| `Point { x, y: 0 }` | A struct with `y == 0`, binding `x` |
| `Some(v)`, `Err(e)` | An enum variant |
| `n @ 1..=5` | Binds `n` and also checks the range |
| `x if cond` | A pattern plus a guard |

---

## 3. Exhaustiveness

```rust,compile_fail
enum Dir { N, S, E, W }
fn turn(d: Dir) -> i32 {
    match d {  // error[E0004]: non-exhaustive patterns: `Dir::W` not covered
        Dir::N => 0,
        Dir::S => 180,
        Dir::E => 90,
    }
}
fn main() { turn(Dir::N); }
```

This is why adding a variant to an enum is safe in Rust. Every `match` that forgot to handle it becomes a compile error you can see, not a silent `default:`.

> Tip: don't reach for `_` by reflex on your own enums. Listing every variant is what makes the compiler warn you when you add one.

---

## 4. The one-pattern shortcuts

```rust
fn parse(s: &str) -> Option<i32> { s.parse().ok() }

fn first_even(v: &[i32]) -> Option<i32> {
    let Some(&x) = v.iter().find(|x| **x % 2 == 0) else {
        return None;              // let-else: must diverge (return/break/panic)
    };
    Some(x * 10)
}

fn main() {
    if let Some(n) = parse("42") {          // match one pattern, ignore the rest
        println!("got {n}");
    } else {
        println!("not a number");
    }

    let mut stack = vec![1, 2, 3];
    while let Some(top) = stack.pop() {     // loop while the pattern matches
        print!("{top} ");
    }
    println!();

    println!("{:?}", first_even(&[1, 3, 4, 5]));

    let is_small = matches!(parse("3"), Some(0..=9)); // a bool test
    println!("{is_small}");
}
```

| You want | Use |
|---|---|
| Handle every case | `match` |
| Handle one case and ignore the rest | `if let` |
| Loop until the pattern stops matching | `while let` |
| Unpack or bail out, with no extra indentation for the happy path | `let PAT = expr else { return … };` |
| Just a `bool` | `matches!(expr, PAT)` |

---

## 5. Under the hood

A `match` on integers or on an enum's tag usually compiles to a **jump table** or a binary search, the same as a dense `switch` in C. Destructuring costs nothing: it reads fields at known offsets. The exhaustiveness check happens only at compile time and doesn't exist at runtime.
