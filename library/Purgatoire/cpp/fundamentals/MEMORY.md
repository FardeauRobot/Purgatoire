# Memory in C++ — Complete Guide

Where objects live, how they're allocated, when they die. The four
storage durations, the `new` / `delete` protocol, the arrays
variant, and the hardware view underneath.

Written with 42 Mod 01 as the anchor, but applies to every module
that owns a resource. If you confuse `new` with `new[]`, or leak
through a non-virtual destructor, the fix starts here.

---

## Table of Contents

1. [The four storage durations](#1-the-four-storage-durations)
2. [Stack — the automatic storage](#2-stack--the-automatic-storage)
3. [Heap — `new` and `delete`](#3-heap--new-and-delete)
4. [Arrays — `new[]` and `delete[]`](#4-arrays--new-and-delete)
5. [Static and thread-local storage](#5-static-and-thread-local-storage)
6. [Object lifetime, in order](#6-object-lifetime-in-order)
7. [RAII — the central idea](#7-raii--the-central-idea)
8. [What happens at the machine level](#8-what-happens-at-the-machine-level)
9. [`new` vs `malloc`, `delete` vs `free`](#9-new-vs-malloc-delete-vs-free)
10. [Common bugs and how to spot them](#10-common-bugs-and-how-to-spot-them)
11. [Valgrind — reading the output](#11-valgrind--reading-the-output)
12. [In the CPP modules](#12-in-the-cpp-modules)
13. [Rules of thumb](#13-rules-of-thumb)

---

## 1. The four storage durations

Every C++ object has exactly one:

| Duration | Where it lives | When it dies | How you get it |
|---|---|---|---|
| **Automatic** | stack | scope exit | `T x;` inside a function |
| **Dynamic** | heap | when you `delete` it | `T* p = new T;` |
| **Static** | `.data` / `.bss` | program exit | `static T x;` or file-scope |
| **Thread** (c++11+) | per-thread storage | thread exit | `thread_local T x;` — not in c++98 |

For 42 (c++98), you work with the first three. Thread storage
doesn't exist yet; `thread_local` was introduced in c++11.

---

## 2. Stack — the automatic storage

### Declaration

```cpp
void f() {
    Zombie z("Bob");    // automatic, on the stack
    // use z...
}                       // ← ~Zombie() fires here; z is gone
```

No `new`, no `delete`. The object lives for the span of the block
it was declared in.

### When to use

- Short-lived values, known size at compile time.
- Anything that fits in a function scope.
- When you want destruction to happen automatically on exit.

### The cost

Zero beyond the object's size. Allocating a stack object is a
**single register update** — the compiler subtracts the object's
size from the stack pointer (`rsp` on x86-64) when the function
enters, adds it back when the function returns.

### The limit

Stack size is bounded (default 8 MB on Linux, 1 MB on macOS
threads). Allocating a 10 MB array on the stack crashes with a
segfault. Rule: if it's bigger than a few KB, put it on the heap.

### The gotcha — lifetime

```cpp
Zombie* bad() {
    Zombie z("Bob");
    return &z;       // ← returning address of a dying object
}                    // z is destroyed before the caller reads *result
```

The stack frame containing `z` disappears when `bad` returns. The
caller now holds a pointer to deallocated storage. Read/write =
undefined behaviour.

---

## 3. Heap — `new` and `delete`

### Declaration

```cpp
Zombie* z = new Zombie("Bob");   // one construction
// use z...
delete z;                         // one destruction + free
```

### The contract

**Every `new` must be paired with exactly one `delete`.**

- Forget the `delete` → **leak**. Memory stays allocated until
  program exit. Valgrind reports "definitely lost".
- Call `delete` twice on the same pointer → **double-free**. Heap
  corruption, often a crash on the second call or later in an
  unrelated allocation.
- `delete` a pointer that wasn't returned by `new` → **undefined
  behaviour**. Same for `delete` on `NULL` — except `NULL` is
  explicitly safe (no-op).

### What `new` does

```cpp
Zombie* z = new Zombie("Bob");
```

...is equivalent to:

```cpp
void* raw = operator new(sizeof(Zombie));   // 1. allocate raw memory
Zombie* z = new (raw) Zombie("Bob");        // 2. construct in place
```

Two steps: **raw allocation** (goes through `malloc` ultimately)
then **in-place construction** (calls the ctor). If the ctor
throws, `operator delete` is called on the raw memory
automatically — no leak.

### What `delete` does

```cpp
delete z;
```

...is equivalent to:

```cpp
z->~Zombie();                     // 1. run destructor
operator delete(z);               // 2. free raw memory
```

Two steps in reverse: **destructor** (may trigger chains of
sub-destructors — e.g. `std::string` freeing its heap buffer),
then **raw deallocation** (back to `free`).

### When to use

- Object must outlive the function that created it.
- Size or count is only known at runtime.
- Polymorphism — you need an `Animal*` that actually points to
  a `Dog`, which means the object can't be an `Animal` on the
  stack (it's a `Dog`; it has to live through a base pointer).

### The gotcha — ownership

When you return a heap pointer, you're handing the caller a
**responsibility**: they must `delete` it. Document the contract
in the header or with a naming convention (`newFoo()` conventionally
returns something the caller must delete).

---

## 4. Arrays — `new[]` and `delete[]`

### The syntax

```cpp
Zombie* horde = new Zombie[10];   // 10 default-constructed zombies
// ...
delete[] horde;                    // ALL 10 destructors run, then free
```

### The key rules

- `new T[N]` invokes `T`'s **default constructor** N times. You
  can't pass constructor arguments per-element in c++98.
- `new T[N]` is **one** `operator new[]` call for `N * sizeof(T) +
  cookie`. Not N allocations.
- `delete[] p` runs **all N destructors** in reverse order, then
  deallocates the whole block in one call.
- **`new T[N]` paired with `delete p` (no `[]`) is UB.** Allocator
  bookkeeping mismatch — usually a crash, sometimes a silent
  corruption.
- **`new T` paired with `delete[] p` is UB** too.

### The cookie

The allocator needs to know N so `delete[]` can run N destructors.
Typical implementation: prepend 8 bytes to the block storing N.

```
   returned ptr
        ↓
┌────┬──┬──┬──┬──┬──────────┐
│ N  │T0│T1│T2│..│(padding) │
└────┴──┴──┴──┴──┴──────────┘
 ← 8 →
(cookie)
```

`delete[] ptr` does `N = ptr[-8]`, then destroys `ptr[N-1..0]`,
then frees `ptr - 8`.

### Why default-ctor-only

`new T[N]("arg")` doesn't exist. If `T` has no default ctor, this
line fails to compile. You initialise elements afterwards:

```cpp
Zombie* horde = new Zombie[N];
for (int i = 0; i < N; ++i)
    horde[i].setName(chosenName(i));
```

### When to use

Rarely, at 42. `std::vector<T>` covers most cases from Mod 08
onwards. Mod 01 ex01 uses `new[]` specifically to make you see
the array-new / array-delete distinction.

---

## 5. Static and thread-local storage

### Static duration

```cpp
static int counter = 0;    // file scope, or inside a function (once)
```

- Initialised **once**, before `main` starts (or on first use if
  function-local).
- Destroyed after `main` returns.
- Lives in the `.data` segment (if initialised) or `.bss` (if
  zero-initialised) of the program image — literally part of the
  binary on disk for `.data`, allocated at load time for `.bss`.

### Class-static members

```cpp
class Account {
    static int _nbAccounts;   // declaration
};

// In .cpp:
int Account::_nbAccounts = 0;   // definition — allocates storage
```

One storage per class, shared across all instances. Mod 00's
`Account` exercise drills exactly this.

### Thread-local — c++11+ only

Not available in c++98. Ignore for 42.

---

## 6. Object lifetime, in order

For an automatic variable:

```
Entering scope:
  1. Raw storage allocated on stack (sp decrement).
  2. Constructor runs.
  3. ← Object is alive, usable.
Exiting scope:
  4. Destructor runs.
  5. Raw storage released (sp increment).
```

For a heap object:

```
new:
  1. operator new: ask for raw memory (typically malloc).
  2. Constructor runs.
  3. ← Object is alive, usable.
delete:
  4. Destructor runs.
  5. operator delete: return memory to the allocator (typically free).
```

For an array:

```
new T[N]:
  1. operator new[]: one big malloc for N*sizeof(T) + cookie.
  2. For i in 0..N-1: construct T at offset i. If ctor i throws,
     destroy 0..i-1 in reverse, then rethrow.
  3. ← N objects alive.
delete[] p:
  4. Read N from cookie.
  5. For i in N-1..0: destroy T at offset i.
  6. operator delete[]: free.
```

---

## 7. RAII — the central idea

**Resource Acquisition Is Initialisation.** Every resource (heap
memory, file handle, socket, mutex) is **owned** by an object, and
**released** in that object's destructor.

```cpp
class FileReader {
    std::ifstream ifs;
public:
    FileReader(std::string const& path) : ifs(path.c_str()) {
        if (!ifs) throw std::runtime_error("open failed");
    }
    ~FileReader() {
        // ifs's destructor closes the file automatically
    }
};

void f() {
    FileReader r("data.txt");
    // ... use r ...
}   // ← file closed here, no matter what (even on exception)
```

The `ifstream` member's destructor closes the file. `r` is on the
stack, so its destructor is guaranteed to run on scope exit —
normal return *or* exception propagation. No leak paths.

**RAII is why you rarely need `try/finally` in C++.** You get
deterministic cleanup for free.

For heap memory, RAII means: **wrap `new`/`delete` in a class whose
destructor does the `delete`**. `std::string` is the textbook
example — you never call `free` on a `std::string`'s buffer
because `~string()` does it.

---

## 8. What happens at the machine level

### Stack allocation

```cpp
Zombie z("Bob");    // sizeof(Zombie) = 32 bytes, say
```

Compiler output (x86-64, simplified):

```asm
sub  rsp, 32              ; reserve 32 bytes on the stack
lea  rdi, [rsp]           ; point rdi at the reserved space
mov  rsi, "Bob"           ; arg 2: name
call Zombie::Zombie       ; run ctor on stack slot
```

On function return:

```asm
lea  rdi, [rsp]
call Zombie::~Zombie      ; dtor
add  rsp, 32              ; release stack
ret
```

Total overhead: 2 instructions for allocation (`sub`/`add`). The
object lives in L1 cache if the stack is hot (nearly always).

### Heap allocation

```cpp
Zombie* z = new Zombie("Bob");
```

...compiles to roughly:

```asm
mov  rdi, 32              ; arg: size
call operator new          ; returns ptr in rax
mov  rdi, rax
mov  rsi, "Bob"
call Zombie::Zombie       ; ctor
```

`operator new` eventually calls `malloc`, which:

1. Checks the allocator's free list for a free 32-byte slot.
2. Hit → O(1), returns the slot.
3. Miss → asks the kernel for more pages (`brk` or `mmap`). The
   kernel marks virtual pages and lazily maps them to physical
   RAM on first access.

Typical cost: 10-100 ns on L1-hot free list, microseconds on a
fresh `mmap`. Allocation isn't free.

### Heap deallocation

```cpp
delete z;
```

...is:

```asm
mov  rdi, z_ptr
call Zombie::~Zombie
mov  rdi, z_ptr
call operator delete
```

`operator delete` → `free`. The memory typically stays with the
allocator (goes back on the free list), not returned to the
kernel. That's why `valgrind` can show a leak even though the
heap is empty at program exit — the process image still holds
those pages.

### Array `new[]`

For `new Zombie[10]`:

```asm
mov  rdi, 328             ; 10 * 32 + 8 (cookie)
call operator new[]
mov  [rax], 10            ; store N in cookie
lea  rbx, [rax + 8]       ; ptr to first element (skipping cookie)
; loop 10 times: ctor at rbx + 32*i
; return rbx
```

On `delete[]`:

```asm
mov  rax, [rbx - 8]       ; read N
; loop: dtor at rbx + 32*i, i = N-1..0
lea  rdi, [rbx - 8]
call operator delete[]
```

---

## 9. `new` vs `malloc`, `delete` vs `free`

| Feature | `malloc`/`free` | `new`/`delete` |
|---|---|---|
| Origin | C | C++ |
| Runs ctor | ✗ | ✓ |
| Runs dtor | ✗ | ✓ |
| Type-safe | ✗ (returns `void*`) | ✓ |
| On OOM | returns `NULL` | throws `std::bad_alloc` |
| Overridable per class | no | yes |
| Pairs with | `free` only | `delete` (or `delete[]`) |

**Never mix them.** `malloc` + `delete` = UB. `new` + `free` = UB.

At 42 you use `new`/`delete` exclusively (the subject forbids
`*alloc`/`free`). That includes the inside of `operator new` —
if you override it, use `malloc` internally is *permitted* because
that's how you'd implement it. But from the consumer side, stay in
`new`/`delete`-land.

---

## 10. Common bugs and how to spot them

### Leak

```cpp
Zombie* z = new Zombie("Bob");
// ... forgot delete ...
```

**Symptom:** `valgrind` reports "definitely lost: 32 bytes in 1
blocks".

**Fix:** add the missing `delete` on every exit path (return,
exception, early break).

### Double-free

```cpp
delete z;
// ... later ...
delete z;   // UB
```

**Symptom:** crash on the second `delete` or later, sometimes with
`*** Error in '...': double free or corruption ***`.

**Fix:** set the pointer to `NULL` after `delete` (harmless double-
`delete` on null is safe), or avoid the duplicate delete entirely.

### Use-after-free

```cpp
delete z;
z->attack("somewhere");   // UB
```

**Symptom:** random crash, or worse, silently reads freed memory
(sometimes works, sometimes garbage). Valgrind catches it with
"Invalid read of size N".

**Fix:** don't.

### Mismatched `new` / `delete[]`

```cpp
Zombie* horde = new Zombie[10];
delete horde;               // UB: should be delete[]
```

**Symptom:** valgrind: "Mismatched free() / delete / delete[]".

**Fix:** match them. `new` ↔ `delete`, `new[]` ↔ `delete[]`.

### Leak through non-virtual destructor

```cpp
Animal* a = new Dog();   // Dog owns a Brain*
delete a;                 // if ~Animal is not virtual: Brain leaks
```

**Symptom:** valgrind shows a leak of the `Brain`. If `Brain`
owns something of its own, the leak cascades.

**Fix:** `virtual ~Animal();`.

---

## 11. Valgrind — reading the output

```bash
valgrind --leak-check=full --show-leak-kinds=all ./program
```

Key messages:

- **`definitely lost`** — a pointer to allocated memory was
  overwritten or went out of scope without a `delete`.
- **`indirectly lost`** — the thing that owns this is itself lost.
  Fix the parent leak, this goes away.
- **`possibly lost`** — valgrind isn't sure (e.g. pointer points
  into the middle of a block). Usually your fault anyway.
- **`still reachable`** — at exit, the pointer was still held by
  something, but there was no `delete` before exit. Not strictly a
  leak (the OS reclaims it), but the subject treats it as one.
- **`Invalid read/write of size N`** — use-after-free or OOB. Stop
  everything, read the stack trace.

Valgrind stack traces point at the **allocation** or
**deallocation** site, not where the bug surfaced. The backtrace
is the thread where the bad op happened.

---

## 12. In the CPP modules

| Module | Memory concept drilled |
|---|---|
| Mod 00 | Stack (local objects, phonebook array of `Contact`) |
| Mod 01 ex00 | Stack vs heap explicitly (`newZombie` vs `randomChump`) |
| Mod 01 ex01 | Array heap (`new Zombie[N]` + `delete[]`) |
| Mod 01 ex03 | Reference member (no allocation, but memory-ish decisions) |
| Mod 02 | Stack, all of it — `Fixed` has no heap |
| Mod 03 | Stack, mostly — classes hold `std::string` only |
| Mod 04 ex01+ | Heap + virtual dtor + deep copy of polymorphic pointers |
| Mod 08+ | `std::vector`, `std::list` — heap but RAII-wrapped |

Mod 04 ex01 is where all of this converges. Get it right there, the
rest feels obvious.

---

## 13. Rules of thumb

1. **Prefer the stack.** If you don't *need* the heap, don't use
   it. Automatic cleanup is the strongest safety feature.
2. **One owner per resource.** Every heap allocation has exactly
   one class/function responsible for its `delete`. If you're
   not sure which, redesign.
3. **RAII everything.** The thing that allocates is the same
   thing that cleans up — usually via a destructor.
4. **Match every `new` with a `delete`, every `new[]` with
   `delete[]`.** No exceptions.
5. **Virtual destructor when deleting through a base pointer.**
   Always.
6. **Self-assignment guard in `operator=`.** Even when it seems
   theatre.
7. **Delete old before assigning new** inside `operator=`, or you
   leak.
8. **Valgrind before every push.** Leak-free is a hard gate, not
   a nice-to-have.

---

## See also

- [`REFERENCE.md`](REFERENCE.md) — references don't allocate but
  have ownership-adjacent semantics.
- [`ORTHODOX_CANONICAL_FORM.md`](../oop/ORTHODOX_CANONICAL_FORM.md)
  — OCF is the RAII machinery for classes that own resources.
- [`POLYMORPHISM.md`](../oop/POLYMORPHISM.md) — virtual destructor,
  object slicing.
- [`CASTS.md`](../advanced/CASTS.md) — casting heap pointers.
