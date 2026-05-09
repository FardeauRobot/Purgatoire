# Memory-Leak Checking Guide (macOS + Linux)

A practical, opinionated guide for finding memory bugs in your 42 C++ projects.
Written for someone who already knows the Linux/Valgrind workflow and is now
working on macOS.

---

## TL;DR — What do I run?

| Situation | Command |
|---|---|
| Quick "do I leak?" check | `make leaks` |
| Find heap-use-after-free, double-free, out-of-bounds | `make asan && ./EXEC_NAME` |
| Step through with a debugger | `make debug` then `lldb ./EXEC_NAME` (mac) / `gdb` (linux) |
| Direct script call with options | `../../tools/leakcheck.sh -v ./EXEC_NAME` |
| Hands-on practice with real bugs | see "Playground" section below |

---

## Why this setup — the design choices

### Why not just use Valgrind on macOS?

Valgrind on recent macOS is effectively unmaintained:

- On **Apple Silicon** (M1/M2/M3/M4): no working port. The Homebrew formula
  was disabled. There is no upstream support for the ARM64 macOS ABI.
- On **Intel macOS**: only ancient versions support it, and those frequently
  crash inside Apple's system libraries before reaching your code.

So instead of pretending Valgrind works, this setup uses two tools that
actually work, and pairs them with a simple cross-platform wrapper:

1. **`leaks`** — Apple's own tool, installed with the Xcode Command Line
   Tools. Fast, accurate for the leak-detection use case, and zero install.
2. **AddressSanitizer (ASan)** — a compiler-instrumentation sanitizer built
   into clang and gcc. Works identically on macOS and Linux. Catches an
   overlapping but DIFFERENT set of bugs from `leaks`/Valgrind (see below).
3. **Valgrind** — still the right answer on Linux. Used unchanged.

### Why `leaks` AND ASan? Aren't they the same thing?

No — and this is the most important thing to understand.

| Bug class | `leaks` (mac) | Valgrind (linux) | ASan (both) |
|---|---|---|---|
| Forgotten `delete` (leak) | ✅ best at this | ✅ | ✅ (linux + recent mac) |
| Heap use-after-free | ⚠ sometimes | ✅ | ✅ best at this |
| Double-free | ⚠ sometimes | ✅ | ✅ best at this |
| Heap buffer overflow | ❌ | ✅ | ✅ best at this |
| Stack-use-after-return | ❌ | ❌ | ✅ only ASan can |
| Uninitialized read | ❌ | ✅ best at this | ⚠ partial |
| Mismatched `new`/`delete[]` | ❌ | ✅ | ✅ |
| Speed | runs at native speed | 10–50× slower | 2–3× slower |
| Setup | installed | `apt install valgrind` | recompile with flag |

**Rule of thumb** for your daily workflow:
- `make leaks` → "is there a leak in my final program?"
- `make asan` → "I have a crash / weird behaviour, where's the heap bug?"
- Use both. They catch different things.

### Why C++98?

The 42 C++ piscine grades you on the C++98 standard. All flags here are
chosen to work with it (no `-std=c++11`+ features). Sanitizers and `leaks`
are runtime tools and don't care about the language standard you compile with.

---

## The `make` targets

Each template Makefile (`minimal`, `basic`, `complete`) gains five new targets.

### `make debug`
Rebuilds with `-g3 -O0`. This is a **prerequisite for any leak tool to give
you readable backtraces**: without `-g`, you'll see hexadecimal addresses
instead of file:line; with `-O` the optimiser can inline functions out of
existence so the stack trace lies about where the leak originates.

### `make asan`
Rebuilds with AddressSanitizer + UBSan and full debug info. Just run
`./EXEC_NAME` after — sanitizers are baked into the binary and report at
exit (or at the moment of the bug, whichever comes first).

This is the one to reach for when:
- a program crashes and you don't know why,
- valgrind/`leaks` says "no leak" but the output is still wrong,
- you suspect a use-after-free, OOB, or double-free.

### `make ubsan`
*(Implicit — bundled into `asan` above.)* Catches signed overflow,
divide-by-zero, null-pointer dereference, mis-aligned loads, bad enum
values, etc. These are not memory leaks but they're undefined behaviour
that can corrupt memory in subtle ways.

