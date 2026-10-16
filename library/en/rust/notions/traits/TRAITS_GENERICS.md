# 🧬 Traits and Generics: Templates and Virtual, Unified

> **TL;DR.** A **trait** is an interface: a set of methods. **Generics** (`fn f<T: Trait>`) are C++ templates whose requirements are **written in the signature and checked when the generic is defined**, not whenever it gets instantiated. The same trait can also be used **dynamically** as `dyn Trait`, the equivalent of `virtual`. Rust has no inheritance of data. You compose structs and implement traits.

Related: [`../../lexique/TRAIT_DYN.md`](../../lexique/TRAIT_DYN.md) · [`../../lexique/IMPL_SELF.md`](../../lexique/IMPL_SELF.md) · C++ side: [`TEMPLATES.md`](../../../cpp/notions/advanced/TEMPLATES.md), [`POLYMORPHISM.md`](../../../cpp/notions/oop/POLYMORPHISM.md)

---

## 1. The C++ problem generics fix

```cpp
template<typename T>
T max(T a, T b) { return a > b ? a : b; }

struct Foo {};
max(Foo(), Foo());   // error appears INSIDE max, pages of template noise
```

C++98 templates are **duck-typed**. Whether `T` supports `>` is only checked when the template is instantiated, and the error comes out of the template's guts. (C++20 concepts fix this, 22 years later.)

Rust puts the requirement **in the signature**:

```rust
fn max<T: PartialOrd>(a: T, b: T) -> T {   // "T must support comparison"
    if a > b { a } else { b }
}

fn main() {
    println!("{} {} {}", max(3, 7), max(2.5, 1.0), max("a", "b"));
}
```

`max(Foo, Foo)` fails **at the call site** with *"the trait `PartialOrd` is not implemented for `Foo`"*. That's one line pointing at the problem. And the body of `max` may only use what `PartialOrd` provides, so it's type-checked once, generically.

---

## 2. Defining, implementing and using a trait

```rust
trait Shape {
    fn area(&self) -> f64;
    fn name(&self) -> String { "shape".to_string() }   // default method
}

struct Circle { r: f64 }
struct Square { side: f64 }

impl Shape for Circle {
    fn area(&self) -> f64 { 3.14159 * self.r * self.r }
    fn name(&self) -> String { "circle".into() }
}
impl Shape for Square {
    fn area(&self) -> f64 { self.side * self.side }
}

fn report<S: Shape>(s: &S) -> String {                 // static dispatch
    format!("{} of area {:.2}", s.name(), s.area())
}

fn total(shapes: &[Box<dyn Shape>]) -> f64 {           // dynamic dispatch
    shapes.iter().map(|s| s.area()).sum()
}

fn main() {
    println!("{}", report(&Circle { r: 1.0 }));
    println!("{}", report(&Square { side: 2.0 }));
    let all: Vec<Box<dyn Shape>> = vec![Box::new(Circle { r: 1.0 }), Box::new(Square { side: 2.0 })];
    println!("{:.2}", total(&all));
}
```

---

## 3. Static vs dynamic dispatch

```
            generic  fn report<S: Shape>(s: &S)             dyn  fn total(&[Box<dyn Shape>])
            ─────────────────────────────────              ─────────────────────────────────
 compiled:  report::<Circle>, report::<Square>             one function
 call:      direct, inlinable                              through a vtable (like virtual)
 size:      one copy per type                              one copy total
 mix types in one Vec?  no                                 yes
 C++ analogue: template                                    Base* + virtual
```

Rust lets you **choose per use site** with the same trait. In C++ you choose when you design the class, by writing `virtual` or not.

---

## 4. The standard traits you'll implement or derive

These traits replace C++'s implicit special member functions and operator overloads:

