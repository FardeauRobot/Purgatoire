# `extern` — "Defined Somewhere Else"

> **TL;DR.** `extern` declares a name that has external linkage but **does not allocate storage** in this translation unit. It promises the linker will find the definition in another `.cpp` (or another library).

Related: [`STATIC.md`](STATIC.md) · [`LIBRARIES.md`](../notions/tooling/LIBRARIES.md) · [`INLINE.md`](INLINE.md)

---

## 1. The problem `extern` solves

Two `.cpp` files want to share a global variable.

```cpp
// counter.cpp
int total = 0;
```

```cpp
// main.cpp
total++;            // ERROR — main.cpp doesn't know `total` exists
```

The compiler compiles each `.cpp` independently. When compiling `main.cpp`, it has never heard of `total`. You need a declaration:

```cpp
// main.cpp
extern int total;   // declaration only — no storage allocated here
total++;            // compiles. Linker resolves to the definition in counter.cpp.
```

This is exactly the same model as functions:

```cpp
extern void log(const char *);   // declaration; defined elsewhere
```

For functions, `extern` is the default — a function declaration without a body has external linkage automatically. For variables, you write `extern` explicitly.

---

## 2. Declaration vs definition

```
┌─────────────────────────────────────────────────────────────────┐
│  declaration                  │  definition                      │
│  "this name exists, here's    │  "allocate storage / generate    │
│   its type"                   │   code for this name"            │
├───────────────────────────────┼──────────────────────────────────┤
│  extern int x;                │  int x;                          │
│  void f();                    │  void f() { … }                  │
│  class C;                     │  class C { … };                  │
└───────────────────────────────┴──────────────────────────────────┘
```

The **One Definition Rule (ODR)** says every name needs **at most one** definition program-wide. You can have many declarations.

Pattern for a shared global:

```
   header  shared.hpp:    extern int total;     ← declaration
   source  shared.cpp:    int total = 0;        ← THE definition
   source  main.cpp:      #include "shared.hpp"
                          total++;
   source  log.cpp:       #include "shared.hpp"
                          log(total);
```

Three TUs see the declaration; only one provides storage.

---

## 3. Linker view

```
   compile phase (each .cpp independent):
   ┌─────────────────────────────────────────────────────────┐
   │ main.cpp     │ compiler sees:                          │
   │              │   extern int total;                      │
   │              │   …uses total…                           │
   │              │ ← emits in main.o:                       │
   │              │     UNDEFINED reference to "total"       │
   ├──────────────┼──────────────────────────────────────────┤
   │ counter.cpp  │ compiler sees:                          │
   │              │   int total = 0;                         │
   │              │ ← emits in counter.o:                    │
   │              │     DEFINED symbol "total" in .data      │
   └─────────────────────────────────────────────────────────┘

   link phase:
   ┌─────────────────────────────────────────────────────────┐
   │ linker resolves UNDEFINED total in main.o               │
   │ to the DEFINED total in counter.o.                      │
   │ Both .o files now point at the same .data slot.         │
   └─────────────────────────────────────────────────────────┘
```

Inspect with `nm`:

```bash
$ nm main.o
                 U total           ← U = undefined; needs linker resolution
$ nm counter.o
000000000000000c D total           ← D = defined data symbol

$ nm program
000000000201020 D total           ← linked binary: one symbol
```

---

## 4. The variants

### 4.1 Plain `extern int x;` — declaration

Tells the compiler the name exists, has type `int`, and external linkage. No storage allocated; the linker must find a definition.

### 4.2 `extern int x = 5;` — definition (rare, usually a mistake)

If you provide an initializer, `extern` is **silently ignored**. This becomes a **definition**. Don't do this in headers — every including TU now defines `x` and the linker explodes.

### 4.3 `extern "C"` — C linkage

Tells the compiler not to **mangle** this name's symbol. Used to interoperate with C libraries:

```cpp
extern "C" {
    void c_function(int);
    int  c_global;
}
```

#### What is name mangling?

C++ supports overloading. Two functions can share a source-level name as long as their signatures differ. The linker, however, sees *symbols*, not signatures. So the compiler **mangles** the source name into a unique symbol that encodes the signature:

```
   void f(int)        →    _Z1fi          (Itanium ABI; varies by compiler)
   void f(double)     →    _Z1fd
   void f(int, int)   →    _Z1fii
```

```
$ nm prog | grep ' f'
0000000000401120 T _Z1fi
0000000000401140 T _Z1fd
```

C does no such thing — `void f(int)` is just `f`. So calling a C function from C++ requires turning off mangling:

```cpp
extern "C" int strcmp(const char *, const char *);   // C symbol: strcmp
```

That's why C headers used in C++ wrap themselves in:

```c
#ifdef __cplusplus
extern "C" {
#endif

void mylib_init(void);
int  mylib_sum(int, int);

#ifdef __cplusplus
}
#endif
```

#### `extern "C"` and overloading

Inside `extern "C"`, **you cannot overload** — there's no signature in the symbol to disambiguate. The compiler will reject:

