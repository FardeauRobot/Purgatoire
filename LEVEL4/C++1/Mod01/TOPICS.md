# C++ Module 01 — TOPICS

## Theme
**Memory allocation** (stack vs heap), **references**, **pointers to
member functions**, and the **`switch`** statement. The central
question across the module: *when do I use stack, heap, pointer, or
reference?*

## Global rules
Same as Mod 00 — `-Wall -Wextra -Werror -std=c++98`, no STL, no
`*printf`/`*alloc`/`free`, include guards, UpperCamelCase, no leaks.

---

## Cross-cutting concepts (read before starting)

| Concept | Where |
|---|---|
| Stack vs heap, `new`/`delete`, `new[]`/`delete[]` | [library/cpp/fundamentals/MEMORY.md](../../../library/cpp/fundamentals/MEMORY.md) |
| References — alias semantics, `const` references | [library/cpp/fundamentals/REFERENCE.md](../../../library/cpp/fundamentals/REFERENCE.md) |
| File I/O with `<fstream>` | [library/cpp/io-errors/FSTREAM_GUIDE.md](../../../library/cpp/io-errors/FSTREAM_GUIDE.md), [library/cpp/io-errors/OPEN.md](../../../library/cpp/io-errors/OPEN.md) |
| `std::string` manipulation | [library/cpp/io-errors/STRING_FUNCTIONS.md](../../../library/cpp/io-errors/STRING_FUNCTIONS.md) |
| Per-exercise walkthrough | [library/cpp/modules/CPP01.md](../../../library/cpp/modules/CPP01.md) |

### The central question — stack, heap, pointer, or reference?

| You want | Use |
|---|---|
| A local, auto-cleaned value, known small size | **stack** (`T obj;`) |
| An object that outlives the function / unknown size / huge | **heap** (`T* obj = new T;` → `delete obj;`) |
| An **alias** that's guaranteed to point to *something*, never reseated | **reference** (`T& ref = obj;`) |
| A nullable, reseatable handle, possibly heap-allocated | **pointer** (`T* p;`) |

Mod01 pulls these apart one exercise at a time. Every ex is a
pedagogical knob on this dial — once the four have a clear home in
your head, the module is done.

### The heap lifetime rule

Every `new` **must** be matched by exactly one `delete`. Every
`new[]` by exactly one `delete[]`. Leak → moulinette fails. Mixing
`new`/`delete[]` (or `new[]`/`delete`) → **undefined behaviour**
(often a crash; sometimes silent corruption).

See [library/cpp/fundamentals/MEMORY.md](../../../library/cpp/fundamentals/MEMORY.md)
for how heap allocation actually works (arena, size cookie, free
lists).

---

## Exercise 00 — BraiiiiiinnnzzzZ

### Goal
Build a `Zombie` class; then two free functions — one that
**heap-allocates** a zombie and returns a pointer, and one that
**stack-allocates** a zombie, announces, and lets it die at scope
exit.

### What the exercise drills

- **Two allocation strategies, same class.** `newZombie` uses `new`
  and returns a pointer the caller must `delete`. `randomChump` uses
  a local (`Zombie z(name);`) that's destroyed automatically.
- **Destructor trace.** The subject requires the `~Zombie()` to
  print the zombie's name. That's how you *see* when each allocation
  strategy actually releases the object.
- **Header/impl split.** `Zombie.hpp` has the class and the two
  free-function declarations; `Zombie.cpp` + `newZombie.cpp` +
  `randomChump.cpp` have the definitions.

### Hardware-level notes

- **Stack allocation** is a single pointer decrement on the
  stack pointer register (`rsp` on x86-64). Zero overhead. The
  object lives in the current stack frame and disappears when the
  frame pops.
- **Heap allocation** (`new`) calls `operator new`, which calls
  `malloc`, which walks the allocator's free list (or asks the
  kernel for more pages via `brk`/`mmap`). Typical cost: 50-100 ns
  on a cache miss. *Plus* the object's constructor runs.
- `delete` calls the destructor, then returns the raw memory to the
  allocator. It does **not** `munmap` in general — most allocators
  keep the memory around for future allocations.
