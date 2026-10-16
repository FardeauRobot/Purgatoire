# `impl`, `self`, `Self`: Methods Without Classes

> **TL;DR.** `impl Type { ... }` attaches functions to a type. A function whose first parameter is `self` is a **method** (`x.f()`). One without `self` is an **associated function** (`Type::f()`), which is Rust's `static` member. `Self` (capital S) is shorthand for "the type this `impl` is for".

Related: [`STRUCT.md`](STRUCT.md) · [`TRAIT_DYN.md`](TRAIT_DYN.md) · [`../notions/fundamentals/BORROWING.md`](../notions/fundamentals/BORROWING.md)

---

## 1. C/C++ bridge

| C++98 | Rust | What the method can do |
|---|---|---|
| `int get() const;` | `fn get(&self) -> i32` | Read the fields |
| `void set(int v);` | `fn set(&mut self, v: i32)` | Modify the fields |
| *(no equivalent)* | `fn into_name(self) -> String` | **Take ownership.** The object is consumed and can't be used afterwards |
| `static Foo create();` | `fn new() -> Self` | No receiver: `Foo::new()` |
| `this->x` | `self.x`. There's no implicit `self`, so you always write it |

The `self` parameter **is** `this`, written out, and its type tells you what the method needs:

```
  self       → I take the object (a move). The caller loses it.
  &self      → I read it.        ≈ a const member function
  &mut self  → I modify it.      ≈ a non-const member function
```

---

## 2. Example

```rust
#[derive(Debug)]
struct Counter { name: String, n: u32 }

impl Counter {
    pub fn new(name: &str) -> Self {         // Self == Counter
        Self { name: name.to_string(), n: 0 }
    }
    pub fn get(&self) -> u32 { self.n }
    pub fn tick(&mut self) -> &mut Self {    // returning &mut Self allows chaining
        self.n += 1;
        self
    }
    pub fn finish(self) -> String {          // consumes the counter
        format!("{} ended at {}", self.name, self.n)
    }
}

fn main() {
    let mut c = Counter::new("hits");
    c.tick().tick().tick();
    println!("{}", c.get());
    let report = c.finish();
    println!("{report}");
    // c.get();   ← error[E0382]: borrow of moved value: `c`
}
```

### Auto-referencing

You write `c.get()`, not `(&c).get()`. The dot operator adds `&`, `&mut` or `*` as needed to match the receiver. That's why there's no `->` in Rust: `.` works through references and smart pointers alike.

### Methods are functions

`c.get()` is sugar for `Counter::get(&c)`. That's exactly how C++ passes `this` as a hidden first argument, and Rust just lets you see it.

---

## 3. Multiple `impl` blocks, and impls for traits

```rust
use std::fmt;

struct Temp(f64);

impl Temp {                                   // inherent methods
    fn celsius(&self) -> f64 { self.0 }
}

impl Temp {                                   // you can split them across several blocks
    fn fahrenheit(&self) -> f64 { self.0 * 9.0 / 5.0 + 32.0 }
}

impl fmt::Display for Temp {                  // a trait impl: like overloading operator<<
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "{:.1}°C", self.0)
    }
}

fn main() {
    let t = Temp(21.5);
    println!("{t} = {:.1}°F ({})", t.fahrenheit(), t.celsius());
}
```

`impl Display for Temp` is how you make `println!("{t}")` work, the same job as `std::ostream& operator<<(std::ostream&, const Temp&)`. See [`TRAIT_DYN.md`](TRAIT_DYN.md).

---

## 4. `self` / `Self` / `super` / `crate` in paths

The same words also work as **path** prefixes, which is a different job:

| In a path | Means |
|---|---|
| `self::foo` | `foo` in the current module |
| `super::foo` | `foo` in the parent module (`../`) |
| `crate::foo` | `foo` from the crate root (an absolute path) |
| `Self::CONST` | An associated item of the current type |

See [`MOD_USE_PUB.md`](MOD_USE_PUB.md).

---

## 5. Traps

| Symptom | Cause |
|---|---|
| `cannot borrow *self as mutable, as it is behind a & reference` | The method takes `&self` but modifies a field. Change it to `&mut self` |
| `cannot borrow c as mutable` | The **binding** is missing `mut` (`let mut c`) |
| `use of moved value` after `c.finish()` | The method takes `self` by value. That's what it's for |
| `no function or associated item named new` | `new` isn't magic. You have to write it |
