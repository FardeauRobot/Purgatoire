# When `new` Fails — `bad_alloc`, `operator new`, and the machinery

The companion to [`MEMORY.md`](MEMORY.md). That note covers *how*
allocation works — stack vs heap, the `new`/`delete` protocol,
lifetime, valgrind. This one covers the part that surprises C
programmers: **what happens when allocation fails**, and **what
`new` actually is underneath**.

The one-line takeaway: in C you check `if (!ptr)` because `malloc`
returns `NULL`. In C++, `new` **never returns `NULL`** — it
**throws**. So that check is dead code, and the whole error model
is different.

> **Keyword zoom-ins:** [`NEW`](../../lexique/NEW.md) · [`DELETE`](../../lexique/DELETE.md) · [`TRY/CATCH/THROW`](../../lexique/TRY_CATCH_THROW.md)
> **See also:** [`ERROR_MANAGEMENT.md`](../io-errors/ERROR_MANAGEMENT.md) (exceptions), [`POLYMORPHISM.md`](../oop/POLYMORPHISM.md) (virtual dtor, why `delete` matters)

---

## Table of Contents

1. [`new` throws, it never returns NULL](#1.%20new%20throws%2C%20it%20never%20returns%20NULL)
2. [Why C and C++ differ](#2.%20Why%20C%20and%20C%2B%2B%20differ)
3. [No `try`/`catch` → `terminate()` → `abort()`](#3.%20No%20try%2Fcatch%20%E2%86%92%20terminate%28%29%20%E2%86%92%20abort%28%29)
4. [`bad_alloc` abort vs segfault](#4.%20bad_alloc%20abort%20vs%20segfault)
5. [Catching it, and the `nothrow` escape hatch](#5.%20Catching%20it%2C%20and%20the%20nothrow%20escape%20hatch)
6. [Simulating a failure (and the traps)](#6.%20Simulating%20a%20failure%20%28and%20the%20traps%29)
7. [Under the hood: the new-expression vs `operator new`](#7.%20Under%20the%20hood%3A%20the%20new-expression%20vs%20operator%20new)
8. [Overriding `operator new`](#8.%20Overriding%20operator%20new)
9. [42 guidance](#9.%2042%20guidance)

---

## 1. `new` throws, it never returns NULL

```cpp
Animal *p = new Animal();
if (!p)            // ❌ DEAD CODE — this branch can never be true
    ...;
```

When `new` **succeeds**, `p` is valid. When it **fails**, it throws
`std::bad_alloc` *before* the assignment happens — so control never
reaches the next line holding a bad `p`. There is no in-between
state where `p` points at something broken. The `if (!p)` check is
pure C reflex; delete it.

**What `p` holds on failure depends on the variant:**

| Form | On failure, `p`… | `if (!p)` valid? |
|---|---|---|
| `new Animal()` | never assigned — control jumps to nearest `catch` | no |
| `new (std::nothrow) Animal()` | holds `NULL` | yes |
| C's `malloc` | holds `NULL` | yes |

---

## 2. Why C and C++ differ

C and C++ are related, but `new` and `malloc` are **two different
tools with two different failure protocols** — and both exist in
C++ (yes, `malloc` still returns `NULL` in C++; it was never
changed).

The deep reason `new` can't just return `NULL` is **constructors**:

```c
void *p = malloc(size);     // C: ONE job — find bytes. NULL = "failed".
```
```cpp
Animal *p = new Animal();   // C++: (1) allocate  AND  (2) run the ctor
```

`new` allocates **and constructs**. A constructor has **no return
value** — it can't `return NULL` to signal failure. C++ already
solved "how does a constructor report failure?" with **exceptions**
(a failing ctor throws). Since `new` includes construction, the
whole operation uses that same channel. So:

- **C / `malloc`** — minimal, no runtime machinery, errors via
  return value. "Trust the programmer." Your `if (!ptr)` drill.
- **C++ / `new`** — adds construction → must use exceptions, because
  return codes can't escape a constructor.

The discipline carries over, it just changes target: in C++ the
"protection" isn't NULL-checking, it's **matching every `new` with
a `delete`** (see [`MEMORY.md`](MEMORY.md#3.%20Heap%20%E2%80%94%20new%20and%20delete)).

---

## 3. No `try`/`catch` → `terminate()` → `abort()`

An uncaught `bad_alloc` doesn't corrupt anything or hang — it
**propagates up the call stack** looking for a handler, and if none
exists:

```
new throws bad_alloc
  → no handler in this function   → unwind up
  → no handler in main()          → unwind past main
  → nothing above main            → std::terminate()
  → terminate() calls abort()     → SIGABRT (6), exit code 134
```

Real output observed on macOS / Apple Silicon (`-std=c++98`):

```
libc++abi: terminating due to uncaught exception of type std::bad_alloc: std::bad_alloc
exit code: 134                 # 128 + 6 (SIGABRT)
```

**Stack unwinding subtlety:** as the exception travels up, automatic
(stack) objects in the abandoned scopes get their destructors run
properly. But **heap objects you already `new`'d and haven't
`delete`'d yet leak** — the `delete` lines get skipped:

```cpp
Animal *a = new Dog();   // succeeds
Animal *b = new Cat();   // throws here
delete a;                // ← SKIPPED — a leaks during unwinding
delete b;
```

(For a *crashing* program this doesn't matter — the OS reclaims
everything on exit. It matters in long-running code like a server.)
Note: whether the stack is unwound at all for an *uncaught*
exception is implementation-defined.

---

## 4. `bad_alloc` abort vs segfault

Both end the program, but they're opposite kinds of death.

| | `std::bad_alloc` abort | Segfault |
|---|---|---|
| Signal / exit | `SIGABRT` (6), exit **134** | `SIGSEGV` (11), exit **139** |
| Who triggered it | **your program**, on purpose | the **OS/hardware**, against your will |
| Behavior class | **defined** — planned failure path | **undefined** — illegal memory access |
| Message | named & specific: `std::bad_alloc` | generic: `Segmentation fault` |
| Catchable? | **yes** — `try`/`catch` recovers | **no** — it's a signal, not an exception |
| C analogy | (C has no equivalent) | the classic C crash (NULL deref, bad free) |

**Mental model:** the `bad_alloc` abort is a *fire alarm* — the
safety system detecting trouble and shutting down in an orderly way
(and you can install a `catch` to handle it). A segfault is the
*floor collapsing* — no warning, no handler, you touched memory you
weren't allowed to and the hardware stopped you. The first is the
*good* failure (named, defined, recoverable); the second is the
symptom of undefined behavior in your pointers.

---

## 5. Catching it, and the `nothrow` escape hatch

If you want to handle it rather than terminate:

```cpp
try {
    Animal *p = new Animal();
    // ...
    delete p;
}
catch (const std::bad_alloc &e) {   // needs #include <new>
    std::cerr << "allocation failed: " << e.what() << std::endl;
}
```

Caught, the program recovers and exits 0.

If you genuinely want C-style `NULL`-on-failure, opt in with the
**`nothrow`** form — the one case where `if (!p)` after `new` is
correct:

```cpp
Animal *p = new (std::nothrow) Animal();
if (!p)            // ✓ valid here — nothrow makes new return NULL
    ...;
```

---

## 6. Simulating a failure (and the traps)

To force `bad_alloc`, ask for more memory than can ever be
satisfied — but two traps make this fiddly:

### Trap A — `()` vs `[]`

```cpp
new char(n);    // a SINGLE char, initialized to value n  → 1 byte, always succeeds
new char[n];    // an ARRAY of n chars                     → n bytes, can fail
```

Round brackets allocate one object and use `n` as its *value*;
square brackets use `n` as a *count*. To request a huge allocation
you need `[]`.

### Trap B — the size must be a runtime value

```cpp
new char[static_cast<std::size_t>(-1)];   // ❌ COMPILE error: "array is too large"
```

A compile-time-constant size gets folded and rejected by the
compiler before the program runs — that's not the runtime failure
you want. Hide the size behind a runtime value:

```cpp
char *boom = new char[static_cast<std::size_t>(-1) - argc];   // ✓ throws at runtime
```

`static_cast<std::size_t>(-1)` is the C++98-clean way to write
`SIZE_MAX` (`-1` wraps to all-bits-set).

---

## 7. Under the hood: the new-expression vs `operator new`

"How `new` is coded" splits into two halves:

**Half 1 — `operator new`: a real, replaceable function.** This is
the allocation function `new` calls. Its declaration lives in
`<new>` (marked *"replaceable"*), and its default definition is
essentially:

```cpp
void *operator new(std::size_t size) throw(std::bad_alloc) {  // C++98 spec
    void *p = malloc(size);
    while (!p) {
        new_handler h = get_new_handler();   // optional installed hook
        if (!h) throw std::bad_alloc();       // give up → throw
        h();                                  // else let it free memory...
        p = malloc(size);                     // ...and retry
    }
    return p;
}
```

So the default `operator new` really **does call `malloc`** and
**does `throw std::bad_alloc`** — confirming everything above. The
`new_handler` loop is a hook you can install to try to free memory
before giving up.

**Half 2 — the new-expression: a language rule.** `new Animal()`
itself is compiler-generated grammar, not a function you can read:

```
new Animal()  expands to:
  1. operator new(sizeof(Animal))     → raw storage     ← Half 1 (a function)
  2. run Animal::Animal() on it       → construct        ← language rule
  3. yield a typed Animal*                                ← language rule
```

This is the allocate-then-construct split: you can *replace* the
allocate step, but the construct step is baked into the language.
`delete` mirrors it in reverse: destruct, then `operator delete`
(→ `free`).

> The C++98 `throw(std::bad_alloc)` is a *dynamic exception
> specification* — deprecated in C++11, removed later, but
> `-std=c++98` still expects it. Omitting it triggers
> `-Wmissing-exception-spec`.

---

## 8. Overriding `operator new`

Because `operator new` is a real function, you can substitute it.
Two distinct moves:

- **Replace the global one** — there's exactly one in the program;
  define your own and the linker uses it everywhere.
- **Per-class `operator new`** — a `static` member; only that class
  (and derivatives) uses it. Others keep the default.

```cpp
class Animal {
    public:
        static void *operator new(std::size_t size) {
            std::cout << "Animal new: " << size << " bytes" << std::endl;
            return ::operator new(size);     // delegate to the global one
        }
        static void operator delete(void *p) {
            ::operator delete(p);
        }
};
```

(`operator new`/`delete` are implicitly `static` — there's no
object yet when allocation happens.) Overloads with *extra*
parameters coexist with the default: `nothrow` and placement-`new`
(`new (ptr) T`) are exactly this.

**Why anyone does it:** allocation tracking / leak counters,
memory pools (pre-carved blocks for objects allocated millions of
times), special memory (alignment, shared/GPU memory). A custom
global `operator new`/`operator delete` pair is the standard trick
for a **homemade leak counter** — useful here since valgrind doesn't
run on Apple Silicon.

---

## 9. 42 guidance

- **Don't `if (!ptr)` after `new`.** Dead code. (Only valid with
  `new (std::nothrow)`.)
- **Don't wrap `new` in `try`/`catch` in Mod00–04.** `bad_alloc`
  only fires on genuine OOM, which never happens for toy programs.
  Letting it propagate (and `terminate`) is the idiomatic default —
  matches the CLAUDE.md rule "no error handling for impossible
  cases."
- **Do match every `new` with a `delete`** (and `new[]` with
  `delete[]`). That's the real C++ "protection" — see
  [`MEMORY.md`](MEMORY.md).
- **Failure experiments are not deliverables.** A `bad_alloc` probe
  is great for *understanding*; strip it before submission — a
  grader shouldn't find a stray giant allocation.

---

## See also

- [`MEMORY.md`](MEMORY.md) — the mechanics: stack/heap, `new`/
  `delete`, `new[]`, lifetime, RAII, valgrind. **Read first.**
- [`ERROR_MANAGEMENT.md`](../io-errors/ERROR_MANAGEMENT.md) —
  exceptions, `try`/`catch`, stack unwinding in general.
- [`POLYMORPHISM.md`](../oop/POLYMORPHISM.md) — virtual destructor:
  why `delete` through a base pointer needs it, object slicing.