### `make leaks`
Rebuilds with `-g`, then auto-detects the OS:
- **macOS**: `MallocStackLogging=1 leaks --atExit -- ./EXEC_NAME`
- **Linux**: `valgrind --leak-check=full --show-leak-kinds=all
  --track-origins=yes --error-exitcode=1 ./EXEC_NAME`

This is what you want for the "before submitting, check no leaks" pass.

### `make help`
Lists the targets above.

---

## The `leakcheck.sh` script

Lives at `templates/cpp/tools/leakcheck.sh`. Use it directly when you want
more control than `make leaks` gives you.

```sh
./tools/leakcheck.sh [MODE] <binary> [args...]
```

### Modes

#### `--basic` (default)
- **macOS**: `MallocStackLogging=1 leaks --atExit -- ./bin`
- **Linux**: `valgrind --leak-check=full --error-exitcode=1 ./bin`

What you get: a list of leaked blocks, each with a backtrace.

#### `--verbose`
On top of `basic`, adds:
- **macOS**:
  - `MallocScribble=1` — fills freed memory with `0x55`. A use-after-free
    then reads recognisable garbage. (Without this, freed slots often still
    contain the right bytes and the bug hides.)
  - `MallocPreScribble=1` — fills fresh allocations with `0xAA`. Surfaces
    code that assumes new memory is zeroed.
  - `MallocGuardEdges=1` — protected pages bracketing large allocations,
    so over/under-runs trap immediately.
- **Linux**:
  - `--track-origins=yes` — when valgrind sees an uninitialised value used
    in a branch, it tells you which allocation produced it. Slow, but the
    diagnostic is gold.

#### `--strict`
On top of `verbose`, adds:
- **macOS**:
  - `MallocStackLoggingNoCompact=1` — also keep backtraces of FREED blocks.
    `leaks` can then say "this freed block was allocated at X and freed at Y"
    when it sees a use-after-free.
  - `MallocCheckHeapStart=1`, `MallocCheckHeapEach=100` — run a heap
    consistency check every 100 mallocs, catching corruption near its source
    instead of when it finally crashes.
- **Linux**:
  - `--show-leak-kinds=all` — also report "still reachable" memory
    (allocations the OS would clean up at process exit anyway). Useful
    when learning what counts as a "real" leak.
  - `--errors-for-leak-kinds=all` — any leak, even reachable, fails the
    exit code. Useful for CI.

---

## The `leak_playground.cpp` — hands-on bugs

A single file with 10 numbered bugs, each in its own `#ifdef BUG_<n>` block.
Goal: build the mental association between **a kind of mistake** and **the
exact thing the tool prints when you make it**.

### How to use it

```sh
cd templates/cpp/tools
c++ -std=c++98 -Wall -Wextra -g -DBUG_1 leak_playground.cpp -o play
./leakcheck.sh -v ./play
```

Then compare the output to the comment in the file. Repeat with `-DBUG_2`,
`-DBUG_3`, ... When you've seen each tool's output for each bug, you'll
recognise the patterns in your real code instantly.

### The 10 bugs (and why each is interesting)

| # | Bug | What you should learn |
|---|---|---|
| 1 | Forgotten `delete` | The most common leak. Tool says "definitely lost". |
| 2 | `new[]` paired with plain `delete` | The compiler can't catch this — types match. `delete[]` is mandatory for arrays. |
| 3 | Double-delete | Once deleted, the pointer is poison. Set to NULL or restructure. |
| 4 | Use-after-free | `delete` doesn't zero the pointer. ASan is best at catching this. |
| 5 | Shallow copy of class with raw pointer | The Rule of Three. If you have a destructor that frees, you need a copy ctor and copy operator= that copy the contents. |
| 6 | Returning `&local_var` | Stack memory dies with the function. Return by value or `new`. |
| 7 | Heap buffer overflow (off-by-one) | `new int[10]` gives indices 0..9. Index 10 corrupts the next allocation. |
| 8 | Reading uninitialised memory | `new int` (no `()`) doesn't initialise. Use `new int(0)` or `new int()`. |
| 9 | Constructor throws | If a ctor throws, the dtor doesn't run — anything already allocated leaks. RAII teaser. |
| 10 | Missing self-assignment check | `a = a` with naive operator= deletes the buffer it's about to read from. |

