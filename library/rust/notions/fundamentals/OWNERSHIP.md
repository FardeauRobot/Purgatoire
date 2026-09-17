# 📦 Ownership: RAII, Made Mandatory

> **TL;DR.** Every value has **one owner**. When the owner goes out of scope, the value is **dropped** (its destructor runs and its memory is freed). Assigning or passing a value **moves** it: the old name becomes unusable. Small plain types (`i32`, `bool`, `char`, `&T`…) are `Copy` and duplicate instead. That's all there is to it, and it removes `delete`, double frees and use-after-free.

Related: [`BORROWING.md`](BORROWING.md) · [`LIFETIMES.md`](LIFETIMES.md) · [`../../lexique/MOVE.md`](../../lexique/MOVE.md) · [`../../lexique/STRUCT.md`](../../lexique/STRUCT.md) · C++ side: [`MEMORY.md`](../../../cpp/notions/fundamentals/MEMORY.md), [`ORTHODOX_CANONICAL_FORM.md`](../../../cpp/notions/oop/ORTHODOX_CANONICAL_FORM.md)

---

## 1. The problem, in C++98 you already know

```cpp
class Buffer {
    char *_data;
public:
    Buffer() : _data(new char[64]) {}
    ~Buffer() { delete[] _data; }
    // forgot the copy constructor and operator=...
};

int main() {
    Buffer a;
    Buffer b = a;     // implicit shallow copy: both point at the same array
}                     // ~b deletes, then ~a deletes AGAIN → double free
```

The Orthodox Canonical Form exists to fix exactly this. You either write a deep copy or forbid copying. Forget once and valgrind complains.

**Rust's answer:** `let b = a;` doesn't *copy* at all. It **moves** `a` into `b`, and `a` is dead from then on. Only one owner, so only one drop.

---

## 2. The three rules

```
 1. Each value has exactly one owner (a variable, a field, a Vec slot…).
 2. There can only be one owner at a time.
 3. When the owner goes out of scope, the value is dropped.
```

```rust
fn main() {
    {
        let s = String::from("hello");   // s owns a heap buffer
        println!("{s}");
    }                                    // s goes out of scope → buffer freed. No `delete`.
}
```

The generated code is what C++ RAII would give you: a call to `drop` at the closing brace. The difference is that the compiler **guarantees** nobody uses `s` after that point.

---

## 3. Move

```rust,compile_fail
fn main() {
    let a = String::from("hi");
    let b = a;              // ownership moves from a to b
    println!("{a}");        // error[E0382]: borrow of moved value: `a`
}
```

What happens in memory:

```
 before `let b = a;`                  after
   stack            heap                stack            heap
 ┌────────────┐                       ┌────────────┐
 │ a: ptr ────┼──► "hi"               │ a: (dead)  │
 │    len 2   │                       ├────────────┤
 │    cap 2   │                       │ b: ptr ────┼──► "hi"
 └────────────┘                       │    len 2   │
                                      │    cap 2   │
                                      └────────────┘
```

A move is a **bitwise copy of the 3-word header** (pointer, length, capacity). The heap data isn't touched. `a` is then statically marked as moved-out, and no destructor runs for it. That's the whole trick: cheap like a shallow copy, safe like a deep one.

Moves happen on **every** by-value transfer:

```rust
fn take(s: String) -> usize { s.len() }  // s is dropped at the end of take

fn give() -> String { String::from("new") } // ownership leaves through the return value

fn main() {
    let s = String::from("owned");
    let n = take(s);                     // s moved into the function
    // println!("{s}");                  // error: moved
    let t = give();                      // t owns what give() made
    let v = vec![t];                     // t moved into the Vec
    println!("{n} {v:?}");
}
```

---

## 4. Copy: when duplicating is free

Types that are **only bits**, with no heap memory and no resource, implement `Copy`. For them, `let b = a;` duplicates the value and both stay usable, the same as `int` in C.

```rust
#[derive(Debug, Clone, Copy)]
struct Point { x: i32, y: i32 }   // every field is Copy → the struct can be too

fn main() {
    let a = 5;
    let b = a;                     // copy
    let p = Point { x: 1, y: 2 };
    let q = p;                     // copy
    println!("{a} {b} {p:?} {q:?}"); // all still usable
}
```

| `Copy` | Not `Copy` (so it moves) |
|---|---|
| Integers, floats, `bool`, `char` | `String`, `Vec<T>`, `Box<T>` |
| `&T` (shared references) | `&mut T` |
| Tuples and arrays of `Copy` types | Anything that implements `Drop` |
| Your structs with `#[derive(Clone, Copy)]` | Structs that contain any of the above |

A type **can't be both `Copy` and `Drop`**. If it needs a destructor, a silent bitwise copy would be the double-free bug again.

---

## 5. Clone: the deep copy, spelled out

```rust
fn main() {
    let a = String::from("deep");
    let b = a.clone();           // a new heap buffer: explicit and visible in the code
    println!("{a} {b}");
}
```

`.clone()` is your copy constructor, except you have to **call it by name**. An expensive copy can't hide behind `=` or a by-value parameter.

> Beginner habit: sprinkling `.clone()` to quiet the borrow checker. It works, but it's usually a sign you wanted a **borrow** (`&`). See [`BORROWING.md`](BORROWING.md).

---

## 6. The OCF, side by side

| Orthodox Canonical Form (C++98) | Rust |
|---|---|
| Default constructor | `fn new() -> Self` or `#[derive(Default)]` |
| Copy constructor | `#[derive(Clone)]` / `impl Clone`, called explicitly |
| Copy assignment `operator=` | `b = a.clone();`. The old `b` is dropped automatically first |
| Destructor | `impl Drop` (and only for non-memory resources) |
| "Did I forget one of the four?" | You can't: the defaults are *move* and *drop*, and both are always correct |

---

## 7. Drop order

```rust
struct Tag(&'static str);
impl Drop for Tag { fn drop(&mut self) { println!("drop {}", self.0); } }

fn main() {
    let _a = Tag("a");
    let _b = Tag("b");
    let moved = Tag("c");
    drop(moved);             // explicit early drop: prints "drop c" now
    println!("end of main");
}                            // prints "drop b", then "drop a": reverse declaration order
```

Struct fields are dropped in **declaration** order, after the struct's own `drop`. Local variables are dropped in **reverse** declaration order. That's the same as C++.

---

## 8. What about valgrind?

In safe Rust, **use-after-free, double free and uninitialised reads can't compile**. Leaks are *possible* but have to be done on purpose: `Box::leak`, `std::mem::forget`, or `Rc` reference cycles. So:

| Tool | Still useful in Rust? |
|---|---|
| valgrind `--leak-check` | Rarely. Mostly for FFI and `unsafe` code |
| ASan / UBSan | For `unsafe` code only. There's also Miri (`cargo +nightly miri test`) |
| Thinking about ownership | **Always.** That's the new skill |

➡️ Next: [`BORROWING.md`](BORROWING.md). How do you *use* a value without taking it?
