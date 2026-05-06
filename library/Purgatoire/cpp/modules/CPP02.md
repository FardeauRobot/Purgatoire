# C++ Module 02 — Learning Guide

**Module theme:** *Ad-hoc polymorphism, operator overloading, and
the Orthodox Canonical Form (OCF).*

This module is where classes stop being C-structs-with-methods and
start being **values** — objects you can add, compare, stream, and
copy like built-in numbers. The vehicle is a `Fixed`-point number
class. The math is decoration; the real learning is OCF +
operator overloading + const-correctness.

See also: the per-module `TOPICS.md` in [`LEVEL4/C++1/Mod02/`](../../../LEVEL4/C++1/Mod02/TOPICS.md)
for exercise-by-exercise deep dives, and [`FLOATING.md`](../../../LEVEL4/C++1/Mod02/FLOATING.md)
for the floating-point / fixed-point background.

---

## Orthodox Canonical Form — the four members you will write 100 times

Every class in Mod 02+ must provide:

1. **Default constructor** — `Fixed()`
2. **Copy constructor** — `Fixed(Fixed const& src)`
3. **Copy assignment operator** — `Fixed& operator=(Fixed const& other)`
4. **Destructor** — `~Fixed()`

Why all four when the compiler would default them for you? Because
the moulinette grades the **trace prints** you put in each one. The
generated defaults are silent, so you can't reuse them.

Full write-up: [`oop/ORTHODOX_CANONICAL_FORM.md`](../oop/ORTHODOX_CANONICAL_FORM.md).

The canonical idiom in `operator=`:

```cpp
Fixed& Fixed::operator=(Fixed const& other) {
    if (this != &other)       // self-assignment guard
        m_raw = other.m_raw;
    return *this;             // allow chaining: a = b = c;
}
```