```cpp
extern "C" {
    void f(int);
    void f(double);   // ERROR — duplicate symbol after un-mangling
}
```

#### `extern "C"` and function pointers

A function pointer's *type* includes its language linkage:

```cpp
extern "C" int qsort_cmp(const void*, const void*);    // C linkage
int (*p)(const void*, const void*) = qsort_cmp;        // technically a type mismatch on
                                                        // strict compilers
```

In practice most compilers accept the conversion, but the standard treats `extern "C"` function types and `extern "C++"` function types as distinct.

---

## 5. `extern` for templates (C++11+ only)

```cpp
extern template class std::vector<int>;   // C++11 — don't instantiate here
```

Tells the compiler to skip instantiating the template in this TU; the definition will be found at link time. Reduces compile time & binary size. **Not available in C++98** — skip for 42.

---

## 6. `extern` and `static` — opposites

```cpp
static int x;        // INTERNAL linkage — local to this TU
extern int y;        // EXTERNAL linkage — visible to other TUs
int       z;         // also external linkage at file scope (but a definition here)
```

| Modifier | Linkage | Storage |
|---|---|---|
| `static` (file scope) | internal | defined here, not visible outside |
| `extern` (with no init) | external | declared here, defined elsewhere |
| `extern` (with init) | external | defined here (initializer wins) |
| neither | external | defined here |

---

## 7. Common patterns

### 7.1 Sharing a global across TUs

```cpp
// app_state.hpp
#ifndef APP_STATE_HPP
# define APP_STATE_HPP

extern bool g_debug;        // declaration

#endif
```

```cpp
// app_state.cpp
#include "app_state.hpp"

bool g_debug = false;       // definition — exactly one
```

### 7.2 Calling C library from C++

```cpp
// my_glue.cpp
extern "C" {
    #include <unistd.h>
    #include <fcntl.h>
}

int x = open("file", O_RDONLY);
```

Most modern C standard headers already wrap themselves in `extern "C"`, so you don't need this for `<cstdio>`, `<cstring>`, etc.

### 7.3 Avoid `extern` for constants — prefer `static const` in a header

```cpp
// avoid:
// header.hpp:  extern const int MAX;
// source.cpp:  const int MAX = 1024;

// prefer:
// header.hpp:
class Config {
public:
    static const int MAX = 1024;   // can be used as a constant expression
};
```

---

## 8. Tips & tricks

### 8.1 An `extern` variable's value is unknown at compile time

```cpp
extern int N;
int arr[N];     // ERROR — N is not a constant expression
```

If you want a constant for array size, use `enum { N = 10 };` or `static const int N = 10;` (in-class).

### 8.2 `extern` doesn't escape the namespace

```cpp
namespace ft {
    extern int counter;   // ft::counter
}

int counter;              // global counter — different symbol
```

### 8.3 Re-declaration is fine

```cpp
extern int x;
extern int x;             // OK — multiple declarations allowed
extern int x;
int x = 0;                // and one definition
```

### 8.4 If you forget to define an `extern`, the linker tells you

```
undefined reference to `total'
```

Different from a compile error — `extern` declarations always compile.

### 8.5 Don't use `extern` to "expose" private state

If you want a global, ask whether it should really be a singleton, or a class member, or just a parameter. `extern` globals are a maintenance hazard; favor encapsulation.

---

## 9. Common errors

| Error | Cause | Fix |
|---|---|---|
| `undefined reference to 'foo'` (linker) | `extern` declaration with no matching definition | Add `int foo;` to one `.cpp`, or link the library that defines it |
| `multiple definition of 'foo'` (linker) | Two `.cpp`s both `int foo = 0;` | Make one `extern int foo;`, keep one definition |
| `'extern' applied to function definition` | `extern void f() { ... }` | Drop `extern` on definitions (it's the default for functions) |
| `cannot convert C function to C++` | Mismatched language linkage on a function pointer | Make both sides agree (`extern "C"` or none) |

---

## 10. Visual summary

```
            ┌──────────────────────────────────────┐
            │           extern keyword             │
            └────────────────┬─────────────────────┘
                             │
       ┌─────────────────────┼─────────────────────┐
       ▼                     ▼                     ▼
  extern T x;          extern "C" {…}         extern template …
  ───────────          ──────────────         ─────────────────
  declaration only,    suppresses C++ name    suppress instantiation
  definition lives     mangling: lets you     in this TU (C++11+,
  in another TU.       call/expose C ABIs.    not in 42 C++98).
                       no overloading
                       allowed inside.
```

---

## 11. Practice

1. Why does `extern int x = 5;` define rather than declare? *(The initializer makes it a definition; `extern` is silently ignored.)*
2. Why must a C function called from C++ be wrapped in `extern "C"`? *(C symbols aren't mangled; without `extern "C"`, the C++ compiler would mangle the call, and the linker would not find the C symbol.)*
3. What's the difference between `static` and `extern` at file scope? *(Internal vs external linkage. Static = TU-private; extern = visible to the linker.)*
4. Can you mark a function `extern "C"` AND overload it? *(No — the un-mangled C symbol can't disambiguate overloads.)*
