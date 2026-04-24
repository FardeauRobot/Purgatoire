# C++ Module 02 — TOPICS

## Theme
Ad-hoc polymorphism, operator overloading, and the **Orthodox Canonical
Form** (OCF). From this module on, every class must provide the four OCF
members:

1. Default constructor
2. Copy constructor
3. Copy assignment operator
4. Destructor

Class code is split into a `ClassName.hpp` (declarations only) and a
`ClassName.cpp` (definitions). See [FLOATING.md](FLOATING.md) for the
floating-point background that motivates the whole fixed-point build-up.

## Global rules
Same as Mod 00/01 — c++98, `-Wall -Wextra -Werror`, no STL, include
guards, no bodies in headers (templates aside), no leaks.

---

## Cross-cutting concepts (read once, apply everywhere)

### Orthodox Canonical Form — why four, and what they do

A class that owns state needs a well-defined answer to four questions:

| Question                          | Answered by       |
|-----------------------------------|-------------------|
| "How do I build a fresh one?"     | Default ctor      |
| "How do I build one from another?"| Copy ctor         |
| "How do I overwrite one?"         | Copy assignment   |
| "How do I tear one down?"         | Destructor        |

If you don't write them, the compiler generates defaults that do
member-wise copies. For `Fixed` (one `int`) those defaults would
actually work. OCF exists because the *moulinette checks the traces* —
the grader wants to see `"Default constructor called"` printed in the
right order. Member-wise defaults are silent, so you must write the
four explicitly.

**Order of member initialisation.** In C++, members are initialised in
the order they are **declared in the class**, not the order they appear
in the init-list. For a single-member class it doesn't matter; it will
matter in ex03 (`Point` has `x` then `y`).

### The header/implementation split

- `.hpp` — declarations only. No function bodies. (This is a hard rule
  in your project; see the memory pin.)
- `.cpp` — definitions, with the class name scope: `Fixed::Fixed()`,
  `Fixed::getRawBits()`, etc.
- Include guards (`#ifndef FIXED_HPP` / `#define` / `#endif`) prevent
  the header being pasted twice when two TUs include each other.
- `static const int` members **declared** in the header can be
  initialised inline for integral types — that's what lets
  `m_fract_bits = 8` sit in the `.hpp` without a matching definition
  in `.cpp` (c++98 integral-constant-expression rule).

### `const`-correctness drill

Every exercise tightens the screws on `const`:

- `int getRawBits() const` — the `const` after the parens is the
  **method-level const**: this method promises not to mutate `*this`.
  It's what lets you call `getRawBits()` on a `const Fixed&`.
