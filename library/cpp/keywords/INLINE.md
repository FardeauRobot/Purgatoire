# `inline` — A Hint, A Linker Promise, And A Header-File Lifehack

> **TL;DR.** Originally a *hint* to inline a function call. In modern compilers the inlining hint is mostly ignored — but the **linker effect** (allow the same function to appear in multiple translation units without a "multiple definition" error) is real and load-bearing.

Related: [`STATIC.md`](STATIC.md) · [`MAKEFILE_CPP.md`](../tooling/MAKEFILE_CPP.md) · [`LIBRARIES.md`](../tooling/LIBRARIES.md)

---

## 1. Two purposes wrapped in one keyword

```cpp
inline int square(int x) { return x * x; }
```

This declaration does two things:

1. **Hints** to the compiler: *"please consider inlining calls to this function — substitute the body at the call site instead of generating a `call` instruction."*
2. **Tells the linker**: *"this function may be defined in multiple translation units, but they all have the same body; just pick one."*

Effect (1) is the famous one. Effect (2) is what actually keeps headers usable.

---

## 2. What is inlining (compiler effect)?

Without inlining:

```cpp
int square(int x) { return x * x; }
int main() { return square(7); }
```

```asm
square(int):
    mov eax, edi
    imul eax, edi
    ret
main:
    mov edi, 7
    call square(int)
    ret
```

With inlining (compiler decides to inline):

```asm
main:
    mov eax, 49        ; constant-folded after inlining
    ret
```

The function call disappears entirely. The body is *substituted* at the call site, then the optimizer can do further folding.

```
   without inlining            with inlining
   ────────────────            ─────────────
   main → call square          main { x*x }   ← copied in
                                                 then folded to 49
```

### When does it pay off?

- Tiny functions (one or two operations) — call overhead may dwarf the work.
- Functions where the body unlocks more optimization (constant folding, dead branch removal).
- Hot loops where every cycle counts.

### When does it hurt?

- Large functions copied at many call sites → **code bloat** → instruction cache pressure → can be *slower*.
- Recursive functions that aren't reduced by inlining.

### What modern compilers do

`-O2` compilers (g++, clang) make their **own** inlining decisions based on heuristics (function size, call-site count, profile data). Your `inline` keyword is generally a *suggestion they may ignore*. Conversely, they'll inline non-`inline` functions whenever profitable.

The optimizer can also `__attribute__((always_inline))` (gcc/clang) and `__forceinline` (MSVC) for hard demands — but those are non-standard.

---

## 3. The linker effect — why headers don't break

Here's a problem in C: if you put a non-trivial function in a header,

```c
// utils.h
int square(int x) { return x * x; }
```

Every `.c` file that includes the header gets its own copy of `square` in its `.o`. Linking two of them together fails:

```
multiple definition of `square'
```

In C, the rule is **one definition** across the whole program — the **One Definition Rule (ODR)**.

C++ relaxes this: an `inline` function may have **multiple identical definitions** across translation units. The linker keeps one and discards the rest. This is what makes headers work for non-trivial bodies.

```cpp
// utils.hpp
inline int square(int x) { return x * x; }   // can be included anywhere
```

```
   utils.hpp included in:
   ┌──────────┐  ┌──────────┐  ┌──────────┐
   │   a.cpp  │  │   b.cpp  │  │   c.cpp  │
   └────┬─────┘  └────┬─────┘  └────┬─────┘
        │             │             │
        ▼             ▼             ▼
       a.o           b.o           c.o
       has          has           has
       square()     square()      square()
        │             │             │
        └─────── linker ────────────┘
                   │
                   ▼
              one square()
              in the binary
```

Without `inline`, the linker would refuse.

### Implicit `inline` — member functions defined in the class body

```cpp
// MyClass.hpp
class MyClass {
public:
    int value() const { return _value; }   // body in class body → IMPLICITLY inline
private:
    int _value;
};
```

Any member function whose body sits in the class definition is implicitly `inline`. That's why class definitions in headers don't break the linker even when they include function bodies.

The same applies to:
- Member functions defined inside the class.
- Function templates (templates are implicitly inline-ish for ODR purposes).

### Implicit `inline` — `constexpr` functions (C++11+, not 42 C++98)

In C++11, a `constexpr` function is implicitly inline. Not relevant for the 42 modules but worth knowing.

---

## 4. The 42 norm pattern

Class declaration in `.hpp`, definitions in `.cpp` — this is what 42 norm wants:

```cpp
// MyClass.hpp
class MyClass {
public:
    int value() const;   // declaration only
};
```

```cpp
// MyClass.cpp
int MyClass::value() const { return _value; }   // out-of-line definition
```

Out-of-line definitions are **not** implicitly inline. That's fine — they live in exactly one `.cpp` so the ODR is satisfied.

If you want to inline a small accessor:

```cpp
// MyClass.hpp
class MyClass {
public:
    int value() const;
private:
    int _value;
};

