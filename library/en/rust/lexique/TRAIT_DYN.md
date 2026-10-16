# `trait`, `dyn`, `where`, `impl Trait`: Rust's Polymorphism

> **TL;DR.** A `trait` is a named set of methods a type promises to implement, somewhat like an abstract base class. You can use it two ways: **statically** with generics (`T: Trait` or `impl Trait`), where each type gets its own copy like a C++ template, or **dynamically** with `dyn Trait`, which uses a vtable like `virtual`. `where` is just a tidier place to write the bounds.

Related: [`../notions/traits/TRAITS_GENERICS.md`](../notions/traits/TRAITS_GENERICS.md) (the full story) · [`IMPL_SELF.md`](IMPL_SELF.md)

---

## 1. C/C++ bridge

| C++98 | Rust |
|---|---|
| `class Animal { virtual void speak() = 0; };` | `trait Animal { fn speak(&self); }` |
| `class Dog : public Animal` | `impl Animal for Dog` |
| `virtual` with a default body | A trait method with a default body |
| `Animal* a = new Dog;` then `a->speak()` | `let a: Box<dyn Animal> = Box::new(Dog);` then `a.speak()` |
| `template<typename T> void f(T x)` | `fn f<T: Animal>(x: T)`, but the bound is **checked when the generic is defined**, not when it's instantiated |
| Inheriting fields | **None.** Traits carry behaviour, never data |
| `virtual`, `override`, `final` | Reserved words that do nothing. See [`RESERVED`](RESERVED.md) |

---

## 2. Defining and implementing a trait

```rust
trait Animal {
    fn name(&self) -> String;               // required
    fn speak(&self) -> String {             // provided (a default body)
        format!("{} makes a sound", self.name())
    }
}

struct Dog;
struct Cat { lives: u8 }

impl Animal for Dog {
    fn name(&self) -> String { "Dog".into() }
    fn speak(&self) -> String { "Woof".into() }   // overrides the default
}

impl Animal for Cat {
    fn name(&self) -> String { format!("Cat({})", self.lives) }
}

fn main() {
    println!("{} / {}", Dog.speak(), Cat { lives: 9 }.speak());
}
```

---

## 3. Static dispatch: generics, `impl Trait`, `where`

```rust
use std::fmt::Display;

fn shout<T: Display>(x: T) -> String {           // bound written inline
    format!("{x}!")
}

fn shout2(x: impl Display) -> String {           // same thing, shorter
    format!("{x}!")
}

fn pair<A, B>(a: A, b: B) -> String
where                                            // bounds moved out of the way
    A: Display + Clone,
    B: Display,
{
    format!("({a}, {b})")
}

fn evens() -> impl Iterator<Item = u32> {        // return position: "some type, I won't name it"
    (0..10).filter(|n| n % 2 == 0)
}

fn main() {
    println!("{} {} {}", shout(3), shout2("hi"), pair(1, 'x'));
    println!("{:?}", evens().collect::<Vec<_>>());
}
```

The compiler generates `shout::<i32>` and `shout::<&str>` separately (**monomorphisation**). Calls are direct and can be inlined, just like templates. The cost is binary size.

---

## 4. Dynamic dispatch: `dyn Trait`

When you need a **mixed collection** or the type is only known at runtime:

```rust
trait Animal { fn speak(&self) -> String; }
struct Dog;
struct Cat;
impl Animal for Dog { fn speak(&self) -> String { "Woof".into() } }
impl Animal for Cat { fn speak(&self) -> String { "Meow".into() } }

fn main() {
    let zoo: Vec<Box<dyn Animal>> = vec![Box::new(Dog), Box::new(Cat)]; // the CPP04 Animal array
    for a in &zoo {
        println!("{}", a.speak());                // vtable call
    }
    let r: &dyn Animal = &Dog;                    // no heap needed; a borrow works too
    println!("{}", r.speak());
}
```

```
 Box<dyn Animal>  =  fat pointer (2 words)
 ┌──────────┬──────────┐        ┌────────── vtable for Dog ─────────┐
 │ data ptr │ vtbl ptr │ ─────► │ drop_in_place · size · align       │
 └────┬─────┴──────────┘        │ speak → <Dog as Animal>::speak     │
      ▼                         └────────────────────────────────────┘
    Dog (zero bytes here)
```

The difference from C++: the vtable pointer lives **in the pointer**, not in the object. A `Dog` carries no hidden vptr. You only pay for the vtable where you actually use `dyn`, and you never need a `virtual` destructor, because `drop` is always in the vtable.

**Dyn compatibility** (formerly called "object safety"): not every trait can be used as `dyn`. A method that returns `Self` or takes generic parameters rules it out, because a vtable needs one concrete signature per method.

---

## 5. Which one?

| Situation | Choose |
|---|---|
| The type is known at compile time, and speed matters | Generics / `impl Trait` |
| A heterogeneous collection (`Vec` of different animals) | `Box<dyn Trait>` |
| A plugin-style open set, known only at runtime | `dyn Trait` |
| A closed set of variants you control | Often neither: use an [`enum`](ENUM.md) |

---

## 6. Traps

| Symptom | Cause |
|---|---|
| `the trait bound T: Display is not satisfied` | Add the bound: `T: Display` |
| `the trait cannot be made into an object` | The trait isn't dyn-compatible (see §4) |
| `size for values of type dyn Animal cannot be known` | `dyn` has to sit behind `&`, `Box`, `Rc`… |
| `only traits defined in the current crate can be implemented for types defined outside` | The orphan rule. Wrap the type in a newtype |
