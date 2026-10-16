# `mod`, `use`, `pub`, `crate`, `super`: Modules and Visibility

> **TL;DR.** A **crate** is the thing you compile (a lib or a binary). Inside it, **modules** (`mod`) form a tree, which is both a namespace and a privacy boundary. Everything is **private to its module** by default; `pub` opens it up. `use` brings a path into scope, like `using`. No headers, no include guards, no prototypes.

Related: [`../notions/tooling/CARGO.md`](../notions/tooling/CARGO.md) · [`IMPL_SELF.md`](IMPL_SELF.md)

---

## 1. C/C++ bridge

| C / C++98 | Rust |
|---|---|
| `.hpp` + `.cpp` pair, `#include`, include guards | One `.rs` per module. The compiler reads the module tree; there's no textual inclusion |
| `namespace ft { ... }` | `mod ft { ... }` |
| `ft::swap(a, b)` | `ft::swap(a, b)`: same `::` |
| `using std::cout;` | `use std::io::Write;` |
| `static` function (file-private) | Any item without `pub` |
| `public:` / `private:` in a class | `pub` on each field and item; the unit of privacy is the **module**, not the type |
| `friend` | Unnecessary: everything in the same module can already see private items |
| A Makefile's list of `.cpp` files | `mod foo;` declares the file; Cargo finds the rest |

---

## 2. Inline modules

```rust
mod geometry {
    pub struct Point { pub x: f64, pub y: f64 }   // struct AND fields public

    pub fn dist(a: &Point, b: &Point) -> f64 {
        square(a.x - b.x).add(square(a.y - b.y)).sqrt()
    }

    fn square(v: f64) -> f64 { v * v }            // private: a helper

    trait Add2 { fn add(self, o: f64) -> f64; }
    impl Add2 for f64 { fn add(self, o: f64) -> f64 { self + o } }

    pub mod shapes {
        use super::Point;                         // super = the parent module
        pub fn origin() -> Point { Point { x: 0.0, y: 0.0 } }
    }
}

use geometry::{dist, shapes::origin, Point};      // a nested import list

fn main() {
    let p = Point { x: 3.0, y: 4.0 };
    println!("{}", dist(&origin(), &p));
    // geometry::square(2.0);   ← error[E0603]: function `square` is private
}
```

---

## 3. Modules as files

```
my_app/
├── Cargo.toml
└── src/
    ├── main.rs          ← crate root: contains `mod parser;`
    ├── parser.rs        ← module `parser`: contains `pub mod token;`
    └── parser/
        └── token.rs     ← module `parser::token`
```

```text
// src/main.rs
mod parser;                   // "compile src/parser.rs as module parser"
use parser::token::Token;

// src/parser.rs
pub mod token;                // "compile src/parser/token.rs"
```

`mod x;` is the **only** way a file joins the build. A `.rs` file that no `mod` mentions is ignored, which is the opposite of a `*.cpp` wildcard in a Makefile.

---

## 4. Paths: `crate`, `self`, `super`

```text
crate::parser::token::Token    // absolute: from the crate root
self::helper()                 // relative: in this module
super::Point                   // relative: in the parent module
```

Think of them as `/`, `./` and `../` in the file system.

---

## 5. The levels of `pub`

| Written | Visible to |
|---|---|
| *(nothing)* | This module and its children |
| `pub(super)` | The parent module |
| `pub(crate)` | The whole crate, but not users of your library |
| `pub` | Everyone, including other crates |

For a struct, `pub struct` makes the **type** public, but every field is still private until you mark it `pub` too. That's the "private fields + public getters" design from CPP00, with the compiler enforcing it.

---

## 6. `use` tricks

```rust
use std::collections::HashMap;
use std::fmt::{self, Display};      // import `fmt` itself and `fmt::Display`
use std::io::Result as IoResult;    // rename on import

fn main() {
    let mut m: HashMap<&str, i32> = HashMap::new();
    m.insert("a", 1);
    let r: IoResult<()> = Ok(());
    let _ = (fmt::Error, r);
    fn show(x: impl Display) -> String { x.to_string() }
    println!("{} {:?}", show(1), m);
}
```

The **prelude** (`Vec`, `String`, `Option`, `Some`, `Box`, `println!`…) is imported into every module automatically, so you never write `use` for those.

---

## 7. Traps

| Symptom | Cause |
|---|---|
| `file not found for module` | `mod foo;` but neither `foo.rs` nor `foo/mod.rs` exists |
| `function is private` (E0603) | Missing `pub`, on the item or on one of the modules in the path |
| `field is private` although the struct is `pub` | Fields need their own `pub` |
| Your new `.rs` file is ignored | No `mod` declaration points at it |