- `Fixed(const int n)` — the `const` on a by-value parameter is
  cosmetic (the copy is the caller's, yours to mutate) but the subject
  asks for it, so keep it.
- `Fixed& operator=(Fixed const& other)` — the argument is a reference
  to an unmodifiable source. You read from it, you don't write to it.
- `Fixed const& min(Fixed const&, Fixed const&)` (ex02) — return type
  *and* both parameters are const-refs: zero-copy, read-only.

The mental rule: if the operation is "look, don't touch", everything
in sight should be `const`. The compiler is an ally here — forgetting
one `const` and getting a cascade of errors is how you learn where
they belong.

### Operator overloading 101

Two flavours you'll use:

- **Member operator** — `Fixed& operator=(Fixed const&)`. The left
  operand is `*this`. Required for `=`, natural for `++`/`--`.
- **Free (non-member) operator** — `std::ostream& operator<<(
  std::ostream&, Fixed const&)`. The left operand is a `std::ostream`,
  which isn't your class, so you can't add a member to it; write a
  free function in the header/`.cpp` pair.

Free operators only need `friend` if they must access **private**
members. `operator<<` for `Fixed` can go through the public
`toFloat()`, so **no `friend` is needed** — keep the header clean.

---

## Exercise 00 — My First Class in OCF

### Goal
Produce the simplest stateful class possible, wired up in full OCF,
with trace prints the subject can grep.

### Spec recap
- **Private:** `int m_raw`, `static const int m_fract_bits = 8`.
- **Public:** OCF × 4, plus `int getRawBits() const` and
  `void setRawBits(int const raw)`.
- Every member prints its canonical trace line on entry.

### What's actually happening in the code

```cpp
Fixed::Fixed() {
    std::cout << "Default constructor called" << std::endl;
    m_raw = 0;
}
Fixed::Fixed(Fixed const& src) {
    std::cout << "Copy constructor called" << std::endl;
    *this = src;                 // delegates to operator=
}
Fixed& Fixed::operator=(Fixed const& other) {
    std::cout << "Copy assignment operator called" << std::endl;
    if (this != &other)          // self-assignment guard
        m_raw = other.getRawBits();
    return *this;
}
Fixed::~Fixed() {
    std::cout << "Destructor called" << std::endl;
}
```

The copy constructor delegating to `operator=` is a common 42 idiom —
keeps the assignment logic in one place. The self-assignment guard
(`this != &other`) is theatre for a single `int`, but becomes
load-bearing once a class owns heap memory (Mod 03+); build the habit
now.

### Hardware-level notes

- A `Fixed` is **4 bytes** in memory — exactly one `int`. The
  `static const` lives in `.rodata`, not in each instance. Verify
  with `sizeof(Fixed)`.
- `Fixed a;` puts those 4 bytes on the stack. No heap, no
  allocation syscall, no cache pressure beyond the current stack page.
- The copy ctor / assignment are one MOV instruction on any
  reasonable optimiser setting once the `std::cout` is stripped.

### Tips & gotchas

- `getRawBits()` printing a trace is **ugly but mandatory** — the
  subject wants it, moulinette diffs against it.
- Don't initialise `m_raw` in the header (`int m_raw = 0;` is c++11).
  Assign it in the ctor body (or use a member init list:
  `Fixed() : m_raw(0) { ... }`).
- The `static const int m_fract_bits = 8;` inline initialisation
  **only works for integral const** in c++98. `static const float`
  would need an out-of-line definition in `.cpp`.
- Filling `.hpp` with any function body (even a one-liner) breaks
  the project's "no bodies in headers" rule.

### Search terms if you get stuck

`"Rule of Three C++"` — same idea as OCF, the industry name.
`"self-assignment safe C++ operator="` — for the `this != &other`
pattern.

---

## Exercise 01 — A useful fixed-point class

### Goal
Make `Fixed` exchange values with the real world: build from `int`
and `float`, convert back, and stream out.

### Spec recap
- `Fixed(const int)` — store `n << 8`.
- `Fixed(const float)` — store `roundf(f * 256)`.
- `float toFloat() const;` — return `m_raw / 256.0f`.
- `int toInt() const;` — return `m_raw >> 8`.
- Non-member `operator<<(std::ostream&, Fixed const&)` prints the
  float form.

### What's actually happening in the code

```cpp
Fixed::Fixed(int const n) : m_raw(n << m_fract_bits) {
    std::cout << "Int constructor called" << std::endl;
}

Fixed::Fixed(float const f)
    : m_raw(static_cast<int>(std::roundf(f * (1 << m_fract_bits)))) {
    std::cout << "Float constructor called" << std::endl;
}

float Fixed::toFloat() const {
    return static_cast<float>(m_raw) / (1 << m_fract_bits);
}
int Fixed::toInt() const {
    return m_raw >> m_fract_bits;
}

std::ostream& operator<<(std::ostream& os, Fixed const& f) {
    os << f.toFloat();
    return os;
}
```

The four conversions are the same ones tabulated in
[FLOATING.md §5](FLOATING.md#the-four-conversions-in-one-place). Every
single method in this exercise is one of those four lines, wrapped in
class scope and a trace (the two constructors trace, the conversion
methods don't — the subject only wants traces on ctors/dtor/assign).

### Hardware-level notes

- `n << 8` compiles to a **single shift instruction** (`shl` on x86,
  `lsl` on ARM). No multiplication unit involved.
- `m_raw >> 8` is the **arithmetic shift right** (`sar`/`asr`) on
  signed ints — sign-extends, so `-256 >> 8 == -1`, not `0x00FFFFFF`.
  That's what you want.
- `f * 256.0f` on modern x86 runs on the **SSE unit** (`mulss`). On
  embedded without an FPU, the compiler emits a soft-float call, which
  is exactly what fixed-point arithmetic is supposed to let you
  *avoid* in the first place — but for building `Fixed`, you have to
  cross the float/int bridge at least once.
- `std::roundf` is **round-half-away-from-zero**: `roundf(0.5) == 1`,
  `roundf(-0.5) == -1`. It differs from the hardware default
  round-to-nearest-even. For the exercise, round-half-away is what
  the subject expects.
- `static_cast<int>` on a float **truncates toward zero** — so the
  `roundf` has to happen *before* the cast. Order matters:
  `static_cast<int>(std::roundf(f * 256))`, not
  `std::roundf(static_cast<int>(f * 256))`.

### Tips & gotchas

- **Include `<cmath>`** for `std::roundf`. No `<math.h>` — this is
  C++, and moulinette is pedantic about it.
- `operator<<` is a **free function**, not a member. Declare it at the
  bottom of `Fixed.hpp` outside the class, define it in `Fixed.cpp`.
  No `friend` needed — go through `toFloat()`.
- Watch the `double` ambiguity: `Fixed a(42.0)` is ambiguous because
  `double → int` and `double → float` are both user-level conversions.
  Tests use `42` and `42.0f`, so you don't need a `double` overload,
  but know why the compiler complains when you slip.
- `std::cout << f` will call your `operator<<` — which calls
  `toFloat()`, which returns `m_raw / 256.0f`. The default `ostream`
  precision for float is **6 significant digits**. `3.14` prints as
  `3.14062` — that's your 8-bit resolution floor (`1/256 ≈ 0.0039`),
  not a bug.
- `Fixed a(-0.5f)` → `roundf(-0.5 * 256) = roundf(-128.0) = -128`.
  Good. But `Fixed a(-0.001f)` → `roundf(-0.256) = 0` — the
  resolution floor eats tiny values. This isn't a bug; it's the
  *definition* of Q8 resolution.

### The scale factor `1 << m_fract_bits` vs hard-coded `256`

Both compile to the same constant. Prefer the shift form for two
reasons: (1) it telegraphs that `m_fract_bits` is the knob, and (2) if
ex02 or your own tinkering changes `m_fract_bits` to 16, you don't
have to grep for magic `256`s.

### Search terms if you get stuck

`"Q notation"`, `"Q23.8 format"`, `"fixed point multiplication shift"`,
`"roundf vs truncation"`, `"C++ operator<< non-member"`.

---

## Exercise 02 — "Now we're talking"

### Goal
Make `Fixed` behave like a number. Full operator set, including
pre/post increment and `static min/max`.

### Spec recap
- **Comparisons:** `>`, `<`, `>=`, `<=`, `==`, `!=` — return `bool`.
- **Arithmetic:** `+`, `-`, `*`, `/` — return `Fixed`. Division by
  zero may crash (the subject explicitly allows it).
- **Increment/decrement:** pre and post, both `++` and `--`. Step is
  **ε = raw ±1**, i.e. `1 / 256` in real-value terms.
- **Static `min/max`:** both a non-const-ref and a const-ref overload,
  returning a reference to the smaller/greater argument.

### What's actually happening in the code

#### Comparisons — compare raw ints, not floats

```cpp
bool Fixed::operator<(Fixed const& rhs) const {
    return m_raw < rhs.m_raw;
}
```

Two `Fixed` values share the same scale, so comparing raw ints
gives the same answer as comparing the real values — and it's
exact, unlike float comparison. All six comparison operators are
one-liners over `m_raw`.

#### Addition / subtraction — raw ± raw

```cpp
Fixed Fixed::operator+(Fixed const& rhs) const {
    Fixed result;
    result.setRawBits(m_raw + rhs.m_raw);
    return result;
}
```

`a + b` at the real-value level is `(a_raw + b_raw) / 256`, which
means the raw result is exactly `a_raw + b_raw`. One machine `ADD`.

#### Multiplication — the shift you can't skip

```cpp
Fixed Fixed::operator*(Fixed const& rhs) const {
    Fixed result;
    result.setRawBits((m_raw * rhs.m_raw) >> m_fract_bits);
    return result;
}
```

Why the `>> 8`: at the real-value level, `a * b = (a_raw/256) *
(b_raw/256) = (a_raw * b_raw) / 65536`. To store that as a Q8 raw
value, you need to multiply by 256, giving `(a_raw * b_raw) / 256`
— which is `(a_raw * b_raw) >> 8`. The raw product is double-scaled;
the shift brings it back to single-scale.

#### Division — the shift you can't skip, other direction

```cpp
Fixed Fixed::operator/(Fixed const& rhs) const {
    Fixed result;
    result.setRawBits((m_raw << m_fract_bits) / rhs.m_raw);
    return result;
}
```

`a / b = (a_raw/256) / (b_raw/256) = a_raw / b_raw` — but that's
the *integer* quotient, scale collapses to zero. You need the raw
result scaled by 256, so pre-shift the dividend:
`(a_raw << 8) / b_raw`.

#### Pre vs post increment

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

The `int` parameter in the post-form is a **tag** — it has no value,
it only exists so the compiler can distinguish the two signatures.
Pre-form returns a **reference** (no copy); post-form returns a
**value** (the pre-increment snapshot).

#### `static min/max`

```cpp
Fixed& Fixed::min(Fixed& a, Fixed& b) {
    return (a < b) ? a : b;
}
Fixed const& Fixed::min(Fixed const& a, Fixed const& b) {
    return (a < b) ? a : b;
}
```

Same pattern for `max`. The two overloads exist so
`Fixed::min(x, y) = z` works on non-const operands (returns a
modifiable reference) while `const`-only callers still compile.

### Hardware-level notes

- `m_raw + rhs.m_raw` is one **`ADD`** instruction. Same for `-`.
- `m_raw * rhs.m_raw` on a 32-bit `int` produces a **32-bit
  truncated** result — the upper bits of the 64-bit full product
  are discarded. On x86 you can get the full result via `IMUL` with
  a 64-bit destination; plain `int * int` does not.
- That truncation is the **overflow problem**: `Fixed(256) *
  Fixed(256)` has raw `65536 * 65536 = 2^32`, which wraps to 0.
  In Q8 with 32-bit ints, anything whose raw product exceeds
  `2^31` silently overflows. The subject accepts this — moulinette
  tests stay well within range — but know the limit.
- `(a_raw << 8) / rhs` in division can overflow for the same
  reason: `a_raw` up to `2^23` (the biggest valid Q23.8 value) times
  `2^8` is exactly the 32-bit signed max. Anything larger
  pre-shift-overflows.
- Integer division on x86 is `IDIV` — ~20–80 cycles, far slower
  than `IMUL` (~3–5). Fixed-point division is expensive; this is
  why real DSP code uses reciprocals.
- Integer `>>` on signed values is implementation-defined in
  c++98 (arithmetic on gcc/clang, but the standard doesn't
  require it). For 42, gcc's behaviour — sign-extending arithmetic
  shift — is fine.

### Tips & gotchas

- `operator=` returns a **reference** (`Fixed&`). `operator+`
  returns a **value** (`Fixed`). The reason: `a = b = c` chains
  through existing objects; `a + b + c` produces fresh temporaries
  at each step.
- Comparisons are declared `const` member functions. You're not
  modifying either operand; the compiler should let you compare two
  `const Fixed&`.
- `setRawBits` is how `operator+` writes to a freshly default-constructed
  `Fixed`. Alternative: add a private `Fixed(int raw, bool)` ctor
  that takes a raw. The subject's design favours `setRawBits`.
- **Don't confuse `Fixed::min` with `std::min`** — you can't use
  `std::min` (no STL), and anyway the exercise demands the class-
  scope version.
- Post-increment by value, pre-increment by reference — this is
  the C++ convention for a reason: pre-form can return `*this`
  for free; post-form has to snapshot. Don't "optimise" by making
  post-form return `*this` — it breaks semantics.
- The four static `min/max` overloads are a drill on **overload
  resolution on const-ness**. Write both versions even if the
  non-const one feels redundant; moulinette checks both signatures.

### Search terms if you get stuck

`"fixed point multiplication shift"`, `"Q format multiply"`,
`"post-increment int parameter C++"`, `"operator overloading member
vs non-member"`, `"const overload resolution C++"`.

---

## Exercise 03 — BSP (bonus)

### Goal
Use `Fixed` to do actual geometry: decide whether a point is
strictly inside a triangle.

### Spec recap
- `Point` class in OCF.
  - Private: `Fixed const x`, `Fixed const y` (note `const`!).
  - Default ctor → `(0, 0)`.
  - Ctor from two `const float` — converts each to `Fixed`.
  - Full OCF.
- Free function
  `bool bsp(Point const a, Point const b, Point const c, Point const point);`
  returns `true` **iff** `point` is *strictly* inside triangle `abc`
  (a vertex or an edge → `false`).

### Why `Fixed` and not `float` for geometry

Two properties you care about:

1. **Determinism.** Two machines running the same BSP on the same
   inputs will agree bit-for-bit. Old multiplayer games (including
   the Doom BSP this exercise nods at) synced state *by trusting
   fixed-point arithmetic to be the same everywhere*.
2. **Safe equality on edges.** The "on-edge" check is a sign-of-
   cross-product test hitting exactly zero. With floats, roundoff
   around zero flickers between `+ε` and `-ε`; with fixed-point,
   zero is zero.

[FLOATING.md §4](FLOATING.md#4-why-a-fixed-class-in-mod-02) covers
the "why fixed-point" angle in more depth. This exercise is the
payoff — BSP is where the whole module lands.

### How the code looks

#### `Point` class (OCF with `const` members)

```cpp
class Point {
    Fixed const x;
    Fixed const y;
public:
    Point();
    Point(float const px, float const py);
    Point(Point const& src);
    Point& operator=(Point const& other);  // see gotcha below
    ~Point();

    Fixed getX() const;
    Fixed getY() const;
};
```

**The `const` trap.** `Fixed const x;` means `x` cannot be assigned
after the ctor runs. That kills the *obvious* `operator=`:

```cpp
Point& Point::operator=(Point const&) {
    // x = other.x;  // ← compile error, x is const
    return *this;
}
```

You can't do it. Options:

1. **Return `*this` without modifying anything.** The subject
   tolerates this because `Point` is effectively immutable by design.
2. **`const_cast` the members.** Ugly, but legal. Don't.
3. **Argue in a comment that `operator=` is a no-op on an immutable
   `Point`.** The moulinette typically accepts an empty-body
   `operator=` that still returns `*this`.

This is deliberate: the subject is teaching you that `const` members
and `operator=` don't mix. A real design would drop `operator=` and
live without it (or drop the `const`).

#### `bsp` — sign-of-cross-products

A clean algorithm:

```cpp
static Fixed cross(Point const& a, Point const& b, Point const& p) {
    // (b - a) × (p - a)
    return (b.getX() - a.getX()) * (p.getY() - a.getY())
         - (b.getY() - a.getY()) * (p.getX() - a.getX());
}

bool bsp(Point const a, Point const b, Point const c, Point const p) {
    Fixed d1 = cross(a, b, p);
    Fixed d2 = cross(b, c, p);
    Fixed d3 = cross(c, a, p);

    Fixed zero(0);
    bool has_neg = (d1 < zero) || (d2 < zero) || (d3 < zero);
    bool has_pos = (d1 > zero) || (d2 > zero) || (d3 > zero);

    // strictly inside: all three same sign AND none is zero
    return !(has_neg && has_pos) && d1 != zero
                                 && d2 != zero
                                 && d3 != zero;
}
```

The intuition: the cross product of `(b - a) × (p - a)` is positive
if `p` is to the **left** of the directed edge `a → b`, negative if
**right**, zero if **on the line**. If all three signs match, `p` is
on the same side of all three edges — i.e. inside. If any cross is
zero, `p` is on (the line containing) an edge, so *not strictly*
inside — return `false`.

### Hardware-level notes

- `cross` does 2 subtractions, 2 Q8 multiplications (with the
  `>> 8` that ex02 wired up), 1 subtraction. Ten instructions
  plus the two shifts.
- Overflow risk: cross products multiply coordinates. With a Q8 raw
  of ±8M, `x * y` can be up to `2^46` — way outside signed 32-bit
  range. For this exercise the moulinette tests stay in a modest
  range; for real code you'd use a wider intermediate type.
- Pass `Point` by **value** (the subject says so): 8 bytes per
  `Point` (two `Fixed`s = two ints), fits in two registers, cheap
  to copy. Real code might pass by `const&`, but here the copy is
  free and the signature is dictated.

### Tips & gotchas

- The `const` on `x` and `y` is what makes the copy-assignment
  question hard. Don't spend hours fighting it — empty-body
  `operator=` returning `*this` is the escape hatch.
- `bsp` is a **free function**, not a member. It lives in its own
  `.cpp`, takes four `Point`s by value, and is declared in a header
  the `main` includes.
- "Strictly inside" — re-read the subject. A vertex of the triangle
  (`p == a`) is not inside. A point on an edge is not inside. The
  `d_i != 0` checks are mandatory.
- Degenerate triangle (`a`, `b`, `c` colinear): all three cross
  products are zero, the function should return `false` — the code
  above does.
- Winding order: the algorithm works regardless of whether
  `a, b, c` is clockwise or counter-clockwise. The "all same sign"
  trick handles both.

### Search terms if you get stuck

`"point in triangle cross product"`, `"barycentric coordinates
point in triangle"`, `"sign of cross product 2D"`, `"BSP tree
fixed point"`, `"Doom fixed point math"` (historical — that's
where this module's aesthetic comes from).

---

## Module-level sanity checks before moulinette

- Every trace line the subject shows appears at the right call site
  and in the right order. The reference output is your grammar.
- `sizeof(Fixed) == sizeof(int)` — the `static const` doesn't
  bloat instances.
- Compile with `-Wall -Wextra -Werror`. A single warning kills the
  grade.
- `valgrind ./fixed` — no leaks, no invalid reads. `Fixed` has no
  heap, so this should be clean; ex03's `Point` copies the same.
- `./fixed` matches the subject's sample output character for
  character (trailing newlines included).

You can pass the module without ex03.
