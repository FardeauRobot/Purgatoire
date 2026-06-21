 # PHILOSOPHY.md

*"Make it correct, make it clear, make it concise, make it fast. In that order."* — Wietse Venema

A north-star document for the 42 C++ piscine. Five pillars to internalize, plus the state of mind that turns *"writing C++"* into *"thinking in C++"*. Read it once now, then come back when each pillar shows up in your modules.

---

## 🧭 The one-line philosophy

> **"You don't pay for what you don't use, and what you do use, you couldn't write better by hand."** — Bjarne Stroustrup

Every C++ design decision flows from this. The language refuses to hide the machine, and refuses to charge you for abstractions you didn't ask for. **Total control, total responsibility.**

---

## 📑 TL;DR — the five pillars

| # | Pillar | Slogan | First seen in |
|---|---|---|---|
| 1 | [Object Lifetime & RAII](#1-object-lifetime--raii) | "Resources live and die with objects." | Mod01 |
| 2 | [Value Semantics & OCF](#2-value-semantics--orthodox-canonical-form) | "What does it mean to copy me?" | Mod02 |
| 3 | [Const-Correctness](#3-const-correctness--type-safety) | "Make the compiler enforce your contracts." | Mod02 |
| 4 | [Static + Dynamic Polymorphism](#4-polymorphism--static-and-dynamic) | "Decide at compile time when you can; at runtime when you must." | Mod03 / Mod07 |
| 5 | [Memory Layout & Zero-Overhead](#5-memory-layout--zero-overhead-abstraction) | "Know what your code compiles to." | Mod02+ implicitly |

---

## 1. Object Lifetime & RAII

> *"In C++, an object isn't just data — it's a thing that exists between two specific moments: construction and destruction."*

### The idea

Every object has a birth (constructor) and a death (destructor). Between them, its destructor *will* run — that's a guarantee the language makes. Whole language idioms are built on this.

**RAII** = **R**esource **A**cquisition **I**s **I**nitialization. Bind any resource — memory, file handle, mutex, socket, lock — to the lifetime of an object. When the object dies (going out of scope, exception thrown, function returns), the resource is released *automatically*.

### Why it matters

In C, `fopen` requires you to remember `fclose` on every code path:

```c
FILE *f = fopen("data.txt", "r");
if (!f) return -1;
if (parse(f) < 0) {
    fclose(f);          // don't forget this one
    return -1;
}
if (validate(f) < 0) {
    fclose(f);          // …or this one
    return -1;
}
fclose(f);              // …or this one
return 0;
```

In C++, the destructor handles every path:

```cpp
{
    std::ifstream f("data.txt");      // ← acquires file
    if (!f) return -1;
    parse(f);
    if (bad)
        throw std::runtime_error("nope");  // ← still cleans up
}                                     // ← f.~ifstream() closes the file. Always.
```

**Goodbye to whole categories of bugs**: leaked file descriptors, dangling sockets, never-released mutexes, forgotten `delete`s.

### The same pattern, every resource

```cpp
class Buffer {
    char *_data;
    size_t _size;
public:
    Buffer(size_t n) : _data(new char[n]), _size(n) {}  // acquire
    ~Buffer() { delete[] _data; }                       // release — guaranteed
};
```

A `Buffer` on the stack? Heap is freed on scope exit. As a member of another class? Freed when the parent dies. Inside a `std::vector<Buffer>`? Each one freed on container destruction. **The pattern composes.**

### Where you meet it

- **Mod01 ex02–06**: when you start writing `Zombie`, you're really learning the difference between stack-allocated zombies (auto-cleanup) and heap-allocated zombies (your responsibility).
- **Mod02+**: every class with `new` in its constructor needs `delete` in its destructor. RAII says *"that's the same object's job."*

📍 See: [`fundamentals/MEMORY.md`](fundamentals/MEMORY.md) · [`keywords/NEW.md`](keywords/NEW.md) · [`keywords/DELETE.md`](keywords/DELETE.md)

---

## 2. Value Semantics & Orthodox Canonical Form

> *"A C++ object should behave like an `int` — copyable, assignable, destroyable, with no surprises."*

### The idea

C, Rust, Java, Python — they all default to different semantics. C++'s default is **value semantics**: when you assign or pass by value, you get a *copy*. Fast (just memory), local (no aliasing surprises), explicit.

But to make value semantics work for *your* class, you owe the language **four members**:

```cpp
class Fixed {
public:
    Fixed();                                  // 1. default ctor
    Fixed(const Fixed& other);                // 2. copy ctor
    Fixed& operator=(const Fixed& other);     // 3. copy-assignment
    ~Fixed();                                 // 4. destructor
};
```

This is the **Orthodox Canonical Form** (OCF) — 42's term for what the C++ community calls the **Rule of Three**.

### The Rule of Three (C++98)

> *If you write **one** of [destructor, copy constructor, copy assignment], you almost certainly need **all three**.*

Why? If your class manages a resource (raw pointer, file, etc.), the **compiler-generated** copy will do the wrong thing — it shallow-copies the pointer, and now two objects believe they own it:

```
Before copy:                   After shallow copy:
┌──────┐                      ┌──────┐    ┌──────┐
│ obj1 │──→ [heap data]       │ obj1 │─┐  │ obj2 │
└──────┘                      └──────┘ ├─→[heap data]
                                       │
                              obj2 ────┘
                              ↑
                              Now both will free the same memory → 💥
```

→ Double-free on destruction. Crash, undefined behavior, valgrind tantrum.

### The deep mindset shift

For every class, ask: ***"What does it mean to copy me?"***

- For a value-like type (`Fixed`, `Vector3D`): "duplicate the bits" — easy.
- For a pointer-owning type (`Buffer` above): "deep-copy the buffer" — must be written.
- For a "I should never be copied" type (`std::ifstream`, mutex wrapper): "make copying a compile error" — in C++98, declare them `private` with no body.

You **decide** — but you must decide. The language won't pick for you.

### The canonical pattern

```cpp
Fixed::Fixed(const Fixed& other) : _value(other._value) {
    std::cout << "Copy constructor called" << std::endl;
}

Fixed& Fixed::operator=(const Fixed& other) {
    std::cout << "Assignment operator called" << std::endl;
    if (this != &other)             // self-assignment guard
        _value = other._value;
    return *this;                   // return *this for chaining
}
```

Three details that catch people:
1. **Copy ctor takes `const T&`** — not `T` (infinite recursion!) and not `T&` (can't bind to temporaries).
2. **`operator=` returns `T&`** — so `a = b = c` works.
3. **Self-assignment check** — `a = a` should be a no-op, not a destruction.

### Where you meet it

- **Mod02**: literally the entire module is teaching OCF via `Fixed`.
- **Mod03**: each derived class needs its own OCF (or to explicitly use base's).
- **Mod04**: virtual destructor enters the OCF.
- **Mod05–08**: it's just always there.

📍 See: [`oop/ORTHODOX_CANONICAL_FORM.md`](oop/ORTHODOX_CANONICAL_FORM.md) · [`oop/OPERATOR_OVERLOADING.md`](oop/OPERATOR_OVERLOADING.md)

---

## 3. Const-Correctness & Type Safety

> *"Express what you mean in the type. Make the compiler your QA engineer."*

### The idea

C++'s type system is its biggest under-appreciated tool. Most beginners treat types as *constraints they must satisfy*. Experts treat types as *contracts they leverage*.

```cpp
int          getValue() const;             // doesn't modify *this
const int&   getValue() const;             // returns read-only ref (no copy)
void         setValue(int v);              // takes by value (cheap for ints)
void         setName(const std::string&);  // const-ref: no copy, no mutation
```

Each `const` is a **promise** — to the caller, to the reader, to future-you. The compiler enforces it. Try to modify a `const` member inside a `const` method? Build fails. Try to pass a `const` object to a non-`const` setter? Build fails. That's not annoying — that's *the language saving you from a bug*.

### The deeper idea: make illegal states unrepresentable

If a function should never modify its argument → take it `const`.
If a method should never modify the object → mark it `const`.
If a variable should never change → declare it `const`.
If a function should never throw → use `throw()` (C++98 spec).

Each one closes a door that bugs could walk through.

### A tale of two interfaces

```cpp
// Bad — every caller has to read the source to know what's safe
class Account {
    int balance;
    std::string owner;
public:
    int          getBalance();
    std::string  getOwner();
    void         deposit(int amount);
};

// Good — the signatures alone document behavior
class Account {
    int _balance;
    std::string _owner;
public:
    int                  getBalance() const;       // const → "I won't change"
    const std::string&   getOwner() const;         // const-ref → "no copy, read-only"
    void                 deposit(int amount);      // non-const → "I will change"
};
```

Reading the second one, a caller knows immediately what's safe to call on a `const Account&`. No source-diving.

### The `const` cheat sheet

```cpp
const int x;            // x is an int that cannot be modified
int *p;                 // p is a pointer to int (both mutable)
const int *p;           // p points to a const int (data immutable, p mutable)
int * const p;          // p is a const pointer to int (p immutable, data mutable)
const int * const p;    // both immutable
const std::string& s;   // reference to const string — *the* parameter idiom
```

**Read right-to-left**: `const int * const p` → "p is a const pointer to a const int."

### The mindset shift

Before writing the body of a function, write its **signature as a complete contract**. Then implement.

### Where you meet it

- **Mod00**: as soon as you write your first method, you'll wonder *"should this be const?"* The answer is *almost always yes*.
- **Mod02**: `getRawBits() const` and `setRawBits(int)` — the canonical contrast.
- **Mod04+**: const-correctness in inheritance trees gets subtle. Stick with it.

📍 See: [`keywords/CONST.md`](keywords/CONST.md)

---

## 4. Polymorphism — Static AND Dynamic

> *"There are two ways to write code that works for many types: choose at compile time (templates), or choose at runtime (virtual). Both are first-class in C++."*

### The idea

Most languages give you one polymorphism. C++ gives you **both**, on purpose, because they have different costs:

| | **Static (templates)** | **Dynamic (virtual)** |
|---|---|---|
| Type chosen | Compile time | Runtime |
| Cost per call | **Zero** (often inlined) | One indirection (vtable lookup) |
| Binary size | Larger (one copy per type) | Smaller |
| Use when | Type is known at compile time, max perf | Heterogeneous collections, plugins |
| Module | Mod07 | Mod03–04 |

### Static — templates

```cpp
template <typename T>
T max(T a, T b) {
    return a > b ? a : b;
}

int main() {
    max<int>(3, 7);          // generates max<int>
    max<double>(1.5, 2.0);   // generates max<double>
    max<std::string>("a", "b"); // generates max<std::string>
}
```

The compiler **stamps out a specialized version** for each type you use. Each call site jumps to specific machine code with zero runtime overhead.

### Dynamic — virtual functions

```cpp
class Animal {
public:
    virtual void speak() const = 0;       // pure virtual → abstract
    virtual ~Animal() {}                  // virtual dtor → mandatory!
};

class Dog : public Animal {
public:
    void speak() const { std::cout << "Woof!\n"; }
};

class Cat : public Animal {
public:
    void speak() const { std::cout << "Meow!\n"; }
};

Animal *zoo[] = { new Dog(), new Cat(), new Dog() };
for (int i = 0; i < 3; ++i)
    zoo[i]->speak();      // dispatched at runtime via vtable
```

Each polymorphic object carries a hidden **vptr** that points to its class's **vtable** — an array of function pointers. Calling `zoo[i]->speak()` becomes *"look up slot N in this object's vtable"* — one indirection, then a normal call.

### Memory picture (single inheritance)

```
new Dog():                              Dog::vtable (.rodata):
┌─────────────────────┐                 ┌───────────────────────┐
│ vptr                │────────────────►│ &Dog::speak           │
├─────────────────────┤                 │ &Dog::~Dog            │
│ Dog members…        │                 └───────────────────────┘
└─────────────────────┘
```

### The gotcha that defines Mod04: virtual destructor

```cpp
class Base { ~Base() {} };                    // BAD: non-virtual dtor
class Derived : public Base { /* owns heap */ };

Base *p = new Derived();
delete p;     // ← only Base::~Base runs. Derived::~Derived skipped. LEAK.
```

Make `~Base` virtual — and now `delete p` looks up the destructor in the vtable, finds `~Derived`, runs it (which then chains up to `~Base`). **Always make destructors virtual in any class meant to be a base class.**

### The philosophy: don't pay for dispatch you don't need

If you know the type at compile time → template, zero cost.
If you genuinely need to choose at runtime (heterogeneous list, plugin system) → virtual, one indirection.
If you reach for `virtual` reflexively because "OOP" → you're paying for nothing.

### The mindset shift

Ask *"when do I actually know the type?"*
- Compile time → template.
- Runtime → virtual.

### Where you meet it

- **Mod03**: inheritance, ctor/dtor chains, the diamond problem.
- **Mod04**: virtual, abstract, interfaces — the heart of dynamic dispatch.
- **Mod07**: function and class templates.
- **Mod08–09**: STL is templates *all the way down*.

📍 See: [`oop/POLYMORPHISM.md`](oop/POLYMORPHISM.md) · [`advanced/TEMPLATES.md`](advanced/TEMPLATES.md) · [`keywords/VIRTUAL.md`](keywords/VIRTUAL.md) · [`keywords/TEMPLATE.md`](keywords/TEMPLATE.md)

---

## 5. Memory Layout & Zero-Overhead Abstraction

> *"Know what your code compiles to. Cache lines, indirection, copies — these are real."*

### The idea

This is the pillar that separates C++ programmers from *good* C++ programmers. C++ exposes the machine. Once you understand:

- **Stack vs heap** — stack is fast (pointer bump, automatic cleanup); heap is flexible but slow (allocator dance, manual cleanup).
- **Pointer chase** — every `->` is potentially a cache miss.
- **Object size** — virtual functions cost 8 bytes (`vptr`); references cost a pointer; padding can double a struct.
- **Pass-by-value vs pass-by-const-ref** — copying a `std::string` allocates; copying an `int` is free.
- **Inlining** — small functions defined in headers can inline to nothing; the same function in `.cpp` cannot cross translation-unit boundaries.

…you start writing code that *looks* high-level but compiles to lean assembly. **That's the zero-overhead promise.**

### The famous claim

These three implementations compile to **identical machine code** under `-O2`:

```cpp
int  add_function(int a, int b)      { return a + b; }
inline int add_inlined(int a, int b) { return a + b; }
template <typename T> T add_tpl(T a, T b) { return a + b; }
```

You don't pay for the abstraction. The compiler sees through it.

### Sizes you should internalize (Apple Silicon, 64-bit)

```cpp
sizeof(char)          == 1
sizeof(int)           == 4
sizeof(long)          == 8
sizeof(double)        == 8
sizeof(void*)         == 8
sizeof(std::string)   == 24      // small-buffer-optimized
sizeof(Empty)         == 1       // empty class still has identity
sizeof(WithVirtual)   == 8       // a vptr appears the moment you add `virtual`
```

Every `virtual` you add to a class costs one pointer per object. Doesn't matter how many virtual methods — the cost is one vptr (one vtable per class, shared by all instances).

### The real lesson: stack-vs-heap intuition

```cpp
void fast() {
    Fixed f;                   // stack: 1 instruction (sub rsp, 16)
    use(f);
}                              // 1 instruction to release (add rsp, 16)

void slow() {
    Fixed *f = new Fixed();    // heap: malloc → search free list → maybe sbrk → maybe lock
    use(*f);
    delete f;                  // heap: free → coalesce → maybe split
}
```

Both correct. The first is **literally hundreds of times faster** when called in a loop. *Use the heap when you must* (lifetime longer than the function, size unknown at compile time, polymorphism), not because it feels "more dynamic."

### Tools to develop the intuition

- `sizeof(T)` printed in `main()` — quick reality check
- `c++ -S file.cpp -o -` → see assembly for unoptimized code
- `c++ -O2 -S file.cpp -o -` → see what the optimizer made of it
- [godbolt.org](https://godbolt.org) → assembly side-by-side with source, online
- `valgrind` → catches use-after-free, leaks, uninitialized reads
- `valgrind --tool=cachegrind` → cache-miss profiler (for when you care about perf)

**Try this once during the piscine**: take your `Fixed` from Mod02, compile with `c++ -O2 -S Fixed.cpp -o -`, and look at the output for `Fixed::operator+`. You'll never look at C++ the same way.

### The mindset shift

Don't optimize prematurely — but **understand what your abstractions cost**. Then choose abstractions that cost nothing.

### Where you meet it

- **Implicit everywhere**, but explicit moments:
  - **Mod02 ex02**: `Fixed::operator+` — copies vs references
  - **Mod04**: `sizeof` of a class with virtuals
  - **Mod07**: template instantiation cost (binary size)
  - **Mod08**: STL container layouts (`vector` is contiguous, `list` is pointer-chase)

📍 See: [`fundamentals/MEMORY.md`](fundamentals/MEMORY.md) · [`keywords/SIZEOF.md`](keywords/SIZEOF.md) · [`keywords/INLINE.md`](keywords/INLINE.md)

---

## 🧠 The state of mind

The five pillars are *what* to learn. Equally important is *how* to think while coding.

### a) Lifetime first, behavior second
Before *"what does this class do?"*, ask *"how long does it live, who owns it, how does it die?"* Most C++ bugs are lifetime bugs in disguise.

### b) Skeptical of magic
Nothing happens automatically. If a constructor ran, *the language called it*. If memory was freed, *some destructor or `delete` did it*. Trace it. The compiler is deterministic.

### c) Compiler as collaborator
`-Wall -Wextra -Werror` is non-negotiable. A warning is the compiler offering free help. The 42 strict flags exist for a reason — they catch bugs the language alone won't.

### d) Cppreference, not Stack Overflow
[en.cppreference.com](https://en.cppreference.com) is the standard's human-readable face. When you wonder *"what does `std::string::find` return when not found?"* — go there. SO is often outdated, often wrong, often C++11+.

### e) Read your own assembly, at least once
Compile `c++ -O2 -S yourfile.cpp -o -`. You don't need to understand it deeply, but seeing your `for` loop become 3 instructions while a virtual call becomes a `jmp *(rdi)` builds intuition no reading replaces.

### f) Patient with yourself, ruthless with the code
C++ is hard. Mod02 will frustrate you. Mod04 will make you draw diagrams. The state to be in: *I expect to be confused, I expect bugs, I will dig until I understand.* Not *"why is this so hard, the language is broken."* The language isn't broken — it's a precise tool with sharp edges.

### g) Build small, build often, valgrind always
1. Write 20 lines.
2. `make && ./prog`.
3. `valgrind --leak-check=full ./prog`.
4. Fix anything, then continue.

Never write 200 lines and hope. C++ punishes that.

---

## 🗺️ Module map — where each pillar shows up

```
Mod00 ─── Pillar 3 (const) introduced via member functions
       └── Pillar 1 (lifetime) implicit — stack-only objects

Mod01 ─── Pillar 1 (RAII) becomes explicit — Zombie on stack vs heap
       └── Pillar 5 (memory) — first taste of new/delete cost

Mod02 ─── Pillar 2 (OCF) — the entire module
       ├── Pillar 3 (const) deepens — getRawBits() const
       └── Pillar 5 (sizeof, layout) — Fixed must be small

Mod03 ─── Pillar 4 (dynamic polymorphism, base part) — virtual + inheritance
       └── Pillar 1 (lifetime) — ctor/dtor chains

Mod04 ─── Pillar 4 (full polymorphism) — abstract, interface, virtual dtor
       └── Pillar 5 (vtable cost) — sizeof now includes vptr

Mod05 ─── Pillar 1 + 3 — exceptions interact with destructors
       └── try/catch, exception safety

Mod06 ─── Pillar 3 — casts express type intent (static/dynamic/const/reinterpret)

Mod07 ─── Pillar 4 (static polymorphism) — templates
       └── Pillar 5 — template bloat is real, learn to manage it

Mod08–09 ─ Pillar 4 + 5 — STL is the synthesis of everything
```

When you start a new module, come back here, re-read the relevant pillar, then open the subject. Framework first, then details.

---

## 📚 How to use this document

- **Now (early piscine)**: skim the TL;DR. Read pillars 1, 2, 3 carefully. Skim 4 and 5.
- **Before each new module**: re-read the matching pillar from the Module Map.
- **When stuck**: search this doc for the keyword you're stuck on. The principle behind it is here.
- **End of piscine**: re-read the whole thing. Half of it that felt abstract on day 1 will feel obvious now.

---

## 🌌 Final thought

> C++ doesn't reward cleverness. It rewards **understanding**.
>
> The senior C++ programmer is not the one with tricks — it's the one who can answer *"what happens, in memory, when this line runs?"* without hesitation.
>
> The five pillars are how you build that understanding. Everything else — STL, exceptions, smart pointers, move semantics — is just *one of these pillars wearing different clothes*.

Master these. Trust the process. The language will reveal itself.

🔥 *Pilgrim, walk on.*
