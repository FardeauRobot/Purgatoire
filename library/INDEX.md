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

## ✨ C++ — three doors

Everything C++ lives behind one entry point: **[`cpp/INDEX.md`](cpp/INDEX.md)**.

| Door | Hub | What's inside |
|---|---|---|
| 📦 Projets | [`cpp/projets/INDEX.md`](cpp/projets/INDEX.md) | Per-module guides CPP00→04, WALKTHROUGH, the whole webserv sub-library. |
| 🧠 Notions | [`cpp/notions/INDEX.md`](cpp/notions/INDEX.md) | Concepts by theme — PHILOSOPHY, fundamentals, OOP, STL containers, I/O & errors, tooling. |
| 🔑 Lexique | [`cpp/lexique/INDEX.md`](cpp/lexique/INDEX.md) | One page per C++98 keyword (all 63) + the concept [`GLOSSAIRE`](cpp/lexique/GLOSSAIRE.md). |

> 🌟 New module starting? Re-read [`cpp/notions/PHILOSOPHY.md`](cpp/notions/PHILOSOPHY.md) first.

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
