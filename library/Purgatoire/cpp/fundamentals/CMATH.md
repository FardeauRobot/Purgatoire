# `<cmath>` — the C++ math library

`<cmath>` is the C++ wrapper around the C `<math.h>` header. Every function
lives in the `std::` namespace (and, because the original C names are also
re-exported globally in most implementations, usually at global scope too —
don't rely on that, prefer `std::sqrt`).

In **C++98** most of these functions exist in three flavours, one per
floating type:
- `double f(double)` — no suffix
- `float  ff(float)` — `f` suffix (e.g. `sqrtf`)
- `long double fl(long double)` — `l` suffix (e.g. `sqrtl`)

The 42 Mod 02 subject authorizes `roundf` from `<cmath>` for the fixed-point
exercises. That's the C99 `float`-suffixed version.

---

## Absolute value & sign

| Function | Meaning |
|---|---|
| `abs(int)` | integer absolute value (from `<cstdlib>`, but also usable here) |
| `fabs(double)` | floating-point absolute value — use this for `float`/`double` |
| `fmod(x, y)` | floating-point remainder of `x / y`, same sign as `x` |

Pitfall: `abs` on a `double` silently converts to `int` in C89; always
reach for `fabs` with floats.

---

## Powers & roots

| Function | Meaning |
|---|---|
| `sqrt(x)` | square root; undefined for `x < 0` (returns NaN) |
| `pow(base, exp)` | `base^exp`; works for non-integer exponents |
| `exp(x)` | `e^x` |

`pow(x, 0.5)` is equivalent to `sqrt(x)` but slower — prefer `sqrt`.

---

## Logarithms

| Function | Meaning |
|---|---|
| `log(x)` | natural logarithm, base `e`. Undefined for `x <= 0` |
| `log10(x)` | base-10 logarithm |

For an arbitrary base: `log(x) / log(base)`.

---

## Trigonometry (radians)

| Function | Meaning |
|---|---|
| `sin(x)` / `cos(x)` / `tan(x)` | the usual three |
| `asin(x)` / `acos(x)` | inverse sine/cosine; input must be in `[-1, 1]` |
| `atan(x)` | inverse tangent, returns value in `(-π/2, π/2)` |
| `atan2(y, x)` | inverse tangent of `y/x`, returns value in `(-π, π]` — takes the quadrant into account, so prefer it over `atan(y/x)` for vectors |

Degrees ↔ radians: `rad = deg * π / 180`. There is no built-in π, so
declare `const double PI = 3.14159265358979323846;` or use `acos(-1.0)`.

---

## Hyperbolic functions

| Function | Meaning |
|---|---|
| `sinh(x)` | `(e^x - e^-x) / 2` |
| `cosh(x)` | `(e^x + e^-x) / 2` |
| `tanh(x)` | `sinh(x) / cosh(x)` |

`asinh`/`acosh`/`atanh` exist only from **C++11** onwards — not available
in c++98 mode.

---

## Rounding

| Function | Meaning |
|---|---|
| `ceil(x)` | smallest integer ≥ `x`, returned as `double` |
| `floor(x)` | largest integer ≤ `x`, returned as `double` |
| `roundf(x)` | **C99** — nearest integer, ties away from zero (`float` variant). `round` and `trunc` officially appear in C++11, but `roundf` is commonly available in C++98 toolchains — which is why Mod 02 whitelists it. |

Casting `(int)x` truncates toward zero, which is **not** the same as
`floor(x)` for negative numbers (`(int)-0.7 == 0`, `floor(-0.7) == -1`).

Used in Mod 02 ex01 to convert a `float` to a fixed-point raw value:
`raw = roundf(f * (1 << bits));`.

---

## Floating-point manipulation

| Function | Meaning |
|---|---|
| `frexp(x, &exp)` | splits `x` into a normalized mantissa in `[0.5, 1)` and a power-of-two exponent stored through `exp` |
| `ldexp(x, exp)` | inverse of `frexp`: returns `x * 2^exp` |
| `modf(x, &intpart)` | splits `x` into its integer part (written through the pointer) and its fractional part (returned) |

These are rarely needed in exercises but useful when you want to inspect
the IEEE-754 layout without reinterpret-casting.

---

## Special values & classification

`<cmath>` in C++98 does **not** provide `isnan`/`isinf` as standard
functions — those are C++11. In c++98 you can detect NaN with the fact
that `x != x` is true iff `x` is NaN, and `HUGE_VAL` is the `double`
infinity constant.

```cpp
bool is_nan(double x) { return x != x; }
bool is_inf(double x) { return x == HUGE_VAL || x == -HUGE_VAL; }
```

---

## What's **not** in C++98 `<cmath>`

Don't reach for these in 42 modules (c++98 is enforced):

- `round`, `trunc`, `nearbyint` — C++11
- `log2`, `exp2`, `cbrt`, `hypot` — C++11
- `asinh`, `acosh`, `atanh` — C++11
- `tgamma`, `lgamma`, `erf`, `erfc` — C++11
- `isnan`, `isinf`, `isfinite`, `isnormal` as functions — C++11
  (the C99 macros from `<math.h>` may work depending on the compiler)

If an exercise needs any of these, the subject will either authorize it
explicitly (as Mod 02 does for `roundf`) or expect you to re-implement
it by hand.

---

## A minimal worked example

```cpp
#include <cmath>
#include <iostream>

int main() {
    double x = 2.0;
    std::cout << "sqrt(2)    = " << std::sqrt(x)       << '\n';
    std::cout << "pow(2, 10) = " << std::pow(x, 10)    << '\n';
    std::cout << "sin(π/2)   = " << std::sin(std::acos(-1.0) / 2) << '\n';
    std::cout << "ceil(-0.5) = " << std::ceil(-0.5)    << '\n';
    std::cout << "floor(-0.5)= " << std::floor(-0.5)   << '\n';
    std::cout << "roundf(-0.5)=" << std::roundf(-0.5f) << '\n'; // -1
    return 0;
}
```

Compile with `c++ -Wall -Wextra -Werror -std=c++98 main.cpp` — no `-lm`
needed on Linux for C++ (the math library is linked in by `c++`).
