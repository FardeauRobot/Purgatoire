# CLAUDE.md

Project-level instructions for Claude Code working in **Purgatoire** — a 42 common-core repository. Claude reads this on every session in this directory.

---

## Context

I'm a **42 student on macOS (Apple Silicon)**, currently working through the C++ piscine (LEVEL4/C++1, Mod00 → Mod05). I'm learning C++ from scratch — explanations should be **didactic**, not just code dumps. Default to the *Explanatory* output style mindset: when you write code, briefly tell me **why** the choice is right.

Map of the repo: see `README.md`. Knowledge library: see `library/INDEX.md`.

---

## Hard constraints

### C++98 only — no exceptions
LEVEL4/C++1 follows the 42 C++ subject strictly. **Never** use:
- `auto`, `nullptr`, range-`for`, lambdas, `<thread>`, `<chrono>`, `<unordered_*>`
- `using` aliases, brace init lists, `override`, `final`, `=default`, `=delete`
- C++11+ features in general, even if Apple Clang supports them

When in doubt, ask before introducing a feature. If you spot drift toward C++11+, flag it.

### Forbidden APIs (per the C++ subject)
- No `printf` family in C++ — use `std::cout` / `std::cerr`
- No `*alloc` / `free` in C++ — use `new` / `delete`, `new[]` / `delete[]`
- No STL containers in early modules (Mod00–02). They're allowed from Mod08 onward; check the subject.

### Norm & memory
- C code: must pass `norminette` (max 25 lines per function, 5 functions per file, no `for`, no ternary, etc.).
- C++ code: not norminette-checked, but keep style consistent across a file.
- Run `valgrind --leak-check=full` on every C/C++ exercise before declaring it done. Zero leaks, zero "still reachable."

---

## Build conventions

Each project is self-contained. Standard targets across the repo: **`all`, `clean`, `fclean`, `re`**.

### C++ Makefiles
```make
CXX      = c++
CXXFLAGS = -Wall -Wextra -Werror -Wswitch -std=c++98
```
The fancier templates also add `-MMD -MP` for header-dep tracking and a `make debug` / `make asan` target with `-g3 -O0 -fsanitize=address,undefined`.

### Templates live in
- `templates/cpp/{minimal,basic,complete}/` — pick by exercise complexity
- `templates/c_template/` — for C projects, with libft vendored

When I ask you to scaffold a new exercise, copy from the appropriate template — don't write a Makefile from scratch.

### libft
Git submodule at `libft/`. Build with `cd libft && make`. Link with `-L./libft -lft`. Header: `libft/libft.h`.

---

## Style preferences

### Orthodox Canonical Form
Every non-trivial C++ class needs the **four canonical members**: default ctor, copy ctor, copy-assign operator, destructor. When you generate a class, list and implement all four — easy to forget the assignment operator. Reference: `library/cpp/oop/ORTHODOX_CANONICAL_FORM.md`.

### Naming
- Classes: `PascalCase` (e.g. `ScavTrap`)
- Files: match the class name (`ScavTrap.hpp` / `ScavTrap.cpp`)
- Member variables: usually `_underscore_prefixed` in this repo
- Constants & macros: `UPPER_SNAKE`

### Comments
- Default to **no comments**. Don't explain *what* the code does — well-named identifiers do that.
- Only write a comment when the *why* is non-obvious (a subject constraint, a workaround, an invariant).

### Explanations
When teaching me something, prefer:
1. Show the code
2. Walk through what each line does in plain terms
3. Connect it to something I already know (C, libft, prior modules)
4. Mention the gotcha or "why this matters"

---

## Knowledge library

`library/` is my **Obsidian vault** — a single source of truth, no mirror. Every `.md`
note lives there, organized by topic (`cpp/`, `netpractice/`, `projects/`, etc.).
Edit notes in place; there is no second copy to keep in sync.

---

## Workflow

- **Plan first** for non-trivial changes — show me the plan before writing code.
- **One exercise at a time** — don't speed-run several Mod files in one prompt.
- **Show me valgrind / norminette output** when relevant; don't just claim "no leaks."
- **Diff before commit** — I review every change manually. Never `git push` without me asking.
- For long compiler errors, paste the **first error only** — most cascade from there.

---

## Don'ts

- ❌ Don't write helper abstractions I didn't ask for ("just in case").
- ❌ Don't add error handling for impossible cases — only at real boundaries.
- ❌ Don't create new docs files (`*.md`) unless I ask. Knowledge goes into `library/`, organized.
- ❌ Don't run destructive git ops (`reset --hard`, `push --force`, `branch -D`) without explicit confirmation.
- ❌ Don't use `--no-verify` to bypass hooks — fix the underlying issue.

---

## Quick references

| Need | Where |
|---|---|
| C++ keyword reference | `library/cpp/keywords/INDEX.md` |
| OCF / OOP patterns | `library/cpp/oop/` |
| Per-module walkthrough | `library/cpp/modules/CPP0X.md` |
| Personal scripts | `library/scripts/SCRIPTS.md` |
| Claude Code manual | `library/meta/CLAUDE_CODE_MANUAL.md` |
| Repo improvement list | `library/meta/IMPROVEMENT.md` |
