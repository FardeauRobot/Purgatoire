# ex01 — Fixed-point class notes

## 1. Powers of 2 in C++

`^` is **XOR**, not exponent. For powers of 2, use a left shift:

```cpp
1 << 8   // = 256  (i.e. 2^8)
1 << n   // = 2^n
```

For arbitrary bases, `<cmath>` gives `std::pow(2, 8)` — but it returns a `double`, so for fixed-point work prefer the bit shift.

## 2. Float → raw (encoding)

Multiply by `2^fractional_bits`, then **round** (don't just cast — `(int)` truncates negatives toward zero):

```cpp
#include <cmath>

float f = 3.14f;
int raw = std::roundf(f * (1 << 8));   // 3.14 * 256 ≈ 804
```

## 3. Raw → float (decoding)

Reverse: divide by the same scale factor.

```cpp
int raw = 804;
float back = raw / (float)(1 << 8);   // 804 / 256.0f ≈ 3.1406
```

The `(float)` cast is **mandatory**. Without it, `raw / 256` is integer division → you'd get `3` instead of `3.14`. Equivalent: `raw / 256.0f`.

## 4. The `<<` insertion operator

### Wrong (member function with no params)

```cpp
void operator<<(void);   // ❌ won't work for std::cout << fixed
```

### Right (free function)

```cpp
// In Fixed.hpp, OUTSIDE the class
std::ostream& operator<<(std::ostream& out, const Fixed& f);
```

```cpp
// In Fixed.cpp
std::ostream& operator<<(std::ostream& out, const Fixed& f) {
    out << f.toFloat();
    return out;
}
```

### How to read the signature

```cpp
std::ostream&     operator<<(std::ostream& out, const Fixed& fixed);
//   ↑                          ↑                ↑
// returns                    param 1          param 2
```

- **`std::ostream&`** — returns a *reference* to an ostream (so chaining works).
- **`operator<<`** — the function's name. Compiler calls it when it sees `cout << x`.
- **`std::ostream& out`** — first param, by reference (streams can't be copied).
- **`const Fixed& fixed`** — second param, const reference (read-only, no copy). `fixed` is just the parameter name.

## 5. Free function vs member function

A **free function** is a function not inside any class.

|                            | Member function       | Free function       |
| -------------------------- | --------------------- | ------------------- |
| Defined inside a class?    | Yes                   | No                  |
| Has implicit `this`?       | Yes                   | No                  |
| Called as                  | `obj.func(args)`      | `func(args)`        |
| Access to private data?    | Yes                   | Only via public API |

**Why `operator<<` MUST be free here:** when you write `std::cout << myFixed`, the left operand is `std::cout`. A member operator would have to live inside `std::ostream` — but you can't modify the standard library. So it has to be a free function.

**42 catch:** `friend` is forbidden in this module, so the free `operator<<` can only call **public** methods of `Fixed` (like `toFloat()`).

## 6. How `<<` actually works (operator = function)

Operators are just **functions with special call syntax**. The compiler rewrites:

```cpp
std::cout << example;
```

into:

```cpp
operator<<(std::cout, example);
```

The parentheses are there — they're just hidden. Same with `a + b` → `operator+(a, b)`.

### Chaining

Because `operator<<` returns the stream, you can chain:

```cpp
std::cout << "a is " << a << std::endl;
// evaluates left-to-right:
((std::cout << "a is ") << a) << std::endl;
//   ↑ returns std::cout&
//                          ↑ becomes the left operand of the next <<
```

Each `<<` consumes the stream returned by the previous one. **That's why returning `std::ostream&` is mandatory** — without it, the chain breaks after the first `<<`.

## 7. How to "guess" the signature from the 42 subject

The subject says:

> "An overload of the **insertion (`<<`) operator** that **inserts** a floating-point representation of the fixed-point number **into the output stream object passed as a parameter**."

Decoded:

- "insertion operator" → C++ name for stream `operator<<`.
- "passed as a parameter" → the stream is an *argument*, not the left side of a method. So it's **not** a member function.
- "into the output stream" → parameter type is `std::ostream&`.

And the example main `std::cout << "a is " << a << std::endl;` shows chaining → must return the stream.
