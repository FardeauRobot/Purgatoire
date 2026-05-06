# Operator Overloading — Complete Guide

Operator overloading lets you give built-in operators (`+`, `==`, `<<`, `[]`, …) a meaning when applied to your class.

```cpp
Fixed a(3);
Fixed b(4);
Fixed c = a + b;             // a.operator+(b) — because you defined it
std::cout << c;              // operator<<(std::cout, c) — because you defined it
```

Under the hood, `a + b` is *just* a function call: `operator+(a, b)` or `a.operator+(b)`. Everything else follows from that.

---

## Table of Contents

1. [Why overload operators?](#1-why-overload-operators)
2. [The list of overloadable operators](#2-the-list-of-overloadable-operators)
3. [Member vs non-member](#3-member-vs-non-member)
4. [Canonical signatures](#4-canonical-signatures)
5. [Arithmetic operators](#5-arithmetic-operators)
6. [Compound assignment (`+=`, `-=`, …)](#6-compound-assignment--)
7. [Comparison operators](#7-comparison-operators)
8. [Increment and decrement (prefix vs postfix)](#8-increment-and-decrement-prefix-vs-postfix)
9. [Stream insertion and extraction (`<<`, `>>`)](#9-stream-insertion-and-extraction--)
10. [Subscript `[]`, function call `()`](#10-subscript--function-call-)
11. [Assignment operator `=`](#11-assignment-operator-)
12. [Conversion operators](#12-conversion-operators)
13. [In CPP02 — the Fixed class](#13-in-cpp02--the-fixed-class)
14. [Rules of thumb](#14-rules-of-thumb)
15. [Gotchas](#15-gotchas)

---

## 1. Why overload operators?

Because arithmetic-like, comparable, or printable user types **read more naturally** with operators than with named methods:

```cpp
// Without overloading
result = sum(mul(a, b), c);

// With overloading
result = a * b + c;
```

The compiler translates the second into the first. Same machine code, different clarity.

**Don't overload for its own sake.** Only when the operator has an obvious, universally-accepted meaning for your type. `+` on `Fixed` is good. `+` on `Car` to mean "add a feature" will make everyone miserable.

---

## 2. The list of overloadable operators

You can overload almost all operators:

```
Arithmetic:   +   -   *   /   %
Comparison:   ==  !=  <   >   <=  >=
Logical:      !   &&  ||
Bitwise:      &   |   ^   ~   <<  >>
Compound:     +=  -=  *=  /=  %=  &=  |=  ^=  <<= >>=
Increment:    ++  --
Subscript:    []
Call:         ()
Member:       ->  ->*
Assignment:   =
Address:      &                (rarely a good idea to touch)
Stream:       <<  >>           (shift, reused for I/O)
Allocation:   new  delete  new[]  delete[]
Comma:        ,                 (rarely a good idea either)
```

**Cannot** be overloaded:
```
::   .   .*   ?:   sizeof   typeid
```
And you cannot invent new operators.

---

## 3. Member vs non-member

Most binary operators can be written either way. The **left operand**'s type decides.

### Member function

The left operand is `*this`:

```cpp
class Fixed {
public:
    Fixed operator+(const Fixed& rhs) const;   // a + b  →  a.operator+(b)
};
```

### Non-member (free) function

Both operands are explicit parameters:

```cpp
Fixed operator+(const Fixed& lhs, const Fixed& rhs);   // a + b  →  operator+(a, b)
```

### When you **must** use non-member

When the left operand isn't your class:

```cpp
std::cout << f;    // left operand is std::ostream — NOT your class
                   // → must be a non-member
```

So `operator<<` and `operator>>` are always free functions.

### Access to private members

A non-member `operator+` can't see private fields directly. Two fixes:
- Build it on top of public methods (`toRaw()`, etc.)
- Declare it `friend`:
  ```cpp
  class Fixed {
      int _raw;
      friend Fixed operator+(const Fixed& a, const Fixed& b);
  };
  ```

---

## 4. Canonical signatures

Memorize these once; you'll write them a hundred times.

```cpp
// Binary arithmetic — returns new value, both sides const
T   operator+(const T& lhs, const T& rhs);

// Compound assignment — modifies *this, returns *this by reference
T&  T::operator+=(const T& rhs);

// Comparison — always bool, both sides const
bool operator==(const T& lhs, const T& rhs);

// Stream insertion — non-member, returns the stream by reference
std::ostream& operator<<(std::ostream& os, const T& v);

// Stream extraction — reads into a non-const T
std::istream& operator>>(std::istream& is, T& v);

// Assignment — returns *this by reference, takes const T&
T& T::operator=(const T& rhs);

// Prefix ++/--
T& T::operator++();                     // ++a
// Postfix ++/--
T  T::operator++(int);                  // a++ (dummy int disambiguates)

// Subscript — usually two overloads: const and non-const
T& T::operator[](size_t i);
const T& T::operator[](size_t i) const;
```

These return types matter — they let chained expressions work: `a = b = c`, `std::cout << a << b`.

---

## 5. Arithmetic operators

Canonical pattern: **define `+=` as a member, then implement `+` in terms of it as a non-member.** No duplicated logic.

```cpp
class Fixed {
    int _raw;
public:
    Fixed& operator+=(const Fixed& rhs) { _raw += rhs._raw; return *this; }
    int    toRaw() const { return _raw; }
};

Fixed operator+(Fixed lhs, const Fixed& rhs) {   // lhs by value — copy we'll modify
    lhs += rhs;
    return lhs;
}
```

### Why `+` returns by value, not by reference

`a + b` creates a *new* object. Returning a reference would be a reference to a local → dangling.

### Why `+=` returns `*this` by reference

So you can chain: `(a += b) += c;`. Also standard C++ convention — mirrors built-in types.

---

## 6. Compound assignment (`+=`, `-=`, …)

These are the natural members. Implement them first:

```cpp
Fixed& Fixed::operator+=(const Fixed& rhs) { _raw += rhs._raw;  return *this; }
Fixed& Fixed::operator-=(const Fixed& rhs) { _raw -= rhs._raw;  return *this; }
Fixed& Fixed::operator*=(const Fixed& rhs) { _raw = (_raw * rhs._raw) >> SCALE; return *this; }
Fixed& Fixed::operator/=(const Fixed& rhs) { _raw = (_raw << SCALE) / rhs._raw; return *this; }
```

Then derive the binary forms:

```cpp
Fixed operator+(Fixed lhs, const Fixed& rhs) { return lhs += rhs; }
Fixed operator-(Fixed lhs, const Fixed& rhs) { return lhs -= rhs; }
// etc.
```

---

## 7. Comparison operators

All return `bool`, all take `const T&` on both sides. For consistency, define them as a **coherent set**. Usually you write `==` and `<` from scratch, then derive the rest:

```cpp
bool operator==(const Fixed& a, const Fixed& b) { return a.toRaw() == b.toRaw(); }
bool operator< (const Fixed& a, const Fixed& b) { return a.toRaw() <  b.toRaw(); }

bool operator!=(const Fixed& a, const Fixed& b) { return !(a == b); }
bool operator> (const Fixed& a, const Fixed& b) { return   b <  a;  }
bool operator<=(const Fixed& a, const Fixed& b) { return !(b <  a); }
bool operator>=(const Fixed& a, const Fixed& b) { return !(a <  b); }
```

This is the C++98 idiom. (C++20 adds `operator<=>` to collapse this, but you don't have it.)

---

## 8. Increment and decrement (prefix vs postfix)

Prefix (`++a`) and postfix (`a++`) need distinct signatures. The dummy `int` parameter disambiguates:

```cpp
class Fixed {
public:
    Fixed& operator++()    { ++_raw; return *this; }        // prefix
    Fixed  operator++(int) { Fixed tmp(*this); ++_raw; return tmp; }  // postfix
};
```

- **Prefix** returns the object by reference, after modification.
- **Postfix** saves the old value, modifies the object, returns the old value by **value**.

**Performance tip:** prefix is faster — it returns a reference, postfix copies. Prefer `++it` over `it++` in loops.

---

## 9. Stream insertion and extraction (`<<`, `>>`)

Always non-member. Always return the stream by reference (so chaining works).

```cpp
std::ostream& operator<<(std::ostream& os, const Fixed& f) {
    os << f.toFloat();
    return os;
}

std::istream& operator>>(std::istream& is, Fixed& f) {
    float x;
    is >> x;
    f = Fixed(x);
    return is;
}

std::cout << a << b << '\n';   // works because each call returns std::cout&
```

If the operator needs private access, declare it `friend`.

---

## 10. Subscript `[]`, function call `()`

### Subscript

Usually a pair — one `const`, one not:

```cpp
class Array {
    int* _data;
public:
    int&       operator[](size_t i)       { return _data[i]; }
    const int& operator[](size_t i) const { return _data[i]; }
};

Array       a(10);   a[0] = 42;
const Array b(10);   int v = b[0];   // calls the const overload
```

### Function call — functors

Overloading `operator()` makes an object **callable**:

```cpp
struct Adder {
    int base;
    Adder(int n) : base(n) {}
    int operator()(int x) const { return base + x; }
};

Adder add5(5);
add5(10);    // 15 — calls Adder::operator()
```

This is how the STL takes "functions" before lambdas existed: `std::sort(..., MyComparator());`.

---

## 11. Assignment operator `=`

Part of the OCF. See [ORTHODOX_CANONICAL_FORM.md](ORTHODOX_CANONICAL_FORM.md). The skeleton:

```cpp
Fixed& Fixed::operator=(const Fixed& rhs) {
    if (this != &rhs) {        // self-assignment check
        _raw = rhs._raw;
    }
    return *this;
}
```

Key points:
- **Takes `const T&`**, because you don't modify the source.
- **Returns `T&` to `*this`**, so `a = b = c` works.
- **Self-assignment check** (`this != &rhs`) prevents bugs when copying heap-owned resources.

---

## 12. Conversion operators

Let your class convert *to* another type:

```cpp
class Fixed {
public:
    operator float() const { return _raw / (float)(1 << SCALE); }
    operator int()   const { return _raw >> SCALE; }
};

Fixed f(3);
float x = f;             // implicit: calls operator float()
int   i = static_cast<int>(f);
```

Note: conversion operators have **no return type** syntactically — the name *is* the target type.

**Careful:** implicit conversion operators are the mirror image of implicit constructors. If you don't want silent conversions, at 42 you sometimes have to mark them (C++11 adds `explicit` for conversion operators; C++98 has no such feature — which is why they can surprise you).

---

## 13. In CPP02 — the Fixed class

The whole module is a study in operator overloading.

**ex02** — you implement, on a fixed-point class:
- `operator+`, `-`, `*`, `/` (arithmetic)
- `operator==`, `!=`, `<`, `>`, `<=`, `>=` (all six comparisons)
- `operator++`, `--` (both prefix and postfix)
- `operator<<` (printing)
- Two static `min`/`max` overloads

**ex03** — the `Fixed` class becomes a building block for `Point` and triangle tests. The goal is that all operators feel natural: `(a + b) / 2`, `if (p1 == p2)`, `std::cout << f`.

The right order to build them:
1. OCF (default ctor, copy ctor, `operator=`, dtor).
2. `toFloat()` / `toInt()` / `toRaw()` — the accessors.
3. `operator<<` — so you can `std::cout` your class from day one (makes debugging infinitely easier).
4. `operator==`, `<` — the base of all comparisons.
5. The other four comparisons in terms of `==` and `<`.
6. `operator+=`, `-=`, `*=`, `/=` — then derive `+`, `-`, `*`, `/`.
7. `operator++`, `--` — both forms.

---

## 14. Rules of thumb

- **Mirror built-in semantics.** `+` should not have side effects. `==` should be reflexive and symmetric. `<` should be a strict weak ordering.
- **`+=` before `+`.** Write the compound one first, derive the binary one.
- **Return types matter.** Binary arith: by value. Compound assign: by reference to `*this`. Comparisons: `bool`. Streams: the stream by reference.
- **Const what can be const.** A comparison doesn't modify anything; every parameter and `*this` should be `const`.
- **Non-member for anything where the left operand might not be your class** (streams, number-to-class arithmetic).
- **Friend only when necessary.** If a free-function operator can be written with public methods, don't make it a friend.

---

## 15. Gotchas

- **Mixing member and non-member overloads of the same operator** creates ambiguity. Pick one form.
- **`operator=` that forgets the self-assignment check** breaks in weird ways when you write `obj = obj` (or when `obj` aliases through another pointer).
- **`operator<<` that takes a non-const `T&`** can't print temporaries. Always `const T&`.
- **Postfix `++`/`--` that returns a reference** is UB — you'd be returning a reference to a local variable.
- **Overloading `&&` or `||`** loses short-circuit evaluation; the overloaded versions become regular function calls, evaluating both sides. Don't do it.
- **`operator,`** looks harmless and is a bottomless trap. Don't overload it.
- **Conversion operators + ctor** can create ambiguity: `Fixed(int)` and `operator int() const` means `a + 1` has two valid interpretations. Mark constructors `explicit` when they're not meant to convert silently.
- **`operator[]` returning by value** makes `a[0] = 42` silently fail (you'd be assigning to a temporary). Always return by reference.

---

## 16. Hardware view — what overloaded operators compile to

### `operator+` on a primitive-like class

```cpp
Fixed Fixed::operator+(Fixed const& rhs) const {
    Fixed r;
    r.setRawBits(m_raw + rhs.m_raw);
    return r;
}
```

Compiles (with RVO) to roughly:

```asm
Fixed::operator+(Fixed const&):
    mov  eax, [rdi]           ; this->raw
    add  eax, [rsi]           ; + rhs.raw
    mov  [rdx], eax           ; store into the caller's return slot
    ret
```

`rdx` here is the "return-by-value" slot the caller pre-allocated.
No temporary `Fixed` object gets constructed and then copied —
RVO builds it in place. That's why the copy ctor's trace print
**doesn't fire** when you write `Fixed c = a + b;`.

### `operator*` with the Q8 shift

```cpp
Fixed Fixed::operator*(Fixed const& rhs) const {
    Fixed r;
    r.setRawBits((m_raw * rhs.m_raw) >> 8);
    return r;
}
```

```asm
Fixed::operator*(Fixed const&):
    mov  eax, [rdi]
    imul eax, [rsi]           ; signed multiply
    sar  eax, 8               ; arithmetic shift right
    mov  [rdx], eax
    ret
```

One `imul`, one `sar`. On modern x86, `imul` is 3 cycles, `sar` is
1 cycle. The full fixed-point multiply is ~5 cycles. Compare to a
floating-point multiply (`mulss`): also ~5 cycles, but with
20-cycle stalls on denormals/NaN handling. Fixed-point wins when
you're operating on integer pipelines.

### `operator<<` for stream insertion

```cpp
std::ostream& operator<<(std::ostream& os, Fixed const& f) {
    os << f.toFloat();
    return os;
}
```

Compiles to a call to `std::ostream::operator<<(float)`, preceded
by the `Fixed::toFloat()` body (typically inlined). The stream
operator is **not** virtual — resolution happens at compile time
based on the `float` argument type. That's why stream extensibility
"just works": add a new `operator<<` overload for your type, the
compiler finds it via argument-dependent lookup.

### Pre-increment vs post-increment — the cost difference

```cpp
Fixed& operator++();      // pre
Fixed  operator++(int);   // post
```

Pre-increment: **one store** (`++m_raw`), return `*this`. One load
(the ref return) at the call site.

Post-increment: **one copy ctor** (the pre-snapshot), **one store**
(`++m_raw`), **one destructor** on the temporary after the caller
reads it. Three operations vs one.

For a `Fixed` (one `int`), the copy is free and the difference
vanishes. For a class with a `std::string` member, the copy is an
allocation. **Rule: prefer `++i` over `i++` when you don't need the
old value.** Pre-increment loops are a tiny, free win on any
object that allocates in its copy ctor.

### Comparison operators — the fast path

```cpp
bool operator<(Fixed const& rhs) const { return m_raw < rhs.m_raw; }
```

```asm
Fixed::operator<(Fixed const&):
    mov  eax, [rdi]
    cmp  eax, [rsi]
    setl al
    ret
```

Three instructions. Branch-free. Modern CPUs execute this in 1
cycle. Comparisons on `Fixed` are as fast as on raw `int`.

Comparing `float`s requires `ucomiss` + a branch on the status
flags, plus NaN handling — slower and full of corner cases.
Another reason fixed-point wins for hot comparison code.
