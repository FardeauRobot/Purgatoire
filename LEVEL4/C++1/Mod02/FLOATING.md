# Floating Point — Notes from three cprogramming.com articles

Source articles (by Jeff Bezanson, cprogramming.com):

1. Understanding Floating Point Numbers — conceptual intro
2. Floating Point Number Representation — IEEE-754 internals & effective use
3. Printing Floating Point Numbers — a nicer `printf` helper

---

## 1. Understanding Floating Point Numbers

**Accuracy vs. precision.** The two concepts are distinct:

- *Accuracy* = how close a value is to the true answer.
- *Precision* = how finely the value is pinned down (significant digits).

**Integers are accurate but imprecise.** `2` is exactly `2`; arithmetic is
bit-perfect when it doesn't overflow. But `5/2` and `4/2` both give `2` —
the fractional part is gone. Ints are bricks: great for cubes, bad for
spheres.

**Floats are precise but inaccurate.** They keep the fractional part, so
they mold to arbitrary shapes (silly putty). The catch: most decimal
fractions have no finite binary representation (`1/3` is hopeless in *any*
base), so results are almost always off by "a little bit".

**Takeaway.** A 4-byte float actually holds *fewer* distinct values than a
4-byte int — the appearance of "real numbers" is an illusion paid for with
roundoff error. Don't use `==` on floats; expect small errors to leak into
every computation.

---

## 2. Floating Point Number Representation (IEEE-754)

### Bit layout

A single-precision `float` (4 bytes) has three parts:

```
 s eeeeeeee mmmmmmmmmmmmmmmmmmmmmmm
31                                0
```

- `s` — sign bit
- `e` — 8-bit exponent (11 for `double`)
- `m` — 23-bit mantissa (52 for `double`)

Value = `(-1)^s * 2^(e-127) * 1.mmm...` — the leading `1` is *implied* so it
doesn't eat a mantissa bit. The exponent is **bias-127 encoded**, so a
stored `0x7f` means an actual exponent of `0`.

### Special cases

- **Zero**: all bits zero (sign ignored).
- **±Infinity**: exponent all ones, mantissa zero.
- **NaN**: exponent all ones, mantissa non-zero.
- **Denormals**: exponent all zeros → implied `1` disappears, precision
  collapses. Smallest representable `float` is `1.4e-45` — with a single
  bit of precision.

### Key constants

| Property             | `float`       | `double`         |
|----------------------|---------------|------------------|
| Largest value        | ~3.4e+38      | ~1.8e+308        |
| Mantissa bits        | 23            | 52               |
| Exponent bits        | 8             | 11               |
| Epsilon              | ~1.19e-7      | ~2.22e-16        |

`epsilon` is the smallest `x` such that `1 + x > 1`. It is a measure of
relative precision (significant bits), *not* absolute magnitude.

### Effective FP programming

**Equality.** The naive `fabs(a - b) < EPSILON` trick ignores magnitude.
Near `1e-20`, two numbers that differ by `1e-20` are *not* equal in any
meaningful sense. Compare significant digits, not raw distance.

**Overflow.** Unlike ints, floats overflow to `±inf`, which behaves sanely
(`inf > anything`, `inf + 1 == inf`). But intermediate overflows still bite:
computing `sqrt(re*re + im*im)` overflows when `re = im = 1e200`, even
though the true magnitude fits. Rearrange: factor the larger component out
of the square root.

**Loss of significance.** Subtracting two close numbers cancels the leading
`1` bits and leaves you with only the tail — possibly just a few bits of
valid result. Summing a series with mixed magnitudes loses the small terms
first; sort small-to-large when it matters.

**Feedback.** Prefer `x = n * inc` over `x += inc` in loops. Incremental
updates corrupt `x` iteration by iteration.

**Don't forget integers.** If you're tracking a rational scaling factor,
store numerator/denominator as ints and reconstruct the float on demand.

---

## 3. Printing Floating Point Numbers

The C library's format specifiers are each wrong in their own way: `%e`
prints `0.000000e+00` for zero, `%f` prints long digit strings for huge
numbers. The article provides `print_real(double r, int width, int dec)`,
which:

