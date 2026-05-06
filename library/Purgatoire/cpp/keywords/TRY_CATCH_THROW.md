# `try` / `catch` / `throw` — Exception Handling

> **TL;DR.** `throw` raises an exception (a value carrying error info); `try` marks a block where exceptions may be raised; `catch` lists handlers that match exception types. The runtime walks back up the call stack — destroying objects in scope (stack unwinding) — until it finds a matching catch. Detailed walkthrough in [`ERROR_MANAGEMENT.md`](../io-errors/ERROR_MANAGEMENT.md).

Related: [`ERROR_MANAGEMENT.md`](../io-errors/ERROR_MANAGEMENT.md) · [`MEMORY.md`](../fundamentals/MEMORY.md) · [`ORTHODOX_CANONICAL_FORM.md`](../oop/ORTHODOX_CANONICAL_FORM.md)

---

## 1. The shape

```cpp
try {
    // code that may throw
    if (problem)
        throw std::runtime_error("something broke");
}
catch (const std::runtime_error &e) {
    std::cerr << "runtime error: " << e.what() << '\n';
}
catch (const std::exception &e) {
    std::cerr << "other exception: " << e.what() << '\n';
}
catch (...) {
    std::cerr << "totally unknown error\n";
}
```

Three pieces:

```
   try     "I'm worried something below me might throw."
   throw   "Something is wrong; abort this code path with this error."
   catch   "If something below me threw a matching type, I'll handle it."
```

---

## 2. The control flow — stack unwinding

When `throw` runs:

```
   1. The thrown object is constructed (it's a real value, copied/moved into a runtime-managed slot).
   2. The current function aborts at the throw point.
   3. Local objects in scope are destroyed in reverse order — destructors run.
   4. Control returns to the caller, which also unwinds — destructors run there too.
   5. Continue up the stack until a matching catch is found.
   6. If no match — std::terminate is called → program ends.
```

```
                 throw point
   ┌───────────────────────────────────────────────┐
   │ caller A    locals: F1 F2                      │
   │  ┌─────────────────────────────────────────┐  │
   │  │ caller B    locals: G1                  │  │
   │  │  ┌────────────────────────────────────┐ │  │
   │  │  │ caller C    locals: H1 H2 H3       │ │  │
   │  │  │   throw expr;       ◄───── here    │ │  │
   │  │  └────────────────────────────────────┘ │  │
   │  │  destructors: ~H3, ~H2, ~H1 (reverse)   │  │
   │  └─────────────────────────────────────────┘  │
   │  destructors: ~G1                              │
   │                                                │
   │  ◄── catch found here? handle.                 │
   │      no? continue → caller A's destructors,    │
   │      then up to its caller, etc.               │
   └───────────────────────────────────────────────┘
```

This is **stack unwinding**. RAII (resources owned by stack objects with destructors) shines here — your file handles, mutex locks, allocations all clean up automatically.

---

## 3. What can be thrown?

**Anything.** Any copyable object.

```cpp
throw 42;                              // an int — please don't
throw "hello";                         // const char * — please don't
throw std::string("oops");             // a string — please don't
throw std::runtime_error("bad input"); // ✓ exception class — yes
```

Convention: throw types derived from `std::exception`, which guarantees a `what()` method:

```cpp
class std::exception {
public:
    virtual const char* what() const throw();
};
```

The standard library provides:

```
   std::exception
       ├── std::logic_error
       │       ├── std::invalid_argument
       │       ├── std::domain_error
       │       ├── std::length_error
       │       └── std::out_of_range
       ├── std::runtime_error
       │       ├── std::range_error
       │       ├── std::overflow_error
       │       └── std::underflow_error
       └── std::bad_alloc, std::bad_cast, std::bad_typeid, ...
```

For 42 modules you'll typically:

```cpp
class MyClass {
public:
    class MyException : public std::exception {
    public:
        const char *what() const throw() { return "MyClass error"; }
    };
};

if (bad)
    throw MyClass::MyException();
```

