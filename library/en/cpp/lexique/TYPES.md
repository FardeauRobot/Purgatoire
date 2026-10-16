# `char` / `int` / `float` / `double` / `short` / `long` / `signed` / `unsigned` / `wchar_t` — Fundamental Types

> **TL;DR.** The built-in value types inherited from C. `int` for integers, `double` for floating point, `char` for bytes/characters, `bool` for truth — that covers 95% of 42 code. `short`, `long`, `signed`, `unsigned` are **modifiers** that trade range for size. Sizes are *minimums*, not guarantees.

Related: [`BOOL.md`](BOOL.md) · [`VOID.md`](VOID.md) · [`SIZEOF.md`](SIZEOF.md) · [`ENUM.md`](ENUM.md) · fixed-point math: [`CPP02.md`](../projets/modules/CPP02.md)

---

## 1. The lineup (typical 64-bit macOS/Linux)

| Type | Size | Range (typical) | Use for |
|---|---|---|---|
| `bool` | 1 B | `true` / `false` | truth values |
| `char` | 1 B | −128…127 | one byte, one ASCII char |
| `unsigned char` | 1 B | 0…255 | raw bytes |
| `short` | 2 B | ±32 767 | rarely — compact storage |
| `int` | 4 B | ±2.1 × 10⁹ | **default integer** |
| `unsigned int` | 4 B | 0…4.2 × 10⁹ | bit masks, never "counts" |
| `long` | 8 B | ±9.2 × 10¹⁸ | when `int` may overflow |
| `float` | 4 B | ~7 digits | **only when the subject says so** (CPP02) |
| `double` | 8 B | ~15 digits | **default floating point** |
| `wchar_t` | 4 B | wide char | wide strings — never at 42 |

The standard only promises `char ≤ short ≤ int ≤ long`. `sizeof(char)` is **always 1**; everything else is platform-dependent — that's what [`sizeof`](SIZEOF.md) is for.

## 2. Literals pick a type

```cpp
42      // int
42u     // unsigned
42l     // long
3.14    // double  ← note: NOT float
3.14f   // float
'a'     // char
"a"     // const char[2] — array, not char!
```

That's why CPP02 makes you write `roundf(3.14f)` — mixing `float` and `double` silently promotes.

## 3. Conversions — where the bodies are buried

```cpp
int    i = 3.99;        // 3     — truncation, not rounding
char   c = 300;         // 💥 implementation-defined — doesn't fit
double d = 1 / 2;       // 0.0   — INTEGER division happened first
double e = 1.0 / 2;     // 0.5   — one double infects the expression
```

**Signed/unsigned comparison** is the classic:

```cpp
std::string s = "hi";
for (int i = 0; i < s.size(); ++i)   // ⚠️ int vs size_t warning
```
`s.size()` returns an unsigned type; if `i` were negative the comparison would go insane (−1 becomes huge). Use `std::string::size_type` or `size_t` for indexes.

```
   unsigned wraparound:      0
                          ▲  │
                 4294967295◄─┘ −1 "underflows" to the top
```

**Overflow:** unsigned wraps around (defined). Signed overflow is **UB** — the compiler may assume it never happens.

## 4. Promotions in one picture

```
   char, short ──► int ──► unsigned ──► long ──► float ──► double
                  (everything small is promoted to int before math)
```

Two consequences: `char + char` is an `int`; and printing a `char` arithmetic result may need a cast back: `static_cast<char>(c + 1)`.

## 5. Tips

- Default pair: `int` + `double`. Reach for others only with a reason (the subject, a byte buffer, a bit mask).
- `unsigned` for "can't be negative" is a trap — subtraction underflows. Use it for **bits**, not for **counts**.
- Comparing floats with `==` fails after arithmetic; compare `fabs(a-b) < epsilon`. CPP02's whole point.
