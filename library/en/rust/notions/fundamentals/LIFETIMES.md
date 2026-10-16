# ⏳ Lifetimes: How Long a Reference Is Valid

> **TL;DR.** A **lifetime** (`'a`) names a region of code where a reference is valid. The compiler infers nearly all of them. You only write them when a function **returns a reference** and the compiler can't tell which input it came from, or when a **struct holds a reference**. Lifetimes don't change how long anything lives. They only *describe* relationships, so the checker can reject dangling references.

Related: [`BORROWING.md`](BORROWING.md) · [`OWNERSHIP.md`](OWNERSHIP.md) · [`../../lexique/CONST_STATIC.md`](../../lexique/CONST_STATIC.md) (`'static`)

---

## 1. The bug they rule out

In C++98 this compiles, maybe with a warning, and it's UB:

```cpp
const std::string& pick() {
    std::string local = "temp";
    return local;            // reference to a dead stack frame
}
```

In Rust:

```rust,compile_fail
fn pick() -> &String {       // error[E0106]: missing lifetime specifier
    let local = String::from("temp");
    &local
}
fn main() { pick(); }
```

The compiler asks *"what does this reference borrow **from**?"* There's no input to borrow from, and `local` dies at `}`, so there's no valid answer. The fix is to **return the owned value**: `fn pick() -> String`.

---

## 2. The notation

```rust
fn longest<'a>(x: &'a str, y: &'a str) -> &'a str {
    if x.len() >= y.len() { x } else { y }
}

fn main() {
    let a = String::from("long string");
    let res;
    {
        let b = String::from("short");
        res = longest(&a, &b);
        println!("{res}");            // OK: a and b are both alive here
    }
    // println!("{res}");             // error: `b` does not live long enough
}
```

Read `<'a>` as *"for some region `'a`…"*. The signature says: *the result is valid for as long as **both** inputs are*. Each call picks `'a` as the **overlap** of the two borrows.

```
   a: ├──────────────────────────────────────┤
   b:        ├────────────┤
  'a:        ├────────────┤   ← the overlap: res can't be used past here
```

The lifetime is part of the **contract**. The caller doesn't need to read the function body to know how long the result is valid.

---

## 3. Elision: why you almost never write them

The compiler fills in lifetimes on function signatures using three rules:

| Rule | Effect |
|---|---|
| 1 | Each reference parameter gets its own lifetime |
| 2 | If there's **exactly one** input lifetime, the output gets it |
| 3 | If there's a `&self` or `&mut self`, the output gets `self`'s lifetime |

```rust
fn first_word(s: &str) -> &str {           // rule 2: output tied to s
    s.split(' ').next().unwrap_or("")
}

struct Doc { text: String }
impl Doc {
    fn title(&self, _fallback: &str) -> &str { // rule 3: output tied to self
        self.text.lines().next().unwrap_or("")
    }
}

fn main() {
    let d = Doc { text: "Title\nbody".into() };
    println!("{} {}", first_word("hi there"), d.title("x"));
}
```

`longest` needed `'a` because it has **two** reference inputs and no `self`, so none of the rules decides.

---

## 4. Structs that hold references

A struct that borrows must say for how long, and it can't outlive what it borrows:

```rust
struct Parser<'a> {
    input: &'a str,     // borrows the input; no copy made
    pos: usize,
}

impl<'a> Parser<'a> {
    fn next_token(&mut self) -> Option<&'a str> {
        let rest = &self.input[self.pos..];
        let rest = rest.trim_start();
        if rest.is_empty() { return None; }
        let start = self.input.len() - rest.len();
        let end = rest.find(' ').map_or(self.input.len(), |i| start + i);
        self.pos = end;
        Some(&self.input[start..end])
    }
}

fn main() {
    let src = String::from("let x = 5");
    let mut p = Parser { input: &src, pos: 0 };
    while let Some(t) = p.next_token() { print!("[{t}] "); }
    println!();
}
```

This is **zero-copy parsing**: tokens are slices of the original buffer, like C `char *` + length pairs into one big string, except you can't free the buffer while a token still points into it.

> Beginner advice: while you're learning, make structs **own** their data (`String`, not `&str`). Borrowing structs are an optimisation to add once the design is settled.

---

## 5. `'static`

`'static` = *"valid for the entire program"*.

```rust
fn greeting() -> &'static str {
    "hello"                       // string literals live in the binary: always valid
}

fn main() {
    let s: &'static str = greeting();
    let leaked: &'static mut Vec<i32> = Box::leak(Box::new(vec![1])); // deliberately never freed
    leaked.push(2);
    println!("{s} {leaked:?}");
}
```

| You see | Means |
|---|---|
| `&'static str` | A reference that's valid forever (literals, leaked memory, `static` items) |
| `T: 'static` (a bound) | T **contains no non-static borrows**. `String` qualifies because it owns its data! |

`thread::spawn` requires `F: 'static`. That's why you `move` data into threads. See [`MOVE`](../../lexique/MOVE.md).

---

## 6. Mental model

```
  Ownership  → who frees it
  Borrowing  → who may look at it or change it right now
  Lifetimes  → for how long a look is valid
```

Lifetimes are **descriptive, not prescriptive**. If the compiler complains, you can't fix it by adding annotations until it's quiet. The fix is to make the data live longer (move the owner up a scope), or to stop borrowing (return an owned value, or clone).
