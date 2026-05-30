# 📚 Library

*"A library is but the memory of the realm, kept against forgetfulness."*

All the markdown scattered across Purgatoire, gathered here and sorted. Code stays with its project; the written word lives in this library.

---

## 🗂️ Layout

```
library/
├── INDEX.md          ← you are here
├── meta/             # repo-wide notes
├── scripts/          # workflow tooling docs
├── projects/         # per-project READMEs and TODO lists
├── cpp/              # C++ study notes, sorted by theme
└── netpractice/      # NetPractice project — networking support
```

---

## 📜 Meta

| File | Purpose |
|---|---|
| [`meta/IMPROVEMENT.md`](meta/IMPROVEMENT.md) | Living checklist of repo upgrades — quick wins → long-term craftsmanship. |
| [`meta/CLAUDE_CODE_MANUAL.md`](meta/CLAUDE_CODE_MANUAL.md) | Short manual for using Claude Code efficiently — setup, prompts, slash commands, 42-piscine tips. |
| [`meta/FLAGS.md`](meta/FLAGS.md) | Compiler flags catalogue — tiered by usefulness for the C++ piscine, with rationale & trade-offs. |

---

## 🛠️ Scripts

| File | Purpose |
|---|---|
| [`scripts/SCRIPTS.md`](scripts/SCRIPTS.md) | Catalogue of personal CLI tools (`autopush`, `solopush`, `remind`, `note`, `ADDClass`, `watch-norm`, …). |

---

## 🧩 Projects

### LEVEL 2 — *IPC · algorithms · graphics*
| Project | README | Scratch / TODO |
|---|---|---|
| Pipex | [`pipex.md`](projects/LEVEL2/pipex.md) | [`pipex-fixlist.md`](projects/LEVEL2/pipex-fixlist.md) |
| Push_Swap | [`push_swap.md`](projects/LEVEL2/push_swap.md) | [`push_swap-roadmap.md`](projects/LEVEL2/push_swap-roadmap.md) |
| So_Long | [`so_long.md`](projects/LEVEL2/so_long.md) | — |

### LEVEL 3 — *concurrency · shells*
| Project | README | Scratch / TODO |
|---|---|---|
| Minishell | [`minishell.md`](projects/LEVEL3/minishell.md) | — |
| Philosophers | — | [`philo-restant.md`](projects/LEVEL3/philo-restant.md) |

> *LEVEL 1 and LEVEL 4 projects have no markdown yet — add one using the template below as you finish each.*

---

## ✨ C++ Notes

Sorted by theme, not by module. Cross-reference with `LEVEL4/C++1/ModXX/` when studying.

> **🌟 Start here:** [`cpp/PHILOSOPHY.md`](cpp/PHILOSOPHY.md) — the 5 pillars of C++ and the mindset for writing efficient, correct code. Re-read before each new module.

> **Need a single keyword (`const`, `virtual`, `template`, …)?** Jump to [`cpp/keywords/INDEX.md`](cpp/keywords/INDEX.md) — one focused page per C++98 keyword, with implementation and hardware notes.

### `cpp/fundamentals/` — *the base layer*
| File | Topic |
|---|---|
| [`BASICS.md`](cpp/fundamentals/BASICS.md) | Core syntax, compilation unit, namespaces. |
| [`REFERENCE.md`](cpp/fundamentals/REFERENCE.md) | References vs pointers, lvalue semantics. |
| [`MEMORY.md`](cpp/fundamentals/MEMORY.md) | Stack vs heap, `new`/`delete`, `new[]`/`delete[]`, RAII, lifetime, valgrind. |
| [`CMATH.md`](cpp/fundamentals/CMATH.md) | `<cmath>` — powers, roots, trig, rounding; what's in C++98 vs C++11. |

### `cpp/modules/` — *per-module walkthrough*
| File | Topic |
|---|---|
| [`CPP00.md`](cpp/modules/CPP00.md) | Module 00 — namespaces, classes, stdio. |
| [`CPP01.md`](cpp/modules/CPP01.md) | Module 01 — memory, references, file I/O. |
| [`CPP02.md`](cpp/modules/CPP02.md) | Module 02 — OCF, operator overloading, fixed-point. |
| [`CPP03.md`](cpp/modules/CPP03.md) | Module 03 — inheritance, ctor/dtor chains, the diamond. |
| [`CPP04.md`](cpp/modules/CPP04.md) | Module 04 — virtual dispatch, abstract classes, interfaces. |
| [`WALKTHROUGH.md`](cpp/modules/WALKTHROUGH.md) | End-to-end CPP walkthrough. |

