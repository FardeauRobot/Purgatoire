# `roundf` — Exact Behavior and Hardware Implementation

## What `roundf` actually does (the spec)

`roundf(float x)` from `<math.h>` rounds to the **nearest integer, with halfway cases rounded away from zero**.

```
roundf( 0.5f) ==  1.0f      roundf(-0.5f) == -1.0f
roundf( 1.5f) ==  2.0f      roundf(-1.5f) == -2.0f
roundf( 2.5f) ==  3.0f      roundf(-2.5f) == -3.0f
roundf( 2.4f) ==  2.0f      roundf(-2.4f) == -2.0f
```

Per C99/C11, it must:

- Round halves away from zero (NOT to even).
- **Not raise** the `FE_INEXACT` floating-point exception, even when the value is fractional.
- Ignore the current floating-point rounding mode — its behavior is fixed regardless of `fesetround()`.

Result is still a `float`, so for `|x| ≥ 2²³` the input is already integer-valued and is returned unchanged.

---

## Why it's the odd one out

There are five "round to integer" functions in `<math.h>`, and they differ in subtle, important ways:

| Function         | Rounding rule                 | Honors FPU mode? | May raise inexact? |
|------------------|-------------------------------|------------------|--------------------|
| `roundf`         | nearest, **ties away from 0** | no               | no                 |
| `nearbyintf`     | whatever FPU mode says        | yes              | no                 |
| `rintf`          | whatever FPU mode says        | yes              | yes                |
| `truncf`         | toward 0                      | no               | no                 |
| `floorf`/`ceilf` | toward −∞ / +∞                | no               | no                 |

The annoying part: **`roundf`'s rule is not one of the four IEEE 754 standard rounding modes.** The IEEE 754 modes are:

1. Round to nearest, **ties to even** (the default — "banker's rounding")
2. Round toward 0 (truncate)
3. Round toward +∞
4. Round toward −∞

"Ties away from zero" is *not* in that list. IEEE 754-2008 added it as an optional mode (`roundTiesToAway`), but most hardware doesn't implement it as a settable mode.

---

## Hardware reality

### x86 / x86-64 (SSE / AVX)

The `ROUNDSS` instruction (SSE4.1) takes an 8-bit immediate that selects:

- `0` — round to nearest (ties to even)
- `1` — round toward −∞
- `2` — round toward +∞
- `3` — round toward 0
- `4` — use current MXCSR mode

There is **no encoding for "ties away from zero"**. So `roundf` cannot be a single instruction on x86. Compilers and libm implement it with a small sequence: tests + bit manipulation + truncate.

### ARMv8 (AArch64)

ARM was kinder. AArch64 added a whole family of `FRINT*` instructions, including the one we need:

- `FRINTA` — **round to nearest, ties away from zero** ← exactly `roundf`'s semantic
- `FRINTN` — nearest, ties to even
- `FRINTM` — toward −∞
- `FRINTP` — toward +∞
- `FRINTZ` — toward zero
- `FRINTX` — current mode, *raises* inexact
- `FRINTI` — current mode, no inexact

So on AArch64, `roundf(x)` compiles down to a single instruction:

```asm
frinta   s0, s0
ret
```

Whereas the same call on a pre-SSE4.1 x86 box might be ~10–15 instructions of bit fiddling and a branch.

---

## What the libm implementation actually does (x86 / portable)

The portable trick is **integer arithmetic on the IEEE 754 bit pattern**, not on the float itself. A `float` is:

```
[ sign : 1 ][ exponent : 8 ][ mantissa : 23 ]
```

Real biased exponent `e = exp_bits - 127` is the position of the binary point.

The algorithm (this is roughly what glibc/musl do):

1. **Extract bits** with a `union` or `memcpy` to a `uint32_t`.
2. **Pull out the unbiased exponent** `e`.
3. **Three cases:**
   - `e < 0` → `|x| < 1`. Result is `±0` if `|x| < 0.5`, else `±1`. Easy.
   - `e ≥ 23` → no fractional bits exist; `x` is already integer (or NaN/Inf). Return as-is.
   - `0 ≤ e < 23` → there's a fractional part to handle:
     - Compute a mask for the fractional bits below position `e`.
     - **Zero them** to get truncate-toward-zero.
     - Then **conditionally add 1 ulp at the integer position** if the topmost discarded bit was 1 (the "0.5 or more" case) — this gives "ties away from zero" naturally because the magnitude is being adjusted before the sign is reapplied.
4. Write the bits back to a float.

Why bit manipulation rather than `(int)(x + copysign(0.5f, x))`? Because that "obvious" approach is **wrong for values near `0.5 - ulp`** — the addition itself can round the value up, producing the wrong answer in the last bit. Bit manipulation avoids any intermediate rounding entirely.

A naive (and subtly broken) version some people write:

```c
// Don't do this. Wrong for some inputs near halves due to FP rounding.
float my_roundf(float x) {
    return (x >= 0.0f) ? floorf(x + 0.5f) : ceilf(x - 0.5f);
}
```

The real glibc version is ~20 lines of bit-banging with no FP arithmetic at all.

---

## Performance summary

- **AArch64:** 1 instruction (`frinta`), ~3-cycle latency, fully pipelined.
- **x86 with SSE4.1+:** typically inlined as a small sequence (compare, mask, add 0.5 with care, `ROUNDSS` toward zero, etc.), ~10–15 cycles.
- **x86 without SSE4.1:** function call into libm, which does the bit-manipulation routine — dozens of cycles, branch-heavy.

This is why on hot paths people often:

- Use `(int)x` (truncation) if they actually want truncation.
- Use `nearbyintf` if they're OK with banker's rounding — it can compile to a single `ROUNDSS` on x86.
- Use `lroundf` if they want the integer result anyway (avoids round-trip through `float`).

---

## TL;DR

**`roundf`'s semantics are mathematically natural but architecturally awkward** — it asks for a rounding mode that IEEE 754 hardware historically didn't support, so it almost always costs more than its cousins.
