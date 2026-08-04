# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

Scope: **C++ Module 05** of the 42 C++ piscine. The repo-root `CLAUDE.md`
(`Purgatoire/CLAUDE.md`) governs all global rules — C++98-only, forbidden APIs,
Orthodox Canonical Form, naming, valgrind discipline, workflow. This file adds
only what is specific to CPP05; read the root file for everything else.

Deep conceptual walkthrough: `library/cpp/projets/modules/CPP05.md`.

## What this module is about

The real subject is **exception-based error handling** (`throw` / `try` / `catch`,
custom classes deriving from `std::exception` with `what()`), taught through one
continuous class chain built up across the four exercises:

- **ex00** — `Bureaucrat` (const `_name`, `int _grade` where 1 = highest, 150 = lowest;
  throws nested `GradeTooHigh`/`GradeTooLow` exceptions; `operator<<` overload).
- **ex01** — `Form` the Bureaucrat signs (grade-gated, more custom exceptions).
- **ex02** — abstract `AForm` + concrete forms (re-encounter with abstract classes).
- **ex03** — `Intern` that builds forms by name (clean-dispatch, no `if/else` ladder).

Current state: only `ex00/` exists and `Bureaucrat` is still a **stub** — it has just
`_name` and the four OCF members; `_grade`, the accessors, `increment/decrementGrade`,
the exception classes, and `operator<<` are not implemented yet.

## Per-exercise convention

Each `exNN/` is fully self-contained (its own `Makefile`, sources, and a copy of the
shared `utils.{hpp,cpp}`). Build from inside the exercise directory.

`utils.hpp` / `utils.cpp` is a repo-wide debug-tracing pattern, not module logic:
ANSI color macros (`BOLD_CYAN`, `BOLD_RED`, …, `RESET`) plus a custom stream
manipulator `endofline` that prints `RESET` then `std::endl`. Every constructor and
destructor prints a colored OCF trace ending in `endofline` — this is how you verify
construction/destruction order by eye. Keep new classes consistent with it.

## Build & verify (run inside an `exNN/` directory)

```
make            # build the binary (ex00 target is `bureaucrat`)
make re         # rebuild from scratch
make debug      # rebuild with -g3 -O0
make asan       # rebuild with AddressSanitizer + UBSan, then run ./binary
make leaks      # -g build, then valgrind --leak-check=full (this is a Linux/Fedora box)
make watch      # auto-rebuild + run on source change (needs fswatch)
make help       # list all targets
```

There is no test framework — verification is running the binary, eyeballing the OCF
traces, and confirming `make leaks` reports zero leaks before declaring an exercise done.

Note: this Makefile is stricter than the flag set described in the root `CLAUDE.md` —
it also enables `-Wpedantic -Wshadow -Wnon-virtual-dtor -Wold-style-cast` and uses
`-MMD -MP` header-dependency tracking. `-Wold-style-cast` in particular means
C-style casts will fail the build; use C++ casts (`static_cast`, etc.).