A nested exception class inside the throwing class. Standard 42-norm pattern.

---

## 4. Matching catches

Catches are tried in order. The first one whose type the thrown object is implicitly convertible to wins.

```cpp
try { throw std::out_of_range("x"); }
catch (const std::logic_error& e)  { /* matches — out_of_range derives from logic_error */ }
catch (const std::exception&  e)  { /* never reached, even though it would also match */ }
```

So **list specific types first, general types later**. Order matters.

### `catch (...)` — match anything

```cpp
catch (...) {
    // catches anything not caught above; you can't inspect the exception object here
}
```

Use sparingly — usually as a last-resort cleanup, often immediately followed by `throw;` to rethrow:

```cpp
try { risky(); }
catch (...) {
    cleanup();
    throw;            // re-throw the current exception, preserving its type
}
```

### Catch by reference — almost always

```cpp
catch (const std::exception& e)    // ✓ — reference, no slicing
catch (std::exception e)           // ✗ — by value, slices any derived class
```

If you catch by value, derived parts of the exception object are sliced off — same problem as polymorphism with value semantics. **Always catch by const reference** for class-type exceptions.

---

## 5. Hardware/runtime view — what exceptions cost

C++ exception handling in modern compilers (gcc/clang) uses **table-based unwinding** (a.k.a. zero-cost exceptions in the happy path):

```
   compiler emits, alongside each function:
     • the function's machine code (.text)
     • an unwind table (.eh_frame)
       ─ describes how to find frame pointers and destructors
         at every instruction in the function

   no try → no instructions added to the function body
   no throw → no runtime cost
   throw → runtime walks .eh_frame, runs destructors, transfers to handler
```

Key consequences:

- **The non-throwing path has zero overhead** on modern x86_64.
- **Throwing is expensive** — the runtime must search tables, run destructors, set up handler context. Hundreds of cycles minimum, often microseconds.
- **Binary size grows** — eh tables are not free.

Don't use exceptions for control flow. Use them for actual exceptional conditions.

---

## 6. Exception specifications (C++98 only — deprecated, then removed)

```cpp
void f() throw();              // C++98: promises NOT to throw
void g() throw(std::exception); // C++98: promises to only throw types derived from std::exception
```

If a function throws something it didn't declare, `std::unexpected` is called → typically calls `std::terminate`.

In practice:
- `throw()` is widely seen, especially on destructors.
- Throwing-list specifications (`throw(X, Y)`) were rarely used and were removed in C++17.

The 42 norm doesn't mandate exception specs, but you'll see them on `what()`:

```cpp
class MyEx : public std::exception {
public:
    virtual const char* what() const throw() { return "..."; }
};
```

The `throw()` after `const` declares this `what()` doesn't throw.

C++11 deprecated `throw()` in favor of `noexcept`. Not in 42.

---

## 7. Exception safety guarantees

When designing classes that may be involved with throwing code:

| Level | Promise |
|---|---|
| **No-throw** | The function never throws. Mandatory for destructors and `swap`. |
| **Strong** | Either the function succeeds, or the program state is unchanged (transactional). |
| **Basic** | The function may fail, but no resources leak and invariants hold. |
| **None** | All bets are off. |

Achieve "basic" by RAII — own resources via objects whose destructors clean up automatically. Achieve "strong" with the **copy-and-swap idiom** (build the new state aside, swap on success).

```cpp
// strong-exception-safe operator=
MyClass& operator=(MyClass other) {        // copy made; if it throws, original untouched
    swap(*this, other);                     // no-throw swap
    return *this;
}                                           // 'other' destructs, cleaning up old data
```

---

## 8. Destructors must not throw

```cpp
class C {
public:
    ~C() {
        if (problem()) throw std::runtime_error("nope");   // BAD
    }
};
```

If the destructor runs during stack unwinding (already handling another exception), throwing a *second* exception calls `std::terminate`. Unrecoverable.

