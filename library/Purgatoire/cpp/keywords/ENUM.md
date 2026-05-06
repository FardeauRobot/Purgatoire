# `enum` — A Set Of Named Integer Constants

> **TL;DR.** `enum` defines a type whose values are named integer constants. C++98 enums are integers under the hood — they implicitly convert to/from `int`, the underlying width is implementation-defined, and the names live in the enclosing scope (not inside the enum). C++11 added `enum class` for stricter typing.

Related: [`STATIC.md`](STATIC.md) · [`CONST.md`](CONST.md)

---

## 1. The basic form

```cpp
enum Color {
    RED,         // 0
    GREEN,       // 1
    BLUE         // 2
};

Color c = GREEN;
int   x = c;     // implicit conversion to int — c is now 1
```

Each name gets the next value, starting from 0. You can override:

```cpp
enum HttpStatus {
    OK            = 200,
    NOT_FOUND     = 404,
    SERVER_ERROR  = 500
};

enum Mixed {
    A,           // 0
    B = 5,
    C,           // 6 — continues from B
    D = 5,       // legal — duplicates allowed
    E            // 6
};
```

---

## 2. The implicit-int trap (C++98)

```cpp
enum State { IDLE, RUNNING, DONE };

State s = IDLE;
int   n = s;            // OK — implicit State → int
s = 42;                 // ERROR — int → State requires a cast
s = static_cast<State>(42);   // OK, but State has no enumerator with value 42
                              // → s holds an out-of-range value, behavior depends
```

The conversion is one-way implicit (enum → int). Going back requires a cast.

This implicit-int conversion is convenient (you can use enum values where ints are expected) and dangerous (mixing enums of different types compiles silently):

```cpp
enum Color { RED, GREEN };
enum Fruit { APPLE, BANANA };

Color c = RED;
Fruit f = APPLE;

if (c == f) { … }        // compiles! both convert to int → both are 0 → equal
```

C++11's `enum class` fixes this; in C++98, be careful.

---

## 3. Enumerators leak into the surrounding scope

```cpp
enum Direction { NORTH, SOUTH, EAST, WEST };

Direction d = NORTH;     // can use NORTH directly
```

`NORTH`, `SOUTH`, etc. are visible **at the enum's enclosing scope** — not inside `Direction`. You don't write `Direction::NORTH`. This causes name pollution:

```cpp
enum Color    { RED, GREEN, BLUE };
enum Severity { GREEN, YELLOW, RED };  // ERROR — RED and GREEN already declared
```

C++11's `enum class` keeps names scoped (`Color::RED`, `Severity::GREEN`).

In C++98 you work around the leak by:

- Putting the enum inside a class or namespace:

```cpp
class Color {
public:
    enum Value { RED, GREEN, BLUE };
};

Color::Value c = Color::RED;
```

---

## 4. Hardware/storage view

The compiler picks an underlying integer type "large enough to hold all the enumerators." On most compilers this is `int` (4 bytes), but it can be smaller or larger:

```cpp
enum Tiny  { A };                           // could be 1, 2, 4 bytes — impl-defined
enum Big   { LARGE = 0x70000000 };          // needs at least 32-bit
enum Huge  { HUGE  = 0x100000000LL };       // requires 64-bit (impl-specific)
```

```
   memory:
   ┌────┐                                       
   │  1 │  enum Color { RED }; sizeof = ? (often 4)
   └────┘                                       
```

You generally can't rely on `sizeof(enum) == sizeof(int)`, but in practice on g++ it is.

In C++11 you can fix the underlying type:

```cpp
enum class Severity : char { LOW, MEDIUM, HIGH };   // 1 byte guaranteed
```

Not in C++98.

---

## 5. Common 42 idiom: enum as typesafe integer constants

```cpp
class Animal {
public:
    enum Type { CAT, DOG, BIRD, UNKNOWN };

    Animal();
    Animal(Type t);

    Type getType() const;
private:
    Type _type;
};

Animal a(Animal::CAT);
if (a.getType() == Animal::DOG) { … }
```

This is far better than:

```cpp
class Animal {
public:
    static const int CAT = 0;
    static const int DOG = 1;
    static const int BIRD = 2;
    static const int UNKNOWN = 3;
};
```

— because the enum is at least *type-distinguishable* from random ints (even if it converts implicitly).

---

## 6. Enum as compile-time constant

```cpp
enum { BUFFER_SIZE = 1024 };          // anonymous enum — just a constant

char buffer[BUFFER_SIZE];              // OK, BUFFER_SIZE is a constant expression
```

This is a common C++98 trick to declare integer constants when `static const int` in-class is awkward (it works, but pre-C++11 you sometimes hit issues if you take its address). The "enum hack" sidesteps that.

```cpp
template <int N>
class Buffer {
    enum { CAPACITY = N * 2 };       // computed compile-time constant
    int data[CAPACITY];
};
```

Useful in templates where you want a constant computed from template parameters.

---

## 7. Iterating an enum

There's no built-in way. Common pattern:

```cpp
enum Day { MON, TUE, WED, THU, FRI, SAT, SUN, DAY_COUNT };

for (int d = MON; d < DAY_COUNT; ++d) {
    Day day = static_cast<Day>(d);
    // ...
}
```