These are the bugs you'll *actually* hit in modules 01–09 of the C++ piscine.
Master them and you've covered the foundation of C++ memory management.

---

## Reading the output — what each tool tells you

### `leaks` (macOS)

```
Process 12345: 1 leak for 16 total leaked bytes.

Leak: 0x600002af4060  size=16  zone: MallocSmallZone_0x10c2e3000  malloc
    Call stack:
       0  libsystem_malloc.dylib  malloc
       1  libc++.1.dylib          operator new(unsigned long)
       2  zombie                  zombieHorde(int, std::__1::string)  zombieHorde.cpp:4
       3  zombie                  main                                main.cpp:13
```

Read it bottom-up: `main` called `zombieHorde`, which called `operator new`,
which leaked 16 bytes. The fix is at `zombieHorde.cpp:4`.

### Valgrind (Linux)

```
==12345== 16 bytes in 1 blocks are definitely lost in loss record 1 of 1
==12345==    at 0x483BE63: operator new[](unsigned long)
==12345==    by 0x10923A: zombieHorde(int, std::string) (zombieHorde.cpp:4)
==12345==    by 0x10920E: main (main.cpp:13)
```

Read it top-down. `definitely lost` is the bad one (you have a leak).
`indirectly lost` means a leaked block held pointers to other blocks.
`still reachable` means the OS would have cleaned it up — usually fine.

### ASan

```
==12345==ERROR: LeakSanitizer: detected memory leaks

Direct leak of 16 byte(s) in 1 object(s) allocated from:
    #0 0x10ab33d68 in operator new[](unsigned long)
    #1 0x10aabea90 in zombieHorde(int, std::string) zombieHorde.cpp:4
    #2 0x10aabe9a4 in main main.cpp:13
```

Same idea, slightly different format. The `#0`, `#1`, ... is the call stack
top-to-bottom (#0 is where `new` was called, the higher numbers are callers).

---

## Common confusions

### "valgrind is silent but my output is wrong"
You have undefined behaviour, not a leak. Try `make asan` — it catches a
much wider range of memory bugs (UAF, OOB, etc.) than valgrind does.

### "leaks says 0 leaks but I'm sure I leak"
Are you actually exiting cleanly? `leaks --atExit` runs the check when your
program exits normally. If you `exit(1)` or crash before `main` returns,
the check may not run. Run it manually: `./your-bin & sleep 1 && leaks $!`.

### "ASan flags a leak in libc / a system library"
Suppress with `LSAN_OPTIONS=suppressions=./suppr.txt` (file lists names to
ignore). For 42 work, this rarely happens — usually it's your code.

### "ASan won't link on macOS"
You forgot to add `-fsanitize=address` to **the link step** as well as the
compile step. The Makefile targets here do it correctly via `LDFLAGS`.

### "I get a leak from `std::string` and I never used new"
`std::string` allocates internally. If your *object* leaks (e.g., you `new`
a class that contains a `std::string` and never `delete` it), the string's
internal buffer is reported as leaked too. Fix the outer object's lifecycle.

---

## Recommended workflow when implementing a 42 module

1. **Write the code.** Build with `make`.
2. **Run normally.** Does it produce the expected output? If not, debug
   the logic first.
3. **Sanitize.** `make asan && ./EXEC_NAME` with your test inputs. ASan
   catches heap bugs while you're still iterating — fast feedback.
4. **Final leak check.** `make leaks`. Should report zero. If not, fix and
   loop back to step 1.
5. **Stress.** Run with edge cases: empty input, max-size input, malformed
   input. Repeat steps 3–4.

You don't need to do all of this on every save. Sanitize when output is
weird; leak-check before you consider an exercise "done".

---

## File layout

```
templates/cpp/
├── tools/
│   ├── leakcheck.sh           ← cross-platform runner script
│   ├── leak_playground.cpp    ← 10 educational bugs
│   └── MEMORY_GUIDE.md        ← this file
├── minimal/Makefile           ← has leaks/asan/debug/help
├── basic/Makefile             ← has leaks/asan/debug/help
└── complete/Makefile          ← has leaks/asan/debug/help + test
```

The `tools/` directory is shared. The Makefiles use **inline shell** for the
`leaks` target so each generated project is self-contained and doesn't
depend on the script's filesystem location. Use the script directly when
you want the verbose/strict modes the inline target doesn't expose.