### `cpp/oop/` — *object orientation*
| File | Topic |
|---|---|
| [`ORTHODOX_CANONICAL_FORM.md`](cpp/oop/ORTHODOX_CANONICAL_FORM.md) | The big four + default ctor. |
| [`GETTERS_SETTERS.md`](cpp/oop/GETTERS_SETTERS.md) | Encapsulation patterns. |
| [`OPERATOR_OVERLOADING.md`](cpp/oop/OPERATOR_OVERLOADING.md) | Member vs free operators, rule of symmetry. |
| [`INHERITANCE.md`](cpp/oop/INHERITANCE.md) | Public/protected/private inheritance, diamonds. |
| [`POLYMORPHISM.md`](cpp/oop/POLYMORPHISM.md) | Virtual, vtables, pure virtual. |

### `cpp/advanced/` — *the harder stuff*
| File | Topic |
|---|---|
| [`TEMPLATES.md`](cpp/advanced/TEMPLATES.md) | Function + class templates, specialisation. |
| [`STL.md`](cpp/advanced/STL.md) | High-level overview — containers, iterators, algorithms. Hub linking into `containers/`. |
| [`ALGORITHMS.md`](cpp/advanced/ALGORITHMS.md) | Exhaustive function reference — every container member + the full `<algorithm>`/`<numeric>`/`<functional>` catalogue, C++98-annotated. |
| [`CASTS.md`](cpp/advanced/CASTS.md) | `static_cast`, `dynamic_cast`, `const_cast`, `reinterpret_cast`. |
| [`MEMBER_FUNCTION_POINTERS.md`](cpp/advanced/MEMBER_FUNCTION_POINTERS.md) | `.*` / `->*`, dispatch tables, the Harl pattern. |

### `cpp/containers/` — *one .md per STL container*
Deep-dive notes for every C++98 STL container. Each page: mental model + ASCII diagram + full API + complexity table + iterator invalidation + efficiency tips + worked example. Hub: [`containers/INDEX.md`](cpp/containers/INDEX.md).

| File | Container — one-line |
|---|---|
| [`VECTOR.md`](cpp/containers/VECTOR.md) | `std::vector` — dynamic contiguous array, the default. |
| [`LIST.md`](cpp/containers/LIST.md) | `std::list` — doubly-linked list, O(1) splice, stable iterators. |
| [`DEQUE.md`](cpp/containers/DEQUE.md) | `std::deque` — block-list, O(1) at both ends + random access. |
| [`STACK.md`](cpp/containers/STACK.md) | `std::stack` — LIFO adapter; `pop()` is `void`. |
| [`QUEUE.md`](cpp/containers/QUEUE.md) | `std::queue` — FIFO adapter; vector backing forbidden. |
| [`PRIORITY_QUEUE.md`](cpp/containers/PRIORITY_QUEUE.md) | `std::priority_queue` — binary max-heap on a vector. |
| [`SET.md`](cpp/containers/SET.md) | `std::set` — sorted unique keys, red-black tree. |
| [`MULTISET.md`](cpp/containers/MULTISET.md) | `std::multiset` — like set but duplicates allowed. |
| [`MAP.md`](cpp/containers/MAP.md) | `std::map` — sorted `K → V`; `m[k]` inserts on miss. |
| [`MULTIMAP.md`](cpp/containers/MULTIMAP.md) | `std::multimap` — like map but duplicate keys; no `operator[]`. |

### `cpp/io-errors/` — *streams & failure*
| File | Topic |
|---|---|
| [`FSTREAM_GUIDE.md`](cpp/io-errors/FSTREAM_GUIDE.md) | `ifstream`/`ofstream`/`stringstream`. |
| [`OPEN.md`](cpp/io-errors/OPEN.md) | File opening flags and modes. |
| [`STRING_FUNCTIONS.md`](cpp/io-errors/STRING_FUNCTIONS.md) | `std::string` API cheat sheet. |
| [`ERROR_MANAGEMENT.md`](cpp/io-errors/ERROR_MANAGEMENT.md) | Exceptions, `try`/`catch`, noexcept. |

