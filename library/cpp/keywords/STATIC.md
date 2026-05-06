# `static` — One Keyword, Three Different Jobs

> **TL;DR.** `static` does **three unrelated things** depending on where you put it. Internal linkage at file scope; persistent storage at function scope; class-wide member at class scope. They share a name and almost nothing else.

Related: [`CONST.md`](CONST.md) · [`EXTERN.md`](EXTERN.md) · [`BASICS.md`](../fundamentals/BASICS.md#11-static-members) · [`MEMORY.md`](../fundamentals/MEMORY.md)

---

## 1. The three meanings at a glance

```
┌────────────────────────────────────────────────────────────────────┐
│ where you put it          │ what it means                          │
├───────────────────────────┼────────────────────────────────────────┤
│ file scope (.cpp)         │ internal linkage — symbol stays in     │
│ static int counter;       │ this translation unit only             │
├───────────────────────────┼────────────────────────────────────────┤
│ inside a function         │ static storage — variable lives for    │
│ void f() { static int n;} │ entire program lifetime, init once     │
├───────────────────────────┼────────────────────────────────────────┤
│ inside a class            │ class-wide member — one copy shared    │
│ static int count;         │ by all instances; called via Class::   │
└───────────────────────────────────────────────────────────────────┘
```

These are three completely different language features that happen to use the same keyword. Don't mix them up.

---

## 2. `static` at file scope — internal linkage

```cpp
// utils.cpp
static int helper(int x) { return x * 2; }   // not visible from other .cpp files
```

The function (or variable) is **not exported** to the linker. Two `.cpp` files can each define their own `static int helper(...)` without colliding.

### Hardware/linker view

```
            translation unit       object file        executable
            (one .cpp + headers)  (.o file)           (linked binary)
                  │                  │                   │
non-static fn ────┼─► global symbol ─┼─► visible ───────┼─► one symbol
                  │                  │   to linker       │   for the whole program
                  │                  │                   │
    static fn ────┼─► local symbol  ─┼─► not exported    │  no risk of collision
                  │                  │                   │
```

Inspect it with `nm`:
```
$ nm utils.o
0000000000000000 T extern_helper       ← T = global text symbol
0000000000000010 t static_helper       ← t = local (lowercase) text symbol
```

### Why use it

- **Avoids name collisions** between translation units.
- **Tells the optimizer** the function can't be called from outside this file → can be inlined more aggressively, dead-code eliminated more easily.
- **Documents intent** — "this is internal."

### Modern alternative — anonymous namespaces

```cpp
namespace {
    int helper(int x) { return x * 2; }   // also internal-linkage
    int counter = 0;
}
```

`static` at file scope is the C-style approach; anonymous namespaces are the C++ way. Both are accepted in 42 norm. Anonymous namespaces also work for types (you can't put `static` on a class).

---

## 3. `static` inside a function — persistent local

```cpp
int next_id() {
    static int counter = 0;   // initialized once, on first call
    return ++counter;
}

next_id();   // 1
next_id();   // 2
next_id();   // 3 — counter survived between calls
```

The variable is **constructed once** (on first entry) and **destroyed at program exit**.

### Where does it live?

Not on the stack. It lives in the **data segment** (initialized) or **BSS** (zero-initialized), exactly like a global — but its **name** is only visible inside the function.

```
function call frames                   static storage (program lifetime)
┌────────────────┐ stack pointer       ┌───────────────────────────────┐
│  next_id() #3  │ ─── reads/writes ──►│    counter = 3                │
└────────────────┘                     └───────────────────────────────┘
       │
   pop on return
       ▼
┌────────────────┐
│  caller frame  │
└────────────────┘
```

### Initialization is "magic"

The first time the function is called, the compiler runs the initializer. To avoid running it twice (in multithreaded code, on every entry, etc.), the compiler emits a hidden flag plus, in C++11+, a thread-safe one-shot guard.

C++98 init was **not thread-safe** by default. If your `static` local is a complex object, two threads can hit the constructor simultaneously. (For 42 CPP single-threaded modules this doesn't matter, but remember it.)

```cpp
void log() {
    static std::ofstream f("trace.log");   // opened once, on first call
    f << "called\n";
}                                          // f closed at program exit
```

### Tip — the Meyers singleton

The cleanest singleton in C++:

```cpp
class Logger {
public:
    static Logger& instance() {
        static Logger inst;   // constructed on first call, destroyed at exit
        return inst;
    }
private:
    Logger() {}
    Logger(const Logger&);                 // disabled
    Logger& operator=(const Logger&);      // disabled
};
```

No `new`, no `delete`, no leak. Construction is lazy.

---

## 4. `static` inside a class — class-wide member

```cpp
class PhoneBook {
    static int _count;        // declaration: shared by all instances
public:
    PhoneBook()  { ++_count; }
    ~PhoneBook() { --_count; }
    static int getCount() { return _count; }
};

int PhoneBook::_count = 0;    // DEFINITION — must live in exactly one .cpp
```

Two parts:
1. **Declaration** in the class. Tells the compiler the member exists.
2. **Definition** in a `.cpp`. Reserves storage. **Forgetting this is the #1 linker error** — `undefined reference to PhoneBook::_count`.

### Memory layout

```
PhoneBook a, b, c;

stack                              static (.data)
┌─────────────┐                    ┌──────────────────┐
│      a      │                    │                  │
│  (no _count │ ─────┐              │  PhoneBook::    │
│   inside!)  │      │              │     _count = 3  │
├─────────────┤      ├─────────────►│                  │
│      b      │      │              └──────────────────┘
├─────────────┤      │              ▲
│      c      │ ─────┘              │
└─────────────┘                     all three instances
                                    share this single int
```

`sizeof(PhoneBook)` does **not** include `_count`. The non-static members are the per-instance footprint.

### Static member functions

```cpp
static int getCount();   // no 'this' pointer
```

Differences from a non-static member function:
- **No implicit `this` parameter.** The function doesn't know which instance called it.
- **Cannot access non-static members** (they would need a `this`).
- **Cannot be `const`** — there's no instance to be const about.
- **Cannot be `virtual`** — there's no instance to dispatch on.
- **Called as `Class::fn()`** — though `obj.fn()` also compiles for syntactic compatibility.

### Static `const` integral members — the special case

```cpp
class Buffer {
    static const int CAPACITY = 1024;   // declaration AND in-class initializer
public:
    char data[CAPACITY];                // can use it as a constant expression
};
```

For **integral or enumeration** types declared `static const`, C++98 lets you put the initializer right in the class body. You typically don't need a separate `.cpp` definition unless you take the address (`&CAPACITY`).

For non-integral types (e.g., `static const std::string`), you still need a `.cpp` definition.

---

## 5. `static` does NOT mean

People coming from C/Java make these mistakes:

- ❌ `static` does not mean "constant." Use [`const`](CONST.md) for that.
- ❌ `static` does not mean "private." Use access specifiers ([PUBLIC/PRIVATE/PROTECTED](PUBLIC_PRIVATE_PROTECTED.md)).
- ❌ A `static` member is not "global." It's scoped to the class.
- ❌ A `static` local is not "free of overhead." First-call init has a hidden guard check (small but real).
- ❌ `static` in a header at file scope gives **each including .cpp its own copy** — usually wrong. Use `inline` (C++17) or `extern` + a single definition.

---

## 6. Storage durations summary

```
┌──────────────────┬────────────────────┬──────────────────────────┐
│ duration         │ when does it exist │ examples                 │
├──────────────────┼────────────────────┼──────────────────────────┤
│ static           │ entire program     │ globals, static locals,  │
│                  │                    │ static class members     │
├──────────────────┼────────────────────┼──────────────────────────┤
│ automatic        │ scope of the block │ ordinary locals          │
├──────────────────┼────────────────────┼──────────────────────────┤
│ dynamic          │ new → delete       │ heap-allocated objects   │
├──────────────────┼────────────────────┼──────────────────────────┤
│ thread (C++11+)  │ thread lifetime    │ thread_local x;          │
└──────────────────┴────────────────────┴──────────────────────────┘
```

The keyword `static` is involved in the **first row**.

---

## 7. The static initialization order fiasco

When you have two file-scope `static` (or non-`static` global) objects in different translation units that depend on each other:

```cpp
// a.cpp
extern Foo foo;
Bar bar = makeBar(foo);   // depends on foo being constructed

// b.cpp
Foo foo;
```

The order across translation units is **unspecified**. `bar` may be initialized before `foo` is constructed → undefined behavior.

**Fix — the construct-on-first-use idiom:**

```cpp
// a.cpp
Bar& bar() {
    static Bar instance = makeBar(foo());   // built on first call
    return instance;
}

// b.cpp
Foo& foo() {
    static Foo instance;
    return instance;
}
```

This works because **static-local init order is well-defined** — first call wins.

---

## 8. Tips & tricks

### 8.1 Default-initialize zero-cost

`static` (and global) variables are **zero-initialized** before any user code runs:

```cpp
static int n;             // n is 0 — guaranteed
static int *p;            // p is NULL — guaranteed
static std::string s;     // s is "" — default-constructed
```

In contrast, `int n;` inside a function is uninitialized garbage.

### 8.2 `static` for module-private constants

```cpp
// parser.cpp
static const int MAX_TOKENS = 4096;     // visible only here
```

No header pollution; no symbol export.

### 8.3 `static_cast` is unrelated

`static_cast<T>(x)` reuses the keyword for a different feature — compile-time casting. See [`CASTS.md`](../advanced/CASTS.md). Don't conflate them.

### 8.4 Counter-debugging

A common lifecycle test for OCF:

```cpp
class Tracer {
    static int _alive;
public:
    Tracer()  { ++_alive; }
    Tracer(const Tracer&)  { ++_alive; }
    ~Tracer() { --_alive; }
    static int alive() { return _alive; }
};
int Tracer::_alive = 0;
```

If `Tracer::alive() != 0` at program exit, you have a leak.

### 8.5 Don't return a reference to a static local that depends on parameters

```cpp
const std::string& bad(int n) {
    static std::string s;
    s = std::to_string(n);   // overwrites every call!
    return s;                // caller holds a reference that mutates from under them
}
```

### 8.6 `static` in headers — the trap

```cpp
// utils.hpp
static int counter = 0;     // every .cpp that includes this gets ITS OWN counter
```

Almost certainly not what you want. Use `extern int counter;` in the header and `int counter = 0;` in exactly one `.cpp`. Or wrap it in a class with a `static` member (then the rule of one definition still holds).

---

## 9. Common errors

| Error | Cause | Fix |
|---|---|---|
| `undefined reference to ClassName::member` (linker) | Forgot the `.cpp` definition of a static class member | Add `int ClassName::member = 0;` to a `.cpp` |
| `cannot declare member function 'X::f' to have static linkage` | Used `static` on a member function definition outside the class | Drop `static` from the out-of-class definition; keep it in the declaration |
| `'this' is unavailable for static member functions` | Using `this` in a static member function | Don't — pass the object explicitly |
| `multiple definition of 'foo'` (linker) | `int foo;` (not `static`, not `extern`) in a header → duplicated in each TU | Move to a single `.cpp` |

---

## 10. Visual summary

```
                  ┌──────────────────────────┐
                  │      keyword: static     │
                  └────────────┬─────────────┘
                               │
        ┌──────────────────────┼──────────────────────┐
        ▼                      ▼                      ▼
  at file scope          inside a function       inside a class
  ───────────            ──────────────────      ──────────────
  internal linkage       static storage          one copy shared
  (not exported)         (lives forever,         by all instances
                          init once)              (Class::member)

  use for:               use for:                 use for:
  - .cpp helpers         - call counters          - shared state
  - file-private state   - lazy construction      - instance counters
  - Meyers singleton                              - class constants
```

---

## 11. Practice

1. Why does putting `static int n = 0;` in a header create one `n` per source file? *(Each TU sees its own definition; `static` gives it internal linkage.)*
2. Why does a static member need a definition in a `.cpp`? *(The class body is just a declaration; storage must be allocated exactly once.)*
3. Why can't a static member function be `virtual`? *(Virtual dispatch needs `this`; static member functions don't have one.)*
4. What's wrong with `static T& bad() { static T t = makeT(); return t; }` if `makeT()` itself uses another static-local-returning function? *(Nothing! That's the construct-on-first-use idiom — it solves the static-init-order fiasco.)*
