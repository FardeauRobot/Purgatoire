# 🚨 Error Handling: No Exceptions, Just Values

> **TL;DR.** Rust has **no exceptions**. A function that can fail returns `Result<T, E>` (`Ok(value)` or `Err(error)`), and a value that may be absent is `Option<T>`. The `?` operator propagates an error to the caller in one character. `panic!` is for **bugs** (broken invariants), not for expected failures. Either way, the possibility of failure is **in the type**.

Related: [`../../lexique/ENUM.md`](../../lexique/ENUM.md) · [`../../lexique/MATCH.md`](../../lexique/MATCH.md) · C++ side: [`ERROR_MANAGEMENT.md`](../../../cpp/notions/io-errors/ERROR_MANAGEMENT.md), [`TRY_CATCH_THROW.md`](../../../cpp/lexique/TRY_CATCH_THROW.md)

---

## 1. C, C++ and Rust

| | C | C++98 | Rust |
|---|---|---|---|
| Mechanism | `return -1;` + `errno` | `throw` / `try` / `catch` | `return Err(e);` |
| Visible in the signature? | No (convention) | No (a `throw()` spec is barely checked) | **Yes**: `-> Result<T, E>` |
| Can you ignore it? | Easily | Easily: it just propagates | Warning: `unused Result that must be used` |
| Propagation | `if (r < 0) return r;` by hand | Automatic and invisible | `?`: automatic and **visible** |
| Cost when nothing fails | None | ~none (table-based unwinding) | None (it's a return value) |
| Cost when it fails | Cheap | Expensive (unwinding) | Cheap (a branch) |

Rust takes C's **errors are return values** and adds C++'s **you can't forget about them**, then makes it short to write with `?`.

---

## 2. `Result` and `?`

```rust
use std::num::ParseIntError;

fn parse_pair(s: &str) -> Result<(i32, i32), ParseIntError> {
    let mut it = s.split(',');
    let a = it.next().unwrap_or("").trim().parse::<i32>()?;  // on Err: return it right away
    let b = it.next().unwrap_or("").trim().parse::<i32>()?;
    Ok((a, b))
}

fn main() {
    match parse_pair("3, 4") {
        Ok((a, b)) => println!("sum = {}", a + b),
        Err(e) => println!("error: {e}"),
    }
    match parse_pair("3, x") {
        Ok(p) => println!("{p:?}"),
        Err(e) => println!("error: {e}"),     // invalid digit found in string
    }
}
```

What `?` does:

```
  let a = expr?;
      ≡
  let a = match expr {
      Ok(v)  => v,
      Err(e) => return Err(e.into()),   // .into() converts to the function's error type
  };
```

It also works on `Option` inside functions that return `Option`: a `None` makes the function return `None`.

---

## 3. Your own error type

The idiomatic replacement for a C++ exception hierarchy is an **enum**, not a class tree:

```rust
use std::fmt;

#[derive(Debug)]
enum BureauError {
    GradeTooHigh(u8),
    GradeTooLow(u8),
}

impl fmt::Display for BureauError {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        match self {
            BureauError::GradeTooHigh(g) => write!(f, "grade {g} is too high"),
            BureauError::GradeTooLow(g) => write!(f, "grade {g} is too low"),
        }
    }
}

impl std::error::Error for BureauError {}   // makes it a "real" error type

#[derive(Debug)]
struct Bureaucrat { name: String, grade: u8 }

impl Bureaucrat {
    fn new(name: &str, grade: u8) -> Result<Self, BureauError> {   // CPP05, reborn
        match grade {
            0 => Err(BureauError::GradeTooHigh(grade)),
            151.. => Err(BureauError::GradeTooLow(grade)),
            _ => Ok(Bureaucrat { name: name.into(), grade }),
        }
    }
    fn promote(&mut self) -> Result<(), BureauError> {
        if self.grade == 1 { return Err(BureauError::GradeTooHigh(0)); }
        self.grade -= 1;
        Ok(())
    }
}

fn main() {
    for g in [0, 42, 200] {
        match Bureaucrat::new("Bob", g) {
            Ok(b) => println!("{} hired at {}", b.name, b.grade),
            Err(e) => println!("rejected: {e}"),
        }
    }
    let mut top = Bureaucrat::new("Ann", 1).unwrap();
    if let Err(e) = top.promote() { println!("{e}"); }
}
```

Compare this with CPP05's `GradeTooHighException : public std::exception` and its `what()`. The enum is the hierarchy, `Display` plays the part of `what()`, and every caller *sees* in the signature that `new` can fail.

---

## 4. `panic!`: for bugs

```rust
fn main() {
    let v = vec![1, 2, 3];
    let r = std::panic::catch_unwind(|| v[10]);  // catching is possible, but not for control flow
    println!("panicked: {}", r.is_err());
}
```

| Use `Result` when… | Use `panic!` when… |
|---|---|
| The failure is **expected** (bad input, missing file, a network error) | It's a **bug**: an invariant is broken ("this can't happen") |
| The caller can do something about it | Nobody can sensibly recover |
| You're writing a library | Prototypes, tests, `main` in small tools |

A panic unwinds the thread (running every `Drop`, like C++ stack unwinding) and prints a message. Out-of-bounds indexing, integer overflow in debug builds, `unwrap()` on `None`, and explicit `panic!`/`unreachable!`/`todo!` all panic.

---

## 5. `unwrap` and friends

| Method | On `None` / `Err` | When to use it |
|---|---|---|
| `.unwrap()` | panic with a generic message | Tests, prototypes |
| `.expect("why it can't fail")` | panic with **your** message | When you *know* it can't fail. Say why |
| `.unwrap_or(default)` | Returns the default | A sensible fallback exists |
| `.unwrap_or_else(\|\| compute())` | Calls the closure | The fallback is expensive to build |
| `.unwrap_or_default()` | `T::default()` | 0, "", an empty Vec |
| `?` | Propagates it | Inside a function that returns `Result` or `Option` |
| `.ok()` / `.ok_or(err)` | Converts `Result` ↔ `Option` | Changing which "failure language" you speak |
| `.map(f)` / `.and_then(f)` | Transforms the success value | Chaining without a `match` |

```rust
fn main() {
    let port: u16 = std::env::var("PORT").ok()
        .and_then(|s| s.parse().ok())
        .unwrap_or(8080);                        // webserv-style config fallback
    let n: i32 = "12".parse().expect("literal is a valid number");
    println!("{port} {n}");
}
```

---

## 6. `main` can return a `Result`

```rust
use std::fs;

fn main() -> Result<(), Box<dyn std::error::Error>> {   // any error type fits in the Box
    let text = fs::read_to_string("Cargo.toml").unwrap_or_default();
    let first: i32 = "7".parse()?;                        // ? works in main too
    println!("{} bytes, {first}", text.len());
    Ok(())
}
```

If `main` returns `Err`, the program prints the error and exits with status 1. `Box<dyn Error>` is the quick "any error" type for binaries. For real projects, the `anyhow` (applications) and `thiserror` (libraries) crates remove the boilerplate.
