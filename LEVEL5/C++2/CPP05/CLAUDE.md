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

Current state: `ex00/`, `ex01/` and `ex02/` are **complete** — all build warning-free under
the strict flag set and report zero leaks. `ex03/` does not exist yet.

- **ex00** — `Bureaucrat` fully implemented (attributes, accessors,
  `increment/decrementGrade`, both nested exceptions, `operator<<`).
- **ex01** — `Form` (const `_name`, `bool _signed`, const `_gradeToSign`,
  const `_gradeToExecute`), its two nested exceptions, getters, `beSigned()`,
  `Bureaucrat::signForm()`, `operator<<`. Note `_gradeToExecute` is stored, bounds-checked
  and printed but **never read** in ex01 — nothing executes a form until `AForm::execute()`
  in ex02.
- **ex02** — abstract `AForm` (`executeAction() const = 0`, protected) plus the three
  concrete forms. `execute()` is public, non-virtual, does the *signed* then *grade* checks
  and only then calls `executeAction()` — the second of the two techniques the eval grid
  explicitly allows. `AForm` has a virtual destructor, so `delete` through an `AForm*`
  destroys the derived part too. `AForm`'s own bound exceptions are unreachable in ex02
  (the concrete forms hardcode their grades); only a subclass passing bad constants would
  fire them.

## Conventions established in ex01 (carry into ex02/ex03)

**Const members drive the OCF.** `Form` has three `const` members, so the copy constructor
*must* initialize everything in its init list (`*this = src` in the body cannot compile),
and `operator=` can only assign `_signed`. After `a = b`, `a` keeps its own name and both
grades. Same shape as `Bureaucrat::operator=`, which only assigns `_grade`. `AForm` in ex02
inherits this constraint — expect to explain it at defense.

**Forward-declare across the Bureaucrat/Form boundary.** `Form::beSigned(const Bureaucrat&)`
and `Bureaucrat::signForm(Form&)` make the two headers mutually dependent. Both headers
use `class X;` and put the real `#include` in their `.cpp`; reference parameters need only
the name, not the definition. Without this you get a circular include with a misleading
error message.

**Exceptions carry a finished message.** The nested exception constructors take the full
`what()` string rather than building it from a name, so each throw site can say something
accurate — the constructor reports an out-of-bounds *form* grade, `beSigned` reports an
insufficient *bureaucrat* grade.

**Who prints what.** `beSigned()` decides and throws; `signForm()` catches and prints
either `X signed Y` or `X couldn't sign Y because <reason>`. Keep the decision and the
reporting in separate methods.

The bound messages describe the *numeric* range: a grade above 150 is "too low, must be
at most 150", a grade below 1 is "too high, must be at least 1". (They used to be inverted;
fixed across `Bureaucrat`, `Form` and `AForm` in all three exercises.)

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
make            # build the binary (ex00 target is `bureaucrat`, ex01 is `form`)
make re         # rebuild from scratch
make debug      # rebuild with -g3 -O0
make asan       # rebuild with AddressSanitizer + UBSan, then run ./binary
make leaks      # -g build, then `leaks --atExit` on macOS / valgrind on Linux
make watch      # auto-rebuild + run on source change (needs fswatch)
make toolow     # build+run with -DTOO_LOW=1
make toohigh    # build+run with -DTOO_HIGH=1
make tests      # build+run with every case enabled
make help       # list all targets
```

In **ex00 / ex01** the `toolow` / `toohigh` / `tests` targets are cosmetic: `main.cpp`
guards the macros with `#ifndef TOO_LOW / # define TOO_LOW 1` and the targets only ever
set a macro to `1`, so both blocks compile in unconditionally and all three targets print
the same thing.

**ex02 does isolate its cases.** `main.cpp` there is split into six independent blocks
(`TEST`, `SHRUBBERY`, `ROBOTOMY`, `PARDON`, `TOO_LOW`, `TOO_HIGH`), each in its own
`{ }` scope so it is self-contained, and each target names **all six** macros — the one it
wants at `1`, the other five at `0`. Naming each macro exactly once is mandatory, not
tidiness: a repeated `-DFOO=0 -DFOO=1` on the command line is `-Wmacro-redefined`, which
`-Werror` turns into a build failure. Targets: `make base`, `shrubbery`, `robotomy`,
`pardon`, `toolow`, `toohigh`, and `make full` for everything (`tests` / `test` are
aliases of `full`). `ShrubberyCreationForm` writes `<target>_shrubbery` into the cwd, so
ex02's `clean` also does `rm -f *_shrubbery`.

There is no test framework — verification is running the binary, eyeballing the OCF
traces, and confirming `make leaks` reports zero leaks before declaring an exercise done.

Note: this Makefile is stricter than the flag set described in the root `CLAUDE.md` —
it also enables `-Wpedantic -Wshadow -Wnon-virtual-dtor -Wold-style-cast` and uses
`-MMD -MP` header-dependency tracking. `-Wold-style-cast` in particular means
C-style casts will fail the build; use C++ casts (`static_cast`, etc.).
