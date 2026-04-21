# Makefiles for C++ — Complete Guide

The 42 CPP modules all expect the same Makefile shape: a binary target, `clean`, `fclean`, `re`, strict flags, and no relinking when nothing changed. This guide shows the pattern, explains each piece, and covers the quirks that come up when compiling C++ (templates, dependency tracking, and header bookkeeping).

---

## Table of Contents

1. [The canonical 42 C++ Makefile](#1-the-canonical-42-c-makefile)
2. [Flags explained](#2-flags-explained)
3. [How `make` decides to rebuild](#3-how-make-decides-to-rebuild)
4. [The four mandatory rules](#4-the-four-mandatory-rules)
5. [Header dependency tracking](#5-header-dependency-tracking)
6. [Why templates break naive Makefiles](#6-why-templates-break-naive-makefiles)
7. [Object files in a build directory](#7-object-files-in-a-build-directory)
8. [Pattern rules and automatic variables](#8-pattern-rules-and-automatic-variables)
9. [`.PHONY` — why it matters](#9-phony--why-it-matters)
10. [Colors and progress output](#10-colors-and-progress-output)
11. [Debugging a Makefile](#11-debugging-a-makefile)
12. [Rules of thumb](#12-rules-of-thumb)
13. [Gotchas](#13-gotchas)

---

## 1. The canonical 42 C++ Makefile

```make
NAME     := program

CXX      := c++
CXXFLAGS := -Wall -Wextra -Werror -std=c++98

SRC_DIR  := src
INC_DIR  := include
OBJ_DIR  := build

SRCS     := $(wildcard $(SRC_DIR)/*.cpp)
OBJS     := $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
DEPS     := $(OBJS:.o=.d)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -MMD -MP -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

-include $(DEPS)

.PHONY: all clean fclean re
```

Every line of that file earns its place. Below is what each earns.

---

## 2. Flags explained

```
-Wall     — enable most common warnings
-Wextra   — enable more warnings (things -Wall missed)
-Werror   — treat warnings as errors (42-required)
-std=c++98 — restrict the language to C++98 (42-required in CPP 00–08)
```

You may also see:

```
-pedantic        — warn about non-standard extensions; good for portability
-Wshadow         — warn when a local variable shadows a field
-Wnon-virtual-dtor — warn when a class with virtuals has no virtual dtor (useful!)
-fsanitize=address — AddressSanitizer — catches UB at runtime; see DEBUGGING_CPP.md
-g               — include debug info (for lldb/gdb)
-O2              — optimize (for performance); avoid while debugging
```

Common mistake: shipping with `-O2 -g` and wondering why debugging is weird. Use `-O0 -g` during development.

### Compiler choice

`c++` on 42 machines aliases `clang++` on macOS and `g++` on Linux. Use `c++` in the Makefile — it works on both. Don't hardcode `clang++` or `g++` unless you have a specific reason.

---

## 3. How `make` decides to rebuild

`make`'s core rule: **rebuild a target if any of its prerequisites is newer than the target** (or the target doesn't exist). Everything follows from that.

```make
program: main.o utils.o
	c++ main.o utils.o -o program

main.o: main.cpp
	c++ -c main.cpp -o main.o
```

If you touch `main.cpp`, `main.o` is older than it → rebuild `main.o` → `program` is older than the new `main.o` → relink. Nothing else re-runs.

This works by **file timestamps**. If your clock is messed up, or you rsync files around losing mtimes, you'll get spurious rebuilds or — worse — *missed* rebuilds.

---

## 4. The four mandatory rules

42 requires `all`, `clean`, `fclean`, `re`:

| Rule | Does |
|---|---|
| `all` (or just `make`) | Build the binary. Don't rebuild anything up to date. |
| `clean` | Delete object files and dependency files. Binary stays. |
| `fclean` | `clean` + delete the binary. |
| `re` | `fclean` then `all`. |

And `re` is **never** `$(MAKE) fclean && $(MAKE) all` with `&&` — it's two target dependencies in order. The distinction matters when sub-makes are involved, but for single-directory projects, both work.

---

## 5. Header dependency tracking

The subtle problem: if you change a `.hpp`, which `.cpp` files need to be recompiled? A naive Makefile doesn't know — it only tracks `.cpp → .o`. Edit `Animal.hpp` and nothing rebuilds; next run of the program uses stale code.

**The solution — auto-generated dependency files.** `-MMD -MP` makes the compiler emit a `.d` file alongside each `.o`:

```
build/main.o: src/main.cpp include/Animal.hpp include/Dog.hpp
```

Then the last line of the Makefile:
```make
-include $(DEPS)
```
merges those dependency snippets into the build graph. Now `make` knows `main.o` depends on `Animal.hpp`; touching the header rebuilds `main.o`.

### What do `-MMD` and `-MP` mean?

- **`-MM`** — output Makefile dependencies for this source, **excluding** system headers (`<iostream>`, `<string>`). Without this, every `.d` would list `/usr/include/...` — slow and brittle.
- **`-MMD`** — like `-MM` but also compile normally (MM alone just emits deps without producing `.o`).
- **`-MP`** — add a "phony" empty rule for each header. Prevents "no rule to make target `<header>.hpp`" errors if you rename a header.

The leading `-` on `-include` means "don't fail if the `.d` files don't exist yet" (first build).

---

## 6. Why templates break naive Makefiles

Templates are instantiated at each `#include` site. That means:

- Changing a templated method body in a header **forces rebuild of every .cpp that uses it.**
- Your Makefile needs to know about that. Hence `-MMD -MP` (§5).

If you skip dependency tracking, you can edit a templated method and the compiler never re-runs on the call site — you end up with a stale binary that mixes old and new object files.

This is why **every professional C++ Makefile uses some form of auto-generated dependencies.** The pattern above is the standard one.

### `.tpp` files

If you split templates into `Array.hpp` + `Array.tpp` (see [TEMPLATES.md §8](TEMPLATES.md)), the `.tpp` is `#include`'d from the `.hpp`. `-MMD` picks up that chain — no extra Makefile work needed.

---

## 7. Object files in a build directory

Putting `.o` files next to `.cpp` files clutters the source tree and fights `git`. Move them to `build/`:

```
project/
├── Makefile
├── src/
│   ├── main.cpp
│   ├── Animal.cpp
│   └── Dog.cpp
├── include/
│   ├── Animal.hpp
│   └── Dog.hpp
└── build/               ← generated, gitignored
    ├── main.o
    ├── main.d
    ├── Animal.o
    └── ...
```

And your `.gitignore`:
```
build/
program
```

### How the Makefile maps paths

```make
SRCS := $(wildcard $(SRC_DIR)/*.cpp)
# SRCS = src/main.cpp src/Animal.cpp ...

OBJS := $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
# substitute src/%.cpp → build/%.o
# OBJS = build/main.o build/Animal.o ...
```

And the rule:
```make
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -MMD -MP -c $< -o $@
```

- `%.o: %.cpp` — pattern rule: for any `.o`, find the matching `.cpp`.
- `| $(OBJ_DIR)` — the pipe marks `$(OBJ_DIR)` as an **order-only prerequisite**. Make sure the dir exists, but don't rebuild the `.o` just because the dir's mtime changed.
- `$<` — the first prerequisite (the `.cpp`).
- `$@` — the target (the `.o`).
- `-c` — compile only, don't link.

---

## 8. Pattern rules and automatic variables

The most useful automatic variables:

| Variable | Means |
|---|---|
| `$@` | The target |
| `$<` | The first prerequisite |
| `$^` | All prerequisites, space-separated, duplicates removed |
| `$?` | Prerequisites newer than the target |
| `$*` | The stem of a pattern rule (the part `%` matched) |

Example:

```make
$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@
#                     ^^  ^^
#                     |   |
#                     |   the binary name (like program)
#                     all objects
```

Using `$^` over `$(OBJS)` is a tiny robustness win: if you add another prerequisite later, it just works.

---

## 9. `.PHONY` — why it matters

`.PHONY: all clean fclean re` declares these as **not files**. Without this, if you happen to have a file named `clean` in your directory, `make clean` silently does nothing (the "target" is already up to date — the file exists and has no prerequisites).

`.PHONY` forces `make` to always run the recipe, regardless of filesystem state.

---

## 10. Colors and progress output

For fun and clarity, you can add simple color escapes:

```make
GREEN := \033[32m
RESET := \033[0m

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@printf "$(GREEN)Compiling$(RESET) %s\n" $<
	@$(CXX) $(CXXFLAGS) -I$(INC_DIR) -MMD -MP -c $< -o $@
```

The leading `@` on a command **suppresses echoing it**. Combined with `printf`, you get a clean progress line instead of the raw compiler command. Most 42 repos do some version of this.

---

## 11. Debugging a Makefile

A few built-in features are lifesavers.

### See what will run

```
make -n                    — print commands but don't execute
```

### Print expanded variables

```make
print-%:
	@echo '$*=$($*)'
```

Then `make print-SRCS` shows what `SRCS` expanded to.

### Full trace

```
make --trace              — print every decision (GNU make 4.0+)
make -d                   — verbose debugging (noisy but thorough)
```

### "I edited a header and nothing rebuilt"

- Do you have `-MMD` in the compile command? Run `make fclean && make` and inspect `build/*.d` to see if dependency files were generated.
- Is the `-include $(DEPS)` line present *and* reached (not inside a conditional)?
- Did the `.d` files get deleted by `clean` but the `.o` files survive? Make sure `clean` wipes both.

---

## 12. Rules of thumb

- **Use `-Wall -Wextra -Werror -std=c++98`.** Don't weaken these unless the subject demands it.
- **Always use `-MMD -MP`.** Without it, a header change silently breaks the build.
- **Keep object files out of the source tree** (`build/` directory).
- **Declare phony targets.** `all clean fclean re` at minimum.
- **Don't touch the compiler manually.** Let `make` decide what to rebuild.
- **Version-control the `Makefile`, not its output.** The binary and `.o` files belong in `.gitignore`.
- **Never `rm -rf $(NAME)`** with a `NAME` variable that could be empty. Make sure `fclean` uses `-f` to avoid errors when the binary doesn't exist.

---

## 13. Gotchas

- **Tabs, not spaces.** Every recipe line must begin with a literal tab. Editors that convert tabs to spaces will silently break your Makefile.
- **`make` caches absolutely nothing about the *content* of files** — only timestamps. If two files have the same mtime, the result is implementation-defined.
- **`$(SRCS)` via `wildcard` is evaluated at Makefile-read time.** If you `touch newfile.cpp` and run `make`, it *will* be picked up (the wildcard is re-evaluated on each invocation). But files you create **mid-build** aren't picked up in that same invocation.
- **Paths in `-I` don't accept `~` or `$HOME`** reliably. Use relative paths.
- **`clean` can race with the build** if you run `make clean` while a `make` is in progress. Don't.
- **Order-only prerequisites (`|`) do not propagate mtime.** That's by design — you want the directory to exist, not to trigger rebuilds when its mtime changes.
- **`@` before a command hides it.** Useful for cosmetics, painful for debugging. If you're mystified about what `make` is doing, remove the `@` temporarily.
- **Forgetting `-std=c++98`** lets the compiler silently accept C++11 features (range-for, `auto`, lambdas). Your code will compile on your machine and blow up at 42 evaluation.
- **The default rule for `.cpp → .o` in GNU make uses `$(CXX)` and `$(CXXFLAGS)`.** Often you don't need to write your own pattern rule — but writing one explicitly is clearer and portable across make implementations.
