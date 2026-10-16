# `fn`, `return`: Functions and the Tail Expression

> **TL;DR.** `fn name(param: Type) -> Ret { body }`. Parameter types are **always** written out; inference stops at function boundaries. The last expression of the body, **without a semicolon**, is the return value. `return` exists for early exits.

Related: [`CONTROL_FLOW.md`](CONTROL_FLOW.md) · [`IMPL_SELF.md`](IMPL_SELF.md) · [`../notions/fundamentals/LIFETIMES.md`](../notions/fundamentals/LIFETIMES.md)

---

## 1. C/C++ bridge

| C++98 | Rust |
|---|---|
| `int add(int a, int b) { return a + b; }` | `fn add(a: i32, b: i32) -> i32 { a + b }` |
| `void f()` | `fn f()`: returns `()`, the unit type |
| Declaration in the `.hpp`, definition in the `.cpp` | One definition; order in the file doesn't matter, and there are no prototypes |
| Overloading `f(int)` / `f(double)` | **None.** Use different names or a trait |
| Default arguments `f(int x = 0)` | **None.** Use `Option<T>` or a builder |
| `static void helper()` (file-local) | Private by default; `pub fn` to export. See [`MOD_USE_PUB`](MOD_USE_PUB.md) |

---

## 2. Expressions vs statements, and why the semicolon matters

In Rust almost everything is an **expression** that produces a value: blocks, `if`, `match`, `loop`. A semicolon turns an expression into a statement and throws the value away, leaving `()`.

```rust
fn square(x: i32) -> i32 {
    x * x              // no `;` → this value is returned
}

fn clamp(x: i32) -> i32 {
    if x < 0 {
        return 0;      // early exit: `return` is for this
    }
    if x > 100 { 100 } else { x }   // `if` is an expression, like C's ternary
}

fn main() {
    let y = {          // a block is an expression too
        let t = 3;
        t + 1
    };
    println!("{} {} {}", square(4), clamp(-5), y);
}
```

```rust,compile_fail
fn square(x: i32) -> i32 {
    x * x;   // error[E0308]: mismatched types, expected `i32`, found `()`
}
fn main() { square(2); }
```

The compiler even tells you: *"remove this semicolon to return this value"*.

---

## 3. Parameters are patterns

```rust
fn dist((x1, y1): (f64, f64), (x2, y2): (f64, f64)) -> f64 {
    ((x2 - x1).powi(2) + (y2 - y1).powi(2)).sqrt()
}

fn main() {
    println!("{}", dist((0.0, 0.0), (3.0, 4.0)));
}
```

Passing is always **by value** (a move or a copy). To pass by reference you write `&T` / `&mut T` in the type, and `&x` / `&mut x` at the call site, so the call itself shows you what might be modified:

```rust
fn bump(n: &mut i32) { *n += 1; }

fn main() {
    let mut a = 1;
    bump(&mut a);   // visible at the call site; C++'s `bump(a)` hides this
    println!("{a}");
}
```

---

## 4. `!`: the function that never returns

```rust
fn fail(msg: &str) -> ! {
    panic!("{msg}");
}

fn main() {
    let n: i32 = if true { 1 } else { fail("unreachable") }; // `!` fits any type
    println!("{n}");
}
```

`!` is the *never* type. `panic!`, `std::process::exit` and an infinite `loop {}` all have it. It's what `[[noreturn]]` is in C++11, or `__attribute__((noreturn))` in C.

---

## 5. Function pointers

```rust
fn double(x: i32) -> i32 { x * 2 }

fn apply(f: fn(i32) -> i32, v: i32) -> i32 { f(v) }

fn main() {
    let ops: [fn(i32) -> i32; 1] = [double]; // a dispatch table, like Harl in CPP01
    println!("{}", apply(ops[0], 21));
}
```

`fn(i32) -> i32` is a plain code pointer, the same as C's `int (*)(int)`. Closures (`|x| x * 2`) are the richer version. See [`MOVE.md`](MOVE.md).

---

## 6. Traps

| Symptom | Cause |
|---|---|
| `expected i32, found ()` | A stray `;` on the tail expression |
| `missing type for function argument` | Parameters are never inferred |
| `this function takes 1 argument but 2 were supplied` | No overloading and no default arguments |
| `-> ()` written out by hand | Legal but noisy. Leave out `->` entirely |