1. Pulls the exponent out of the `ieee754_double` union and converts it to
   a decimal order of magnitude via `mag = exponent / log2(10)`.
2. Chooses `%e` when `mag` falls outside `[-max_digs_rt, max_digs_lf-1]`
   (thresholds configurable), otherwise `%f`.
3. Drops the decimal part if the number is effectively an integer
   (`fabs(fpart) < EPSILON`).
4. Builds the final `printf` format string with `snprintf` and prints.

Takeaway: the "right" way to print a float depends on its magnitude; you
usually want to switch between fixed and scientific notation based on the
stored exponent, not on the value itself.

---

## 4. Why a `Fixed` class in Mod 02?

The three articles above make the pitch against you: `float` trades
precision for range and gives you roundoff error on operations as trivial
as `0.1 + 0.2`. `int` is exact but can't represent anything between two
consecutive integers. **Fixed-point** is the middle ground: an integer
with an *implicit, constant scale factor*, giving you a finite slice of
the real line at uniform resolution.

That uniformity is the whole point:

- **Deterministic arithmetic.** No hidden normalization, no bias-127
  exponent juggling. `a + b` is literally one machine `ADD`. The same
  input always produces the exact same output, bit-for-bit, on any CPU.
- **Safe equality.** Two fixed-point numbers are equal iff their raw
  ints are equal — no `fabs(a - b) < EPSILON` gymnastics.
- **Speed on integer-only hardware.** Useful for embedded, DSP, old
  consoles, GPUs that used fixed-point rasterizers, and financial code
  that cannot tolerate FP drift.
- **Predictable precision.** Every representable value is exactly
  `k / 2^bits` for some integer `k`. Unlike floats, precision doesn't
  silently degrade at large magnitudes.

The trade-off is **range**: a 32-bit `Fixed` with 8 fractional bits can
only span roughly ±8.3 million, compared to `float`'s ±3.4 × 10³⁸. You
pick the scale that fits your problem — Mod 02 fixes it at 8 for
simplicity.

Typical homes for fixed-point: 2D graphics (sub-pixel coordinates),
physics simulations needing determinism across machines, audio
processing, money. Mod 02 ex03 (BSP) lands right in the graphics use
case.

---

## 5. Anatomy of the `Fixed` class

```cpp
class Fixed {
    int                 _raw;
    static const int    _bits = 8;
public:
    // OCF + getRawBits/setRawBits + conversions + operators
};
```

Two members. Each one earns its keep.

### `int _raw` — the stored value

This is the entire state of a `Fixed` object. The *logical* real number
it represents is

```
real_value = _raw / 2^_bits     (with _bits = 8, divisor = 256)
```

So `1.0` is stored as `256`, `0.5` as `128`, `3.14` as
`roundf(3.14f * 256) = 804`.

Why `int` and not something fancier:

- **Self-contained state.** No pointer, no dynamic allocation, so the
  copy constructor and copy assignment operator are trivial — a plain
  member-wise copy of one int. That's what makes the OCF implementation
  in ex00 essentially free.
- **Fast arithmetic.** `operator+` is `Fixed(lhs._raw + rhs._raw)`. No
  conversion, no shifting. `operator*` needs one multiplication and one
  shift (to compensate for the doubled scale factor). Still orders of
  magnitude cheaper than FPU ops on hardware without a fast FPU.
- **Signed by design.** Using `int` (not `unsigned`) gives you negative
  values for free and preserves symmetric behavior around zero.

`getRawBits()` and `setRawBits()` exist precisely because two `Fixed`
values only make sense to combine when their scales match — the raw
accessors let arithmetic operators talk in the common currency without
round-tripping through `float`.

### `static const int _bits = 8` — the scale

The scale factor must be a compile-time constant shared by every
instance, and it must never change. That's exactly the contract of
`static const`:

- **`static`** — one copy for the whole class, not one per object. A
  `Fixed` takes the same 4 bytes as an `int`; the `8` lives in the
  binary's rodata, not in each instance. If it were a regular member,
  every `Fixed` would be 8 bytes and you'd have to check at runtime
  that two operands agree on their scale.
