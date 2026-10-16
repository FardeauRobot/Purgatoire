# 🚩 Compiler Flags — C++ piscine reference

A working catalogue of the compiler flags worth knowing for the 42 C++ piscine. Sorted by **how much value you'll get** per flag, with the rationale, the trade-off, and where each one lives in the repo's templates.

> **Default baseline (in `templates/cpp/{minimal,basic,complete}/Makefile`):**
> ```make
> CXXFLAGS = -Wall -Wextra -Werror -Wswitch \
>            -Wpedantic -Wshadow -Wnon-virtual-dtor -Wold-style-cast \
>            -std=c++98 -MMD -MP
> ```

---

## 🟢 Tier 0 — the non-negotiables (always on)

| Flag | What it does | Why it's mandatory in 42 |
|---|---|---|
| `-Wall` | Enables a curated set of common warnings. | Subject mandate. |
| `-Wextra` | Enables a second tier of less-common warnings. | Subject mandate. |
| `-Werror` | Promotes every warning to an error — the build refuses to produce a binary while *anything* is unaddressed. | Subject mandate. **The single most useful flag.** Without it, warnings rot. |
| `-std=c++98` | Locks the language standard to C++98. | Subject mandate. Pair with `-Wpedantic` to enforce it. |

---

## 🟡 Tier 1 — added by default in this repo's templates

These four are the **highest-value warnings the subject doesn't strictly require**. They catch real bugs you will write in this piscine.

### `-Wswitch`
**Catches:** `switch` on an enum where a case is missing.
**Why useful:** When you add a new enum value (e.g. a new `LogLevel`), the compiler shows you every `switch` you forgot to update. Essential for the Harl exercise (CPP01) and any state-machine code.
**Cost:** ~0. No false positives.

### `-Wpedantic`
**Catches:** anything not in pure standard C++98 — compiler extensions, GNU-isms, designated initialisers, accidental C++11+ features (`auto`, range-`for`, `nullptr`, `=default`, …).
**Why useful:** Your hard constraint is C++98. Apple Clang supports C++11+ even with `-std=c++98` unless `-Wpedantic` is on. This flag is the only thing that screams when you drift.
**Cost:** Occasionally complains about something you didn't realise was an extension (variadic macros, `long long`, …). When it does, you usually want to fix the code, not silence the warning.

### `-Wshadow`
**Catches:** a local variable, parameter, or member that has the same name as one in an outer scope.
**Why useful:** The classic 42 footgun is
```cpp
ScavTrap::ScavTrap(std::string name) : _name(name) {
    std::string name = "default";   // ← shadows the parameter
}
```
or
```cpp
for (int i = 0; i < n; i++) {
    for (int i = 0; i < m; i++) { … }  // ← inner i shadows outer
}
```
Silent under `-Wall -Wextra`. Loud under `-Wshadow`.
**Cost:** Slightly annoying around member-initialiser lists where the param is intentionally the same name as the member without an `_` prefix. Solution: actually use the `_` prefix.

### `-Wnon-virtual-dtor`
**Catches:** a class with virtual functions but a non-virtual destructor.
**Why critical from CPP03 onward:** If you `delete basePtr;` and the destructor isn't virtual, the derived-class destructor is **skipped**. That means: leaked members, unreleased resources, and a valgrind report you'll spend an hour debugging.
**Cost:** None in well-formed inheritance trees.
**Cross-reference:** [`cpp/oop/INHERITANCE.md`](../cpp/notions/oop/INHERITANCE.md), [`cpp/oop/POLYMORPHISM.md`](../cpp/notions/oop/POLYMORPHISM.md).

### `-Wold-style-cast`
**Catches:** any C-style cast `(T)x` instead of the C++ alternatives (`static_cast`, `const_cast`, `reinterpret_cast`, `dynamic_cast`).
**Why useful:** The subject explicitly wants you to use C++ casts. C-style casts are dangerous because they silently combine all four cast operations — `(T*)p` will happily reinterpret a pointer as a different type when you meant a polite upcast.
**Cost:** Forces you to spell out intent at every cast. Which is the point.
**Cross-reference:** [`cpp/advanced/CASTS.md`](../cpp/lexique/CASTS.md).

---

## 🟠 Tier 2 — useful but situational

Worth turning on by hand for an exercise where you want to learn deeply. Maybe too noisy to leave on permanently.

### `-Wconversion`
**Catches:** implicit narrowing conversions, e.g. `float → int`, `int → short`, `size_t → int`.
**Example it would have caught for me:** in `RemakeCPP02/ex02` I had `m_raw = roundf(nb * 256);` — `roundf` returns `float`, assigning to `int` is an implicit narrowing. `-Wall -Wextra` is silent; `-Wconversion` flags it. Fix is `static_cast<int>(roundf(…))`.
**Why didactic:** you'll genuinely understand C++'s implicit conversion rules after one painful round with this flag.
**Trade-off:** can be noisy on existing codebases. Best used on a fresh exercise.