For a class owning only primitives (Mod 02's `Fixed`), the guard
is theatre. For a class owning heap memory (Mod 04's `Dog`), it
prevents double-free. Build the habit now.

---

## Exercise 00 — My First Class in OCF

### What you're learning
**How to write a minimal class in full OCF**, with trace prints,
following the project's header/impl split.

### Class shape
- Private: `int m_raw`, `static const int m_fract_bits = 8`.
- Public: the four OCF members + `int getRawBits() const` and
  `void setRawBits(int const raw)`.

### Tips
- Initialise `m_raw` in the ctor's **init list** where possible:
  `Fixed() : m_raw(0) { ... }`. Init-list before body is the C++
  idiom.
- `static const int m_fract_bits = 8;` is *declaration +
  initialisation* in one line inside the header — a special allowance
  for integral `const` in c++98. No out-of-line definition needed.
- `getRawBits()` is `const` — the method promises not to mutate
  `*this`. Every accessor that just reads should be `const`.

### Gotcha
Subject wants every OCF member and `getRawBits` to print a trace.
`setRawBits` typically does not trace. Re-read the sample output.

---

## Exercise 01 — A useful fixed-point class

### What you're learning
**Converting between `int` / `float` and the `Fixed` representation,
and overloading `operator<<`** as a non-member.

### The four conversions, in one place

```cpp
raw_from_int(int n)       →  n << 8
raw_from_float(float f)   →  roundf(f * 256)
to_int(int raw)           →  raw >> 8
to_float(int raw)         →  raw / 256.0f
```

`<< 8` is shift-left-by-8 = multiply by 256. `>> 8` is
arithmetic shift-right on a signed int (sign-extending).
`roundf` is from `<cmath>`.

### `operator<<` as a free function

```cpp
std::ostream& operator<<(std::ostream& os, Fixed const& f) {
    os << f.toFloat();
    return os;
}
```

- **Free (non-member)**, because the left operand is a
  `std::ostream&` — you can't add a member to `ostream`.
- **Returns the stream** so chaining works: `std::cout << a << b`.
- **No `friend` needed** if it goes through public accessors
  (`toFloat()`).

### Tips
- **Order-of-operations trap on the float ctor.** Apply `roundf`
  *before* casting to `int`, not after. `static_cast<int>(roundf(f
  * 256))` is right. `roundf(static_cast<int>(f * 256))` rounds
  the already-truncated value — a no-op.
- **`std::cout` prints floats to 6 significant digits by default.**
  `Fixed(3.14f)` stored as raw `804` prints as `3.14062` — that's
  Q8 resolution, not a bug.
- **Ambiguity:** `Fixed a(42.0)` (a `double` literal) is ambiguous
  under `Fixed(int)` + `Fixed(float)`. The subject's tests use
  `int` and `float` only, so you don't need a `double` overload.

### Gotcha
Include `<cmath>` for `std::roundf`. Not `<math.h>` — C++ convention.

---

## Exercise 02 — "Now we're talking"

### What you're learning
The **full operator set** on a number-like class: comparisons,
arithmetic, increment/decrement, and **static `min` / `max`** with
const and non-const overloads.

See [`oop/OPERATOR_OVERLOADING.md`](../oop/OPERATOR_OVERLOADING.md).

### Comparisons — six operators, six one-liners

```cpp
bool Fixed::operator<(Fixed const& rhs) const {
    return m_raw < rhs.m_raw;
}
```

Both operands share the same scale, so comparing raw ints gives the
same answer as comparing the real values. All six (`<`, `>`, `<=`,
`>=`, `==`, `!=`) are variations on that line.

### Arithmetic — raw math, with a shift for `*` and `/`

| Operator | Raw formula          | Why the shift |
|----------|----------------------|---------------|
| `a + b`  | `a_raw + b_raw`      | scales match |
| `a - b`  | `a_raw - b_raw`      | scales match |
| `a * b`  | `(a_raw * b_raw) >> 8` | raw product is double-scaled, shift right to compensate |
| `a / b`  | `(a_raw << 8) / b_raw` | raw quotient loses the scale, shift left to restore |

`*` and `/` are the interesting ones. Proof is one line of
arithmetic — see [`TOPICS.md` for Mod02 §ex02](../../../LEVEL4/C++1/Mod02/TOPICS.md).

### Increment / decrement — pre vs post

```cpp
Fixed& Fixed::operator++() {          // pre: ++a
    ++m_raw;
    return *this;
}
Fixed Fixed::operator++(int) {        // post: a++
    Fixed tmp(*this);
    ++m_raw;
    return tmp;
}
```

The `int` parameter on the post-form is a **tag** the compiler
uses to pick the overload — it's never read. Pre returns a
reference; post returns a copy of the pre-state.

Step size is the **smallest representable ε** = raw ±1 = `1/256`
in real-value terms.

### Static `min` / `max` — two overloads each

```cpp
static Fixed&       min(Fixed& a, Fixed& b);         // non-const
static Fixed const& min(Fixed const& a, Fixed const& b);  // const
```

Both versions needed — non-const lets the caller modify the
returned reference (`Fixed::min(x, y) = z;`), const version works
on const inputs.

### Tips
- `operator+` returns by **value** (fresh temporary); `operator=`
  returns by **reference** (`*this`). Different for a reason —
  read [`oop/OPERATOR_OVERLOADING.md §5`](../oop/OPERATOR_OVERLOADING.md).
- Declare all comparison operators `const` — they don't mutate
  their operands.
- Don't use `std::min` or `std::max` (no STL). Write the class
  versions.

### Gotcha
`Fixed(256) * Fixed(256)` has raw product `65536 * 65536 = 2^32`
— overflows 32-bit signed. Subject tests stay well inside the
safe range, but know the limit.

---

## Exercise 03 — BSP (bonus)

### What you're learning
**Using `Fixed` for geometry**: determining whether a point lies
*strictly* inside a triangle.

### Class shape
`Point` with private `Fixed const x` and `Fixed const y` —
the `const` on members is the exercise's twist.

### The `const`-members trap
`Fixed const x;` means `x` can't be assigned after construction.
So `operator=` can't do its normal job:

```cpp
Point& Point::operator=(Point const&) {
    // x = other.x;  // ← compile error
    return *this;
}
```

**Accepted escape:** empty-body `operator=` returning `*this`.
The class is effectively immutable post-construction; that's the
design intent.

### Point-in-triangle — sign of cross products

Given triangle `abc` and point `p`:

```
d1 = cross(a, b, p)
d2 = cross(b, c, p)
d3 = cross(c, a, p)

strictly inside ⟺ all three signs match AND none is zero
```

`cross(a, b, p) = (b.x - a.x) * (p.y - a.y) - (b.y - a.y) * (p.x - a.x)`.

Positive = `p` is left of directed edge `a→b`. Negative = right.
Zero = on the line containing the edge.

### Why `Fixed` (not `float`) here
Two reasons:
1. **Determinism** across machines and compilers — integer math
   doesn't drift.
2. **Safe on-edge test.** `d == 0` with fixed-point is exact; with
   floats, roundoff makes it flicker.

### Tips
- `bsp` is a **free function**, not a member. Lives in `bsp.cpp`,
  declared in a header `main` includes.
- Winding order (`abc` clockwise or counter-clockwise) doesn't
  matter — "all three signs match" handles both cases.
- Degenerate triangle (`a, b, c` colinear) → all three crosses are
  zero → returns `false`. Good.

### Gotcha
"Strictly inside" means **vertices and edges return `false`**.
Add the `d_i != 0` checks, not just the sign agreement.

---

## What this module leaves you with

- You can write OCF without thinking about it.
- You know which operators are members and which are free functions.
- `const`-correctness is automatic, not a chore.
- You understand that fixed-point is a simple idea (integer with
  implied decimal place) and a powerful tool (deterministic,
  exact-equal geometry).

Everything from Mod 03 onward **assumes** you've internalised OCF
and operator overloading. If they still feel awkward, loop back
through ex00/01 before starting Mod 03.
