# `unsafe`, `extern`: The Escape Hatches

> **TL;DR.** `unsafe { … }` doesn't switch the borrow checker off. It unlocks **five specific operations** the compiler can't verify, and you take responsibility for them. `extern` declares functions that come from another language (almost always C) and sets their calling convention. Together they're how Rust talks to libc, to the OS and to hardware.

Related: [`../notions/PHILOSOPHY.md`](../notions/PHILOSOPHY.md) · [`CONST_STATIC.md`](CONST_STATIC.md) · [`../notions/tooling/CARGO.md`](../notions/tooling/CARGO.md)

---

## 1. The five superpowers

Inside `unsafe`, and only there, you can:

| # | Operation | Why the compiler can't check it |
|---|---|---|
| 1 | Dereference a raw pointer (`*const T` / `*mut T`) | It could be null, dangling or misaligned, like any C pointer |
| 2 | Call an `unsafe fn` (including any FFI function) | The function has preconditions the compiler doesn't know about |
| 3 | Read or write a `static mut` | It could be a data race |
| 4 | Implement an `unsafe trait` (`Send`, `Sync`…) | You're vouching for a property of the whole type |
| 5 | Read a field of a `union` | Nothing records which field is currently valid |

Everything else (borrowing, types, lifetimes, bounds checks on slices) **stays checked** inside `unsafe`.

---

## 2. Raw pointers: C pointers, back again

```rust
fn main() {
    let mut x = 10;
    let p = &raw mut x;         // creating a raw pointer is safe...
    let q = &raw const x;
    unsafe {
        *p += 1;                // ...dereferencing it is not
        println!("{}", *q);
    }
    let null: *const i32 = std::ptr::null();
    println!("{}", null.is_null());
}
```

| Rust | C |
|---|---|
| `*const T` | `const T *` |
| `*mut T` | `T *` |
| `std::ptr::null()` | `NULL` |
| `p.add(n)` | `p + n` |
| `&raw const x` / `&raw mut x` | `&x` (without creating a reference first) |

Raw pointers are `Copy`, can be null, carry no lifetime and aren't tracked by the borrow checker. They're exactly C pointers.

---

## 3. `extern`: calling C

```rust
unsafe extern "C" {                          // 2024 edition: the block is marked unsafe
    fn abs(x: i32) -> i32;                   // from libc: calling it needs unsafe
    safe fn labs(x: i64) -> i64;             // `safe`: you vouch it can be called from safe code
}

fn main() {
    let a = unsafe { abs(-5) };
    let b = labs(-7);
    println!("{a} {b}");
}
```

`extern "C"` means *"use the C ABI"*: the platform's C calling convention and symbol names. The same string, placed on a Rust function, lets **C call into Rust**:

```rust
#[unsafe(no_mangle)]                         // keep the symbol name plain: `rs_add`, not a mangled one
pub extern "C" fn rs_add(a: i32, b: i32) -> i32 {
    a + b
}
```

That's `extern "C"` in a C++ header, the same keyword with the same meaning. Linking against a real library (`-lft`) is done through a `build.rs` script or `#[link(name = "ft")]`. See [`CARGO`](../notions/tooling/CARGO.md).

---

## 4. `unsafe fn` and the "safe wrapper" pattern

```rust
/// # Safety
/// `ptr` must point to at least `len` valid, initialised `i32`s.
unsafe fn sum_raw(ptr: *const i32, len: usize) -> i32 {
    let mut s = 0;
    for i in 0..len {
        s += unsafe { *ptr.add(i) };         // 2024: an unsafe op still needs its own block
    }
    s
}

fn sum(v: &[i32]) -> i32 {
    unsafe { sum_raw(v.as_ptr(), v.len()) }   // a slice GUARANTEES the precondition
}

fn main() {
    println!("{}", sum(&[1, 2, 3]));
}
```

This pattern is the core of the whole language. Put the unsafe code in one small function, write down its contract (`# Safety`), and expose a **safe** API whose types make the contract impossible to break. `Vec`, `String` and `HashMap` are all built this way.

---

## 5. How it compares to C

| | C | Rust `unsafe` |
|---|---|---|
| Where the danger is | Every line | Only inside the `unsafe` blocks, which you can grep for |
| Borrow checker | none | Still runs, for references |
| UB if you're wrong | Yes | Yes. Same consequences |
| Where to point valgrind / ASan | Everywhere | Mostly at the `unsafe` blocks. Also: Miri (`cargo +nightly miri test`) |

> Rule of thumb: while you're learning, you should almost never write `unsafe`. If you reach for it to get past a borrow error, the design is what needs to change.
