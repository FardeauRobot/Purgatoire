# The Four C++ Casts — Complete Guide

C inherits a single, brutal cast: `(int)x`. It will do *anything* you ask — strip const, reinterpret bits, downcast a pointer — with no way for the reader or the compiler to know *which* of those things you meant.

C++ replaces it with **four explicit casts**, each narrowing what you can do. The rule at 42 is: **never use C-style casts in C++ code.** Always pick the right named cast.

```
C:       (T)value                         // one cast, does everything, tells you nothing
C++:     static_cast<T>(value)            // safe, compile-time conversions
         dynamic_cast<T>(value)           // runtime-checked downcast in polymorphic hierarchies
         const_cast<T>(value)             // add/remove const or volatile
         reinterpret_cast<T>(value)       // bit-for-bit reinterpretation
```

---

## Table of Contents

1. [Why C casts are a problem](#1-why-c-casts-are-a-problem)
2. [`static_cast`](#2-static_cast)
3. [`dynamic_cast`](#3-dynamic_cast)
4. [`const_cast`](#4-const_cast)
5. [`reinterpret_cast`](#5-reinterpret_cast)
6. [Side-by-side comparison](#6-side-by-side-comparison)
7. [Which cast to reach for](#7-which-cast-to-reach-for)
8. [RTTI — `typeid` and `dynamic_cast`](#8-rtti--typeid-and-dynamic_cast)
9. [In CPP06](#9-in-cpp06)
10. [Gotchas](#10-gotchas)

---

## 1. Why C casts are a problem

A C cast silently chains up to three different conversions until something works:

```c
const int *p = ...;
int *q = (int *)p;          // strips const silently
double *d = (double *)q;    // reinterprets bits silently — same syntax
```

The reader sees `(T)x` and can't tell which of those happened. The compiler can't warn, because the cast tells it to shut up. C++ splits those intentions apart so both the compiler and the reviewer can see them.

---

## 2. `static_cast`

**Purpose:** explicit but **safe, compile-time checked** conversions between related types.

```cpp
static_cast<T>(expr)
```

### Use it for:

- Numeric conversions that can lose precision (the compiler would warn otherwise):
  ```cpp
  double d = 3.7;
  int    i = static_cast<int>(d);        // 3 — truncation is explicit
  ```
- Converting between related pointer types in a non-polymorphic hierarchy:
  ```cpp
  Base *b = new Derived();
  Derived *d = static_cast<Derived*>(b); // you promise b really points at a Derived
  ```
- Converting `void*` back to a typed pointer:
  ```cpp
  void *raw = ...;
  int  *p   = static_cast<int*>(raw);
  ```
- Enum ↔ int, explicit constructor calls, user-defined conversions.

### Will NOT compile:

- Adding or removing `const` (use `const_cast`).
- Reinterpreting unrelated pointers like `int*` → `std::string*` (use `reinterpret_cast` — and question your life choices).
- Downcasting **across a non-related hierarchy**: the compiler cannot check the hierarchy for you.

### How the machine sees it

For numeric conversions, it emits a real conversion instruction (`cvttsd2si` for `double→int` on x86). For pointer adjustments within a class hierarchy, it may emit **a compile-time offset addition** (for multiple inheritance). That's all — no runtime check.

---

## 3. `dynamic_cast`

**Purpose:** safely downcast a base pointer/reference to a derived one, **with a runtime check**.

```cpp
dynamic_cast<Derived*>(base_ptr)    // returns NULL if the cast is invalid
dynamic_cast<Derived&>(base_ref)    // throws std::bad_cast if invalid
```

### Requirement

The base class **must be polymorphic** — it must have at least one `virtual` function. Otherwise, there's no vtable, and `dynamic_cast` has nothing to check against.

```cpp
class Animal { public: virtual ~Animal() {} };       // has a vtable — OK
class Dog : public Animal {};
class Cat : public Animal {};

Animal *a = new Dog();
Dog *d = dynamic_cast<Dog*>(a);        // d == original pointer
Cat *c = dynamic_cast<Cat*>(a);        // c == NULL — runtime said "not a Cat"
```

### How the machine sees it

`dynamic_cast` walks the object's **type_info** through the vtable:

```
a
│
├─► vptr ──► vtable ──► type_info "Dog"
│
└─► runtime: "is Dog derived-from-or-equal-to target? yes ⇒ return adjusted pointer"
                                                     "no  ⇒ return NULL"
```

This runtime walk is why `dynamic_cast` is slower than `static_cast`. For hot paths, prefer to redesign rather than downcast.

### Use it for:

- The rare case where you hold a `Base*` and need to do something only meaningful for one `Derived`.
- Library code that *must* check the real type (serializers, visitors).

### Rule of thumb

If you reach for `dynamic_cast`, first ask: "can I add a virtual function to `Base` instead, so the subclass does the right thing itself?" Most of the time, yes. `dynamic_cast` is a last resort.

---

## 4. `const_cast`

**Purpose:** add or remove `const` (or `volatile`) from a type. It's the **only** cast that can do that.

```cpp
const int x = 5;
int *p = const_cast<int*>(&x);       // remove const from the pointer type
*p = 10;                              // UNDEFINED BEHAVIOR if x was originally const
```

### The one legitimate use

Interfacing with legacy or C APIs that take `char*` where `const char*` would be correct:

```cpp
void legacy_print(char *s);          // C API, doesn't modify s but declared non-const

const std::string msg = "hello";
legacy_print(const_cast<char*>(msg.c_str()));    // safe — the API really doesn't modify
```

**Modifying a truly-const object via `const_cast` is Undefined Behavior.** The cast only lets you *express* the type change; it doesn't make the storage writable.

### How the machine sees it

`const_cast` generates **zero instructions**. `const` is a compile-time promise; the cast just erases the promise from the type system.

### Rule of thumb

If you find yourself writing `const_cast`, you almost certainly have a design issue. The right fix is usually to make the function that needs the non-const take a non-const parameter from the start.

---

## 5. `reinterpret_cast`

**Purpose:** reinterpret the **bit pattern** of a value as a different type. No conversion happens; the compiler just agrees to look at the same memory through a different type.

```cpp
int          n = 0x41424344;
const char  *s = reinterpret_cast<const char*>(&n);   // peek at the bytes
// s[0..3] are the bytes of n — endianness-dependent
```

### Use it for (rare):

- Low-level type punning between unrelated pointer types.
- Converting a pointer to an integer (`uintptr_t`) for logging/hashing.
- Hardware or serialization code reading fixed byte layouts.

### What it does NOT do:

- It does **not** change the bits.
- It does **not** add or remove `const` (you'd need `const_cast` too).
- It does **not** check anything — misuse is UB.

### How the machine sees it

`reinterpret_cast` emits **zero instructions**. The returned value is exactly the same bits as the input. You've just told the compiler "treat these bits as type T from now on."

### Rule of thumb

If you're reaching for `reinterpret_cast` in a normal CPP module, stop. You almost certainly want `static_cast` (for numeric) or `dynamic_cast` (for hierarchy). `reinterpret_cast` is a screwdriver for driving nails — technically works, usually wrong.

---

## 6. Side-by-side comparison

| Cast                | Checked?         | Runtime cost   | Changes bits? | Typical use                          |
|---------------------|------------------|----------------|---------------|--------------------------------------|
| `static_cast`       | Compile-time     | Zero (or conv) | Maybe (num.)  | Safe related-type conversions        |
| `dynamic_cast`      | **Runtime**      | vtable walk    | No            | Downcast in polymorphic hierarchy    |
| `const_cast`        | None             | Zero           | No            | Remove `const` on legacy APIs        |
| `reinterpret_cast`  | None             | Zero           | No            | Bit-pattern reinterpretation         |
| C-style `(T)expr`   | None (any chain) | Varies         | Varies        | **Never in C++**                     |

---

## 7. Which cast to reach for

Ask yourself these questions in order:

```
1. Am I just adding/removing const?            → const_cast
2. Am I going from Base* to Derived*
   in a polymorphic hierarchy, and I'm
   not 100% sure it's really a Derived?        → dynamic_cast
3. Is this a normal conversion the compiler
   would accept if I used a function call?     → static_cast
4. Am I reinterpreting raw memory?             → reinterpret_cast
5. Any other situation.                         → you're probably wrong, step back
```

---

## 8. RTTI — `typeid` and `dynamic_cast`

**RTTI** (Run-Time Type Information) is the mechanism that backs `dynamic_cast`. You can query it directly:

```cpp
#include <typeinfo>

Animal *a = new Dog();

if (typeid(*a) == typeid(Dog))
    std::cout << "it's a Dog\n";

std::cout << typeid(*a).name() << '\n';   // implementation-defined mangled name
```

Two rules:
- `typeid(*p)` on a polymorphic object returns the **dynamic** (actual) type.
- `typeid(p)` on a pointer returns the **static** (declared) type — almost never what you want.

RTTI has a cost: it adds a `type_info` object to each class's vtable. Some projects disable RTTI with `-fno-rtti`. At 42, RTTI is assumed on.

---

## 9. In CPP06

The module is structured around the casts:

- **ex00 — Serialization.** Convert a `std::string` to an `int`, then to a `float`, then to a `double`. Decide between `static_cast` (numeric) and simple parsing.
- **ex01 — Serialization round-trip.** Take a pointer, turn it into an unsigned integer, then back. This is the textbook `reinterpret_cast` use:
  ```cpp
  uintptr_t serialize(Data *p)   { return reinterpret_cast<uintptr_t>(p); }
  Data*     deserialize(uintptr_t raw) { return reinterpret_cast<Data*>(raw); }
  ```
- **ex02 — Real type.** Given a `Base*`, figure out which `Derived` it really points to. This is the `dynamic_cast` exercise:
  ```cpp
  if (dynamic_cast<A*>(base)) std::cout << "A\n";
  else if (dynamic_cast<B*>(base)) std::cout << "B\n";
  // or use references + try/catch for std::bad_cast
  ```

The module is a tour — by the end, you've touched every cast for its intended purpose.

---

## 10. Gotchas

- **Never C-cast in C++ code.** It bypasses the distinctions above. The 42 reviewer will catch it.
- **`dynamic_cast` needs a polymorphic base.** No virtual function → compile error. At minimum, add a `virtual` destructor.
- **`dynamic_cast` on references throws**, pointers return NULL. Pick based on whether "failure" is acceptable.
- **Modifying a truly-const object via `const_cast` is UB.** Only cast away const on things that aren't actually const in storage.
- **`reinterpret_cast` between unrelated types and then dereferencing is almost always UB** (strict aliasing). The safe exception: reading bytes as `char*` / `unsigned char*`.
- **`static_cast` does not check class hierarchies at runtime.** `static_cast<Dog*>(someAnimalPtr)` on an actual `Cat` compiles fine and blows up when you use it. Use `dynamic_cast` if you're not sure.
- **Pointer arithmetic after a cast respects the *new* type's size.** `reinterpret_cast<char*>(int_ptr) + 1` advances by 1 byte, not 4.