| Trait | Gives you | C++98 equivalent | Derivable? |
|---|---|---|---|
| `Debug` | `{:?}` printing | A debug `operator<<` | ✅ |
| `Display` | `{}` printing | `operator<<` | ❌ write it |
| `Clone` | `.clone()` | Copy constructor | ✅ |
| `Copy` | Implicit bitwise copy | Trivially copyable | ✅ |
| `Default` | `T::default()` | Default constructor | ✅ |
| `PartialEq` / `Eq` | `==`, `!=` | `operator==` | ✅ |
| `PartialOrd` / `Ord` | `<`, `>`, sorting | `operator<` | ✅ |
| `Hash` | Usable as a `HashMap` key | `std::hash` specialisation | ✅ |
| `Drop` | Destructor | `~T()` | ❌ |
| `Add`, `Sub`, `Mul`, `Neg`… | `+`, `-`, `*`, unary `-` | `operator+`… (the CPP02 `Fixed` exercise!) | ❌ |
| `Index` | `x[i]` | `operator[]` | ❌ |
| `From` / `Into` | Conversions | Converting constructors | ❌ |
| `Iterator` | `for x in thing` | Iterator classes | ❌ |

```rust
use std::fmt;
use std::ops::Add;

#[derive(Debug, Clone, Copy, PartialEq, PartialOrd, Default)]
struct Fixed { raw: i32 }                          // CPP02, reborn

impl Fixed {
    const FRAC_BITS: i32 = 8;
    fn from_f32(f: f32) -> Self { Fixed { raw: (f * (1 << Self::FRAC_BITS) as f32).round() as i32 } }
    fn to_f32(self) -> f32 { self.raw as f32 / (1 << Self::FRAC_BITS) as f32 }
}

impl Add for Fixed {                                // operator+
    type Output = Fixed;
    fn add(self, o: Fixed) -> Fixed { Fixed { raw: self.raw + o.raw } }
}

impl fmt::Display for Fixed {                       // operator<<
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result { write!(f, "{}", self.to_f32()) }
}

fn main() {
    let a = Fixed::from_f32(1.5);
    let b = Fixed::from_f32(2.25);
    println!("{} {} {:?}", a + b, a < b, Fixed::default());
}
```

The whole OCF + operators exercise is one `derive` line and two small impls.

---

## 5. Associated types and generic traits

```rust
struct Countdown(u32);

impl Iterator for Countdown {
    type Item = u32;                         // associated type: "what I yield"
    fn next(&mut self) -> Option<u32> {
        if self.0 == 0 { None } else { self.0 -= 1; Some(self.0 + 1) }
    }
}

fn main() {
    let v: Vec<u32> = Countdown(3).collect();       // every iterator adapter is free
    let s: u32 = Countdown(4).filter(|n| n % 2 == 0).sum();
    println!("{v:?} {s}");
}
```

Implement one method, `next`, and you get about 70 adapter methods (`map`, `filter`, `zip`, `sum`…) for free, as **default methods** of the trait. That's C++'s `<algorithm>`, but attached to the iterator itself.

---

## 6. No inheritance: composition instead

| C++ inheritance use | Rust replacement |
|---|---|
| Share an **interface** (`virtual` methods) | A trait |
| Share **code** (base class methods) | Default trait methods, or a helper function |
| Share **data** (base class fields) | Composition: put a struct inside a struct |
| A closed set of subclasses (`Animal` = Dog \| Cat) | An `enum` + `match` |
| "is-a" requirement between interfaces | Supertraits: `trait Pet: Animal { … }` |

The diamond problem from CPP03 (`DiamondTrap`) can't happen, because there's no data to inherit twice.

---

## 7. Traps

| Symptom | Cause |
|---|---|
| `the trait bound T: X is not satisfied` | Add `T: X` to the generic's bounds, or implement X for the type |
| `binary operation > cannot be applied to type T` | The same thing: add `T: PartialOrd` |
| `the trait X cannot be made into an object` | The trait has generic methods or returns `Self`, so it can't be `dyn` |
| `conflicting implementations` | Two impls overlap. Rust has no specialisation on stable |
| `only traits defined in the current crate…` | The orphan rule. Wrap the foreign type in a newtype |