### `-Wfloat-equal`
**Catches:** `==` or `!=` between floating-point values.
**Why:** float equality is almost never what you want — `0.1 + 0.2 != 0.3`. Use an epsilon comparison.
**Relevance:** CPP02 `Fixed`, any geometry/physics code.

### `-Woverloaded-virtual`
**Catches:** a derived class declares a function with the same name as a base virtual but a different signature — silently *hiding* the base version instead of overriding it.
**Why critical from CPP04 onward:** without `override` (which doesn't exist in C++98), this is your only line of defence against the "I forgot to match the signature" bug.

### `-Wundef`
**Catches:** `#if FOO` where `FOO` was never defined — silently treated as 0.
**Why:** typo'd macro names (`#if DEBG` instead of `#if DEBUG`) compile fine and silently take the wrong branch.

---

## 🔵 Tier 3 — debug builds

Set on a separate target so production builds stay fast. Already wired into `make debug` and `make asan` in this repo's templates.

| Flag | Purpose |
|---|---|
| `-g3` | Maximum debug info — variable names, macro definitions, source line tables. Needed for any decent gdb/lldb session. |
| `-O0` | No optimisation. Stack frames, variables, and execution order stay where you expect — essential for stepping through code or reading valgrind traces. |
| `-fsanitize=address` | **ASan** — catches heap-use-after-free, double-free, heap/stack buffer overflow, leaks (on Linux + recent macOS). Runtime cost ~2×. |
| `-fsanitize=undefined` | **UBSan** — catches signed overflow, null dereference, invalid casts, shift errors. Runtime cost negligible. |
| `-fno-omit-frame-pointer` | Keeps the frame pointer register live so stack traces under sanitizers / profilers stay readable. |

> **Why both `ASan` and `valgrind`?** They catch overlapping but not identical sets of bugs. Run both. ASan is faster and gives clearer reports for the things it sees; valgrind sees a few things ASan doesn't (uninitialised reads via `--track-origins=yes`, for example).

---

## 🟣 Tier 4 — build system, not strictly warnings

| Flag | Purpose |
|---|---|
| `-MMD` | Generate a `.d` file alongside each `.o` listing the header dependencies. |
| `-MP` | Add a phony target for each header — prevents "no rule to make target X.hpp" errors if you delete a header. |
| `-I include` | Add `include/` to the header search path. Used in the `basic` / `complete` templates that have an `include/` directory. |

The pair `-MMD -MP` plus a `-include $(DEPS)` line in the Makefile is what makes `make` rebuild a `.cpp` when one of its headers changes. Without it, `make` only tracks `.cpp` → `.o` edges and you get stale builds.

---

## ⛔ Things to **not** enable

| Flag | Why avoid |
|---|---|
| `-Weverything` | Clang only. Enables literally every warning, including mutually-contradictory ones. Useful to **browse** for a flag you've forgotten the name of; never to compile with. |
| `-O3` / `-O2` (for piscine) | Pointless — exercises aren't perf-bound — and it disables some debug info. You want **legibility** of backtraces, not speed. |
| `-fpermissive` | Does the opposite of what you want: downgrades errors to warnings. Avoid. |
| `--no-verify` *(git, not compiler — included for symmetry)* | Skips pre-commit hooks. If a hook fails, fix the underlying issue. |

---

## 🧪 Quick experiment — see a warning in action

To see what a specific flag does, the fastest path is a tiny test file:

```sh
# create a 4-line test that demonstrates a narrowing conversion
cat > /tmp/test.cpp <<'EOF'
int main() { int x = 1.5f; return x; }
EOF

# stock flags — silent
c++ -Wall -Wextra -Werror -std=c++98 /tmp/test.cpp -o /tmp/test
# strict flags — error
c++ -Wall -Wextra -Werror -Wconversion -std=c++98 /tmp/test.cpp -o /tmp/test
```

Use this pattern any time you wonder *"what would `-Wxxx` catch?"* — write the smallest program that triggers it, toggle the flag, compare.

---

## 🔗 Related

- [`meta/IMPROVEMENT.md`](IMPROVEMENT.md) — repo-wide upgrade checklist (template polish, missing notes).
- [`cpp/tooling/MAKEFILE_CPP.md`](../cpp/notions/tooling/MAKEFILE_CPP.md) — full Makefile recipe walkthrough.
- [`cpp/advanced/CASTS.md`](../cpp/lexique/CASTS.md) — why `-Wold-style-cast` matters in detail.
- [`cpp/oop/INHERITANCE.md`](../cpp/notions/oop/INHERITANCE.md) / [`POLYMORPHISM.md`](../cpp/notions/oop/POLYMORPHISM.md) — why `-Wnon-virtual-dtor` matters from CPP03 onward.
