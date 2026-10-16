# 🔗 Borrowing: References the Compiler Checks

> **TL;DR.** Instead of moving a value, you can **borrow** it: `&T` for reading, `&mut T` for writing. At any moment you can have **either** any number of `&T` **or** exactly one `&mut T`, never both. A reference can **never outlive** what it points to. Those two rules remove dangling pointers, iterator invalidation and data races at compile time.

Related: [`OWNERSHIP.md`](OWNERSHIP.md) · [`LIFETIMES.md`](LIFETIMES.md) · [`../../lexique/IMPL_SELF.md`](../../lexique/IMPL_SELF.md) · C++ side: [`REFERENCE.md`](../../../cpp/notions/fundamentals/REFERENCE.md)

---

## 1. C++ bridge

| C++98 | Rust | Difference |
|---|---|---|
| `const T&` | `&T` | Also guarantees nobody **else** mutates it while you look |
| `T&` | `&mut T` | Also guarantees nobody else can even **read** it while you hold it |
| `f(x)` with `void f(T&)` (mutation hidden at the call site) | `f(&mut x)` | You can see the mutation at the call |
| A reference to a dead local compiles (UB) | Refused | Lifetimes |
| `const_cast` | None (see *interior mutability*, coming soon) | |

---

## 2. Borrowing instead of moving

```rust
fn len(s: &String) -> usize { s.len() }        // borrow for reading
fn shout(s: &mut String) { s.push('!'); }      // borrow for writing

fn main() {
    let mut s = String::from("hey");
    let n = len(&s);        // lend it; s is still mine
    shout(&mut s);          // lend it for mutation
    println!("{s} ({n})");  // hey! (3)
}
```

`&` makes a reference and `*` dereferences one, just like in C. The `.` operator dereferences automatically, so you'll rarely write `*` except for things like `*n += 1` on an `&mut i32`.

---

## 3. The core rule: aliasing XOR mutation

```
         many readers                      one writer
   ┌─────┐ ┌─────┐ ┌─────┐                 ┌──────────┐
   │ &T  │ │ &T  │ │ &T  │     OR          │  &mut T  │     never both
   └──┬──┘ └──┬──┘ └──┬──┘                 └────┬─────┘
      └───────┼───────┘                         │
              ▼                                 ▼
          ┌───────┐                         ┌───────┐
          │ value │                         │ value │
          └───────┘                         └───────┘
```

```rust,compile_fail
fn main() {
    let mut v = vec![1, 2, 3];
    let first = &v[0];          // shared borrow of v...
    v.push(4);                  // error[E0502]: cannot borrow `v` as mutable because it is also borrowed as immutable
    println!("{first}");        // ...still in use here
}
```

**This is a real C++ bug.** `push` may reallocate the buffer, and then `first` points into freed memory. It's `std::vector` iterator invalidation, and C++98 compiles it without a word. Rust rejects it.

### Why this rule, exactly?

Nearly every memory bug has two ingredients: **aliasing** (two paths to the same memory) and **mutation** (one path changes it). Either one alone is harmless:

| Aliasing | Mutation | Safe? | Example |
|---|---|---|---|
| ✅ | ❌ | ✅ | Many threads reading a config |
| ❌ | ✅ | ✅ | One function modifying its own buffer |
| ✅ | ✅ | 💥 | Iterator invalidation, data races, `memcpy` on overlapping buffers |

Rust forbids the last row, and it does so in the type system.

---

## 4. Non-lexical lifetimes: a borrow ends at its last use

```rust
fn main() {
    let mut v = vec![1, 2, 3];
    let first = &v[0];
    println!("{first}");        // last use of `first` → the borrow ends HERE
    v.push(4);                  // fine: nobody is borrowing v anymore
    println!("{v:?}");
}
```

The borrow checker reasons about **where the references are actually used**, not about the braces. Moving a line is often all it takes to fix an error.

---

## 5. Common fixes

| Error | Typical fix |
|---|---|
| `cannot borrow as mutable because it is also borrowed as immutable` (E0502) | Finish using the reader before you write. Copy the value out (`let x = v[0];`), or restructure |
| `cannot borrow as mutable more than once at a time` (E0499) | Split the work, or use `split_at_mut`, or borrow separate *fields* (allowed) |
| `cannot move out of borrowed content` (E0507) | You tried to take ownership through a `&`. Clone it, `std::mem::take`, or change the signature |
| `borrowed value does not live long enough` (E0597) | The owner dies first. See [`LIFETIMES.md`](LIFETIMES.md) |

```rust
struct Player { hp: i32, mana: i32 }

fn main() {
    let mut p = Player { hp: 10, mana: 5 };
    let hp = &mut p.hp;        // borrowing two DIFFERENT fields mutably: OK
    let mana = &mut p.mana;
    *hp -= 1;
    *mana += 1;
    println!("{} {}", p.hp, p.mana);

    let mut arr = [1, 2, 3, 4];
    let (left, right) = arr.split_at_mut(2);  // two &mut to disjoint halves
    left[0] = right[1];
    println!("{arr:?}");
}
```

---

## 6. Slices: borrowed views

A **slice** is a borrow of part of a collection: `&[T]` or `&str`. It's a fat pointer (pointer + length), the same as a `(ptr, len)` pair in C, but checked.

```rust
fn sum(xs: &[i32]) -> i32 { xs.iter().sum() }      // accepts arrays, Vecs, sub-ranges

fn first_word(s: &str) -> &str {
    s.split(' ').next().unwrap_or("")
}

fn main() {
    let v = vec![1, 2, 3, 4, 5];
    let a = [10, 20];
    println!("{} {} {}", sum(&v), sum(&v[1..3]), sum(&a));
    println!("{}", first_word("hello world"));
}
```

> Rule of thumb for parameters: take `&str`, not `&String`, and `&[T]`, not `&Vec<T>`. That accepts more callers and costs nothing extra.

---

## 7. What it buys you beyond memory safety

The same rule gives you **fearless concurrency**. A `&mut` is unique, so two threads can't write to the same data through safe code. The Philosophers mutex discipline you had to get right by hand in C is enforced by the type checker. (The full story, with `Send`, `Sync`, `Mutex` and `Arc`, is on the ⏳ `CONCURRENCY` page, coming soon.)

➡️ Next: [`LIFETIMES.md`](LIFETIMES.md). How does the compiler know a reference doesn't outlive its owner?
