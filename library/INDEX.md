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
└── cpp/              # C++ study notes, sorted by theme
```

---

## 📜 Meta

| File | Purpose |
|---|---|
| [`meta/IMPROVEMENT.md`](meta/IMPROVEMENT.md) | Living checklist of repo upgrades — quick wins → long-term craftsmanship. |

---

## 🛠️ Scripts

| File | Purpose |
|---|---|
| [`scripts/SCRIPTS.md`](scripts/SCRIPTS.md) | Catalogue of personal CLI tools (`autopush`, `remind`, `note`, `watch_norminette`, …). |

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

### `cpp/fundamentals/` — *the base layer*
| File | Topic |
|---|---|
| [`BASICS.md`](cpp/fundamentals/BASICS.md) | Core syntax, compilation unit, namespaces. |
| [`REFERENCE.md`](cpp/fundamentals/REFERENCE.md) | References vs pointers, lvalue semantics. |

### `cpp/modules/` — *per-module walkthrough*
| File | Topic |
|---|---|
| [`CPP00.md`](cpp/modules/CPP00.md) | Module 00 — namespaces, classes, stdio. |
| [`CPP01.md`](cpp/modules/CPP01.md) | Module 01 — memory, references, file I/O. |
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
| [`STL.md`](cpp/advanced/STL.md) | Containers, iterators, algorithms. |
| [`CASTS.md`](cpp/advanced/CASTS.md) | `static_cast`, `dynamic_cast`, `const_cast`, `reinterpret_cast`. |

### `cpp/io-errors/` — *streams & failure*
| File | Topic |
|---|---|
| [`FSTREAM_GUIDE.md`](cpp/io-errors/FSTREAM_GUIDE.md) | `ifstream`/`ofstream`/`stringstream`. |
| [`OPEN.md`](cpp/io-errors/OPEN.md) | File opening flags and modes. |
| [`STRING_FUNCTIONS.md`](cpp/io-errors/STRING_FUNCTIONS.md) | `std::string` API cheat sheet. |
| [`ERROR_MANAGEMENT.md`](cpp/io-errors/ERROR_MANAGEMENT.md) | Exceptions, `try`/`catch`, noexcept. |

### `cpp/tooling/` — *build & link*
| File | Topic |
|---|---|
| [`MAKEFILE_CPP.md`](cpp/tooling/MAKEFILE_CPP.md) | C++ Makefile recipes, `-Wall -Wextra -Werror -std=c++98`. |
| [`LIBRARIES.md`](cpp/tooling/LIBRARIES.md) | Static vs shared libraries, linking order. |

---

## 📝 Adding a new note

1. Choose the right folder — or open a new one if a theme doesn't fit.
2. `library/projects/LEVELX/<project>.md` for project READMEs, `<project>-<tag>.md` for scratch/TODO files.
3. Add a row to this index in the matching table.

Keep the root `README.md` as the single landing page. Every other markdown lives here.