Rule: **destructors must not let exceptions propagate.** Either don't throw, or catch internally:

```cpp
~C() {
    try { cleanup(); }
    catch (...) { /* swallow */ }
}
```

---

## 9. Tips & tricks

### 9.1 Throw by value, catch by reference

```cpp
throw MyException();                // construct and throw a temporary
catch (const MyException& e)        // bind by const reference
```

### 9.2 Inherit from `std::exception`

Always provide a `what()` returning a meaningful message. Tools (debuggers, loggers) expect it.

### 9.3 Use exceptions for **exceptional** conditions

For things like "user gave a bad command" in your phonebook, `if (badInput) printError()` is fine — no exceptions needed. Save exceptions for situations where the local code genuinely cannot proceed and the caller needs to be informed.

### 9.4 Don't `catch (...)` and do nothing

Silently swallowing exceptions hides bugs. At least log:

```cpp
catch (...) {
    std::cerr << "unknown error caught and ignored\n";
}
```

Or rethrow.

### 9.5 Rethrow with bare `throw;`

```cpp
catch (const std::exception& e) {
    log(e.what());
    throw;             // preserves the exception type and content
}
```

`throw e;` would slice if `e` is the base type — copying just the base part. Bare `throw;` rethrows the original.

### 9.6 Function-try-block — handle ctor exceptions

```cpp
class C {
    int *_data;
public:
    C(int n) try : _data(new int[n]) {
        // body...
    }
    catch (...) {
        // can clean up here, but cannot suppress; ctor throw always propagates
    }
};
```

Mostly used for member-init list throws. Niche.

### 9.7 `try` blocks are scoped

Variables declared inside `try { … }` are not visible after the block:

```cpp
try {
    int x = 5;
}
std::cout << x;        // ERROR — x out of scope
```

If you need a value from a try block, declare it outside.

---

## 10. Common errors

| Error | Cause | Fix |
|---|---|---|
| `terminate called after throwing an instance of …` | Exception thrown but not caught | Add a matching `catch`, or fix the bug |
| Slicing in `catch (Base e)` | Caught by value | Catch by `const Base&` |
| `std::terminate` during cleanup | A destructor threw during unwinding | Don't throw from destructors |
| Memory leak after exception | Manual `new` with no RAII wrapper | Use stack objects / smart pointers / RAII |
| Catch order ineffective | More-general catch listed before more-specific | Reorder: specific first, general last |

---

## 11. Visual summary

```
              ┌─────────────────────────────────────────┐
              │   try {                                  │
              │       throw X();   ← raises an exception │
              │   }                                       │
              │   catch (const X& e) { ... }              │
              │   catch (const std::exception& e) { ... } │
              │   catch (...) { ... }                     │
              └────────────────────┬────────────────────┘
                                   │
                                   ▼
              throw causes:
                1. construct exception object
                2. unwind stack — destructors run
                3. find matching catch (specific to general)
                4. transfer control to that catch
                5. no match? std::terminate.

              cost:
                non-throwing path: ~zero on modern x86_64
                throw: hundreds of cycles + destructor work
                binary size: +eh_frame tables

              rules of thumb:
                - throw classes derived from std::exception
                - throw by value, catch by const reference
                - destructors never throw
                - use for exceptional conditions, not control flow
                - RAII makes exception safety almost automatic
```

---

## 12. Practice

1. What happens if a thrown exception doesn't match any `catch`? *(`std::terminate` is called → program ends; no more destructors run beyond what unwinding had already covered.)*
2. Why must destructors not throw? *(If unwinding is already in progress for another exception, two simultaneous exceptions cause `std::terminate`.)*
3. Why is `catch (const std::exception&)` better than `catch (std::exception)`? *(By-value slices the exception object — derived parts (including the actual `what()` override) are lost.)*
4. What's the runtime cost of `try { ... }` if no exception is thrown? *(Effectively zero on table-based unwinding implementations — gcc/clang on x86_64.)*
