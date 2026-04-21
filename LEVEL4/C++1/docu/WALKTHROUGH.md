# C++98 — A Complete Walkthrough

> A single-sitting walkthrough of the C++98 language for someone coming from C.
> Read it once, then keep it open beside you while writing the 42 CPP modules.
> Every concept is paired with **what the machine actually does**, because understanding
> that layer is what makes the rest of C++ stop feeling like magic.

---

## Table of Contents

1. [A Short History of C++](#1-a-short-history-of-c)
2. [The Philosophy of C++ (and Why It Matters)](#2-the-philosophy-of-c-and-why-it-matters)
3. [What C++ is Used For](#3-what-c-is-used-for)
4. [The Mental Shift from C to C++](#4-the-mental-shift-from-c-to-c)
5. [What Stays the Same](#5-what-stays-the-same)
6. [What is New — A Guided Tour](#6-what-is-new--a-guided-tour)
7. [How C++ Code Becomes a Program](#7-how-c-code-becomes-a-program)
8. [Memory Model & Object Lifetime](#8-memory-model--object-lifetime)
9. [Classes — The Core Concept](#9-classes--the-core-concept)
10. [Orthodox Canonical Form](#10-orthodox-canonical-form)
11. [References](#11-references)
12. [`new` / `delete` — and What Happens in Memory](#12-new--delete--and-what-happens-in-memory)
13. [`const` Correctness](#13-const-correctness)
14. [`static` — Three Meanings, One Keyword](#14-static--three-meanings-one-keyword)
15. [Namespaces](#15-namespaces)
16. [Operator Overloading](#16-operator-overloading)
17. [Inheritance](#17-inheritance)
18. [Polymorphism & Virtual Functions](#18-polymorphism--virtual-functions)
19. [Exceptions](#19-exceptions)
20. [Templates](#20-templates)
21. [The STL (Standard Template Library)](#21-the-stl-standard-template-library)
22. [I/O Streams](#22-io-streams)
23. [Casts](#23-casts)
24. [Undefined Behavior — The Sharp Edge of C++](#24-undefined-behavior--the-sharp-edge-of-c)
25. [Tips That Make Things Click](#25-tips-that-make-things-click)
26. [Common Pitfalls & How to Spot Them](#26-common-pitfalls--how-to-spot-them)
27. [An Effective Learning Path](#27-an-effective-learning-path)
28. [Glossary](#28-glossary)

---

## 1. A Short History of C++

Understanding **why** C++ looks the way it does will save you from fighting the language.
It was not designed in one sitting by one person with a clean slate — it was grown, iteratively, on top of C, by someone trying to build large systems at Bell Labs.

### Timeline at a glance

```
1972  ── C is created by Dennis Ritchie at Bell Labs.
         Goal: rewrite UNIX in a portable language.

1979  ── Bjarne Stroustrup (Danish computer scientist at Bell Labs) starts
         tinkering with "C with Classes". He had used Simula (which invented
         objects and classes) at Cambridge for his PhD, and missed those
         ideas when he moved to C.

1983  ── "C with Classes" is renamed to C++.
         The ++ is the C increment operator — a pun meaning "C, incremented".
         First features: classes, derived classes, access control (public/private),
         strong typing, inline functions, default arguments.

1985  ── First commercial release. Stroustrup publishes
         "The C++ Programming Language" — the de-facto spec for years.

1989  ── C++ 2.0 adds: multiple inheritance, abstract classes, static members,
         protected members, const member functions.

1990  ── Templates and exceptions are added to the Annotated Reference Manual.

1998  ── C++ is officially standardized as ISO/IEC 14882:1998 — known as C++98.
         This is the version you use at 42. It includes:
         - Full template support
         - Exceptions (try/catch/throw)
         - The STL (Standard Template Library), contributed by Alexander Stepanov
         - Namespaces
         - RTTI (Run-Time Type Information), dynamic_cast
         - The bool type
         - New-style casts (static_cast, dynamic_cast, etc.)

2003  ── C++03 — a minor "bug-fix" release of C++98.
         From the programmer's perspective, C++98 and C++03 are the same language.

2011  ── C++11 — a MAJOR release. Adds: auto, range-based for, lambdas,
         move semantics, smart pointers (unique_ptr, shared_ptr), nullptr,
         constexpr, threading library, initializer lists.

2014  ── C++14 — polish on C++11.
2017  ── C++17 — structured bindings, std::optional, std::variant, filesystem.
2020  ── C++20 — concepts, modules, coroutines, ranges.
2023  ── C++23 — small refinements; big features (reflection, senders/receivers)
         slated for C++26.
```

### Why did Stroustrup build it?

Two forces drove the design:

1. **He had lived through large-project pain.** Writing the UNIX kernel simulator in BCPL at Cambridge, he saw a C-like language with no tools for organizing big codebases — no way to bundle data and the code that operates on it, no way to express "this thing is-a kind of that thing." Everything devolved into function pointers, `void*`, and pages of manual bookkeeping.

2. **He had used Simula 67**, the first language with classes, inheritance, and virtual methods. He knew what an alternative looked like — but Simula's runtime made it too slow for systems work. He wanted the abstractions **without the runtime tax**.

So the brief became: *"Take C. Add Simula's abstractions. Pay for them only when you use them."*

That one-line brief explains nearly every design decision in C++ — the awkward ones and the brilliant ones alike.

### What each era added (macro view)

| Era | What it gave you | What "clicks" if you remember it |
|---|---|---|
| C with Classes (early 80s) | `class`, access control, constructors, destructors | Objects manage their own invariants. |
| C++ 1.0 (1985) | Operator overloading, references, `virtual` functions, `const` | Types become expressive; polymorphism is real. |
| C++ 2.0 (1989) | Multiple inheritance, abstract classes, `protected`, static members | Inheritance is now a design tool, not a trick. |
| C++98 / C++03 | Templates, exceptions, STL, namespaces, RTTI, `bool` | Generic programming + structured error handling + reusable data structures. |

C++98 is the plateau you'll spend all of 42 CPP on. Everything post-2011 is off the table — that's why `auto`, `nullptr`, lambdas, and smart pointers don't apply to you yet.

---

## 2. The Philosophy of C++ (and Why It Matters)

C++ has a design motto that nothing else in mainstream programming has:

> **"You don't pay for what you don't use."**
> — Bjarne Stroustrup

This is not marketing. It's the single rule the language committee has held for 40 years, and it explains almost everything weird about C++:

- **No virtual functions unless you ask.** A `class` without a `virtual` method has no vtable, no runtime dispatch, zero overhead vs a C struct.
- **No exceptions unless you throw.** If your program never throws, the only cost of having exceptions enabled is a little extra metadata in the binary.
- **No garbage collector.** You pick stack or heap. You decide lifetime. No thread interrupts your code.
- **Templates generate real code, not runtime indirection.** A `std::vector<int>` is as fast as a hand-written C resizable int array.

The flipside of the motto is: **you pay when you ask, and sometimes you ask accidentally.**
A `std::string` passed by value makes a heap copy. A virtual function goes through a pointer. An exception unwinds the stack. The language gives you tools with known costs — and the discipline to **know** those costs is what separates a C++ programmer from someone who writes C++.

### A second principle: "the compiler is your friend"

C++ pushes as many checks as possible to **compile time**:

- `private` / `public` / `protected` — compile-time access rules.
- `const` — compile-time immutability contract.
- Templates — compile-time code generation.
- Type checking — far stricter than C (no implicit `void*` conversions, no missing prototypes).

The deal is: the compiler is harsh, but once it's happy, your program has passed a lot of invariants you never had to test for. **Errors caught at build time cannot happen at runtime.** That's the whole game.

---

## 3. What C++ is Used For

C++ is not one language — it's five or six overlapping dialects depending on the domain. Knowing where it's used helps you understand why certain features exist.

| Domain | Why C++ | Famous examples |
|---|---|---|
| **Operating systems / low-level** | Speed, direct hardware access, no runtime | Windows kernel parts, LLVM, parts of macOS |
| **Game engines** | Hard real-time 60fps loops, tight memory layout, zero-cost abstractions | Unreal Engine, Unity's native core, CryEngine, id Tech |
| **Browsers** | Performance + enormous codebases needing modular design | Chromium, Firefox (mostly C++ today), Safari/WebKit |
| **High-frequency trading / finance** | Microseconds matter; no GC pauses allowed | Bloomberg terminal, HFT firms, most exchange matching engines |
| **Databases** | Memory control + decades of code | MySQL, PostgreSQL internals, MongoDB, RocksDB |
| **Embedded / robotics** | Deterministic timing, small footprint | Tesla autopilot, industrial controllers, some Mars rover code |
| **Graphics / CAD / 3D** | Float-heavy math, GPU interop, huge data | Adobe Photoshop, Blender, AutoCAD, Maya |
| **Compilers & language runtimes** | Because compilers need to be fast | GCC, Clang, V8 (JavaScript engine), HotSpot (JVM) |
| **Scientific computing** | Performance + interop with Fortran/CUDA | LAMMPS, ROOT (CERN), TensorFlow's C++ core |

**The pattern:** wherever you cannot afford a garbage collector, a runtime interpreter, or unpredictable latency — C++ is there. It's the language of "I need performance and I will manage the cost myself."

At 42, you're being trained into this worldview. The CPP modules are less about finishing projects and more about **acquiring the habits** (OCF, const correctness, lifetime thinking) that make you safe in a no-GC world.

---

## 4. The Mental Shift from C to C++

This is the single hardest part. The syntax is 70% the same; the thinking is completely different.

### In C you think:

- **Functions** that manipulate **data**.
- **Structs** that hold data.
- **Pointers** that connect them.
- **Ownership** is an unwritten contract: "the caller must `free` this."

### In C++ you think:

- **Objects** that *are* something and *do* things.
- **Classes** that define what those objects are.
- **Lifetimes** that the compiler enforces: constructor runs when the object appears, destructor runs when it disappears.
- **Interfaces** (via inheritance, templates) that let different objects share behavior.

### The concrete example

```c
/* C — the programmer tracks everything */
t_phonebook *pb = phonebook_create();  // did it succeed? check pb.
phonebook_add(pb, contact);            // hope this doesn't corrupt.
// 500 lines later...
phonebook_destroy(pb);                 // did we forget? probably.
```

```cpp
// C++ — the language tracks lifetime
{
    PhoneBook pb;                      // constructor runs automatically
    pb.add(contact);                   // method, same object
}                                      // destructor runs automatically here
```

That second version is the insight. **The object knows when it was born and when it dies, and runs code at both events, automatically.** This is called **RAII** — *Resource Acquisition Is Initialization* — and it's the single most important idea in modern C++.

### RAII in one picture

```
        CONSTRUCTION                      DESTRUCTION
            │                                 ▲
            ▼                                 │
      ┌───────────┐                     ┌───────────┐
      │  acquire  │                     │  release  │
      │ resources │ ─── object's life ──│ resources │
      │ (memory,  │                     │ (same set │
      │  files,   │                     │  cleaned  │
      │  locks)   │                     │  up here) │
      └───────────┘                     └───────────┘
            ▲                                 │
            │                                 ▼
          scope begin                     scope end
```

You'll see RAII appear in every exercise:
- A `std::string` member in a `Contact` — the string's destructor frees its heap memory automatically.
- An `std::ifstream` opening a file — its destructor closes it.
- A class owning a `new`'d buffer — **you** write the destructor that frees it.

Once you absorb RAII, you stop thinking "where do I `free` this?" and start thinking "whose destructor owns this?"

---

## 5. What Stays the Same

Coming from C, **everything you already know still works**. This is not exaggeration — C++98 was designed to be a near-superset of C89. Your C code mostly compiles as C++ with zero changes.

- All primitive types: `char`, `int`, `short`, `long`, `float`, `double`, `size_t`, `void*`, ...
- All operators: arithmetic, logical, bitwise, comparison, assignment, pointer arithmetic, `sizeof`.
- Pointers: same syntax, same `&`/`*`, same dangers (dangling, NULL, aliasing).
- Arrays: same stack/heap rules, same decay-to-pointer behavior in function signatures.
- Control flow: `if`, `else`, `for`, `while`, `do while`, `switch`, `break`, `continue`, `return`, `goto`.
- The preprocessor: `#include`, `#define`, `#ifdef`, `#ifndef`, `#endif`, `#pragma`.
- `malloc` / `free` / `printf` / `scanf` / `strlen` / etc. — all still available (just use `<cstdio>`, `<cstring>`, etc. for the C++ version of the headers).
- `main(int argc, char **argv)` — identical.
- `typedef`, `enum`, `struct`, `union` — all work.
- Bitfields, unions, function pointers.
- `const`, `volatile`, `static` at file scope.
- The compilation pipeline: preprocess → compile → link.

**Rule of thumb:** if your C89 file has no identifiers called `class`, `new`, `delete`, `template`, `this`, `throw`, `catch`, `try`, `namespace`, `virtual`, `operator`, `typename`, `explicit`, or `mutable` — it compiles as C++.

This compatibility is *the* reason C++ is so ugly in places: it couldn't break C. So modern C++ idioms coexist with raw pointer arithmetic and macros. At 42, you'll write both within the same file.

---

## 6. What is New — A Guided Tour

The following sections cover each new concept in depth. This section is the **30-second summary** of all of them — so you can see the forest before the trees.

| New concept | What it does | What it replaces in C |
|---|---|---|
| **Classes** | Bundle data + functions + access control + lifecycle | Structs + function pointers + naming conventions |
| **Constructors / destructors** | Code that runs at object birth and death | Manual `init()` / `destroy()` functions |
| **References (`&`)** | Alias for a variable; cannot be null, cannot be rebound | Pointers |
| **`new` / `delete`** | Allocate + call constructor; call destructor + free | `malloc` / `free` |
| **Namespaces** | Scope names under a label to prevent collisions | Prefixes like `ft_` |
| **`std::string`** | Self-managing string type (see [STRING_FUNCTIONS.md](STRING_FUNCTIONS.md)) | `char*` + `malloc`/`free` |
| **Function overloading** | Multiple functions with the same name, different parameters | Different names (`strcpy_s`, `strncpy_s`) |
| **Default arguments** | Parameter with a default value | Wrapper functions |
| **Operator overloading** | Give operators like `+`, `==`, `<<` meaning for your types | N/A — C has no equivalent |
| **Templates** | Generate code for any type | `void*` + function pointers + macros |
| **Exceptions** | Propagate errors up the stack automatically | Return codes + `errno` + cleanup labels |
| **Inheritance** | "Is-a" relationships between classes | Struct-embedding + manual dispatch |
| **Virtual functions** | Runtime dispatch based on actual object type | Manual function pointer tables |
| **`const` member functions** | Methods that promise not to modify the object | N/A |
| **STL containers** | Ready-made, efficient, type-safe data structures | You wrote them all from scratch |
| **I/O streams** | Type-safe, extensible I/O | `printf` / `scanf` (format-string unsafe) |
| **Four C++ casts** | Separate, explicit casts for four distinct intents | One `(type)value` for everything |
| **RTTI** (`typeid`, `dynamic_cast`) | Ask the runtime what type a polymorphic object actually is | N/A |
| **`bool`** | A proper boolean type | `int` with 0/non-zero convention |

If you can write working code that uses the first ten rows, you know "C with classes." If you can use all 19, you know C++98.

---

## 7. How C++ Code Becomes a Program

Before diving into features, burn this pipeline into your head. When something goes wrong, *where* it went wrong (preprocessor? compiler? linker?) is the single biggest clue.

```
 your .hpp and .cpp files
            │
            ▼
   ┌───────────────────┐
   │   PREPROCESSOR    │  expands #include, #define, #ifdef
   └───────────────────┘     → a single big "translation unit" (one per .cpp)
            │
            ▼
   ┌───────────────────┐
   │     COMPILER      │  parses C++, enforces the type system,
   │                   │  generates machine code
   └───────────────────┘     → one .o file per .cpp
            │
            ▼
   ┌───────────────────┐
   │      LINKER       │  resolves symbols across all .o files,
   │                   │  glues them + libraries into one binary
   └───────────────────┘     → ./program
            │
            ▼
   ┌───────────────────┐
   │   LOADER / OS     │  loads the binary into RAM,
   │                   │  sets up the stack, jumps to main()
   └───────────────────┘     → your program runs
```

### Why does this matter in C++ more than in C?

Because C++ has **much more stuff that lives in headers**:
- Class definitions
- Template definitions (all of them — templates cannot be separately compiled in C++98)
- `inline` functions

So headers get included into many `.cpp` files, and the **same class definition** appears in many translation units. The linker has to **deduplicate** these. If you put a non-inline function body in a header and include it twice, you'll get a **"multiple definition" linker error**.

### The three kinds of error, mapped to the pipeline

| Where | Error looks like | Examples |
|---|---|---|
| **Preprocessor** | `No such file or directory` | Missing `#include`, wrong path, missing header guard causing recursion |
| **Compiler** | `error: expected ';'`, `error: 'Foo' was not declared in this scope`, `error: no matching function for call to ...` | Syntax, type mismatch, missing `const`, unknown identifier |
| **Linker** | `undefined reference to 'X'`, `multiple definition of 'Y'` | Declared but not defined, static member without `.cpp` definition, function body in header without `inline` |

When you see `undefined reference`, don't look at syntax. Look at whether the function or static member is **defined somewhere**. When you see `error: expected`, it's syntax. The error message tells you which stage failed.

---

## 8. Memory Model & Object Lifetime

This is where C++ earns its reputation. Get this right and the rest makes sense.

### The five memory regions (same as C, but used differently)

```
   ┌──────────────────┐   high addresses
   │  command line    │
   │  environment     │
   ├──────────────────┤
   │                  │
   │      STACK       │  ◄── local variables, function call frames
   │  (grows down)    │      destroyed automatically when function returns
   │                  │
   │        │         │
   │        ▼         │
   │                  │
   │        ▲         │
   │        │         │
   │                  │
   │       HEAP       │  ◄── 'new' / 'malloc' — YOU own these
   │   (grows up)     │      destroyed when YOU call delete / free
   │                  │
   ├──────────────────┤
   │   BSS (zero'd    │  ◄── static / global vars not initialized
   │    uninitialized │       (int g_count;)
   │   globals)       │
   ├──────────────────┤
   │   DATA (init'd   │  ◄── static / global vars with initial value
   │   globals)       │       (int g_count = 5;)
   ├──────────────────┤
   │                  │
   │     TEXT         │  ◄── your code, string literals
   │ (read-only code) │
   │                  │
   └──────────────────┘   low addresses
```

### Object lifetime — what runs when

In C++, every non-trivial variable has a **lifetime**: a well-defined birth and death, with code running at each event.

```cpp
void demo() {
    Foo a;                  // BIRTH: constructor runs here
    Foo b(42);              // BIRTH: parameterized constructor
    Foo *c = new Foo;       // BIRTH: heap allocation + constructor

    if (something) {
        Foo d;              // BIRTH (scope begin)
        // ...
    }                       // DEATH: ~Foo() of d runs here (scope end)

    delete c;               // DEATH: ~Foo() runs, then memory freed
}                           // DEATH: ~Foo() of b, then of a (reverse order)
```

**The rule of scope:** stack objects are destroyed in the **reverse** order of their construction, **at the `}` that ends their enclosing scope**.

### Where does each thing live?

| Declaration | Lives in |
|---|---|
| `int x = 5;` inside a function | Stack |
| `int x = 5;` outside any function | Data segment |
| `static int x = 5;` inside a function | Data segment (but only visible in that function) |
| `int *p = new int(5);` | Pointer `p` on stack; `int` itself on heap |
| `"hello"` (a string literal) | Text / rodata segment (read-only) |
| `Foo f;` (class instance) | Stack — *including* all its member variables |
| `Foo *f = new Foo;` | Pointer on stack; object (with its members) on heap |

A subtlety that catches beginners: `std::string s;` is a stack variable, but `s` has an internal `char*` that points to heap-allocated bytes (unless the string is small, in which case SSO kicks in). **The `s` object is on the stack; its contents are on the heap.** Its destructor handles the heap part for you.

### Storage duration (the formal name)

- **Automatic** — lifetime tied to scope (stack).
- **Dynamic** — lifetime tied to `new`/`delete` (heap).
- **Static** — lifetime of the entire program (globals, `static` locals).
- **Thread-local** — one copy per thread (C++11+, not in C++98).

---

## 9. Classes — The Core Concept

A `class` in C++ is a C `struct` with three superpowers added:
1. **Member functions** (methods).
2. **Access control** (`public`, `private`, `protected`).
3. **Lifecycle hooks** (constructors and destructor).

```cpp
class Point {
private:                       // only Point's own code can touch these
    int _x;
    int _y;

public:                        // anyone can call these
    Point();                           // default constructor
    Point(int x, int y);               // parameterized constructor
    Point(const Point& other);         // copy constructor
    Point& operator=(const Point& o);  // assignment operator
    ~Point();                          // destructor

    int  getX() const;                 // getter (promises not to modify)
    int  getY() const;
    void setX(int x);
    void setY(int y);
    void move(int dx, int dy);         // behavior
};
```

### What the machine sees

A `Point` object is **literally just two ints back to back in memory**. Access control, the method names, the `private` label — **none of that exists at runtime**. The compiler erases them all.

```
In memory, a Point is:        The compiler enforces access rules
┌─────────┬─────────┐         at build time only. The CPU has no
│   _x    │   _y    │         idea what 'private' means.
│ 4 bytes │ 4 bytes │
└─────────┴─────────┘
 offset 0  offset 4

Total size: 8 bytes (plus padding, same rules as a C struct)
```

When you write `p.move(1, 2)`, the compiler transforms it into roughly:

```
Point__move(&p, 1, 2);
```

That `&p` becomes the hidden first argument called `this` inside the method. **Every non-static member function has a secret `this` parameter.** It's a plain pointer — nothing more.

### `struct` vs `class` in C++

- `struct Foo { ... };` — default access is **public**.
- `class Foo { ... };` — default access is **private**.

That's the *only* difference. Both can have methods, inheritance, access specifiers, etc. The convention at 42 is: use `class` for everything; use `struct` only when you really want a bag of public data.

### Member initialization list — use it always

```cpp
// Bad — default-constructs then assigns
Point::Point(int x, int y) {
    _x = x;                     // _x was already default-int-ified
    _y = y;
}

// Good — constructs directly into place
Point::Point(int x, int y) : _x(x), _y(y) {}
//                         ↑ initialization list
```

For primitive types the difference is mostly stylistic. For class-type members, it's performance: you skip a default-construct-then-overwrite cycle. For **`const` members, reference members, or members without a default constructor**, the initialization list is **mandatory** — you cannot assign to them in the body.

**Pitfall — declaration order wins.** Members are initialized in the order they are **declared in the class**, not the order they appear in the initialization list. This bites when one member's initializer uses another:

```cpp
class Buffer {
    int   _size;
    char *_data;
public:
    // WRONG IN SPIRIT: compiler initializes _size first (declaration order),
    // so _data is initialized with the correct _size — but the list misleads you.
    Buffer(int n) : _data(new char[n]), _size(n) {}
};
```
Write the list in declaration order to avoid confusion. Good compilers warn with `-Wreorder`.

### `explicit` — block implicit conversions

A constructor taking a single argument is an **implicit conversion** by default:

```cpp
class Fixed {
public:
    Fixed(int n);          // implicit: Fixed f = 42; works
};

void print(const Fixed& f);
print(42);                 // silently converts int → Fixed
```

Prefix it with `explicit` to block that:

```cpp
class Fixed {
public:
    explicit Fixed(int n); // now `Fixed f = 42;` is an error — must write `Fixed f(42);`
};
print(42);                 // ERROR — no implicit conversion
```

Use `explicit` whenever a single-arg constructor isn't meant to act as a silent converter.

### Pointers to member functions

A pointer to a member function is **not** a regular function pointer — it stores an offset into the class plus information the compiler needs to dispatch (virtual or not). You need the object to call it through.

```cpp
class HumanA {
public:
    void hello()   const { std::cout << "hi\n"; }
    void goodbye() const { std::cout << "bye\n"; }
};

// Declaration:  return-type  (ClassName::*name)(params) [qualifiers]
void (HumanA::*fn)() const;

HumanA h;
fn = &HumanA::hello;      // note the `&ClassName::method` syntax — required
(h.*fn)();                // call through object:    h.fn()
HumanA *p = &h;
(p->*fn)();               // call through pointer:   p->fn()
```

A common pattern (used in CPP01 ex05) is a **lookup table** of member pointers:

```cpp
void (HumanA::*actions[3])() const = {
    &HumanA::hello,
    &HumanA::goodbye,
    &HumanA::hello
};
for (int i = 0; i < 3; ++i)
    (h.*actions[i])();
```

The `.*` and `->*` operators are the only place this syntax appears. It feels alien the first time; after you use it once, it never surprises you again.

### The `this` pointer

Inside any non-static member function, `this` is the address of the object the method was called on.

```cpp
void Point::move(int dx, int dy) {
    this->_x += dx;     // explicit — same as just `_x += dx`
    this->_y += dy;
}

Point& Point::operator=(const Point& other) {
    if (this != &other)     // self-assignment check — same object?
        _x = other._x, _y = other._y;
    return *this;           // return the object itself by reference
}
```

Inside a **`const` method**, `this` is a `const T*` — you can't modify members through it.

---

## 10. Orthodox Canonical Form

See [ORTHODOX_CANONICAL_FORM.md](ORTHODOX_CANONICAL_FORM.md) for the deep dive. Short version:

**Every non-trivial class must explicitly define all four of:**

1. Default constructor — `ClassName();`
2. Copy constructor — `ClassName(const ClassName& other);`
3. Copy assignment operator — `ClassName& operator=(const ClassName& other);`
4. Destructor — `~ClassName();`

**Why?** If you don't, the compiler generates them implicitly — and the generated versions do **shallow copies**. If your class owns heap memory, shallow copies are catastrophic: two objects end up pointing at the same buffer, each trying to `delete` it at destruction time. That's a double-free.

The OCF makes the lifecycle explicit, visible, and under your control. At 42, it's mandatory.

**A mnemonic:** the Orthodox Canonical Form is "**the four functions C++ would invent for you, written by hand so they do the right thing for your specific class.**"

---

## 11. References

See [REFERENCE.md](REFERENCE.md) for the full guide. Short version:

A **reference** is an alias — another name for an existing variable. It is not a new object, it has no separate memory (in most cases), it is not a pointer.

```cpp
int x = 10;
int &r = x;       // r is another name for x
r = 20;           // x is now 20
std::cout << &x << " == " << &r;   // same address
```

**The three hard rules:**
1. Must be initialized when declared (`int &r;` is an error).
2. Cannot be rebound (`r = y;` assigns `y` *into* `x`, does not make `r` refer to `y`).
3. Cannot be null.

**When to use what:**

| Need | Use |
|---|---|
| Must exist, single target for its lifetime | Reference |
| May not exist, or must be reseated | Pointer |
| Modify caller's variable in a function | `T&` parameter |
| Pass large object cheaply, read-only | `const T&` parameter |
| Return `*this` from `operator=`, `operator<<`, etc. | Reference |
| Store "one of many" (container element, array) | Pointer |

### How a reference is implemented

Usually, **as a pointer under the hood**. The compiler turns `int &r = x;` into the same machine code as `int *r = &x;`, and turns `r = 20;` into `*r = 20;`. But because references cannot be null or rebound, the compiler is free to optimize them away completely — for instance, when a reference parameter is inlined, it just becomes a direct memory access. That's the "zero cost" of references.

---

## 12. `new` / `delete` — and What Happens in Memory

In C, you had `malloc` (raw bytes) and `free` (give them back). C++ gives you `new` and `delete` — which do **more**.

```cpp
// malloc: just raw bytes
Foo *a = (Foo*)malloc(sizeof(Foo));   // no constructor called!
free(a);                              // no destructor called!

// new: allocates AND constructs
Foo *b = new Foo(arg);                // 1) allocates sizeof(Foo) bytes
                                      // 2) calls Foo::Foo(arg) on those bytes
delete b;                             // 1) calls Foo::~Foo()
                                      // 2) frees the bytes

// Arrays
int *arr = new int[10];               // 10 ints allocated; each default-init'd
delete[] arr;                         // MUST use delete[] — matches new[]
```

### Critical pairings

| Allocation | Matching deallocation |
|---|---|
| `new T` | `delete ptr` |
| `new T[n]` | `delete[] ptr` |
| `malloc(n)` | `free(ptr)` |

**Mixing is undefined behavior.** `delete` on something from `malloc` is UB. `free` on something from `new` is UB. `delete` on something from `new[]` is UB. This is a common interview trick: *why* is mixing UB? Because `new T[n]` may store the element count in a hidden prefix so `delete[]` knows how many destructors to run. `malloc` doesn't know about that prefix.

### What `new` actually does

```
new Foo(42);
    │
    ├─► operator new(sizeof(Foo))    ── raw allocation (usually calls malloc)
    │
    ├─► Foo::Foo(42) at that address  ── construction (placement new)
    │
    └─► returns typed pointer
```

And `delete`:

```
delete ptr;
    │
    ├─► ptr->~Foo()                   ── destructor runs
    │
    └─► operator delete(ptr)          ── raw free (usually calls free)
```

If the constructor throws, `operator new` will automatically release the allocated memory before the exception propagates — you don't leak.

### `new` throws on failure

Unlike `malloc` (returns `NULL`), `new` throws `std::bad_alloc` on failure. At 42, you generally don't need to check for null after `new` unless you use the `std::nothrow` variant:

```cpp
int *p = new (std::nothrow) int[1000000];
if (p == NULL)   // only this variant returns NULL on failure
    return handle_error();
```

### The 42 rule

In the CPP modules, **always use `new` and `delete`**, never `malloc`/`free`. Using C allocation on C++ types skips constructors/destructors and is a guaranteed fail.

---

## 13. `const` Correctness

`const` in C++ does far more than in C. It's a **contract** enforced by the compiler.

### The four places `const` appears

```cpp
const int x = 5;           // (1) value-level const: x cannot be modified
const int *p;              // (2) pointer to const: *p cannot be modified
int *const p;              // (3) const pointer: p cannot be reseated
const int *const p;        // (4) both

void f(const Foo &foo);    // parameter: f promises not to modify foo

int Foo::getX() const;     // method: doesn't modify the object it's called on
```

### The trick for reading `const` declarations

Read right to left around `const`:

- `const int *p` — "p is a pointer to an int that is const" → can't modify `*p`, can reseat `p`.
- `int *const p` — "p is a const pointer to an int" → can modify `*p`, can't reseat `p`.
- `const int *const p` — "p is a const pointer to a const int" → can't modify either.

### `const` member functions

```cpp
class Circle {
    double _radius;
public:
    double area() const   { return 3.14159 * _radius * _radius; }  // can't modify
    void   scale(double k) { _radius *= k; }                       // can modify
};

const Circle c(5.0);
c.area();       // OK — area is const
c.scale(2.0);   // ERROR — scale is not const, c is const
```

**The rule:** inside a `const` method, `this` is `const ClassName*`. Attempting to modify any member is a compile error.

**The 42 rule:** every getter — and more generally every method that doesn't modify the object — should be `const`. Forgetting `const` on a getter is the most common norm violation in the modules.

### Why `const` on parameters matters

Without `const`, a function like:
```cpp
void print(std::string &s);   // takes a MUTABLE reference
```
...cannot be called with a temporary or a literal:
```cpp
print("hello");                // ERROR — "hello" is a temporary
```

With `const`:
```cpp
void print(const std::string &s);   // const ref
print("hello");                     // OK — const refs can bind temporaries
```

This is the reason copy constructors and `operator=` take `const ClassName&`.

### `const` is a one-way ratchet in interfaces

Once a function signature is `const`, you can't call non-const methods inside it. That sometimes feels annoying — but it means callers get a reliable guarantee: *this function cannot mutate what I pass it*. That guarantee compounds. Across a large codebase, `const` is how you sleep at night.

---

## 14. `static` — Three Meanings, One Keyword

`static` is the most overloaded keyword in C++. Context decides meaning.

### Meaning 1: inside a class — "shared by all instances"

```cpp
class Account {
    static int _nbAccounts;     // ONE variable, shared by every Account
public:
    Account()  { _nbAccounts++; }
    ~Account() { _nbAccounts--; }
    static int getCount() { return _nbAccounts; }
};

// In .cpp — REQUIRED: the declaration above only promised it existed
int Account::_nbAccounts = 0;
```

```
Without static              With static

a1: ┌──────────┐            a1: ┌──────────┐
    │ _nb = 0  │                │          │
    └──────────┘                └──────────┘
a2: ┌──────────┐            a2: ┌──────────┐
    │ _nb = 0  │                │          │
    └──────────┘                └──────────┘
                            Class-level:
                            ┌──────────────┐
                            │ _nb = 2      │  ◄── shared
                            └──────────────┘
```

A `static` member function has **no `this` pointer** — it's not "called on" any particular object. It can only touch other `static` members.

### Meaning 2: inside a function — "persists between calls"

```cpp
void count() {
    static int n = 0;     // initialized ONCE, first time we enter
    n++;
    std::cout << n;
}

count();   // prints 1
count();   // prints 2
count();   // prints 3
```

The `static` local lives in the data segment, not the stack. Its initializer runs exactly once, the first time the function is reached.

### Meaning 3: at file scope — "internal linkage"

```cpp
// helper.cpp
static int _internal = 42;   // only helper.cpp can see this
static void helper() { }     // only helper.cpp can call this
```

This is for hiding implementation details from the linker. If two `.cpp` files define a non-static variable with the same name, you get a linker error. With `static`, each file has its own private copy.

### Why one keyword for three things?

Historical accident. C inherited it from older languages where "static" meant "allocated for the duration of the program" — and all three uses share that flavor:
- A static member lives for the program's lifetime (not per-object).
- A static local lives for the program's lifetime (not per-call).
- A static global lives for the program's lifetime (and is file-local).

The through-line is **storage duration = program lifetime**. The visibility/sharing rules are consequences of where it's declared.

---

## 15. Namespaces

Namespaces prevent name collisions. In C, if two libraries both define a function called `sort`, you're stuck. In C++, you wrap them in namespaces:

```cpp
namespace ft {
    int  max(int a, int b) { return a > b ? a : b; }
    void sort(int *arr, int n) { /* ... */ }
}

namespace sam {
    int max(int a, int b) { return a < b ? a : b; }  // weird, but fine
}

int main() {
    ft::max(1, 2);        // 2
    sam::max(1, 2);       // 1
}
```

### `using` — bring names into scope

```cpp
using ft::max;              // bring just this one name
max(1, 2);                  // works now

using namespace ft;         // bring everything from ft
sort(arr, 10);              // works now — but risks collisions
```

**42 rule (and C++ professional practice):** never write `using namespace std;` in a header file. It leaks into every file that includes the header and undoes the whole point of namespaces. In `.cpp` files it's tolerable but often discouraged — prefer explicit `std::` prefixes.

### `std::` — the standard library's namespace

Every standard C++ symbol (`cout`, `cin`, `string`, `vector`, `map`, `endl`, ...) lives in `std`. You always write `std::cout`, `std::string`, etc.

### Nested namespaces

```cpp
namespace outer {
    namespace inner {
        int x = 5;
    }
}

outer::inner::x;   // full path
```

### The anonymous namespace (file scope)

```cpp
namespace {
    int _helper = 42;    // like `static int _helper` — file-scope
}
```

This is the modern C++ equivalent of file-scope `static`. Both work; both achieve internal linkage.

---

## 16. Operator Overloading

C gives you `+`, `-`, `==`, `<<`, etc., but only for built-in types. C++ lets you define what they mean for your own classes.

```cpp
class Fixed {
    int _raw;
public:
    Fixed(int raw) : _raw(raw) {}

    Fixed operator+(const Fixed& rhs) const {
        return Fixed(_raw + rhs._raw);
    }

    bool operator==(const Fixed& rhs) const {
        return _raw == rhs._raw;
    }
};

Fixed a(3), b(4);
Fixed c = a + b;       // calls a.operator+(b)
if (a == b) { ... }    // calls a.operator==(b)
```

### What's happening under the hood

`a + b` is literally rewritten by the compiler as `a.operator+(b)`. It is a function call with a prettier syntax. Nothing more, nothing less.

### Operators you'll overload in the modules

| Operator | Purpose | Return type convention |
|---|---|---|
| `operator=` | Assignment (OCF) | `ClassName&` (for chaining) |
| `operator+`, `-`, `*`, `/` | Arithmetic | `ClassName` (new value) |
| `operator+=`, `-=`, `*=`, `/=` | Compound assignment | `ClassName&` |
| `operator==`, `!=`, `<`, `>`, `<=`, `>=` | Comparisons | `bool` |
| `operator++`, `operator--` | Increment/decrement | `ClassName&` (prefix) or `ClassName` (postfix) |
| `operator<<` | Stream output | `std::ostream&` |
| `operator>>` | Stream input | `std::istream&` |
| `operator[]` | Subscripting | Reference to element |

### Member vs non-member

Most operators can be written either way:

```cpp
// As a member function — `this` is the left operand
class Fixed {
    Fixed operator+(const Fixed& rhs) const { /* ... */ }
};

// As a free function — both operands explicit
Fixed operator+(const Fixed& lhs, const Fixed& rhs) { /* ... */ }
```

When you need to accept the **left** operand as something that's not your class (like `std::cout << fixed`), you **must** write it as a free function:

```cpp
std::ostream& operator<<(std::ostream& os, const Fixed& f) {
    os << f.toFloat();
    return os;
}
// Now: std::cout << myFixed;  works.
```

### Rules of thumb

- **Only overload when the operator has an obvious meaning** for your type. Overloading `+` to mean "concatenate" or "sum" is fine. Overloading `+` to mean "send email" will make your teammates hate you.
- **Binary arithmetic operators return by value.** `a + b` creates a new object.
- **Compound assignment returns by reference.** `a += b` returns `*this`.
- **Comparison returns bool.**
- **Stream operators take and return `std::ostream&` / `std::istream&`.**

---

## 17. Inheritance

Inheritance expresses an **"is-a"** relationship. A `Dog` **is-a** `Animal`. A `ScavTrap` **is-a** `ClapTrap`.

```cpp
class Animal {
protected:                          // accessible by subclasses
    std::string _type;
public:
    Animal() : _type("Unknown") {}
    Animal(const std::string &t) : _type(t) {}
    virtual ~Animal() {}            // virtual! more on this below
    std::string getType() const { return _type; }
    virtual void makeSound() const { std::cout << "..."; }
};

class Dog : public Animal {         // Dog is-a Animal
public:
    Dog() : Animal("Dog") {}        // call parent constructor explicitly
    void makeSound() const { std::cout << "Woof!"; }
};

Dog d;
d.getType();     // "Dog" — inherited method
d.makeSound();   // "Woof!"
```

### Three kinds of inheritance: `public`, `protected`, `private`

At 42 you'll use `public` inheritance almost exclusively. Quick map:

| Inheritance | `public` members of parent become... | `protected` members become... |
|---|---|---|
| `class D : public Parent` | `public` in D | `protected` in D |
| `class D : protected Parent` | `protected` in D | `protected` in D |
| `class D : private Parent` | `private` in D | `private` in D |

`public` = "is-a". `private` = "implemented-in-terms-of" (prefer composition instead). `protected` = almost never useful.

### Composition vs inheritance — a design note

Inheritance is **not** the default way to reuse code. The rule of thumb:

- **Is-a** (`Dog` is an `Animal`) → inheritance.
- **Has-a** (`Car` has an `Engine`) → composition (make `Engine` a member).

Composition is simpler, looser, and easier to reason about. Use inheritance only when polymorphism is genuinely needed — when callers want to hold a base pointer and call virtual methods. If `ClassB` just wants to *reuse* a few methods from `ClassA`, make an `ClassA` member instead of deriving.

```cpp
// Composition — Car has an Engine
class Car {
    Engine _engine;
public:
    void start() { _engine.ignite(); }
};

// Inheritance — Dog IS-A Animal (used polymorphically)
class Dog : public Animal { /* ... */ };
```

### Construction and destruction order

```
Construction of Dog d:
  1. Animal::Animal() runs  (parent first)
  2. Dog::Dog() runs         (then child)

Destruction:
  1. Dog::~Dog() runs        (child first)
  2. Animal::~Animal() runs  (then parent)
```

Think of it as a nested object: the `Dog` object *contains* an `Animal` sub-object. The sub-object is built first and destroyed last, just like a member.

### Memory layout

```
Dog d;

Memory:
┌────────────────────────────┐
│  Animal sub-object         │   ◄── from the parent class
│    _type: "Dog"            │
├────────────────────────────┤
│  Dog-specific members      │   ◄── anything Dog adds
│    (none in this example)  │
└────────────────────────────┘
```

A `Dog*` and an `Animal*` can both point to the start of this block — because the layout guarantees the `Animal` part comes first. That's why you can pass a `Dog*` wherever an `Animal*` is expected (upcasting), for free.

### Multiple inheritance (exists, tread carefully)

```cpp
class FlyingThing { virtual void fly() const = 0; };
class SwimmingThing { virtual void swim() const = 0; };
class Duck : public FlyingThing, public SwimmingThing { /* ... */ };
```

Multiple inheritance creates headaches (the *diamond problem* when two parents share a grandparent). At 42, you'll see **virtual inheritance** in CPP04 ex03 ("Interesting idea") as a taste. Otherwise, avoid it unless you're modeling **interfaces** (pure-virtual-only classes), which is the safe use case.

---

## 18. Polymorphism & Virtual Functions

Polymorphism = "one interface, many behaviors." It lets you write code that works with `Animal*` and automatically calls the right method for the actual type (`Dog`, `Cat`, etc.).

```cpp
Animal* animals[3];
animals[0] = new Dog();
animals[1] = new Cat();
animals[2] = new Dog();

for (int i = 0; i < 3; ++i)
    animals[i]->makeSound();     // Dog's sound, Cat's sound, Dog's sound
```

This is only possible because `makeSound` is declared `virtual` in `Animal`.

### What does `virtual` actually do?

It enables **dynamic dispatch**: the method to call is decided at runtime based on the actual object type, not the pointer type.

Under the hood, the compiler creates a hidden table — the **vtable** — for each class that has virtual functions. Every object of such a class stores a hidden pointer (the **vptr**) to its class's vtable.

```
Dog object in memory:
┌─────────────────┐
│ vptr ──────────►│ Dog's vtable:
├─────────────────┤  ┌─────────────────────────┐
│ _type: "Dog"    │  │ Dog::makeSound          │
│ other members   │  │ Dog::~Dog (virtual dtor)│
└─────────────────┘  └─────────────────────────┘

Cat object in memory:
┌─────────────────┐
│ vptr ──────────►│ Cat's vtable:
├─────────────────┤  ┌─────────────────────────┐
│ _type: "Cat"    │  │ Cat::makeSound          │
│ other members   │  │ Cat::~Cat (virtual dtor)│
└─────────────────┘  └─────────────────────────┘
```

When you call `animals[i]->makeSound()`, the compiler generates:
```
1. Read vptr from *animals[i]        (one memory load)
2. Read makeSound slot from vtable    (one memory load)
3. Call that address                   (indirect jump)
```

Three extra operations. That's the cost of virtual — minimal but nonzero. This is why C++ doesn't make everything virtual by default: **you pay for what you use**.

### The `virtual` destructor rule

If your class is meant to be a base class (you intend anyone to `delete` it through a base pointer), **its destructor MUST be virtual**. Otherwise:

```cpp
Animal* a = new Dog();
delete a;     // WITHOUT virtual: calls ~Animal only. Dog's resources leak.
              // WITH virtual:    calls ~Dog THEN ~Animal. Correct.
```

At 42, this is checked. Every polymorphic base class has `virtual ~ClassName();`.

### Pure virtual functions and abstract classes

```cpp
class Shape {
public:
    virtual ~Shape() {}
    virtual double area() const = 0;      // = 0 makes it pure virtual
};
```

A **pure virtual function** has no implementation — derived classes must provide one. A class with any pure virtual is **abstract**: you cannot instantiate it directly.

```cpp
Shape s;        // ERROR: Shape is abstract
Shape *s;       // OK: pointer to abstract is fine
Circle c;       // OK: Circle overrides area(), so it's concrete
Shape *s = new Circle();   // OK
s->area();                 // calls Circle::area()
```

This is how you define **interfaces** in C++: an abstract class with only pure virtual functions.

### The virtual rule of thumb

- Virtual destructor if ANYONE will `delete` a derived object through a base pointer.
- Virtual functions for any behavior that should differ per subclass.
- Pure virtual to force subclasses to implement a method.
- Non-virtual for "this behavior is final and never overridden."

---

## 19. Exceptions

Exceptions are the C++ way to signal errors that travel up the call stack automatically.

```cpp
void risky() {
    if (bad_condition)
        throw std::runtime_error("something broke");
}

int main() {
    try {
        risky();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
    }
}
```

See [ERROR_MANAGEMENT.md](ERROR_MANAGEMENT.md) for a full treatment, including every standard exception class. Summary:

### What happens when you `throw`

1. The thrown object is **copied** into a special runtime-owned memory area (separate from the stack).
2. The runtime begins **stack unwinding**: it walks up the call stack, looking for a `catch` that matches.
3. For every stack frame it passes, it **runs destructors** of every local object — in reverse construction order.
4. When it finds a matching `catch`, control jumps there.
5. If no match, the program calls `std::terminate()` → abort.

This destructor-running during unwinding is why **RAII** is so important: any resource tied to an object's destructor is released automatically even if an exception flies through.

### Custom exception classes

```cpp
class FileNotFoundException : public std::exception {
public:
    virtual const char* what() const throw() {
        return "File not found";
    }
};
```

`throw()` here (the trailing empty throw-specification) is C++98's way of declaring the function **throws nothing**. It's required on the override of `what()`.

### When to use exceptions

- **Exceptional / unrecoverable situations** where continuing is wrong (file not found, invalid argument, out of memory).
- **Constructors** that fail — you can't return an error code from a constructor, so exceptions are the natural choice.

When **not** to use exceptions:
- For normal control flow (like iterating until not found).
- In code that must have bounded, deterministic time (games, real-time).

---

## 20. Templates

Templates are C++'s answer to "how do I write code that works for any type?"

### Function templates

```cpp
template <typename T>
T max(T a, T b) {
    return a > b ? a : b;
}

max(3, 5);         // T = int
max(3.14, 2.71);   // T = double
max(std::string("abc"), std::string("xyz"));   // T = std::string
```

### Class templates

```cpp
template <typename T>
class Array {
    T *_data;
    int _size;
public:
    Array(int n) : _data(new T[n]), _size(n) {}
    ~Array() { delete[] _data; }
    T& operator[](int i) { return _data[i]; }
};

Array<int> ints(10);
Array<std::string> strings(5);
```

### How templates actually work — code generation

The compiler generates a **separate copy** of the code for every type you instantiate the template with. `max<int>` and `max<double>` are two completely different functions in the final binary — same source, two compilations.

```
template <typename T>
T max(T a, T b) { return a > b ? a : b; }
               │
   instantiations:
               │
          ┌────┴────┬────────────┐
          ▼         ▼            ▼
     int max       double max    string max
     (in binary)   (in binary)   (in binary)
```

### Why templates must go in the header

Because the compiler needs the full template definition at every `.cpp` that uses it (to generate the instantiation). If you put a template implementation in a `.cpp`, no other file can instantiate it.

```cpp
// Array.hpp
template <typename T>
class Array {
    // ...
    T& operator[](int i);
};

// Template member functions: DEFINE THEM IN THE HEADER
template <typename T>
T& Array<T>::operator[](int i) {
    return _data[i];
}
```

### Template specialization

Sometimes you want a specific type to behave differently:

```cpp
template <typename T>
void print(T val) { std::cout << val; }

template <>
void print<bool>(bool val) { std::cout << (val ? "true" : "false"); }
```

### Templates in the 42 modules

CPP07 is entirely about templates. You'll implement:
- Function templates (`swap`, `min`, `max`).
- Class templates (`Array`).
- Iterators, `iter()` functions that apply a function template to each element of an array.

The syntax looks heavy at first. Write one function template from scratch and it clicks.

---

## 21. The STL (Standard Template Library)

The STL is the crown jewel of C++98: a library of ready-made data structures and algorithms, all templated so they work for any type.

### Three pillars

```
        ┌──────────────┐
        │   CONTAINERS │   vector, list, deque, map, set, stack, queue, ...
        └──────┬───────┘
               │
               │ traversed by
               ▼
        ┌──────────────┐
        │   ITERATORS  │   an abstraction over "pointer into a container"
        └──────┬───────┘
               │
               │ consumed by
               ▼
        ┌──────────────┐
        │  ALGORITHMS  │   sort, find, copy, transform, accumulate, ...
        └──────────────┘
```

### Containers (the important ones)

| Container | Big-O access | Big-O insert | Use when |
|---|---|---|---|
| `std::vector<T>` | O(1) random | O(1) amortized at back | "array-like," fast indexing |
| `std::list<T>` | O(n) | O(1) anywhere | Many insertions/removals in the middle |
| `std::deque<T>` | O(1) random | O(1) at both ends | Double-ended queue |
| `std::map<K, V>` | O(log n) | O(log n) | Key → value, sorted |
| `std::set<T>` | O(log n) | O(log n) | Unique, sorted elements |
| `std::stack<T>` | top: O(1) | push/pop: O(1) | LIFO |
| `std::queue<T>` | front/back: O(1) | push/pop: O(1) | FIFO |

### Iterators

```cpp
std::vector<int> v;
v.push_back(1);
v.push_back(2);
v.push_back(3);

for (std::vector<int>::iterator it = v.begin(); it != v.end(); ++it)
    std::cout << *it << ' ';     // 1 2 3
```

Iterators are **"pointers with attitude"**: `*it` gets the value, `++it` moves to the next, `it1 == it2` compares. Different containers have different iterator kinds (random-access for `vector`, bidirectional for `list`, etc.), but the interface is consistent.

### Algorithms

```cpp
#include <algorithm>

std::sort(v.begin(), v.end());
std::find(v.begin(), v.end(), 42);
std::for_each(v.begin(), v.end(), print_function);
```

Algorithms take **iterator pairs** (a "range") and work on any container whose iterators match the required category. That's the "generic" part of "generic programming."

### The STL in the 42 modules

- **CPP08** — `std::vector`, `std::list`, `std::deque`, iterators, basic algorithms.
- **CPP09** — `std::stack`, `std::queue`, `std::map`, sorting algorithms.

Earlier modules (CPP00–CPP07) **forbid** the STL entirely. You implement everything by hand so you understand what the STL is doing under the hood.

---

## 22. I/O Streams

C++ replaced `printf`/`scanf` with **streams**: type-safe, extensible, stateful objects.

```cpp
std::cout << "Hello, " << name << "! You are " << age << " years old.\n";
std::cin  >> age;
```

See [LIBRARIES.md](LIBRARIES.md), [FSTREAM_GUIDE.md](FSTREAM_GUIDE.md), and [OPEN.md](OPEN.md) for details. Summary:

### The stream hierarchy

```
              std::ios_base
                    │
              std::ios
                  / \
                 /   \
          std::istream  std::ostream
                  \   /
                   \ /
               std::iostream

std::cin  is a std::istream
std::cout is a std::ostream
std::cerr is a std::ostream (unbuffered)
std::clog is a std::ostream (buffered)

File streams:
  std::ifstream → istream that reads from a file
  std::ofstream → ostream that writes to a file
  std::fstream  → iostream that does both

String streams:
  std::istringstream, std::ostringstream, std::stringstream
```

### Why streams beat `printf`

1. **Type safety.** `printf("%d", some_string)` is a runtime crash; `std::cout << some_string` just works.
2. **Extensible.** You overload `operator<<` once for your class and it works everywhere.
3. **Composable.** A file stream and a string stream share the same interface — you can swap them without changing downstream code.

Streams are slower than `printf` in raw throughput, but much safer. For logging and normal I/O, always prefer streams in C++.

### Common patterns

```cpp
// Read a whole file into a string
std::ifstream f("data.txt");
std::stringstream ss;
ss << f.rdbuf();
std::string content = ss.str();

// Convert anything to a string
std::stringstream ss;
ss << 42 << " is my number";
std::string s = ss.str();

// Parse a number out of a string
std::stringstream ss("42 3.14");
int i; double d;
ss >> i >> d;
```

---

## 23. Casts

C has one cast: `(type)value`. It's a sledgehammer — it will force almost any conversion, silently, no matter how wrong.

C++ replaces it with **four explicit casts**, each with a narrower, clearer purpose.

| Cast | Purpose | When used |
|---|---|---|
| `static_cast<T>(v)` | "Regular" conversions: numeric, up/down pointers in a known hierarchy | `int` ↔ `double`, upcast `Derived*` → `Base*` |
| `dynamic_cast<T>(v)` | Downcast with **runtime type check** (requires virtual) | Safely `Base*` → `Derived*` — returns NULL / throws if wrong |
| `const_cast<T>(v)` | Remove (or add) `const` or `volatile` | Calling a legacy C API that takes `char*` when you have `const char*` |
| `reinterpret_cast<T>(v)` | Raw bit-reinterpretation, same address, different type | Pointer ↔ integer, reading bytes as a different type — dangerous |

```cpp
double d = 3.14;
int    i = static_cast<int>(d);         // 3 — clear numeric conversion

Base *b = new Derived();
Derived *dp = dynamic_cast<Derived*>(b); // NULL if b isn't actually a Derived

const char *s = "hello";
char *mut = const_cast<char*>(s);        // legal, but writing through it is UB

int *p = reinterpret_cast<int*>(0xDEADBEEF); // treat this address as an int*
```

### Why four casts are better than one

- Each cast **documents intent**. `static_cast` says "safe conversion." `reinterpret_cast` screams "I am doing something bit-level risky." Grep for `reinterpret_cast` to audit dangerous code.
- Each cast has **its own rules**. `static_cast` can't discard `const`. `dynamic_cast` can't fake a runtime type. `const_cast` can't change the type. This narrowing prevents the "I used (T)v to shut up the compiler and now nothing works" bug.

**42 rule:** only use the four named casts in C++ code. C-style casts are forbidden after CPP06.

---

## 24. Undefined Behavior — The Sharp Edge of C++

**Undefined behavior (UB)** is any program behavior the language standard says nothing about. In practice, it means **"the compiler may do literally anything."** Not "crash." Not "return garbage." *Anything* — including appearing to work fine until the worst possible moment.

### Common sources of UB

- Dereferencing a NULL pointer.
- Dereferencing an invalid / dangling pointer.
- Using an uninitialized variable.
- Signed integer overflow.
- Out-of-bounds array access.
- Double-free (`delete` twice).
- Mismatched `new` / `delete[]`.
- Returning a reference to a local variable.
- Modifying a `const` object through `const_cast`.
- Reading from a union member other than the last one written.
- Data races (concurrent read/write without synchronization).

### Why UB is particularly dangerous in C++

Because compilers **optimize based on the assumption that UB does not happen**. If your code does something UB, the optimizer may assume that code path is unreachable, and delete checks, reorder instructions, or produce surprising results. UB isn't "the program crashes" — it's "the program runs but does the wrong thing, possibly only on certain hardware, possibly only when compiled with `-O2`."

### How to avoid UB

- Always initialize variables (including class members — use the initialization list).
- Never `delete` the same pointer twice; set pointers to `NULL` after `delete` if in doubt.
- Always pair `new` with `delete`, and `new[]` with `delete[]`.
- Bound-check array accesses. Prefer `std::vector::at()` over `operator[]` when you want runtime bounds checking (the former throws, the latter is UB on OOB).
- Run tests under **valgrind** — it catches most memory-related UB.
- Compile with `-Wall -Wextra -Werror -std=c++98 -pedantic`.

### The mental model

> **UB is a contract.** You promise the compiler you will not do certain things (null deref, overflow, OOB, etc.). In exchange, the compiler makes your code fast. Breaking the contract is not punished with a helpful error — the compiler assumes you would never break it, and the program degrades silently.

---

## 25. Tips That Make Things Click

These are the insights that turn "I wrote code that works" into "I understand *why* it works." Some are repeated from earlier sections because they matter.

### 1. RAII is not a pattern — it's the language

Every object has a constructor and a destructor. **Any resource that should be released can be tied to an object's lifetime.** Memory, file handles, mutex locks, network connections — wrap them in a class, acquire in the constructor, release in the destructor. Then you never have to remember to release. The compiler does it for you.

```cpp
{
    std::ifstream f("data.txt");   // file opened in constructor
    // ... use f ...
}                                  // file closed in destructor, no matter what
```

You don't call `f.close()`. You let `f` die. RAII.

### 2. The compiler is not your enemy — it's a safety net

Every error message is the compiler saying *"I caught something that would have hurt you at runtime."* When you see `no matching function for call to...`, don't fight it. Read it. Understand what types it *wanted* versus what you gave. Fix the types. The compiler wins; your program ships correct.

### 3. Stack first, heap last

If a variable can live on the stack, put it on the stack. Stack is:
- Automatic cleanup (no forgot-to-delete bug).
- Much faster allocation (no syscall, just move the stack pointer).
- Cache-friendly.

Use the heap (`new`) **only when**:
- The object must outlive its declaring scope.
- The size is not known at compile time (and you can't use `std::vector`).
- You need polymorphism (`Base *b = new Derived()`).

### 4. Read declarations right-to-left, and remember "cdecl"

```
const int *const *p
```

Reads as: `p` is a pointer to a const pointer to a const int.

Practice on cdecl.org if these make your head hurt. It's the same grammar every time.

### 5. References are pointers with manners

Under the hood, a reference is almost always compiled to a pointer. The difference is **the rules the compiler enforces on you**: no null, no reseat, must be initialized. Which means you never write null checks on reference arguments.

### 6. Every `new` needs an owner

Ask yourself: *"When this object is no longer needed, WHO will `delete` it?"* If you can't answer immediately, you have a design problem. Ownership is the single most common source of bugs in C++. Document it in comments if it's not obvious.

### 7. `const` correctness propagates outward, not inward

Once a function takes `const T&`, it can only call other `const` methods on `T`. If you forget `const` somewhere deep, it breaks every caller above. Start `const` from the bottom: make getters `const`, make non-mutating methods `const`, and `const`-correctness flows upward naturally.

### 8. `std::string` is a class, not a primitive

It has a constructor. It has a destructor. It does heap allocation. Passing it by value **copies** the heap buffer. **Always pass it as `const std::string&`** unless you explicitly need a copy. For the full API (find, substr, compare, append, …) see [STRING_FUNCTIONS.md](STRING_FUNCTIONS.md).

### 9. "Shallow copy is deep copy of pointers"

When the compiler-generated copy runs on a class with a `T*` member, it copies the **pointer value**, not what it points to. So you get two objects sharing one allocation — a double-free waiting to happen. Write a real copy constructor that allocates fresh and copies the data. This is the bread-and-butter of OCF.

### 10. Understand the layers

```
┌────────────────────────┐
│ Your source code       │  syntax, intent
├────────────────────────┤
│ Preprocessed code      │  #includes, #defines expanded
├────────────────────────┤
│ Compiled object files  │  machine code per .cpp, unresolved symbols
├────────────────────────┤
│ Linked executable      │  all .o files + libraries stitched together
├────────────────────────┤
│ Process in memory      │  text, data, bss, heap, stack — loaded by OS
├────────────────────────┤
│ CPU instructions       │  registers, memory loads, jumps
└────────────────────────┘
```

When something breaks, *where in this stack did it break?* That question cuts your debugging time in half.

### 11. `virtual` costs one pointer per object, one indirection per call

If that's too much, don't use it. If it's fine, use it freely. Most of your classes either don't need virtual at all (no inheritance) or clearly need it (polymorphic behavior). There's rarely a middle ground worth agonizing over.

### 12. The rule of "how would I do this in C?"

When a C++ feature confuses you, ask: *"how would I do this in C?"* Then look at what C++ generates.

- `class`? → struct + functions + naming convention + `this` pointer.
- References? → pointers you can't null-check or reseat.
- Virtual? → struct with a function-pointer table at its head.
- Templates? → multiple copies of a function with different `typedef T` macros.
- Operator overloading? → function calls with prettier syntax.
- Exceptions? → complicated `setjmp`/`longjmp`-style unwinding.

Seeing the C-shaped machine under C++ removes a lot of fear.

### 13. Read destructors to understand ownership

When looking at a class for the first time, **read the destructor first**. What does it release? That tells you what the class owns. Its copy constructor and `operator=` must match that ownership. Its members should line up with that ownership. The destructor is the truth.

### 14. "Private" is a compile-time convention, not a runtime barrier

`private` prevents *your own future typos and your teammates' accidents*. The CPU has no concept of access control. `private` is a seatbelt, not a safe. This is actually liberating: it lets you design cleanly without worrying about performance — all access control evaporates at compile time.

### 15. `std::cout << "\n"` vs `std::endl`

`std::endl` = `"\n"` + flush the buffer. Flushing is expensive. In a tight loop, use `"\n"`. At the end of a major section or before blocking for input, use `std::endl` so your output is visible. Don't blindly use `endl` everywhere — it's a common performance footgun.

### 16. When in doubt, print the constructors and destructors

Add `std::cout << "Foo constructed\n";` to every constructor, `std::cout << "Foo destroyed\n";` to every destructor. Run your program. *Watch the lifecycle*. This will teach you more about C++ semantics in ten minutes than reading the spec for an hour. (Every 42 CPP module expects this — it's part of the evaluation.)

### 17. "Headers declare, .cpp files define"

- Headers (`.hpp`): tell the world what exists — class definitions, function prototypes, `extern` declarations, templates (whole thing, because they need to be seen everywhere), inline functions.
- `.cpp` files: the *one and only* place where function bodies, static member definitions, and global variables actually live.

Violating this gets you linker errors: "multiple definition" (body in header, included twice) or "undefined reference" (prototype without matching definition).

### 18. Compilation errors cascade — fix the first one first

The compiler gets confused fast. Error #1 causes Errors #2 through #47. Fix #1 and recompile before even reading the rest. A 300-line error dump often collapses to "you forgot a `;`" at the top.

### 19. Don't use pointers when references would work

Pointers are for:
- Optional things (can be `NULL`).
- Things that can be retargeted (`p = &other;`).
- Arrays and C APIs.
- Polymorphism via base pointers.

For everything else, references are clearer and safer. Idiomatic C++ reaches for `const T&` first.

### 20. Learn to love `const`, then love it some more

Every parameter you pass that you don't modify? `const &`. Every method that doesn't change the object? `const`. Every local that won't change? `const`. You will write programs with far fewer bugs. Caveat: you need to start `const`-marking from the **leaves** (the getters) and work upward, or you'll have to fix dozens of call sites at once.

---

## 26. Common Pitfalls & How to Spot Them

| Symptom | Likely cause | Fix |
|---|---|---|
| `undefined reference to 'Foo::_x'` | Static member declared but not defined in a `.cpp` | Add `int Foo::_x = 0;` at file scope in `Foo.cpp` |
| `multiple definition of 'bar'` | Function body in a `.hpp` included twice | Move body to `.cpp`, or mark `inline` |
| Program segfaults after a copy | Shallow copy of a pointer member — double free | Write real copy constructor & `operator=` (OCF) |
| Silent "works then breaks" | Returning reference/pointer to a local | Return by value, or manage lifetime elsewhere |
| `error: invalid use of incomplete type 'Foo'` | You forward-declared `Foo` but need the full definition | `#include "Foo.hpp"` instead of `class Foo;` |
| `error: passing 'const Foo' as 'this' argument discards qualifiers` | You're calling a non-const method on a `const` object | Mark the method `const` |
| Mysterious destructor ran twice | You copied an object whose class doesn't have a copy constructor | Implement OCF |
| Leak in valgrind | Missed a `delete` — or used `delete` for `new[]` | Audit every `new`; pair with proper `delete`/`delete[]` |
| `error: cannot bind non-const reference to ...` | Passing a temporary to a `T&` parameter | Change parameter to `const T&` |
| Segfault only in optimized build | UB — compiler optimized based on your assumed-valid code | Find the UB: uninitialized var, OOB, null deref |
| "Pure virtual method called" at runtime | Calling virtual method in a base class constructor/destructor | Don't call virtual methods during construction/destruction |
| Output appears later than expected | Buffered stream — `std::cout` flushed only at newlines or program end | `std::endl` or `std::flush` where needed |
| Name-hiding warning | A derived class method shadows a base class overload | `using Base::method;` or rename |

---

## 27. An Effective Learning Path

The CPP modules at 42 are ordered carefully. Respect that order — each module fills holes the next one assumes.

```
          ┌────────┐
          │ CPP00  │ namespaces, classes, encapsulation, OCF intro
          └───┬────┘
              ▼
          ┌────────┐
          │ CPP01  │ new/delete, references, pointers to members, switch
          └───┬────┘
              ▼
          ┌────────┐
          │ CPP02  │ operator overloading, OCF, fixed-point numbers
          └───┬────┘
              ▼
          ┌────────┐
          │ CPP03  │ inheritance, constructor chaining
          └───┬────┘
              ▼
          ┌────────┐
          │ CPP04  │ virtual, polymorphism, abstract classes, interfaces
          └───┬────┘
              ▼
          ┌────────┐
          │ CPP05  │ exceptions, try/catch/throw
          └───┬────┘
              ▼
          ┌────────┐
          │ CPP06  │ the four C++ casts, RTTI
          └───┬────┘
              ▼
          ┌────────┐
          │ CPP07  │ templates (function + class)
          └───┬────┘
              ▼
          ┌────────┐
          │ CPP08  │ STL containers, iterators, algorithms
          └───┬────┘
              ▼
          ┌────────┐
          │ CPP09  │ more STL — stack, queue, map, real algorithms
          └────────┘
```

### Weekly rhythm that works

1. **Day 1 — read the subject slowly.** Every word matters. Note the forbidden keywords.
2. **Day 2 — write the skeleton.** OCF, headers, empty `main`, empty methods. Make it compile.
3. **Day 3 — implement the simplest exercise.** Run it. Print constructor/destructor lifecycle. Check with valgrind.
4. **Day 4 — implement the harder exercises.** At this point the patterns start repeating.
5. **Day 5 — review and refactor.** Re-read the subject. Run the norm. Prepare for defense.

### Things to do alongside every module

- Write ten constructors by hand without copying. OCF should be muscle memory.
- Explain out loud (or to a rubber duck) *why* you need the copy constructor in that specific exercise.
- Run valgrind on every binary. Zero leaks is table stakes.
- Read one section of this guide per day that matches what you're working on. Re-read it after the exercise.

### Resources worth bookmarking

- [cppreference.com](https://en.cppreference.com/w/cpp) — the encyclopedia. Dense but authoritative.
- [LearnCpp.com](https://www.learncpp.com/) — gentler, tutorial-style, covers C++98 → modern clearly.
- "The C++ Programming Language" by Stroustrup — the author's own book. Slow but deep.
- `man c++` and `man gcc` — nothing wrong with local docs.

---

## 28. Glossary

Terms are grouped by theme. Within each group, entries are ordered so earlier ones set up later ones — read top to bottom the first time through, then use it as a lookup later.

---

### 28.1 Objects, classes & design

The vocabulary for building and thinking about types.

- **Class** — A user-defined type that bundles data, functions, access control, and lifecycle hooks. Default access is `private`.
- **Struct** — Identical to a class except default access is `public`. Convention: use `struct` for pure data bags, `class` for everything else.
- **Object / instance** — A concrete variable of a class type that lives somewhere in memory.
- **Member** — Anything declared inside a class. Two kinds: **data members** (fields, attributes) and **member functions** (methods).
- **Method** — A member function. Has an implicit `this` pointer unless declared `static`.
- **Encapsulation** — Bundling data with the code that manages it, and hiding the internals behind a controlled interface.
- **Access specifier** — `public`, `private`, `protected`. Compile-time access rules; erased at runtime.
- **Friend** — A function or class granted access to another class's private members. Breaks encapsulation; forbidden at 42.
- **Getter / setter** — Public methods that read or modify a private field. Getters should always be `const`.
- **Composition** — A class holding another class as a value or reference member ("has-a"). Usually preferred over inheritance.
- **Aggregation** — A weaker form of composition where the contained object is not owned (typically held by pointer or reference to something that lives elsewhere).
- **POD (Plain Old Data)** — A class that behaves like a C struct: no virtuals, no user-defined constructors, trivially copyable. Safe to `memcpy`.

---

### 28.2 Object lifecycle

Everything that runs when an object is born, copied, assigned, or destroyed.

- **Constructor** — Code that runs when an object is created.
- **Default constructor** — `ClassName();` — invoked when an object is created without arguments.
- **Parameterized constructor** — A constructor that takes arguments to initialize members.
- **Copy constructor** — `ClassName(const ClassName&);` — builds a new object as a copy of an existing one.
- **Copy assignment operator** — `ClassName& operator=(const ClassName&);` — assigns one already-existing object to another.
- **Destructor** — `~ClassName();` — runs when an object dies; releases resources.
- **Initialization list** — Syntax `: _a(1), _b(2)` that initializes members before the constructor body runs. **Mandatory** for `const` members, reference members, and members without a default constructor.
- **Constructor chaining** — Explicit call to a base-class constructor from the derived class's initialization list.
- **RAII** (Resource Acquisition Is Initialization) — Tying a resource (memory, file, lock) to an object's lifetime so its destructor releases it automatically.
- **Scope** — The region of code where a name is valid. Stack objects are destroyed at the closing `}` of their scope.
- **Object lifetime** — The span between an object's construction and destruction.
- **Storage duration** — How long storage lasts: **automatic** (stack), **dynamic** (heap), **static** (data segment, whole program), **thread-local** (C++11+).
- **OCF (Orthodox Canonical Form)** — 42's discipline requiring every class to define default ctor, copy ctor, `operator=`, and destructor explicitly.
- **Rule of Three** — If you need an explicit destructor, copy constructor, OR `operator=`, you almost certainly need all three.
- **Shallow copy** — Member-by-member bitwise copy. Safe for value members, catastrophic for owned pointers.
- **Deep copy** — Copy that also duplicates the pointed-to resource, not just the pointer.
- **Object slicing** — Copying a derived object into a base-type variable; the derived parts are silently dropped.
- **Dangling pointer** — A pointer to memory that has been freed or to an object that has been destroyed.
- **Double free** — Calling `delete` or `free` twice on the same pointer. Undefined behavior.
- **Memory leak** — Memory allocated with `new`/`malloc` and never released.
- **Copy elision** — Compiler optimization that skips unnecessary copies (for example, return-value optimization).

---

### 28.3 References, pointers & values

Ways to refer to memory, and the vocabulary for what can be assigned where.

- **Pointer** — A variable holding an address. Can be null, reassigned, aliased. Syntax: `T *p`.
- **Reference** — An alias for an existing variable. Cannot be null, cannot be rebound, must be initialized. Syntax: `T &r`.
- **`const` reference** — A read-only alias. Can bind to temporaries (`const T&`) — the canonical function-parameter type.
- **Lvalue** — An expression with a persistent identity / address (a variable). Can appear on the left of `=`.
- **Rvalue** — A temporary value with no stable address (a literal, `a + b`, a returned-by-value object). Cannot be assigned to.
- **Null pointer** — A pointer pointing to nothing. Written `NULL` or `0` in C++98; `nullptr` in C++11+.
- **Dereferencing** — Accessing the value a pointer or reference points at (`*p`, `r`).
- **Address-of** — Taking a pointer to a variable (`&x`).
- **`this`** — Hidden pointer to the current object, automatically passed to every non-static member function.
- **Pass by value** — The function receives a copy of the argument; modifications don't propagate.
- **Pass by pointer / reference** — The function receives an alias to the caller's variable; modifications propagate.

---

### 28.4 Inheritance & polymorphism

How one class builds on another, and how the right method gets called at runtime.

- **Base class** — The class being inherited from. Also called parent or super-class.
- **Derived class** — A class that inherits from another. Also called child or subclass.
- **Inheritance** — An "is-a" relationship between classes. `class D : public B {}`.
- **Multiple inheritance** — Deriving from more than one base class. Carries pitfalls (name clashes, diamond problem).
- **Virtual inheritance** — `class D : virtual public B {}` — ensures only one shared base sub-object when a diamond hierarchy occurs.
- **Diamond problem** — Two parents of a class both inherit from the same grandparent; without `virtual` inheritance, the grandparent appears twice inside the child.
- **Virtual function** — A method whose call is resolved at runtime based on the object's actual type.
- **Pure virtual function** — `virtual void f() = 0;` — a virtual with no body; derived classes must implement it.
- **Abstract class** — A class with at least one pure virtual function. Cannot be instantiated directly.
- **Interface** (in C++) — An abstract class containing only pure virtual functions and a virtual destructor.
- **Overriding** — A derived class providing its own version of a base's virtual function.
- **Overloading** — Multiple functions with the same name but different parameter lists in the same scope.
- **Name hiding** — A derived class member hiding an identically-named base member; unhide with `using Base::name;`.
- **Operator overloading** — Defining what operators like `+`, `==`, `<<` do for your own types.
- **Dynamic dispatch** — Runtime selection of which function to call, via the vtable.
- **Static dispatch** — Compile-time selection (non-virtual calls, templates).
- **Polymorphism** — Code that operates on a base type but dispatches to derived-specific behavior at runtime.
- **Upcast** — Converting a derived pointer/reference to a base one. Always safe.
- **Downcast** — Converting a base pointer/reference to a derived one. Safe only via `dynamic_cast`.
- **Vtable** — Per-class hidden table of virtual function pointers.
- **Vptr** — Per-object hidden pointer to its class's vtable.
- **RTTI** (Run-Time Type Information) — Lets you query an object's actual type at runtime; powers `typeid` and `dynamic_cast`.

---

### 28.5 Templates & generics

Compile-time code generation.

- **Template** — A blueprint that generates code parameterized by types (or values).
- **Template parameter** — The placeholder in `template <typename T>` — `T` here.
- **Template instantiation** — The compiler generating a concrete copy of a template for specific arguments.
- **Template specialization** — Providing a custom implementation for a specific type: `template <> void print<bool>(bool);`.
- **Generic programming** — Writing algorithms and data structures that work for any type satisfying certain requirements.

---

### 28.6 Keywords & qualifiers

The small-but-important keywords that shape semantics.

- **`const`** — Compile-time promise not to modify. Applies to variables, parameters, pointers, and methods.
- **`static`** — Three meanings: class-shared member, function-local persistent variable, or file-local symbol.
- **`virtual`** — Enables runtime dispatch for a method. Required on base destructors used polymorphically.
- **`explicit`** — Prevents a single-argument constructor from being used for implicit conversions.
- **`mutable`** — Allows a member to be modified inside a `const` method (rare; used for caches or lazy evaluation).
- **`volatile`** — Tells the compiler a variable may change outside the program's control (hardware registers, signal handlers). Not for threading.
- **`inline`** — Hints that a function body may be substituted at the call site; also permits the body to live safely in a header.
- **`extern`** — Declares a symbol defined in another translation unit.
- **`typedef`** — Creates a type alias (C-era syntax).
- **`typename`** — Disambiguates that a dependent name inside a template refers to a type.
- **Namespace** — A named scope for symbols, preventing collisions between libraries.

---

### 28.7 Compilation model

The pipeline from source to binary, and the words that describe what lives where.

- **Translation unit** — One `.cpp` file plus everything it `#include`s; one compilation input.
- **Declaration** — Tells the compiler a name exists and what its type is (prototype, class declaration).
- **Definition** — Provides the actual body or storage. There must be exactly one per program.
- **ODR (One Definition Rule)** — Every function, variable, and class must have exactly one definition across the entire program.
- **Header guard** — `#ifndef X / #define X / #endif` — prevents a header from being processed twice in the same TU.
- **Forward declaration** — Declaring a class or function without defining it, to break include cycles or reduce coupling.
- **Linkage** — Whether a name is visible across translation units. `extern` → external; file-scope `static` or anonymous namespace → internal.
- **Name mangling** — How the compiler encodes namespaces, classes, and parameter types into linker-visible symbol names (this is why C++ overloads work and C-with-same-name doesn't).
- **ADL** (Argument-Dependent Lookup) — How the compiler finds `operator<<` overloads defined in the argument's namespace. Rarely needs conscious thought at 42.
- **Inline function** — A function whose body the compiler may substitute at the call site; also the only way to safely put a non-template function body in a header.
- **Compile time vs runtime** — Type checks, template instantiation, and `const` enforcement happen during compilation; virtual dispatch, allocation, and I/O happen when the program runs.

---

### 28.8 Errors, UB & safety

How things go wrong, and how C++ names the ways they go wrong.

- **Exception** — An object representing an error, propagated up the call stack until caught.
- **`throw`** — Raises an exception.
- **`try` / `catch`** — A block that can catch exceptions thrown inside it.
- **Stack unwinding** — The runtime walking back up the stack after a `throw`, destroying local objects along the way until it finds a matching `catch`.
- **Undefined behavior (UB)** — Any program state the standard does not define. The compiler may do anything; results are not predictable and may differ with optimization level.
- **Race condition** — Two threads reading/writing the same memory without synchronization. Not a C++98-at-42 concern, but useful to know the name.
- **Self-assignment** — `a = a;` — guard against it in `operator=` (check `this != &other`) so you don't free a resource before copying it.
- **`assert`** — A runtime check that aborts the program on false. Useful for invariants during development.

---

### 28.9 Standard library

The names for the parts of the C++ standard library you'll actually use.

- **STL** (Standard Template Library) — The containers, iterators, and algorithms portion of the standard library.
- **Container** — A template class that holds elements (`vector`, `list`, `map`, `set`, ...).
- **Iterator** — A pointer-like abstraction for traversing a container.
- **Algorithm** — A generic function that operates on iterator ranges (`sort`, `find`, `for_each`, ...).
- **Stream** — An object representing an input/output source or sink (`cin`, `cout`, `ifstream`, `stringstream`).
- **Manipulator** — A helper used with `<<` or `>>` to change formatting or stream state (`std::endl`, `std::setw`, `std::hex`).
- **`std`** — The namespace containing every standard library symbol.

---

### 28.10 The four C++ casts

- **`static_cast<T>(v)`** — Ordinary, compile-time-checked conversion (numeric conversion, upcast).
- **`dynamic_cast<T>(v)`** — Runtime-checked downcast of a polymorphic pointer or reference. Returns `NULL` (or throws, for references) on failure.
- **`const_cast<T>(v)`** — Adds or removes `const` / `volatile`. Changes nothing else.
- **`reinterpret_cast<T>(v)`** — Treats the bits as a different type. Dangerous; for hardware/serialization only.
- **C-style cast** — `(T)v` — tries each of the four in sequence. Works but hides intent; forbidden after CPP06.

---

## Closing Thought

C++ is large. You will not master it in a summer. Nobody has "finished learning C++" — not even Stroustrup. But at 42's CPP98 level, you don't need to.

What you need is:
- **Correct mental models** for objects, lifetimes, references, and virtual dispatch.
- **Muscle memory** for OCF, initialization lists, and `const`.
- **Intuition** for where memory lives, where errors come from, and what the compiler is actually doing.

Every time you write a class, ask yourself three questions:
1. *Who owns its resources?* (Answers OCF, destructor, copy semantics.)
2. *Is this method const-correct?* (Answers `const` on every getter.)
3. *What runs when this object is created, copied, assigned, destroyed?* (Answers how it composes with other classes.)

Answer those three consistently and you'll pass every defense, read any C++98 codebase, and have a foundation that transfers directly to modern C++ when you move on to C++11/14/17.

Good luck. Keep your destructors honest.
