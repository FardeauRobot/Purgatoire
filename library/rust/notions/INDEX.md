# 🧠 NOTIONS — Concepts by Theme

The zoomed-out layer: each page explains one concept properly, with a mental model, diagrams, worked examples and gotchas, always starting from what C/C++98 does. Keywords get their zoomed-in pages in [`../lexique/`](../lexique/INDEX.md).

> 🌟 **Start here:** [`PHILOSOPHY.md`](PHILOSOPHY.md). It covers why Rust exists and the four ideas that make the rest of the language make sense.

---

## 🧱 `fundamentals/`: the base layer

| File | Topic |
|---|---|
| [`OWNERSHIP.md`](fundamentals/OWNERSHIP.md) | The 3 rules, move vs `Copy`, `Drop` = RAII without the Rule of Three. |
| [`BORROWING.md`](fundamentals/BORROWING.md) | `&` / `&mut`, "aliasing XOR mutation", the borrow checker vs C++ references. |
| [`LIFETIMES.md`](fundamentals/LIFETIMES.md) | `'a`, the elision rules, the dangling-pointer bugs they rule out. |
| [`TYPES.md`](fundamentals/TYPES.md) | Scalars, fixed-width ints, overflow, `String` vs `&str`, arrays and slices. |

## 🧬 `traits/`: polymorphism

| File | Topic |
|---|---|
| [`TRAITS_GENERICS.md`](traits/TRAITS_GENERICS.md) | Generics + monomorphisation (≈ templates), trait objects (≈ virtual), `derive`. |

## 🚨 `errors/`

| File | Topic |
|---|---|
| [`ERROR_HANDLING.md`](errors/ERROR_HANDLING.md) | `Option` / `Result`, the `?` operator, `panic!` vs exceptions, `unwrap` hygiene. |

## 🛠️ `tooling/`

| File | Topic |
|---|---|
| [`CARGO.md`](tooling/CARGO.md) | Cargo vs Makefile, `build`/`run`/`test`/`clippy`/`fmt`, editions, `Cargo.toml`. |

---

## ⏳ Coming soon

| Planned page | Topic | C++98 counterpart |
|---|---|---|
| `SMART_POINTERS` | `Box`, `Rc`, `Arc`, `RefCell`, interior mutability | `new`, hand-rolled refcounting |
| `COLLECTIONS` | `Vec`, `HashMap`, `BTreeMap`, `VecDeque` | `std::vector`, `std::map`, `std::deque` |
| `ITERATORS_CLOSURES` | `Iterator`, adapters (`map`, `filter`…), `Fn`/`FnMut`/`FnOnce` | `<algorithm>`, functors |
| `CONCURRENCY` | Threads, `Send`/`Sync`, `Mutex`, channels | pthreads (Philosophers!) |
| `MACROS` | `macro_rules!`, why `println!` has a `!` | the C preprocessor, done right |
| `ASYNC` | `async`/`await`, futures, executors | `poll()`/`epoll` loops (webserv!) |
