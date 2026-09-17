# `struct`: Data, No Class Attached

> **TL;DR.** A `struct` is only data: named fields, and no constructors, destructors, inheritance or access specifiers beyond `pub`. Behaviour goes in separate `impl` blocks, and the "canonical form" is chosen field by field with `#[derive(...)]`.

Related: [`IMPL_SELF.md`](IMPL_SELF.md) · [`ENUM.md`](ENUM.md) · [`../notions/fundamentals/OWNERSHIP.md`](../notions/fundamentals/OWNERSHIP.md)

---

## 1. C/C++ bridge

| C++98 `class` | Rust |
|---|---|
| Fields + methods together | `struct { fields }` + `impl { methods }`, kept apart |
| Constructor `Point(int x, int y)` | No constructors. By convention an associated fn `Point::new(x, y)` |
| Default ctor | `#[derive(Default)]` → `Point::default()` |
| Copy ctor + `operator=` | `#[derive(Clone)]` → explicit `.clone()`; `Copy` if bitwise copies are fine |
| Destructor | `impl Drop for Point` (only when you really need it) |
| `private:` by default in a `class` | Fields are private **to the module** by default; `pub` to expose |
| Inheritance | None. Use composition + traits |

**The OCF in one line:** `#[derive(Debug, Clone, Default, PartialEq)]` asks the compiler for printing, deep copy, default construction and `==`, all correct by construction. You don't write a `Drop` for memory, because fields that own heap data drop themselves.

---

## 2. Three flavours

```rust
#[derive(Debug, Clone, Copy, PartialEq, Default)]
struct Point { x: i32, y: i32 }            // named fields

#[derive(Debug)]
struct Meters(f64);                         // tuple struct: a "newtype" wrapper

#[derive(Debug)]
struct Marker;                              // unit struct: zero bytes

fn main() {
    let p = Point { x: 1, y: 2 };
    let x = 5;
    let q = Point { x, ..p };               // field shorthand + "rest from p"
    let o = Point::default();               // (0, 0)
    let Point { x: px, y: py } = q;         // destructure
    let d = Meters(3.5);
    println!("{p:?} {q:?} {o:?} {px} {py} {} {:?}", d.0, Marker);
    println!("{}", p == Point { x: 1, y: 2 });
    println!("{}", std::mem::size_of::<Marker>()); // 0: a ZST
}
```

**Newtypes** (`struct Meters(f64)`) are a zero-cost way to stop mixing up units. `Meters` and `Seconds` are different types even though both are an `f64` underneath. In C++98 you'd need a whole class to get that.

---

## 3. Constructors are just functions

```rust
#[derive(Debug)]
pub struct Account {
    owner: String,   // private: only this module can touch it
    balance: i64,
}

impl Account {
    pub fn new(owner: &str) -> Self {
        Account { owner: owner.to_string(), balance: 0 }
    }
    pub fn balance(&self) -> i64 { self.balance }   // a getter
}

fn main() {
    let a = Account::new("ferris");
    println!("{a:?} {}", a.balance());
}
```

A struct literal needs **every field**, and outside the module you can't name private fields. So the only way to build an `Account` from outside is through `new`, and that's how you enforce invariants. It does the same job as a private constructor plus a factory.

---

## 4. `Drop`: the destructor

```rust
struct Noisy(&'static str);

impl Drop for Noisy {
    fn drop(&mut self) { println!("drop {}", self.0); }
}

fn main() {
    let _a = Noisy("a");
    {
        let _b = Noisy("b");
    }                        // prints "drop b"
    let _c = Noisy("c");
}                            // prints "drop c", then "drop a": reverse order, like C++
```

You write `Drop` for **non-memory** resources: file descriptors, locks, sockets. A `String` or `Vec` field frees itself. You can't call `x.drop()` directly; use `drop(x)`, which moves the value in and ends it.

---

## 5. Layout

Rust **may reorder fields** to cut down on padding. If you need C's layout (for FFI), use `#[repr(C)]`:

```rust
#[repr(C)]
struct Header { tag: u8, len: u32, flag: u8 }   // C order, C padding → 12 bytes

struct Packed { tag: u8, len: u32, flag: u8 }   // Rust may reorder → 8 bytes

fn main() {
    println!("{} {}", std::mem::size_of::<Header>(), std::mem::size_of::<Packed>());
}
```

---

## 6. Traps

| Symptom | Cause |
|---|---|
| `missing field` in a struct literal | Every field is required. Use `..Default::default()` |
| `field is private` | Private is the default outside the module. Add `pub`, or a constructor |
| `` `Point` doesn't implement `Debug` `` | Add `#[derive(Debug)]` to use `{:?}` |
| `use of moved value` after `let b = a;` | The struct isn't `Copy`. Derive `Clone, Copy` if every field is `Copy`, or call `.clone()` |