- `std::string` member inside `Zombie` means the destructor chain
  has real work to do — `std::string::~string()` frees its heap
  buffer. Leak the `Zombie`, and you also leak the string.

### Tips & gotchas

- **`randomChump` doesn't return anything.** It creates a local
  `Zombie`, calls `announce()`, and returns. The destructor fires
  before the function returns.
- **`newZombie` returns `Zombie*`.** The caller is responsible for
  `delete`. Document it (comment in the header).
- **Pass `std::string` by `const&`** to the ctor to avoid a
  copy. See
  [library/cpp/fundamentals/REFERENCE.md §4.2](../../../library/cpp/fundamentals/REFERENCE.md).
- **Order of stdout lines** — the subject expects a specific order.
  `announce` prints, then the destructor prints on scope exit.

### Search terms
`"c++ stack vs heap allocation"`,
`"c++ automatic vs dynamic storage duration"`,
`"new delete pair"`, `"c++ const reference parameter"`.

---

## Exercise 01 — Moar brainz!

### Goal
`Zombie* zombieHorde(int N, std::string name);` — allocate **N**
zombies in a **single call** to `new[]`, name each one, return a
pointer to the first. Caller must `delete[]`.

### What the exercise drills

- **`new T[N]`** vs N individual `new T`. A single contiguous block,
  one allocation, one deallocation.
- **Default construction of each element**. `new Zombie[N]` calls
  the default constructor N times. That's why `Zombie` must have
  one — and why you need a **setter** (or a two-step init in the
  ctor) to name each zombie after the array is allocated.
- **`delete[]` vs `delete`**. Mismatch = UB. The language tracks
  array-ness separately in the allocator's bookkeeping.

### Why default-ctor + setter, not a ctor with a name

`new Zombie[N]("Bob")` does **not** compile in c++98. Array-new can
only invoke the default constructor. So:

```cpp
Zombie* horde = new Zombie[N];   // N default-constructed zombies
for (int i = 0; i < N; ++i)
    horde[i].setName(name);      // name them afterwards
```

You need a `setName(const std::string&)` method on `Zombie`. Or a
two-phase init where the default ctor leaves `name` empty and
`setName` fills it.

### Hardware-level notes

- `new Zombie[N]` is **one** `operator new[]` call for
  `N * sizeof(Zombie) + cookie`. The "cookie" stores N so that
  `delete[]` knows how many destructors to run — typically 8 bytes
  prepended to the returned pointer.
- The returned pointer skips the cookie: `delete[] ptr` walks backwards
  to read N, destructs `ptr[0..N-1]`, then frees the whole block.
- If you wrote `delete ptr;` instead of `delete[] ptr;`, the allocator
  sees garbage where it expected the single-object metadata. Result:
  usually a crash, sometimes a silent heap corruption. Valgrind
  catches this immediately (`mismatched free / delete / delete[]`).
- All N zombies are **contiguous in memory**. Cache-friendly. If
  `Zombie` is small, iterating `horde[0..N-1]` is one cache line
  per few zombies.

### Tips & gotchas

- **Always valgrind this one.** If you leak, you leak N zombies at
  once and the diff is visible.
- **`delete[] horde;` in the caller.** Document this in the
  function's declaration. Callers forget.
- **Don't return `Zombie**`** (pointer to pointer). `Zombie*` is the
  pointer to the first element; the array decays naturally.

### Search terms
`"c++ new delete array"`,
`"c++ new[] default constructor"`,
`"valgrind mismatched free"`.

### Cross-reference
- [library/cpp/modules/CPP01.md §ex01](../../../library/cpp/modules/CPP01.md)
- [library/cpp/fundamentals/MEMORY.md — array new/delete](../../../library/cpp/fundamentals/MEMORY.md)

---

## Exercise 02 — HI THIS IS BRAIN

### Goal
Exposition: declare a `std::string`, a `std::string*` pointing to
it, and a `std::string&` referring to it. Print the address of all
three, then the value of all three.

### What the exercise drills

- **A reference is literally another name** for the same storage.
  `&string_ref` prints the *referent's* address — not the
  reference's own address, because references don't have a
  distinct address at the language level.