The trailing `DAY_COUNT` is a sentinel — the count of real values. Common idiom for "make this enum iterable."

---

## 8. Enum vs `#define` vs `static const`

```cpp
#define MAX 1024                  // textual replacement, not typed, no scope
const int MAX = 1024;             // typed, scoped, has an address
static const int MAX = 1024;      // file-local typed constant
enum { MAX = 1024 };              // compile-time constant, no address
```

Pick:
- `enum` for a *set* of related constants.
- `static const int` for a single in-class integer constant (`class C { static const int N = 10; };`).
- `const int` at namespace scope for a single typed constant (with proper linkage).
- `#define` only for include guards and conditional compilation.

---

## 9. C++11 `enum class` — for context only (not 42 C++98)

```cpp
enum class Color : int { RED, GREEN, BLUE };

Color c = Color::RED;             // must qualify
int   x = c;                       // ERROR — no implicit conversion
int   y = static_cast<int>(c);     // OK
```

`enum class` is **strongly typed** and **scoped**:
- Names don't leak — write `Color::RED`, not `RED`.
- No implicit conversion to int.
- Optional underlying type: `: int`, `: char`, etc.

If you graduate from 42 to a modern C++ codebase, prefer `enum class` everywhere it's available.

---

## 10. Tips & tricks

### 10.1 Always include an UNKNOWN / NONE / DEFAULT

```cpp
enum LogLevel { DEBUG, INFO, WARN, ERR, NONE };
```

A no-op or sentinel value is useful for default-constructed objects.

### 10.2 Use the count sentinel pattern

```cpp
enum Direction { NORTH, SOUTH, EAST, WEST, DIR_COUNT };
const char *names[DIR_COUNT] = { "north", "south", "east", "west" };
```

Adding a direction means changing the enum and adding a name — the array size automatically tracks `DIR_COUNT`.

### 10.3 Watch for name collisions

```cpp
enum Color { RED, GREEN };

void RED() { }       // ERROR — RED is already declared
```

Plain enum names live in the enclosing scope and can shadow / be shadowed.

### 10.4 Don't compare enums of different types

```cpp
enum A { X };
enum B { Y };

if (X == Y) { }      // compiles — both are 0 — silently true
```

Two unrelated enums comparing equal because their underlying ints match is one of the most common silent C++98 bugs.

### 10.5 Switch on enum — turn on warnings

```cpp
enum State { IDLE, RUNNING, DONE };

void f(State s) {
    switch (s) {
        case IDLE:    handleIdle(); break;
        case RUNNING: handleRun();  break;
        // forgot DONE!
    }
}
```

Compilers can warn (`-Wswitch`) when a switch on an enum doesn't cover every enumerator. Always enable it.

### 10.6 Convert enum to string for debugging

There's no built-in way in C++98. Pattern:

```cpp
const char *toString(LogLevel l) {
    switch (l) {
        case DEBUG: return "DEBUG";
        case INFO:  return "INFO";
        case WARN:  return "WARN";
        case ERR:   return "ERR";
        case NONE:  return "NONE";
    }
    return "??";
}
```

Tedious but explicit. C++ has no reflection.

---

## 11. Common errors

| Error | Cause | Fix |
|---|---|---|
| `cannot convert int to enum X` | Assigning a raw int to an enum | `static_cast<X>(value)` |
| `redeclaration of 'RED'` | Two enums in same scope use same name | Wrap in classes/namespaces, or rename |
| Comparing enums silently equal | Different enum types both 0 | Wrap in classes; enable `-Wsign-compare`/`-Wenum-compare` |
| `enumerator value too large` | Value exceeds underlying type | Add a cast, or specify a wider underlying type (C++11) |

---

## 12. Visual summary

```
              ┌──────────────────────────────────────────────┐
              │   enum Color { RED, GREEN, BLUE };           │
              │                                              │
              │   integer constants with names.              │
              │   names live in the ENCLOSING scope.         │
              │   implicit conversion to int.                │
              │   underlying type implementation-defined.    │
              └────────────────────┬─────────────────────────┘
                                   │
              ┌────────────────────┼─────────────────────────┐
              ▼                    ▼                         ▼
       set of constants       compile-time constant     scoping trick
       ─────────────────      ──────────────────        ──────────────
       enum Day { MON, …};    enum { N = 10 };          class C {
       enum Status {…};       int arr[N];                   enum E { A,B };
                                                          };
                                                          C::E e = C::A;

       C++11 alternative: enum class — strongly typed, scoped,
       no implicit conversion. Use when you have the option.
```

---

## 13. Practice

1. Why does `enum Color {RED, GREEN}; enum Severity {GREEN, YELLOW};` fail to compile? *(Both define `GREEN` in the enclosing scope — name collision.)*
2. Why is `enum E { A };` followed by `int x = A;` legal? *(Implicit enum-to-int conversion.)*
3. What's the underlying type of `enum E { X };`? *(Implementation-defined, but typically `int` on 32/64-bit compilers.)*
4. What's the "enum hack" for compile-time constants? *(Anonymous `enum { N = …};` — yields a constant expression usable for array sizes, etc.)*
