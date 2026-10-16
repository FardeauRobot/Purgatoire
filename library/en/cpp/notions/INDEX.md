# 🧠 NOTIONS — Concepts by Theme

The zoom-out layer: each file explains one concept properly — mental model, diagrams, worked examples, gotchas. Keywords get their zoom-in pages in [`../lexique/`](../lexique/INDEX.md); exercise-driven reading lives in [`../projets/`](../projets/INDEX.md).

> 🌟 **Start here:** [`PHILOSOPHY.md`](PHILOSOPHY.md) — the 5 pillars of C++ and the mindset for writing correct code. Re-read before each new module.

---

## 🧱 `fundamentals/` — the base layer

| File | Topic |
|---|---|
| [`BASICS.md`](fundamentals/BASICS.md) | Core syntax, compilation unit, namespaces — the C→C++ shift. |
| [`CONTRACTS.md`](fundamentals/CONTRACTS.md) | The mental model behind `const`, `private`, `explicit`: promises the compiler enforces. |
| [`REFERENCE.md`](fundamentals/REFERENCE.md) | References vs pointers, lvalue semantics. |
| [`MEMORY.md`](fundamentals/MEMORY.md) | Stack vs heap, `new`/`delete`, RAII, lifetime, valgrind. |
| [`ALLOCATION_FAILURE.md`](fundamentals/ALLOCATION_FAILURE.md) | When `new` fails — `bad_alloc` vs NULL, `operator new` internals. |
| [`STRING.md`](fundamentals/STRING.md) | `std::string` deep dive — SSO vs COW, growth, the `char*` bridge. |
| [`CMATH.md`](fundamentals/CMATH.md) | `<cmath>` — powers, roots, trig, rounding; C++98 vs C++11. |

## 🧬 `oop/` — object orientation

| File | Topic |
|---|---|
| [`ORTHODOX_CANONICAL_FORM.md`](oop/ORTHODOX_CANONICAL_FORM.md) | The big four + default ctor. The 42 must-know. |
| [`GETTERS_SETTERS.md`](oop/GETTERS_SETTERS.md) | Encapsulation patterns. |
| [`OPERATOR_OVERLOADING.md`](oop/OPERATOR_OVERLOADING.md) | Member vs free operators, rule of symmetry. |
| [`INHERITANCE.md`](oop/INHERITANCE.md) | Public/protected/private inheritance, diamonds. |
| [`POLYMORPHISM.md`](oop/POLYMORPHISM.md) | Virtual, vtables, pure virtual, abstract classes. |

## ⚙️ `advanced/` — the harder stuff

| File | Topic |
|---|---|
| [`TEMPLATES.md`](advanced/TEMPLATES.md) | Function + class templates, specialisation. |
| [`STL.md`](advanced/STL.md) | Containers + iterators + algorithms — the overview hub. |
| [`ALGORITHMS.md`](advanced/ALGORITHMS.md) | Exhaustive `<algorithm>`/`<numeric>` reference, C++98-annotated. |
| [`MEMBER_FUNCTION_POINTERS.md`](advanced/MEMBER_FUNCTION_POINTERS.md) | `.*` / `->*`, dispatch tables, the Harl pattern. |

*The four casts moved to the lexique: [`lexique/CASTS.md`](../lexique/CASTS.md).*

## 📦 `containers/` — one page per STL container

Mental model + ASCII diagram + full API + complexity + iterator invalidation, for each. **Hub: [`containers/INDEX.md`](containers/INDEX.md)** — `vector`, `list`, `deque`, `stack`, `queue`, `priority_queue`, `set`, `multiset`, `map`, `multimap`.

## 🌊 `io-errors/` — streams & failure

| File | Topic |
|---|---|
| [`FSTREAM_GUIDE.md`](io-errors/FSTREAM_GUIDE.md) | `ifstream`/`ofstream`/`stringstream`. |
| [`OPEN.md`](io-errors/OPEN.md) | File opening flags and modes. |
| [`STRING_FUNCTIONS.md`](io-errors/STRING_FUNCTIONS.md) | `std::string` API cheat sheet (deep dive: [`fundamentals/STRING.md`](fundamentals/STRING.md)). |
| [`ERROR_MANAGEMENT.md`](io-errors/ERROR_MANAGEMENT.md) | Exceptions, `try`/`catch`, safety guarantees. |

## 🛠️ `tooling/` — build & link

| File | Topic |
|---|---|
| [`MAKEFILE_CPP.md`](tooling/MAKEFILE_CPP.md) | C++ Makefile recipes, `-Wall -Wextra -Werror -std=c++98`. |
| [`LIBRARIES.md`](tooling/LIBRARIES.md) | Static vs shared libraries, linking order. |

---

## 🧭 Suggested reading orders

- **Starting the piscine:** `PHILOSOPHY` → `fundamentals/BASICS` → `fundamentals/MEMORY` → `oop/ORTHODOX_CANONICAL_FORM`
- **Inheritance not clicking:** `oop/INHERITANCE` → `oop/POLYMORPHISM` → [`lexique/VIRTUAL.md`](../lexique/VIRTUAL.md)
- **Entering Mod08 / containers:** `advanced/STL` → `containers/INDEX` → `advanced/ALGORITHMS`