- **A pointer has its own address.** `&stringPTR` is the address of
  the pointer variable itself; `stringPTR` is the address of the
  string; `*stringPTR` is the string.
- **References disappear at the assembly level.** The compiler
  typically lowers `T& ref = obj;` to the same machine code as
  `T* const ref = &obj;`. The difference is a *type-system* thing
  — the language pretends references aren't pointers so you can't
  reseat them or null them.

### Hardware-level notes

- A `std::string` object (24-32 bytes on typical libstdc++) lives
  on the stack here. Its heap buffer (if any) is elsewhere. When
  you print `&str`, you get the address of the `std::string` *object*,
  not the address of its character data.
- The reference and the pointer hold the **same bit pattern** (the
  address of `str`). The only difference is how the language lets
  you use them.
- Output is three identical addresses and three identical values.
  That's the whole point — the exercise proves references and
  pointers are, at the machine level, the same thing.

### Tips & gotchas

- Don't overthink it. It's 20 lines total.
- Print the address in hex — `std::cout` does that automatically
  for `void*` and `T*`.
- No `delete`. No `new`. Pure stack.

### Search terms
`"c++ reference vs pointer address"`,
`"c++ reference is alias not pointer"`.

### Cross-reference
- [library/cpp/fundamentals/REFERENCE.md §2](../../../library/cpp/fundamentals/REFERENCE.md) — mental model

---

## Exercise 03 — Unnecessary violence

### Goal
Three classes: `Weapon`, `HumanA`, `HumanB`.

- `Weapon` — private `std::string type`; `getType()` returns a
  **const reference**; `setType()` mutates.
- `HumanA` — takes its `Weapon` in the constructor; **always**
  armed. Store as **`Weapon&`** (reference member).
- `HumanB` — may have no weapon initially; `setWeapon()` can
  (re)assign one. Store as **`Weapon*`** (pointer member).

Both have `attack()` printing `<name> attacks with their <weapon type>`.

### What the exercise drills

- **Reference member vs pointer member.** The choice encodes the
  *relationship*:
  - `HumanA` **always** has a weapon → reference, bound at
    construction, no way to rebind, no null state.
  - `HumanB` **may or may not** have a weapon, and might swap it →
    pointer, defaults to `NULL`, reassignable.
- **Reference member + OCF.** A class with a reference member can't
  be copy-assigned by default, because references can't be rebound
  after construction. `HumanA`'s `operator=` is effectively a no-op
  or disallowed. The subject doesn't care; the compiler does.
- **`getType()` returns `const std::string&`**. Callers can read
  the string without copying it, but can't modify it. Returning
  by value would work but copy the string.
- **Weapon update propagates.** If you call `weapon.setType("axe")`,
  both `HumanA` and `HumanB` see the new type on their next `attack`
  — because they don't own the weapon, they reference it. That's
  the test the subject's main is checking.

### Why not store `Weapon` by value

`HumanA` **sharing** a weapon with `HumanB` is the point. Stored
by value, each human has an independent copy. Reference/pointer
semantics give you shared access to the same object.

### Hardware-level notes

- A reference member is stored as a pointer — the compiler-generated
  layout for `class HumanA { std::string name; Weapon& w; }` is
  effectively `{ std::string, Weapon* }`. `sizeof(HumanA)` includes
  both.
- `HumanB`'s pointer member is literally a pointer. It can be
  `NULL`, so `attack()` must check before dereferencing — otherwise
  it's a null-deref crash (SIGSEGV).
- The **reference member** must be initialised in the constructor's
  init list, not the body. You cannot "assign" a reference after it
  exists.

### Tips & gotchas

- `HumanA(std::string name, Weapon& weapon) : m_name(name), m_weapon(weapon) {}`
  — init list mandatory.
- `HumanB::attack()` should no-op (or print a "no weapon" line) if
  `m_weapon == NULL`. Decide and stick to one behaviour.
- `Weapon::getType()` returning `const std::string&` — if you
  accidentally drop the `const`, callers could modify the weapon
  through the accessor. That's a silent bug the `const` prevents.

### Search terms
`"c++ reference member initialization"`,
`"c++ pointer vs reference member"`,
`"c++ return const reference to member"`.

