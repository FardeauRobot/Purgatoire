# 📖 GLOSSAIRE — Rust Concepts, A→Z

The words that show up in Rust errors and docs and aren't keywords. One paragraph each, with the closest C/C++ idea. For keywords, see [`INDEX.md`](INDEX.md).

---

| Term | Meaning | Closest C/C++ idea |
|---|---|---|
| **Associated function** | A function in an `impl` block that has no `self`, called as `Type::f()`. `new` is only a convention, not a keyword. | `static` member function |
| **Associated type** | A type declared inside a trait (`type Item;`) that each implementor fixes. | `typedef` inside a class template, like `vector<T>::value_type` |
| **Borrow** | A reference `&T` or `&mut T` to a value someone else owns. It can't outlive the owner. | `const T&` / `T&`, but checked |
| **Borrow checker** | The compiler pass that enforces the borrowing rules and lifetimes. | Nothing. It's the thing you did in your head for C |
| **Box** | `Box<T>`: an owned pointer to heap memory, freed when dropped. | `new T` plus an automatic `delete` |
| **Clone** | A trait for an explicit, possibly expensive copy: `.clone()`. | Copy constructor, but you have to call it by name |
| **Copy** | A marker trait for types that are copied bit for bit implicitly (ints, `bool`, `char`, `&T`). | Trivially-copyable types, POD |
| **Crate** | One compilation unit: a library or a binary. What `cargo` builds. | A `.a` / `.so` or an executable |
| **Deref coercion** | `&String` becomes `&str`, `&Box<T>` becomes `&T` automatically when a function wants it. | Implicit conversion, but only this one kind |
| **Derive** | `#[derive(Debug, Clone, PartialEq)]` asks the compiler to generate the trait implementation. | Compiler-generated OCF members, but you choose which ones |
| **Drop** | The trait whose `drop(&mut self)` runs when a value goes out of scope. | Destructor |
| **DST** (dynamically sized type) | A type with no size known at compile time, like `str`, `[T]` or `dyn Trait`. You only ever use it behind a pointer. | An incomplete type |
| **Edition** | A language version (`2015`, `2018`, `2021`, `2024`) chosen per crate. Crates from different editions link together. | `-std=c++98` vs `-std=c++11`, except you can mix them |
| **Fat pointer** | A pointer plus metadata: a length for slices, a vtable pointer for `dyn Trait`. Two words wide. | A `(char *, size_t)` pair |
| **Interior mutability** | Mutating through a `&T` in a controlled way (`Cell`, `RefCell`, `Mutex`). | `mutable` |
| **Lifetime** | The region of code where a reference is valid, written `'a`. | The scope you had to reason about by hand |
| **Macro** | Code that writes code, called with `!`: `println!`, `vec!`. It works on syntax, not text. | `#define`, but hygienic and typed |
| **Monomorphisation** | Generating a separate copy of a generic function for each concrete type it's used with. | Template instantiation |
| **Move** | Transferring ownership. The source can't be used afterwards. This is the default for non-`Copy` types. | `std::move` (C++11), except that here it's the default and the compiler checks it |
| **NLL** (non-lexical lifetimes) | A borrow ends at its last use, not at the closing `}`. | none |
| **Orphan rule** | You can only `impl Trait for Type` if your crate defines the trait or the type. | none (C++ lets you specialise anything) |
| **Owner** | The one binding responsible for dropping a value. | Whoever is supposed to call `delete` |
| **Panic** | An unrecoverable error: the thread unwinds (or aborts) and prints a message. | `abort()` or an uncaught exception |
| **Pattern** | A shape you match against: `Some(x)`, `(a, b)`, `Point { x, .. }`. Used in `let`, `match`, `if let` and function parameters. | none (structured bindings are C++17) |
| **Prelude** | Names imported into every file automatically: `Vec`, `String`, `Option`, `Some`, `None`, `Ok`, `Err`… | The headers you'd `#include` everywhere |
| **Shadowing** | `let x = ...;` again in the same scope creates a *new* binding that hides the old one. | Legal in an inner scope in C++, but not in the same scope |
| **Slice** | `&[T]` or `&str`: a borrowed view into contiguous memory, stored as pointer + length. | `(T *ptr, size_t len)` |
| **Trait** | A set of methods a type can implement. Used for both static and dynamic polymorphism. | An abstract base class *and* a template concept at once |
| **Trait object** | `dyn Trait` behind a pointer (`&dyn Trait`, `Box<dyn Trait>`): dispatched through a vtable at runtime. | A `Base*` pointing at a derived object |
| **UB** (undefined behaviour) | Same meaning as in C. Safe Rust can't cause it; `unsafe` code can. | UB |
| **Unit** | `()`: the empty tuple. What a function returns when it returns "nothing". | `void`, but it's a real value |
| **Unwinding** | Walking back up the stack on a panic, running `Drop` for each value. | Stack unwinding for exceptions |
| **ZST** (zero-sized type) | A type that takes 0 bytes, like `()` or `struct Marker;`. It's optimised away. | none (in C++ an empty class takes at least 1 byte) |
