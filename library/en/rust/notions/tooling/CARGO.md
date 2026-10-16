# 🛠️ Cargo: The Makefile You Don't Write

> **TL;DR.** `cargo` is Rust's build system, package manager, test runner and doc generator in one. `cargo new` scaffolds a project, `cargo run` builds and runs it, `cargo test` runs the tests, and `cargo clippy` is your `-Wall -Wextra` on steroids. Dependencies go in `Cargo.toml`, one line each. There's nothing like a Makefile to maintain.

Related: [`../../lexique/MOD_USE_PUB.md`](../../lexique/MOD_USE_PUB.md) · [`../../lexique/UNSAFE_EXTERN.md`](../../lexique/UNSAFE_EXTERN.md) · C++ side: [`MAKEFILE_CPP.md`](../../../cpp/notions/tooling/MAKEFILE_CPP.md), [`FLAGS.md`](../../../meta/FLAGS.md)

---

## 1. Makefile ↔ Cargo

| 42 Makefile | Cargo | Note |
|---|---|---|
| Copy a template, edit `NAME`/`SRCS` | `cargo new my_app` | Creates `Cargo.toml`, `src/main.rs`, and a git repo |
| `make` / `make all` | `cargo build` | Debug build → `target/debug/my_app` |
| `make && ./my_app` | `cargo run -- arg1 arg2` | Arguments go after `--` |
| `-O2` | `cargo build --release` | → `target/release/my_app` |
| `make clean` / `fclean` | `cargo clean` | Removes `target/` |
| `make re` | `cargo clean && cargo build` | Rarely needed: incremental builds are correct |
| `-Wall -Wextra -Werror` | Warnings are on by default; `cargo clippy -- -D warnings` | Clippy has hundreds of extra lints |
| `norminette` | `cargo fmt` | Formats the code instead of just complaining |
| `-MMD -MP` header deps | Automatic | Cargo tracks the module tree itself |
| `make asan` | Mostly unnecessary in safe code; `cargo +nightly miri test` for `unsafe` | |
| vendoring libft as a submodule | `cargo add some_crate` | Downloaded from crates.io and pinned in `Cargo.lock` |
| writing a test `main.cpp` | `cargo test` | Tests live next to the code |
| Doxygen | `cargo doc --open` | Turns `///` comments into HTML |

---

## 2. Project layout

```
my_app/
├── Cargo.toml        ← manifest: name, edition, dependencies
├── Cargo.lock        ← exact resolved versions (commit it for binaries)
├── src/
│   ├── main.rs       ← binary crate root (fn main)
│   ├── lib.rs        ← optional library crate root
│   └── parser.rs     ← a module, declared with `mod parser;`
├── tests/            ← integration tests (each file is its own crate)
└── target/           ← build output (gitignored automatically)
```

```toml
[package]
name = "my_app"
version = "0.1.0"
edition = "2024"          # language edition: like -std=, but set per crate

[dependencies]
rand = "0.9"              # semver: any compatible 0.9.x

[profile.release]
opt-level = 3
debug = true              # keep symbols in release builds (for perf/lldb)
```

---

## 3. The daily loop

```sh
cargo new sandbox && cd sandbox   # scratch project for trying out snippets
cargo run                         # build + run
cargo check                       # type-check only, much faster than build: use it constantly
cargo clippy                      # lints: "you could write this more simply…"
cargo fmt                         # format everything
cargo test                        # run all #[test] functions
rustc --explain E0382             # a long explanation of any error code
```

`cargo check` is the command you'll run most. It does the whole borrow check without generating code, so compile errors show up in about a second.

---

## 4. Tests live next to the code

```rust
pub fn add(a: i32, b: i32) -> i32 { a + b }

#[cfg(test)]                      // only compiled for `cargo test`
mod tests {
    use super::*;

    #[test]
    fn adds() {
        assert_eq!(add(2, 2), 4);
    }

    #[test]
    #[should_panic]
    fn index_out_of_bounds_panics() {
        let v: Vec<i32> = Vec::new();
        let _ = v[0];
    }
}
```

No framework to install and no separate test binary to write. `assert_eq!` prints both values when it fails. Code examples in `///` doc comments are compiled and run as tests too, so the documentation can't drift from the code.

---

## 5. Toolchain management: `rustup`

```sh
rustup update                  # update stable
rustup toolchain install nightly
cargo +nightly miri test       # use a toolchain for one command
rustup component add clippy rustfmt
rustup doc --book              # "The Rust Programming Language", offline
```

Rust releases a new stable version **every 6 weeks**. Editions (2015, 2018, 2021, 2024) are the opt-in, larger changes, and a crate keeps the edition it declares, so old code keeps compiling.

---

## 6. Where to learn next

| Resource | What it is |
|---|---|
| `rustup doc --book` | *The Rust Programming Language*: the official book, free and offline |
| [Rustlings](https://github.com/rust-lang/rustlings) | Small exercises that make you fix compiler errors. Very 42-piscine in spirit |
| *Rust for C++ Programmers* / *Rust by Example* | Snippet-first, good for your background |
| `rustc --explain EXXXX` | Every error code comes with an essay |