### Cross-reference
- [library/cpp/fundamentals/REFERENCE.md §7.3](../../../library/cpp/fundamentals/REFERENCE.md) — reference in class members
- [library/cpp/oop/GETTERS_SETTERS.md](../../../library/cpp/oop/GETTERS_SETTERS.md) — return-by-const-ref

---

## Exercise 04 — Sed is for losers

### Goal
CLI: `./program filename s1 s2`. Read `filename`, write a new file
`filename.replace` where every occurrence of `s1` is replaced by `s2`.

- **No C file I/O.** Use `<fstream>`.
- **`std::string::replace` is forbidden.** Build the replacement
  with `find` + `substr` + concatenation.

### What the exercise drills

- **`<fstream>`** — `std::ifstream` to read, `std::ofstream` to
  write. See
  [library/cpp/io-errors/FSTREAM_GUIDE.md](../../../library/cpp/io-errors/FSTREAM_GUIDE.md).
- **Reading a file whole or line-by-line.** Line-by-line preserves
  newlines easily; whole-file via `rdbuf()` is terse. Both work.
- **Manual replacement loop.** `find` returns the index of the
  first match (or `std::string::npos`); `substr` slices. Concatenate
  `[before match] + s2 + [rest after recursing]`.
- **Error handling.** Missing args, unreadable file, empty `s1`
  (empty → infinite loop, must reject), unreadable output path.

### The replacement loop, sketched

```
while there's a match at index i in the input:
    output += input[0 .. i)
    output += s2
    input  = input[i + s1.size() .. end]
output += input   // remainder with no more matches
```

Implemented naïvely with `std::string::find` and `substr`, it's
O(n · m) in the size of the input times the number of matches.
Good enough for this exercise.

### Hardware-level notes

- `std::ifstream` uses a buffered read under the hood (default
  buffer ≈ 8 KiB). Reading a 1 MB file is ~128 `read()` syscalls,
  not one per char.
- `std::string` concatenation with `+=` amortises to O(n) total by
  doubling capacity on reallocation. A million `+=` calls don't
  cost a million allocations.
- `ifstream::rdbuf()` returns the stream's buffer object; piping it
  through `ofstream` copies the whole file in one shot, going
  through libc's `fread`/`fwrite`. Fast but loses control over
  per-byte handling — you can't replace mid-stream.

### Tips & gotchas

- **Empty `s1` → infinite loop.** `find("")` returns `0`. Reject
  it explicitly.
- **Overlapping matches.** `s1 = "aa"`, input = `"aaaa"`. Match at
  0 consumes 2 chars; next find starts at 2, finds another `"aa"`.
  The spec doesn't say much — match non-overlapping left-to-right,
  that's the safest interpretation.
- **Unreadable input file.** Check `if (!ifs)` after opening.
  Print an error, exit non-zero.
- **Output file opens in truncate mode by default.** `filename.replace`
  gets clobbered if it already exists — that's the expected behaviour
  for this exercise.

### Search terms
`"c++ ifstream read entire file"`,
`"c++ string find substr replace manual"`,
`"std::ifstream open check failed"`.

### Cross-reference
- [library/cpp/io-errors/FSTREAM_GUIDE.md](../../../library/cpp/io-errors/FSTREAM_GUIDE.md)
- [library/cpp/io-errors/OPEN.md](../../../library/cpp/io-errors/OPEN.md)
- [library/cpp/io-errors/STRING_FUNCTIONS.md](../../../library/cpp/io-errors/STRING_FUNCTIONS.md)

---

## Exercise 05 — Harl 2.0

### Goal
A `Harl` class with private `debug()`, `info()`, `warning()`,
`error()`, and a public `complain(std::string level)` that dispatches
to the right one **without** an `if/else if` chain. The designated
tool: **pointer to member function**.

### What the exercise drills

- **Pointer to member function** syntax. Very different from free-
  function pointers because the type has to carry the class:
  `void (Harl::*p)()` — "pointer to a member of `Harl` taking no
  args and returning `void`".
- **Calling via a member-function pointer.** `(this->*p)()` or
  `(obj.*p)()`. The extra `.*` / `->*` is the member-function-call
  operator. Parentheses are mandatory — `.*` has awkward
  precedence.
