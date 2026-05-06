# Error Management in C++ — Complete Guide

A complete guide to failing well in C++ — return codes, stream state, and the full exception machinery — including a reference of every standard exception class you'll meet.

## Table of Contents

1. [The Three Strategies](#1-the-three-strategies)
2. [Return Codes](#2-return-codes)
3. [Stream State Checks](#3-stream-state-checks)
4. [Exceptions — The Basics](#4-exceptions--the-basics)
5. [What Actually Happens When You `throw`](#5-what-actually-happens-when-you-throw)
6. [The Standard Exception Hierarchy](#6-the-standard-exception-hierarchy)
7. [Every Standard Exception Class, Explained](#7-every-standard-exception-class-explained)
8. [Catching — Best Practices](#8-catching--best-practices)
9. [Rethrowing](#9-rethrowing)
10. [Custom Exception Classes](#10-custom-exception-classes)
11. [Exceptions in Constructors](#11-exceptions-in-constructors)
12. [Exceptions in Destructors — Danger Zone](#12-exceptions-in-destructors--danger-zone)
13. [`throw` Specifications (C++98) and `noexcept` (modern)](#13-throw-specifications-c98-and-noexcept-modern)
14. [Exception Safety Guarantees](#14-exception-safety-guarantees)
15. [Performance — The Real Cost](#15-performance--the-real-cost)
16. [When to Use Exceptions (and When Not To)](#16-when-to-use-exceptions-and-when-not-to)
17. [Common Pitfalls](#17-common-pitfalls)
18. [Decision Cheat Sheet](#18-decision-cheat-sheet)
19. [Template Project Utilities](#19-template-project-utilities)
20. [TL;DR](#20-tldr)

---

## 1. The Three Strategies

C++ has three ways to report that something went wrong, roughly from simplest to most powerful:

| Strategy | When to use it |
|---|---|
| **Return codes** | Small programs, argument validation, simple wrappers around syscalls. |
| **Stream state flags** | Any time you're reading/writing with `<iostream>` or `<fstream>`. |
| **Exceptions** | Errors deep in the call stack, constructor failures, library boundaries. |

They compose. A real program uses all three — streams signal their own failures, your code wraps them in return codes at thin boundaries, and exceptions carry errors up several levels at once.

---

## 2. Return Codes

The classic C-style pattern: the function returns a value that tells the caller whether it succeeded.

```cpp
#define OK  0
#define ERR 1

int open_file(const std::string& path)
{
    std::ifstream file(path.c_str());
    if (!file)
    {
        std::cerr << "Error: cannot open " << path << std::endl;
        return (ERR);
    }
    // ...
    return (OK);
}

int main()
{
    if (open_file("data.txt") != OK)
        return (ERR);
}
```

**Strengths:** zero machinery, no runtime cost, easy to reason about.
**Weakness:** callers can forget to check — silent failures are the classic bug.

---

## 3. Stream State Checks

Streams (`std::ifstream`, `std::ofstream`, `std::cin`) maintain their own flags that tell you whether the last operation succeeded.

```cpp
std::ifstream file("data.txt");

// Check 1: did it open?
if (!file)                         // same as file.fail()
    return (err_msg("Cannot open file"));

// Check 2: read loop (stops on EOF or error)
std::string line;
while (std::getline(file, line))
    std::cout << line << std::endl;

// Check 3: WHY did the loop stop?
if (file.bad())                    // serious I/O error (disk failure, etc.)
    return (err_msg("Read error"));
// if file.eof() — normal end of file, not an error
```

### The four state bits

| Flag      | Meaning                              | Serious?    |
|-----------|--------------------------------------|-------------|
| `good()`  | Everything fine                      | No          |
| `eof()`   | Reached end of file                  | No          |
| `fail()`  | Logical error (bad format, can't open) | Yes       |
| `bad()`   | Broken stream (hardware/OS error)    | Very        |

`fail()` implies `good()` is false. `bad()` implies `fail()` is true. `eof()` is independent — a read can succeed and set `eof()` because it happened to reach the end.

### Opt-in exceptions for streams

If you prefer exception-based I/O, streams can throw on specific flag bits:

```cpp
std::ifstream file;
file.exceptions(std::ios::failbit | std::ios::badbit);
file.open("data.txt");    // now throws std::ios_base::failure on failure
```

Most 42 code uses plain flag checks; this is just good to know.

---

## 4. Exceptions — The Basics

An **exception** is an object that represents an error and propagates up the call stack automatically until someone catches it.

### Throwing

```cpp
#include <stdexcept>

void divide(int a, int b)
{
    if (b == 0)
        throw std::invalid_argument("Division by zero");
    std::cout << a / b << std::endl;
}
```

### Catching

```cpp
int main()
{
    try
    {
        divide(10, 0);
    }
    catch (const std::exception& e)      // catches any standard exception
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return (1);
    }
}
```

### The basic shape

```cpp
try
{
    risky();                              // code that might throw
}
catch (const SpecificType& e)             // first match wins
{
    // ran only if `risky()` threw something compatible with SpecificType
}
catch (const std::exception& e)           // fallback
{
    // any std::exception descendant not caught above
}
// execution resumes here normally, whichever branch ran (if any)
```

Three rules to memorize:

- A `try` block **must** be followed by at least one `catch`.
- Catches are tried **top to bottom**; the first matching type wins.
- If no catch matches, the exception keeps propagating up the call stack.

---

## 5. What Actually Happens When You `throw`

### Step 1 — the exception object is copied to safe storage

The thrown object is copied into a hidden region of memory owned by the runtime — **not** the stack, because the stack is about to be destroyed. That's why it's safe to throw a local variable: the runtime has already duplicated it.

```cpp
void foo() {
    std::runtime_error err("boom");
    throw err;         // `err` is copied into exception storage
}                      // `err` itself is destroyed here — the copy survives
```

### Step 2 — stack unwinding

The runtime walks **up** the call stack, frame by frame, looking for a matching `catch`. For each frame it leaves, it **runs the destructors** of every local object in that frame, in reverse construction order, before discarding the frame.

This is the mechanism that makes **RAII** work:

```cpp
void bar() {
    std::ifstream file("data.txt");   // resource acquired
    std::vector<int> buf(1000);       // another resource

    mayThrow();                       // if this throws...
    // ...on the way out, buf's destructor runs, then file's destructor runs
    // → file is flushed & closed, memory is freed, automatically
}
```

This is the **fundamental reason to prefer exceptions over `exit()`**: `exit()` skips unwinding, so none of these destructors run.

### Step 3 — the catch runs (or `std::terminate`)

When unwinding finds a matching `catch`, it stops, binds the exception to the catch parameter, and runs the handler.

If the exception reaches `main()` without being caught, the runtime calls `std::terminate()`, which by default calls `std::abort()` — your program dies hard, usually without running destructors of any remaining static objects. **Always** have a top-level `catch (const std::exception&)` in `main` as a safety net.

---

## 6. The Standard Exception Hierarchy

All standard exceptions derive from `std::exception`. There are two big families under it — `std::logic_error` (programmer mistakes) and `std::runtime_error` (things that fail at runtime) — plus several direct children for specific situations.

```
std::exception                       <exception>      base — has .what()
│
├── std::bad_alloc                   <new>            'new' failed
├── std::bad_cast                    <typeinfo>       dynamic_cast of a reference failed
├── std::bad_typeid                  <typeinfo>       typeid on null polymorphic pointer
├── std::bad_exception               <exception>      dynamic-spec violation (C++98 quirk)
├── std::ios_base::failure           <ios>            stream error (when opted in)
│
├── std::logic_error                 <stdexcept>      "this shouldn't happen if the code is correct"
│   ├── std::invalid_argument                         bad argument
│   ├── std::domain_error                             value outside mathematical domain
│   ├── std::length_error                             container/string past max size
│   └── std::out_of_range                             index past valid range
│
└── std::runtime_error               <stdexcept>      "can happen even when the code is correct"
    ├── std::range_error                              result outside representable range
    ├── std::overflow_error                           arithmetic overflow
    └── std::underflow_error                          arithmetic underflow
```

### The mental divider

> **`logic_error`** = "this shouldn't happen if the code is correct" — passing a bad argument, accessing out of bounds, violating an invariant.
> **`runtime_error`** = "this can happen even if the code is correct" — a file isn't there, a network is down, a conversion produced a non-representable value.

That distinction is the whole reason there are two families. If you can't decide which to derive from for a custom exception, ask yourself: "could a perfect program still hit this?" If yes → `runtime_error`. If no → `logic_error`.

---

## 7. Every Standard Exception Class, Explained

Each entry has: the header it lives in, when the standard library throws it, and when you might throw it yourself.

### `std::exception` — the root

- **Header:** `<exception>`
- **Role:** base class for everything standard. Never thrown directly by the library; usually caught as the "safety-net" base type.
- **Interface:** one important virtual member — `virtual const char* what() const throw();` — returns a human-readable message.

```cpp
try { /* ... */ }
catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
}
```

---

### `std::bad_alloc` — allocation failure

- **Header:** `<new>`
- **Thrown when:** `new` fails to allocate memory, or any container operation that needs memory can't get it.
- **Catch it:** rarely worth handling specifically — on modern OSes, running out of memory usually means the process is going to die soon anyway.

```cpp
try {
    int *huge = new int[std::size_t(-1) / 2];   // absurdly big
}
catch (const std::bad_alloc& e) {
    std::cerr << "out of memory: " << e.what() << '\n';
}
```

The `new (std::nothrow) T` form avoids the throw and returns `NULL` instead.

---

### `std::bad_cast` — failed reference downcast

- **Header:** `<typeinfo>`
- **Thrown when:** `dynamic_cast` converts a **reference** to a type the object actually isn't. (Pointer versions of `dynamic_cast` return `NULL` instead of throwing.)

```cpp
class Base { public: virtual ~Base() {} };
class Derived : public Base {};
class Other   : public Base {};

Base *b = new Other;
try {
    Derived& d = dynamic_cast<Derived&>(*b);    // throws
}
catch (const std::bad_cast& e) {
    std::cerr << "wrong type: " << e.what() << '\n';
}
```

You'll meet this in CPP06.

---

### `std::bad_typeid` — `typeid` on null polymorphic pointer

- **Header:** `<typeinfo>`
- **Thrown when:** you apply `typeid` to an expression that dereferences a null pointer of a polymorphic type.

```cpp
Base *b = NULL;
try {
    std::cout << typeid(*b).name();   // throws
}
catch (const std::bad_typeid& e) { /* ... */ }
```

Obscure — you'll almost never write code that hits this.

---

### `std::bad_exception` — C++98 dynamic-spec violation

- **Header:** `<exception>`
- **Thrown when:** a function with a dynamic exception specification (`void f() throw(A, B);`) throws something outside the spec, AND the spec includes `std::bad_exception` — the runtime then substitutes it.
- **In practice:** a C++98 curiosity. Dynamic specs were deprecated in C++11 and removed in C++17.

---

### `std::ios_base::failure` — stream errors (opt-in)

- **Header:** `<ios>`
- **Thrown when:** you've enabled stream exceptions via `stream.exceptions(...)` and a corresponding flag bit is set.

```cpp
std::ifstream f;
f.exceptions(std::ios::failbit | std::ios::badbit);
try {
    f.open("missing.txt");
}
catch (const std::ios_base::failure& e) {
    std::cerr << "I/O error: " << e.what() << '\n';
}
```

By default streams do **not** throw — they silently set flags (see §3).

---

### `std::logic_error` — the base of "programmer error"

- **Header:** `<stdexcept>`
- **Role:** base of the logic-error family; never thrown by the library directly.
- **Throw it yourself when:** a precondition has been violated — bad caller input, invariant broken. Prefer one of its more specific children when applicable.

---

### `std::invalid_argument` — a wrong argument reached a function

- **Header:** `<stdexcept>`
- **Thrown by:** `std::bitset` constructor on bad input; `std::stoi`/`std::stol` (C++11+) when the string can't be parsed.
- **Throw it yourself when:** a function received a value that violates its contract.

```cpp
void setAge(int age) {
    if (age < 0)
        throw std::invalid_argument("age must be non-negative");
}
```

---

### `std::domain_error` — input outside mathematical domain

- **Header:** `<stdexcept>`
- **Thrown by:** nothing in the C++98 standard library (reserved for user code).
- **Throw it yourself when:** a mathematical function is called outside its valid domain.

```cpp
double safeSqrt(double x) {
    if (x < 0.0)
        throw std::domain_error("sqrt of negative");
    return std::sqrt(x);
}
```

---

### `std::length_error` — size would exceed the maximum

- **Header:** `<stdexcept>`
- **Thrown by:** `std::string` and `std::vector` when an operation would exceed `max_size()`.
- **Throw it yourself when:** requested size for your own container-like type would be unsupportable.

```cpp
try {
    std::string s;
    s.resize(s.max_size() + 1);     // throws
}
catch (const std::length_error& e) { /* ... */ }
```

---

### `std::out_of_range` — index beyond valid range

- **Header:** `<stdexcept>`
- **Thrown by:** `vector::at(i)`, `deque::at(i)`, `string::at(i)`, `bitset::operator[]` (with checking), `string::substr(pos)` when `pos > size()`, `map::at(key)` (C++11+).
- **Throw it yourself when:** a caller passes an index or key that isn't valid.

```cpp
std::vector<int> v(3);
try {
    v.at(10) = 5;                   // throws
}
catch (const std::out_of_range& e) {
    std::cerr << e.what() << '\n';
}
```

One of the most common exceptions in 42 CPP exercises — especially in CPP05 and CPP07.

---

### `std::runtime_error` — the base of "runtime failure"

- **Header:** `<stdexcept>`
- **Role:** base of the runtime-error family; never thrown directly by the library.
- **Throw it yourself when:** something failed that *could* fail in a correct program — external I/O, parsing, resource acquisition. It's the workhorse base for your custom exceptions.

```cpp
class FileNotFound : public std::runtime_error {
public:
    FileNotFound(const std::string& path)
        : std::runtime_error("file not found: " + path) {}
};
```

---

### `std::range_error` — computation produced an out-of-range result

- **Header:** `<stdexcept>`
- **Thrown by:** `std::wstring_convert` (C++11+), and user code.
- **Throw it yourself when:** a function's output cannot be represented.

Less common than `overflow_error`/`underflow_error` but semantically broader — use it when the issue is "the *result* doesn't fit," independent of arithmetic direction.

---

### `std::overflow_error` — arithmetic overflow

- **Header:** `<stdexcept>`
- **Thrown by:** `std::bitset::to_ulong()` when the value doesn't fit.
- **Throw it yourself when:** a conversion or arithmetic would produce a too-large value.

---

### `std::underflow_error` — arithmetic underflow

- **Header:** `<stdexcept>`
- **Thrown by:** nothing in C++98's standard library.
- **Throw it yourself when:** a value has gone below representable range.

---

### Quick reference table

| Class | Header | Thrown by stdlib when | Typical user throw |
|---|---|---|---|
| `exception` | `<exception>` | never (base only) | don't — derive instead |
| `bad_alloc` | `<new>` | `new` fails | usually let it propagate |
| `bad_cast` | `<typeinfo>` | `dynamic_cast<T&>` fails | rarely |
| `bad_typeid` | `<typeinfo>` | `typeid(*null_ptr)` | rarely |
| `bad_exception` | `<exception>` | dynamic-spec violation | never — C++98 quirk |
| `ios_base::failure` | `<ios>` | stream flag triggers, if opted in | stream wrappers |
| `logic_error` | `<stdexcept>` | never (base) | precondition base |
| `invalid_argument` | `<stdexcept>` | `bitset` bad init; `stoi` parse fail | argument validation |
| `domain_error` | `<stdexcept>` | never | math function outside domain |
| `length_error` | `<stdexcept>` | `string`/`vector` exceed `max_size()` | size-limited containers |
| `out_of_range` | `<stdexcept>` | `.at()`, `.substr(pos>size)` | index/key validation |
| `runtime_error` | `<stdexcept>` | never (base) | runtime-failure base |
| `range_error` | `<stdexcept>` | `wstring_convert` | result doesn't fit |
| `overflow_error` | `<stdexcept>` | `bitset::to_ulong` | arithmetic overflow |
| `underflow_error` | `<stdexcept>` | never | arithmetic underflow |

### A note on post-C++98 additions

C++11 added `std::system_error`, `std::nested_exception`, `std::exception_ptr`, and a few others. None of them apply to your 42 work — `-std=c++98` doesn't know they exist. Listed here only so you won't be confused when you meet them in real-world code.

---

## 8. Catching — Best Practices

### Catch by reference. Always.

```cpp
catch (std::exception e)          // BAD — slices derived exceptions
catch (std::exception* e)         // BAD — pointer-to-what? lifetime issues
catch (const std::exception& e)   // GOOD
```

**Why not by value?**
**Slicing.** If you throw a `std::runtime_error` (derived from `std::exception`) and catch it by value as `std::exception`, the copy is a plain `std::exception` — the derived part is lost, including any extra fields or a `what()` string stored in the derived class.

**Why not by pointer?**
Ownership becomes ambiguous: does the catcher `delete` it? Did the thrower allocate on the heap or throw the address of a local (which no longer exists)? Throwing and catching by reference sidesteps the whole question — the runtime owns the exception object.

**Why `const`?**
A convention. You almost always only read `e.what()`. Drop the `const` only if you need to modify the exception before rethrowing.

### Order matters — most specific first

Catches are matched **top-down with no overload resolution**. The first compatible type wins. Order from most specific to most general:

```cpp
try { /* ... */ }
catch (const std::out_of_range& e)  { /* most specific */ }
catch (const std::logic_error& e)   { /* less specific */ }
catch (const std::exception& e)     { /* most general  */ }
catch (...)                         { /* anything else */ }
```

If you flip the order and put `std::exception` first, it will swallow everything below it and the compiler will warn you that the later handlers are unreachable.

### The `catch (...)` catch-all

Matches **any** exception, including non-standard ones (`throw 42;`, `throw "oops";`, or user types that don't derive from `std::exception`).

```cpp
catch (...)
{
    std::cerr << "unknown error\n";
    throw;   // optional: rethrow after cleanup
}
```

Use it sparingly. It's useful in two cases:

- At the very top of `main()` as an ultimate safety net (after catching `std::exception`, for completeness).
- At a language boundary (C API, destructor) where **no exception may escape**.

Everywhere else, it hides bugs — you learn nothing about what went wrong.

---

## 9. Rethrowing

### Plain rethrow — `throw;`

Inside a `catch`, `throw;` (with no argument) rethrows the **same** exception object. Use it to log, clean up, and let a higher layer handle the actual decision:

```cpp
catch (const std::exception& e)
{
    std::cerr << "DB layer noticed: " << e.what() << '\n';
    throw;   // let the next layer up deal with it
}
```

### Wrapping — throw a different exception

If the higher layer shouldn't need to know about the low-level cause, wrap it:

```cpp
catch (const std::ios_base::failure& e)
{
    throw ConfigLoadError("bad config file: " + std::string(e.what()));
}
```

A rule of thumb: rethrow when the layer above is better positioned to decide; wrap when you want to hide the lower layer's vocabulary.

---

## 10. Custom Exception Classes

For meaningful errors, derive from `std::exception` (or one of its children) and override `what()`.

### The simplest path — derive from `std::runtime_error`

```cpp
#include <stdexcept>

class FileError : public std::runtime_error
{
public:
    FileError(const std::string& path)
        : std::runtime_error("Cannot open: " + path) {}
};

// Usage:
throw FileError("/tmp/missing.txt");
// Catchable as: FileError, std::runtime_error, or std::exception
```

This is the most common pattern — `std::runtime_error`'s constructor takes a `std::string` and stores it; `what()` returns that string. You inherit everything you need.

### The 42-style path — derive from `std::exception`

CPP05 specifically trains you to make exception classes by deriving straight from `std::exception` and overriding `what()` yourself:

```cpp
class GradeTooHighException : public std::exception {
public:
    virtual const char* what() const throw() {
        return "Grade is too high";
    }
};
```

Two things to note in that signature:
- `virtual` — it's inherited as virtual from `std::exception`; restating it is a habit.
- `throw()` — an empty **C++98 exception specification** meaning "this function itself throws nothing." You must repeat it when overriding, or the compiler complains about a mismatched specification.

### When to derive from which base

| Parent | Use for |
|---|---|
| `std::exception` | You want full control; you're returning a fixed message. |
| `std::runtime_error` | You want to attach a `std::string` message easily. Runtime failure. |
| `std::logic_error` | Same convenience as runtime_error, but for programmer mistakes. |
| `std::invalid_argument` / `std::out_of_range` / ... | You want the message *and* the semantic type. |

---

## 11. Exceptions in Constructors

Constructors have **no return value**. If initialization fails, exceptions are the *only* clean way to signal it.

When a constructor throws:

- The object is considered **never constructed**.
- Its destructor is **not called**.
- But the destructors of any **member sub-objects that were already constructed** **are** called (in reverse order).

```cpp
class Connection {
    Socket s;       // constructed first
    Buffer b;       // constructed second
public:
    Connection() : s(...), b(...) {
        if (!s.alive()) throw std::runtime_error("dead socket");
    }
    ~Connection() { /* ... */ }
};
```

If `b`'s constructor throws, `s` is destroyed (it was already built), but `~Connection` does **not** run — because the `Connection` never fully came into being.

This is why exceptions + RAII compose so well: failed construction leaves no half-built zombie behind, and no resources leak.

---

## 12. Exceptions in Destructors — Danger Zone

**Rule of thumb: destructors must not throw.**

If a destructor throws while another exception is already propagating (i.e., during stack unwinding), the runtime calls `std::terminate()` immediately. No second chances.

```cpp
class Bad {
public:
    ~Bad() {
        throw std::runtime_error("nope");   // if this runs during unwinding → terminate
    }
};
```

If a destructor does a risky cleanup (flushing a network socket, for example), wrap it so nothing escapes:

```cpp
~MyClass() {
    try { flush(); }
    catch (...) { /* log but do not rethrow */ }
}
```

---

## 13. `throw` Specifications (C++98) and `noexcept` (modern)

Since you're working in C++98 (`-std=c++98`), you'll see:

```cpp
void foo() throw();                // C++98 — promises no exceptions
void bar() throw(std::exception);  // C++98 — only std::exception may escape
```

These are the **dynamic exception specifications** — deprecated in C++11 and removed in C++17. In modern C++, the replacement is:

```cpp
void foo() noexcept;               // C++11+
```

For 42's C++98 modules you mostly just need to know they exist — and specifically that **`throw()` after a member function signature is standard when overriding `std::exception::what()`**:

```cpp
virtual const char* what() const throw();
```

You write it to match the base class's specification; omitting it is a compile error on strict toolchains.

---

## 14. Exception Safety Guarantees

When you write a function that might throw, it provides one of four guarantees:

| Level         | Meaning                                                                   |
|---------------|---------------------------------------------------------------------------|
| **No-throw**  | Never throws. (Destructors, `swap`, move ops should aim for this.)        |
| **Strong**    | If it throws, program state is unchanged (transactional).                 |
| **Basic**     | If it throws, no leaks or broken invariants, but state may be modified.   |
| **None**      | If it throws, anything goes. Avoid.                                       |

The STL containers mostly offer the **strong** or **basic** guarantee. Knowing which guarantee your code provides is a big part of writing exception-safe C++.

The typical technique for a strong guarantee is the **copy-and-swap idiom**: do everything on a side copy, and only commit changes (via a no-throw `swap`) once everything is safely done.

---

## 15. Performance — The Real Cost

Two camps of cost:

- **Zero-cost when nothing throws.** Modern compilers use table-based unwinding: no runtime check on every call; cost is paid only when an exception actually fires.
- **Very expensive when one throws.** Stack unwinding walks frames, reads unwind tables, runs destructors — orders of magnitude slower than a plain `return`.

**Takeaway:** exceptions are for *exceptional* paths. Don't use them as control flow for normal logic (e.g., "not found" in a map lookup). Use them for conditions that should stop the current operation.

---

## 16. When to Use Exceptions (and When Not To)

### Use them when:

- **Errors occur deep in the call stack.** A function 8 levels deep finds a corrupt file. Threading return codes back up through 8 functions is noise. One `throw`, one `catch` in `main` — done.
- **A constructor fails.** The only clean way for `Sed O_Content(path, a, b)` to report "path doesn't exist" is to throw.
- **You want guaranteed cleanup on failure.** Combined with RAII, exceptions guarantee that local `ifstream`/`ofstream`/`vector`/mutex-holders release their resources even when things go wrong.
- **You cross a library boundary that throws.** `std::stoi` throws `std::invalid_argument`; `vector::at` throws `std::out_of_range`. You need a `catch` or the program dies.
- **In `main`, as an ultimate safety net.**

```cpp
int main(int argc, char** argv) {
    try {
        return run(argc, argv);
    }
    catch (const std::exception& e) {
        std::cerr << "fatal: " << e.what() << '\n';
        return 1;
    }
    catch (...) {
        std::cerr << "fatal: unknown error\n";
        return 1;
    }
}
```

### Do NOT use them:

- **For expected outcomes.** "User typed a bad command" is not exceptional — it's common input. Handle it with a return code or a validating function.
- **In tight loops / hot paths.** Throwing is slow. Don't use exceptions for flow control in performance-critical code.
- **Across a C API boundary.** C doesn't understand C++ exceptions. If you expose a function to C, wrap the body in `try { ... } catch (...) { return -1; }`.
- **In destructors.** See §12.
- **In signal handlers.** Undefined behavior.

---

## 17. Common Pitfalls

### Don't loop on `eof()`

```cpp
// BAD — processes last line twice
while (!file.eof())
{
    std::getline(file, line);
    process(line);              // runs once more after the real last line
}

// GOOD — stops as soon as read fails
while (std::getline(file, line))
    process(line);
```

### Don't catch everything silently

```cpp
// BAD — hides bugs
try { doStuff(); }
catch (...) { }                 // swallowed, you'll never know what broke

// GOOD — at least log
try { doStuff(); }
catch (const std::exception& e)
{
    std::cerr << "Error: " << e.what() << std::endl;
    return (1);
}
```

### Don't ignore return values

```cpp
// BAD
open_file("data.txt");          // what if it failed?

// GOOD
if (open_file("data.txt") != OK)
    return (ERR);
```

### Don't throw a pointer or a literal

```cpp
throw "oops";      // throws a const char* — valid but annoying to catch
throw 42;          // throws an int — catch with catch(int)
throw new Err();   // throws a pointer — who deletes it? DON'T
```

Always throw an **object** deriving from `std::exception`.

### Remember that `catch` doesn't loop

A `catch` runs **once**. If the handler itself throws, you're back in unwinding mode from that point. There's no automatic retry.

### Catching what you can't handle

If a catch can't meaningfully recover, don't catch it — let it propagate to a layer that can. Empty or pass-through catches are usually a smell.

### Assuming destructors ran after `exit()` or `abort()`

They did **not** (for stack objects). Only exceptions and `return` unwind the stack. `exit()` runs `atexit` handlers and destructors of *static* objects only; `abort()` skips everything.

---

## 18. Decision Cheat Sheet

| Situation                              | Use                           |
|----------------------------------------|-------------------------------|
| Validating `argc/argv` in `main()`     | Return code                   |
| Opening a file                         | Stream check + return code    |
| Error deep in the call stack           | Exception                     |
| Constructor that can fail              | Exception (no return value)   |
| Checking read/write operations         | Stream state flags            |
| Library call (`.at()`, `stoi`, `dynamic_cast<T&>`) | Exception (you must catch) |
| Unrecoverable error (must quit now)    | `err_exit()` or exception     |
| Destructor cleanup that might fail     | `try`/`catch`, swallow        |

---

## 19. Template Project Utilities

The project template provides three helper functions in `include/utils/errors.hpp`:

```cpp
err_msg("something went wrong");     // prints to stderr, returns ERR (1)
warn_msg("this is suspicious");      // prints to stderr, continues
err_exit("cannot recover");          // prints to stderr, exits immediately
```

Use `err_msg` as a one-liner return:

```cpp
if (argc != 3)
    return (err_msg("Usage: ./program arg1 arg2"));
```

Remember: `err_exit` calls `exit()` and therefore **skips stack-object destructors**. If any object on the stack owns a resource, prefer throwing.

---

## 20. TL;DR

- Three tools: **return codes** for simple paths, **stream flags** for I/O, **exceptions** for deep or constructor errors.
- Throw objects deriving from `std::exception`. Prefer the specific child that fits (`out_of_range`, `invalid_argument`, `runtime_error`, ...).
- Know the split: **`logic_error`** = programmer bug, **`runtime_error`** = external failure.
- Always catch by `const&`.
- Order catches from most specific to most general.
- Destructors must not throw.
- Always have a top-level `catch (const std::exception&)` in `main`.
- Pair exceptions with **RAII** — that's where their real power comes from.
- `exit()` skips unwinding; exceptions and `return` don't.
