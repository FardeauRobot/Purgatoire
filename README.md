# Purgatoire 🔥

*"And I beheld a great repository, wherein the souls of students were tested and refined through trials of code."*

A pilgrimage through the 42 common core. Each trial lives in its own circle, judged by norminette, valgrind, and the unforgiving peer-to-peer review.

---

## 🗺️ Map of the Realm

```
Purgatoire/
├── libft/          # Submodule — the sacred standard library
├── LEVEL1/         # First circle: I/O, output, sysadmin
├── LEVEL2/         # Second circle: IPC, algorithms, graphics
├── LEVEL3/         # Third circle: concurrency & command interpreters
├── LEVEL4/         # Fourth circle: graphics engine & object orientation
├── library/        # All notes, READMEs, study guides (see library/INDEX.md)
├── Scripts/        # Workflow incantations
└── templates/      # Skeletons for new projects
```

---

## 🧭 Quickstart

```bash
git clone --recurse-submodules git@github.com:FardeauRobot/Purgatoire.git
cd Purgatoire
```

Forgot `--recurse-submodules`?

```bash
git submodule update --init --recursive
```

Each project is self-contained with its own `Makefile`. Enter, `make`, run. Example:

```bash
cd LEVEL2/PushSwap && make && ./push_swap 3 1 2
```

---

## 🔱 libft — *The Foundation Stone*

Before the ascent begins, thou must forge thine own sacred library. Memory, strings, conversion, linked lists — the utilities that sustain every project below.

- **Path:** `libft/` (git submodule → [libft_official](https://github.com/FardeauRobot/libft_official))
- **Build:** `cd libft && make`
- **Output:** `libft.a` — link with `-L./libft -lft`

---

## ⛰️ LEVEL 1 — *The First Circle*

The pilgrim learns to read, to speak, and to guard the gates.

| Project | Path | Judgement |
|---|---|---|
| 📜 **Get Next Line** | `LEVEL1/GNL/` | Read from file descriptors line by line. Handle static buffers with wisdom; do not leak. |
| 🖨️ **ft_printf** | `LEVEL1/PRINTF/` | Recreate formatted output with variadic arguments. `%c %s %p %d %i %u %x %X %%`. |
| 🤖 **Born2beRoot** | `LEVEL1/B2B/` | Debian/Rocky VM, UFW, SSH, sudo policies, cron monitoring. Defense scripts in `aliasB2B`, `monitoring.sh`. |

---

## 🏔️ LEVEL 2 — *The Second Circle*

Pipes, algorithms, and the graphical arts now stand between thee and salvation.

| Project | Path | Judgement |
|---|---|---|
| 🔗 **Pipex** | `LEVEL2/Pipex/` | `fork`, `execve`, `pipe`, `dup2`. Reproduce `< in cmd1 \| cmd2 > out`. Test harness: `./test_pipex.sh`. |
| 🔄 **Push_Swap** | `LEVEL2/PushSwap/` | Sort two stacks with 11 ops. Radix, chunked, or LIS — the fewer thy moves, the closer to grace. |
| 🎮 **So_Long** | `LEVEL2/So_Long/` | 2D tile game on MiniLibX. Map parsing, sprite rendering, event loop. Flood fill or flee. |

---

## 🌋 LEVEL 3 — *The Third Circle*

Where threads deadlock and shells are rewritten from scratch.

| Project | Path | Judgement |
|---|---|---|
| 🍝 **Philosophers** | `LEVEL3/philo/` | Dining philosophers with `pthread` + mutexes. No data races, no deaths — or all deaths declared on time. |
| 🐚 **Minishell** | `LEVEL3/minishell/` | Bash-lite: lexer, parser, expander, executor. Built-ins, pipes, redirections, heredocs, signals, env. |

---

## 🗻 LEVEL 4 — *The Fourth Circle*

The final ascent: raycasters and the orthodox canonical form.

| Project | Path | Judgement |
|---|---|---|
| 🧱 **cub3d** | `LEVEL4/cub3d/` | DDA raycaster on MiniLibX. `.cub` map parsing, textured walls, minimap. |
| ✨ **CPP (00→05)** | `LEVEL4/C++1/` | Module-by-module descent into C++98: classes, OCF, polymorphism, templates, exceptions. Notes under `docu/`. |

---

## 📜 Scripts — *The Scrolls of Automation*

Sacred incantations gathered along the way — git workflows, compile helpers, terminal utilities. Full catalogue in [`library/scripts/SCRIPTS.md`](./library/scripts/SCRIPTS.md).

Highlights:

- `autopush` — debounced file-watch → auto `git add/commit/push`
- `remind` — timed broadcast to every open terminal (in pagga + lolcat)
- `note` — tiny terminal notepad backed by `~/.local/share/notes.md`
- `watch_norminette.sh` — rainbow-coloured norminette on save

---

## 🧪 Templates

`templates/c_template/` — drop-in skeleton with `Makefile`, `includes/`, `src/`, vendored `libft`, and `init.sh` bootstrap. Spin up a new project in one command.

C++ skeletons (basic / complete / cppinit) live under `LEVEL4/C++1/template/`.

---

## ⚖️ The Commandments

1. Thou shalt respect the Norm.
2. Thou shalt not leak memory.
3. Thou shalt handle errors with grace.
4. Thou shalt code with clarity and purpose.
5. Thou shalt test thoroughly before submission.
6. Thou shalt commit often and push before nightfall.

---

## 📚 Library

All written material — project READMEs, C++ study notes, scripts doc, improvement roadmap — lives in [`library/`](./library/INDEX.md). Browse the index to find anything by topic.

---

## 🗺️ Want to improve?

See [`library/meta/IMPROVEMENT.md`](./library/meta/IMPROVEMENT.md) — a living checklist of structural, tooling, and craftsmanship upgrades, sorted from quick wins to long-term investments.

---

*"Through fire and function calls, through segfaults and success, the student walks the narrow path. Each project completed is a step toward enlightenment, each bug fixed a sin absolved. Welcome, wanderer, to thy Purgatoire."*

**Status:** 🔥 *Refining in progress — currently at LEVEL 4.*