- **A table-driven dispatcher.** An array of
  `{std::string, void (Harl::*)()}` pairs, looped over. Match the
  string → call the member.

### The dispatch, sketched

```cpp
void Harl::complain(std::string level) {
    std::string names[4] = {"DEBUG", "INFO", "WARNING", "ERROR"};
    void (Harl::*fns[4])() = {&Harl::debug, &Harl::info,
                              &Harl::warning, &Harl::error};
    for (int i = 0; i < 4; ++i)
        if (names[i] == level)
            (this->*fns[i])();
}
```

### Why not `if/else`

The subject says so — that's the pedagogical point. In real code, a
4-way string dispatch is fine with `if/else`. The exercise exists
so you've *held* a member-function pointer at least once.

### Hardware-level notes

- A pointer to a **non-virtual** member function is one pointer
  under the hood (just the function's address). The hidden `this`
  argument is passed as the first parameter when you call it.
- A pointer to a **virtual** member function is more complex — on
  typical ABIs it's a pair `(offset_into_vtable, adjustment)`. The
  call looks up the vtable at runtime. You don't have virtuals in
  Harl, so this is the simpler case.
- `sizeof(void (Harl::*)())` may be 8 or 16 bytes depending on
  whether virtuals are involved in the hierarchy. Don't assume
  `sizeof` parity with a plain function pointer.

### Tips & gotchas

- **`&Harl::debug`** — the `&` is mandatory in c++98. It's not
  optional like for free-function pointers.
- **`(this->*fns[i])();`** — don't drop the parens. `this->*fns[i]()`
  parses as `this->*(fns[i]())` and fails.
- **Make the four methods private.** The subject insists. `complain`
  is the public front door.
- **No match → do nothing** (or print once, depending on how you
  read the spec). The subject's sample output is quiet on unknown
  levels.

### Search terms
`"c++ pointer to member function syntax"`,
`"c++ .* operator"`,
`"c++ dispatch table member functions"`.

---

## Exercise 06 — Harl filter (bonus)

### Goal
`./harlFilter <level>` — print every message from `<level>` up. If
`level = "WARNING"`, print warning + error, but not debug/info. Must
use a **`switch`**.

### What the exercise drills

- **`switch` on an `int`** (c++98 can't switch on `std::string`).
  The usual trick: a small function `levelToInt(const std::string&)`
  returning 0..3 (plus a sentinel for "unknown").
- **Intentional fall-through.** The point of `switch` here is that
  `case DEBUG:` falls through to `case INFO:` falls through to
  `case WARNING:` falls through to `case ERROR:`, each calling the
  matching method. No `break;` between the cases you want to chain.
- **Default for unknown levels.** Print `"[ Probably complaining about
  insignificant problems ]"` or similar — subject wording varies.

### Hardware-level notes

- A c++98 `switch` on a small integer range typically compiles to a
  **jump table** — an array of code addresses indexed by the
  switch value. That's `O(1)` dispatch, faster than the
  `if/else if` chain.
- For sparse or widely-spaced case values, the compiler emits a
  chain of compares instead. For 4 consecutive ints (0..3),
  definitely a jump table.

### Tips & gotchas

- **Fall-through is legal and intended here.** Modern compilers
  sometimes warn about it. `-Wall` on gcc c++98 usually doesn't,
  but if you see `-Wimplicit-fallthrough`, it's a false positive
  in this context.
- **Reuse ex05's `Harl::complain`** or rewrite the dispatch — both
  fine.
- **Argv count check.** Exactly one argument (the level).

### Search terms
`"c++ switch on string workaround"`,
`"c++ switch fall-through intended"`,
`"c++ jump table switch"`.

---

## Module-level sanity checks before moulinette

- `valgrind ./program` on every ex — no leaks, no invalid reads.
  ex01 (`new[]`/`delete[]`) is the common leak source.
- `-Wall -Wextra -Werror` — zero warnings.
- ex00 trace output (destructors firing) matches subject order.
- ex04 output file (`filename.replace`) diffs clean against a known
  good run.
- No `friend`, no `using namespace`, no function bodies in headers.

The module does not have a bonus-gate; ex06 is extra.