inline int MyClass::value() const { return _value; }   // out-of-class but in the header
```

This works because of the linker effect: the header may be included by many `.cpp`s, and `inline` legalizes the duplication.

---

## 5. Hardware view — the call cost you're trying to skip

A non-inlined function call on x86_64 (System V ABI):

```
caller:                        callee:
   push args  to stack/regs       prologue: push rbp; mov rbp,rsp; sub rsp, N
   call    foo                    body: do work
                                  epilogue: leave; ret
   continue
```

Costs:
- Pushing arguments / setting registers
- `call` instruction (push return address, jump)
- Function prologue (frame setup)
- Function epilogue (frame teardown)
- `ret` (pop return address, jump back)
- Branch predictor + pipeline disruption
- Possible spilling of caller-saved registers

For a function as small as `int square(int x) { return x*x; }`, this overhead can be **larger than the work**. Inlining saves all of it.

```
                  call  cost ≈ 2–10 cycles + register spills
                  body  cost ≈ 1 cycle  (imul)

     before inline:  10+ cycles
     after inline:   1   cycle  (often constant-folded to 0 cycles)
```

---

## 6. When inlining backfires — code bloat

```
   square(7)   square(8)   square(9)   ...   square(99)
   inlined    inlined     inlined            inlined

   if each inlined body is 30 bytes:
   100 × 30 = 3 KB of duplicated code
```

If those call sites are spread across multiple loops, the duplicated bodies push **other** code out of L1 instruction cache. A single shared `call square` would be much smaller.

Compilers know this and back off when bodies are big or call sites many. Your `inline` keyword usually doesn't override their judgment.

---

## 7. Tips & tricks

### 7.1 Don't put `inline` on everything

It's not a free win. The compiler often inlines without you, and over-marking signals "I don't know what I'm doing."

### 7.2 `inline` does NOT mean "fast"

The keyword is about ODR + a hint, not performance. Profile before optimizing.

### 7.3 The header-only library pattern

If you want a single-header library (one `.hpp`, no `.cpp`), every free function must be `inline` (or a template, which is implicitly inline for ODR purposes):

```cpp
// my_header.hpp
inline int helper(int x) { return x * 2; }

template <typename T>
T add(T a, T b) { return a + b; }   // templates: ODR-safe in headers
```

### 7.4 Static + inline — different tools

```cpp
// in a header
static int helper(int x) { return x * 2; }
```

vs

```cpp
inline int helper(int x) { return x * 2; }
```

- `static` → each `.cpp` gets its **own** internal-linkage copy (different addresses for `&helper` in different TUs!).
- `inline` → all `.cpp`s share **one** symbol after linking (same address everywhere).

Almost always `inline` is what you want in a header.

### 7.5 Recursive functions

```cpp
inline int factorial(int n) {
    return n <= 1 ? 1 : n * factorial(n - 1);
}
```

Recursive functions can be marked `inline` (it's a hint), but the compiler can only inline a finite depth. With `-O2 -fno-inline-functions` or just heuristics, it usually emits a real call after one or two unrollings.

### 7.6 Inspect what the compiler did

```bash
$ g++ -O2 -S -o - file.cpp | less
```

Look for `call square` — if it's gone, you got inlined. If it's there, you didn't.

Also useful: [godbolt.org](https://godbolt.org/) lets you see assembly for any snippet under any compiler flag.

### 7.7 `inline` and virtual functions

A `virtual` member function can be marked `inline`. The compiler can only inline virtual calls when it **statically knows** the dynamic type — which is rare. Don't expect inlining of virtual dispatch.

---

## 8. Common errors

| Error | Cause | Fix |
|---|---|---|
| `multiple definition of 'foo'` (linker) | Function with body in a header, not marked `inline` | Add `inline`, or move body to a `.cpp` |
| Slow build / huge binary | Heavy `inline` everywhere | Move bodies to `.cpp` for non-trivial functions |
| Inlining doesn't happen at `-O0` | Optimizer disabled | Use `-O2`; or `__attribute__((always_inline))` for forced inlining |

---

## 9. Visual summary

```
                ┌────────────────────────────────┐
                │     keyword: inline            │
                └─────────────┬──────────────────┘
                              │
              ┌───────────────┴────────────────┐
              ▼                                ▼
        compiler hint                    linker permission
        ─────────────                    ─────────────────
        "consider                        "this function
         substituting                    may have multiple
         the body at                     identical definitions
         call sites"                     across TUs"
              │                                │
        modern compilers                 lets you put non-trivial
        decide on their own.             function bodies in headers.
        the keyword is mostly            REQUIRED for free functions
        decorative for inlining.         in header-only libraries.
              │                                │
              └────────── one keyword ─────────┘

   implicit inline:
     • member functions defined in the class body
     • function templates
     • C++11+ constexpr functions
```

---

## 10. Practice

1. Why does putting a non-template free function with a body in a header fail to link? *(ODR — every including TU defines its own copy. `inline` lets the linker dedupe.)*
2. Are member functions defined inside the class body inline? *(Yes — implicitly.)*
3. Will `inline` make my code faster? *(Maybe — but the optimizer usually decides without you. Profile, don't guess.)*
4. Why are templates "ODR-safe" in headers? *(Templates have implicit linkage rules akin to `inline`; identical instantiations are merged.)*