### `cpp/keywords/` — *one .md per C++98 keyword*
Detailed per-keyword pages: language meaning, compiler/linker implementation, hardware behavior, ASCII diagrams, tips & tricks, common errors. Hub: [`keywords/INDEX.md`](cpp/keywords/INDEX.md).

| Group | Files |
|---|---|
| Storage & qualifiers | [`CONST`](cpp/keywords/CONST.md) · [`STATIC`](cpp/keywords/STATIC.md) · [`VOLATILE`](cpp/keywords/VOLATILE.md) · [`MUTABLE`](cpp/keywords/MUTABLE.md) · [`EXPLICIT`](cpp/keywords/EXPLICIT.md) · [`INLINE`](cpp/keywords/INLINE.md) · [`EXTERN`](cpp/keywords/EXTERN.md) |
| OOP | [`CLASS`](cpp/keywords/CLASS.md) · [`STRUCT`](cpp/keywords/STRUCT.md) · [`PUBLIC/PRIVATE/PROTECTED`](cpp/keywords/PUBLIC_PRIVATE_PROTECTED.md) · [`THIS`](cpp/keywords/THIS.md) · [`FRIEND`](cpp/keywords/FRIEND.md) · [`VIRTUAL`](cpp/keywords/VIRTUAL.md) · [`OPERATOR`](cpp/keywords/OPERATOR.md) |
| Memory · templates · scope | [`NEW`](cpp/keywords/NEW.md) · [`DELETE`](cpp/keywords/DELETE.md) · [`TEMPLATE`](cpp/keywords/TEMPLATE.md) · [`TYPENAME`](cpp/keywords/TYPENAME.md) · [`NAMESPACE`](cpp/keywords/NAMESPACE.md) · [`TYPEDEF`](cpp/keywords/TYPEDEF.md) · [`USING`](cpp/keywords/USING.md) · [`ENUM`](cpp/keywords/ENUM.md) |
| Exceptions & types | [`TRY/CATCH/THROW`](cpp/keywords/TRY_CATCH_THROW.md) · [`SIZEOF`](cpp/keywords/SIZEOF.md) · [`TYPEID`](cpp/keywords/TYPEID.md) · [`VOID`](cpp/keywords/VOID.md) · [`BOOL`](cpp/keywords/BOOL.md) |

### `cpp/tooling/` — *build & link*
| File | Topic |
|---|---|
| [`MAKEFILE_CPP.md`](cpp/tooling/MAKEFILE_CPP.md) | C++ Makefile recipes, `-Wall -Wextra -Werror -std=c++98`. |
| [`LIBRARIES.md`](cpp/tooling/LIBRARIES.md) | Static vs shared libraries, linking order. |

---

## 🌐 NetPractice

Everything needed to validate the 42 NetPractice project — IP fundamentals, masks, subnetting, routing, per-level strategy, and an evaluation cheatsheet. Hub: [`netpractice/INDEX.md`](netpractice/INDEX.md).

| File | Topic |
|---|---|
| [`INDEX.md`](netpractice/INDEX.md) | Reading order, validation checklist, mental model. |
| [`01_FOUNDATIONS.md`](netpractice/01_FOUNDATIONS.md) | IPv4 addresses, classes, CIDR, mask table. |
| [`02_BINARY_AND_MASKS.md`](netpractice/02_BINARY_AND_MASKS.md) | Decimal↔binary, the AND operation, block sizes. |
| [`03_SUBNETTING.md`](netpractice/03_SUBNETTING.md) | Network/broadcast/usable range, VLSM, /30 links. |
| [`04_ROUTING.md`](netpractice/04_ROUTING.md) | Routing tables, default route, longest-prefix match. |
| [`05_LEVEL_GUIDE.md`](netpractice/05_LEVEL_GUIDE.md) | What each of the 10 levels teaches — strategy, not answers. |
| [`06_TROUBLESHOOTING.md`](netpractice/06_TROUBLESHOOTING.md) | Decision tree for diagnosing a red network. |
| [`07_EVAL_CHEATSHEET.md`](netpractice/07_EVAL_CHEATSHEET.md) | One-page reference for evaluation defense. |

---

## 📝 Adding a new note

1. Choose the right folder — or open a new one if a theme doesn't fit.
2. `library/projects/LEVELX/<project>.md` for project READMEs, `<project>-<tag>.md` for scratch/TODO files.
3. Add a row to this index in the matching table.

Keep the root `README.md` as the single landing page. Every other markdown lives here.
