# `move`: Closures That Take Ownership

> **TL;DR.** Closures (`|x| x + 1`) capture the variables around them, and by default they **borrow** them as lightly as they can. Writing `move |x| …` makes the closure **take ownership** of everything it captures. You need it when the closure outlives the current scope, usually because you're starting a thread or returning the closure.

Related: [`../notions/fundamentals/OWNERSHIP.md`](../notions/fundamentals/OWNERSHIP.md) · [`FN.md`](FN.md) · [`TRAIT_DYN.md`](TRAIT_DYN.md)

---

## 1. C/C++ bridge

C++98 has no closures. The nearest thing is a **functor**, a class holding copies or references plus an `operator()`. A C++11 lambda is the same idea:

| C++11 lambda | Rust closure |
|---|---|
| `[&](int x){ return x + n; }` | `\|x\| x + n` (borrows `n`, the default) |
| `[=](int x){ return x + n; }` | `move \|x\| x + n` (moves `n` in; for `Copy` types that's a copy) |
| `[&]` capturing a local that then dies | **Can't compile.** Lifetimes catch the dangling reference |
| `std::function<int(int)>` | `Box<dyn Fn(i32) -> i32>` |

"Move semantics" also exists as a general idea: in Rust, **every** assignment or by-value pass of a non-`Copy` type is a move. The keyword `move` is only for closures. See [`OWNERSHIP`](../notions/fundamentals/OWNERSHIP.md).

---

## 2. Default capture vs `move`

```rust
fn main() {
    let name = String::from("ferris");

    let greet = || println!("hi {name}");   // borrows name (&String)
    greet();
    println!("still mine: {name}");          // fine: only borrowed

    let own = move || println!("bye {name}"); // name is MOVED into the closure
    own();
    // println!("{name}");                    // error[E0382]: borrow of moved value
}
```

The closure picks the lightest capture that works: `&` if it only reads, `&mut` if it modifies, and by value if it consumes. `move` forces by-value for everything it captures.

---

## 3. When you need `move`

### Threads

```rust
use std::thread;

fn main() {
    let data = vec![1, 2, 3];
    let handle = thread::spawn(move || {       // without `move`: error[E0373]
        let sum: i32 = data.iter().sum();      // the thread might outlive main's stack frame
        sum
    });
    println!("{}", handle.join().unwrap());
}
```

This is the Philosophers bug class: a thread holding a pointer into a stack frame that has returned. In C you had to get it right by hand. Rust refuses to compile it unless the data is moved in, or shared with `Arc`.

### Returning a closure

```rust
fn make_adder(n: i32) -> impl Fn(i32) -> i32 {
    move |x| x + n      // n would die when make_adder returns, so it has to be moved
}

fn main() {
    let add5 = make_adder(5);
    println!("{}", add5(10));
}
```

---

## 4. The three closure traits

| Trait | The closure… | Can be called |
|---|---|---|
| `Fn` | only reads what it captured | any number of times, even concurrently |
| `FnMut` | modifies what it captured | many times, but needs `&mut` |
| `FnOnce` | consumes what it captured | **once** |

```rust
fn main() {
    let mut count = 0;
    let mut inc = || count += 1;         // FnMut
    inc();
    inc();
    println!("{count}");

    let s = String::from("gone");
    let consume = move || s;             // FnOnce: returns the String it owns
    let back = consume();
    println!("{back}");
    // consume();                        // error: closure cannot be invoked more than once
}
```

`move` decides **how things get captured**, and `Fn` / `FnMut` / `FnOnce` describe **what the body does with them**. They're separate questions. A `move` closure that only reads is still `Fn`.

---

## 5. Under the hood

A closure is an anonymous struct holding its captures, with the call method implemented on it. That's a C++ functor, generated for you:

```
 move |x| x + n        ≈   struct __closure { n: i32 }
                           impl Fn(i32) -> i32 for __closure { fn call(&self, x) { x + self.n } }
```

Each closure has its own unique type. That's why you pass closures as `impl Fn(...)` or through a generic `F: Fn(...)`, which is monomorphised and has zero cost, or as `Box<dyn Fn(...)>`, which goes through a vtable.