- **`const`** — the value is fixed at compile time. The compiler can
  fold the `1 << _bits` (= 256) directly into the generated code, and
  the `setRawBits(int)` operator cannot accidentally corrupt it.
- **Value `8`** — a deliberate choice the subject makes for you:
  - 8 fractional bits means a resolution of `1 / 256 ≈ 0.0039` — fine
    enough for pixel-aligned graphics, coarse enough that the integer
    part still covers ±8.3 million on a 32-bit `int`.
  - `2^8 = 256` fits in a byte — the multiply/divide by the scale can
    be a single 8-bit shift.
  - It's small enough to reason about in your head. With `_bits = 16`
    the raw values look alien (`3.14` becomes `205887`); with `8` you
    can still recognize numbers when debugging.

A different application would pick a different `_bits` — 16 for audio
DSP, 32 for high-precision money maths. Parameterizing it as a
`static const` (rather than hard-coding `256` everywhere) makes that
change a one-line edit.

### The four conversions, in one place

Every method in ex01 boils down to one of these four lines:

```cpp
raw_from_int(int n)       →  n << 8           // int → fixed
raw_from_float(float f)   →  roundf(f * 256)  // float → fixed
to_int(int raw)           →  raw >> 8         // fixed → int
to_float(int raw)         →  raw / 256.0f     // fixed → float
```

Why shifts instead of multiplies: `x << 8` and `x * 256` produce the
same bits for a signed int, but the shift telegraphs intent ("I am
moving the implicit decimal point by 8 binary places") and compiles to
a single instruction on every target.

Why `roundf` on the float path: plain truncation biases every positive
value downward. `roundf(f * 256)` picks the nearest representable
fixed-point value, which is the behavior the subject's reference
outputs expect.

### Conversion table — memorise this, not the formulas

| Real value | Raw (`_bits = 8`) | Computation          |
|------------|-------------------|----------------------|
| `0`        | `0`               | `0 * 256`            |
| `1`        | `256`             | `1 << 8`             |
| `-1`       | `-256`            | `-1 << 8`            |
| `42`       | `10752`           | `42 << 8`            |
| `0.5`      | `128`             | `0.5 * 256`          |
| `0.25`     | `64`              | `0.25 * 256`         |
| `0.00390625` | `1`             | smallest positive    |
| `3.14`     | `804`             | `roundf(3.14 * 256)` |

When you `std::cout` a `Fixed`, the stream operator prints
`_raw / 256.0f`. So `3.14` round-trips as `3.14062…` — that's the
resolution floor biting, not a bug.

### Finding real documentation on this

The reason the web looks empty is the search term. **"42 Fixed class"**
finds only other cadets. The actual name for this technique is
**Q notation** (or **Qm.n format**). Your exercise is specifically
**Q23.8** — 23 integer bits, 8 fractional bits, plus one sign bit,
packed into a 32-bit signed int.

Useful search terms:

- `"fixed-point arithmetic"` — Wikipedia's article is enough on its own
- `"Q notation"` or `"Q format"` — the naming convention (`Q15`, `Q31`,
  `Q23.8`) used by every DSP vendor on the planet
- `"ARM Q15"`, `"TI fixed point DSP"` — vendor docs with worked examples
- `"fixed point multiplication shift"` — covers the `(a * b) >> _bits`
  trick ex02 will eventually need

This technique predates floating-point hardware by decades. Old video
games (SNES, PS1), early 3D rasterizers, embedded control loops, and
most financial systems still run on it. The scarcity is in 42-specific
write-ups, not in the topic itself.

### Why this design is Orthodox-Canonical-friendly

Because the class holds only an `int` and a `static const int`:

- The **default constructor** just sets `_raw = 0`.
- The **copy constructor** and **copy assignment** do a member-wise
  copy — no deep-copy, no ownership, no risk of double-free.
- The **destructor** has nothing to release.

The whole OCF machinery in ex00 is theater around a single integer —
which is the point. Mod 02 isn't really about fixed-point; it uses the
simplest possible stateful class to make you drill OCF, operator
overloading, and the mechanical wiring between `.hpp` declarations and
`.cpp` definitions. The fixed-point motivation is what keeps the
exercise honest instead of purely academic.
