# `<cctype>`, `<cstdlib>`, `<cstring>` — the C toolbox in C++98

C++ inherits the whole C standard library. Each C header `<xxx.h>` has a C++
twin `<cxxx>` that puts the same functions in the `std::` namespace:

| C header | C++ header | What's inside |
|---|---|---|
| `<ctype.h>` | `<cctype>` | character tests and case conversion (`isdigit`, `toupper`…) |
| `<stdlib.h>` | `<cstdlib>` | string → number, `abs`, `exit`, `rand`… |
| `<string.h>` | `<cstring>` | raw `char*` strings and memory blocks (`strlen`, `memset`…) |

Always include the `<cxxx>` form and write `std::isdigit`, not `isdigit`.
Most implementations *also* expose the global name, but the standard does not
promise it — code that compiles on macOS may break on the Linux evaluation box.

> These are the functions you rewrote in **libft**. `std::isdigit` is
> `ft_isdigit`, `std::strlen` is `ft_strlen`, `std::atoi` is `ft_atoi` —
> same contract, same pitfalls.

Siblings: [`CMATH.md`](CMATH.md) (`<cmath>`), [`STRING.md`](STRING.md) and
[`../io-errors/STRING_FUNCTIONS.md`](../io-errors/STRING_FUNCTIONS.md) (`std::string`).

---

## `<cctype>` — one character at a time

Every function takes an `int` and returns an `int`.

### Tests (`is…`)

| Function | True for | Example `true` |
|---|---|---|
| `isdigit(c)` | `'0'`–`'9'` | `'7'` |
| `isxdigit(c)` | hex digit: `0-9`, `a-f`, `A-F` | `'B'` |
| `isalpha(c)` | letter `a-z`, `A-Z` | `'q'` |
| `isalnum(c)` | letter or digit | `'3'`, `'x'` |
| `isupper(c)` | `A-Z` | `'Q'` |
| `islower(c)` | `a-z` | `'q'` |
| `isspace(c)` | `' '`, `'\t'`, `'\n'`, `'\v'`, `'\f'`, `'\r'` | `'\n'` |
| `ispunct(c)` | printable, not alnum, not space | `'!'`, `'.'` |
| `isprint(c)` | printable, **space included** (32–126) | `' '` |
| `isgraph(c)` | printable, **space excluded** (33–126) | `'~'` |
| `iscntrl(c)` | control chars (0–31, 127) | `'\0'`, `'\t'` |

`isblank` (space or tab only) is C99 / C++11 — not available in `-std=c++98`.

### Conversion

| Function | Does |
|---|---|
| `toupper(c)` | `'a'` → `'A'`; anything else returned unchanged |
| `tolower(c)` | `'A'` → `'a'`; anything else returned unchanged |

They return an `int`: cast back to `char` when you store it.

```cpp
std::string s = "Hello";
for (std::string::size_type i = 0; i < s.size(); ++i)
    s[i] = static_cast<char>(std::toupper(static_cast<unsigned char>(s[i])));
```

### Gotcha 1 — the result is "non-zero", not `1`

```cpp
if (std::isdigit(c) == 1)   // WRONG: glibc may return 2048
if (std::isdigit(c))        // right
```

The `is…` functions are usually a table lookup returning a bit mask, so
"true" can be any non-zero value. Never compare to `1` or `true`.

### Gotcha 2 — negative `char` is undefined behavior

The argument must be **`EOF` or a value representable as `unsigned char`**
(0–255). On x86/ARM Linux and macOS, plain `char` is signed, so `'é'` in
Latin-1 or a UTF-8 byte is negative → the table is indexed out of bounds.

```cpp
char c = s[i];
std::isdigit(c);                                // UB if c < 0
std::isdigit(static_cast<unsigned char>(c));    // always safe
```

Same rule for every function in `<cctype>`. It's the classic reason
`ft_isdigit` tests pass while the real one crashes on weird input.

### Typical use — "is this string only digits?"

```cpp
#include <cctype>
#include <string>

bool isAllDigits(const std::string& s)
{
    if (s.empty())
        return false;
    for (std::string::size_type i = 0; i < s.size(); ++i)
        if (!std::isdigit(static_cast<unsigned char>(s[i])))
            return false;
    return true;
}
```

Note the `empty()` check: without it, `""` would count as a valid number.

---

## `<cstdlib>` — numbers, exit, randomness

### String → number: the `ato…` family (quick, blind)

| Function | Returns |
|---|---|
| `atoi(str)` | `int` |
| `atol(str)` | `long` |
| `atof(str)` | `double` (yes, `double`, despite the `f`) |

Skips leading whitespace, reads an optional sign and digits, stops at the
first invalid character. **No error reporting at all**: `atoi("abc")` and
`atoi("0")` both return `0`, and overflow is undefined behavior. Fine for a
quick test, never for validating user input.

`atoll` is C99/C++11 — not in C++98.

### String → number: the `strto…` family (robust)

| Function | Returns | Extra |
|---|---|---|
| `strtol(str, &end, base)` | `long` | base 2–36, or `0` = auto (`0x` → hex, `0` → octal) |
| `strtoul(str, &end, base)` | `unsigned long` | ⚠️ accepts `"-1"` and wraps it to `ULONG_MAX` |
| `strtod(str, &end)` | `double` | understands `"1e3"`, `"inf"`, `"nan"`, `"0x1p3"` |

The second argument `end` is set to point **just after the last character
consumed**. That's what makes these functions usable for validation:

- `end == str` → nothing was parsed (`"abc"`, `""`)
- `*end != '\0'` → trailing garbage (`"42abc"`)
- `errno == ERANGE` → overflow/underflow (`<cerrno>`)

```cpp
#include <cerrno>
#include <climits>
#include <cstdlib>
#include <string>

bool parseInt(const std::string& s, int& out)
{
    const char* str = s.c_str();
    char*       end;

    errno = 0;
    long v = std::strtol(str, &end, 10);
    if (end == str || *end != '\0')
        return false;
    if (errno == ERANGE || v < INT_MIN || v > INT_MAX)
        return false;
    out = static_cast<int>(v);
    return true;
}
```

Why each line:
- `errno = 0` first — `strtol` only *sets* `errno` on error, it never clears it.
- `long` then range-check — `strtol` returns `long`; on 64-bit, `long` is
  wider than `int`, so `"3000000000"` fits in `long` but not `int`.
- `s.c_str()` is the bridge from `std::string` to `const char*`.
- `strtol` silently skips **leading** whitespace, so `" 7"` is accepted. If
  the subject wants it rejected, test `std::isspace` on `s[0]` first.

This is the pattern behind CPP06 (`ScalarConverter`) and CPP09
(parsing `BitcoinExchange` values, `PmergeMe` arguments). See [`../../projets/modules/CPP06.md`](../../projets/modules/CPP06.md).

`strtof`, `strtoll`, `strtold` are C99/C++11 — in C++98 parse with `strtod`
and cast. The C++ alternative is `std::stringstream` (see
[`../io-errors/FSTREAM_GUIDE.md`](../io-errors/FSTREAM_GUIDE.md)):
`ss >> n` then check `ss.fail()` and `ss.eof()`.

### Absolute value

| Function | For |
|---|---|
| `abs(int)` | `int` |
| `labs(long)` | `long` |
| `fabs(double)` | floating point — that one's in `<cmath>` |

`std::abs(INT_MIN)` is undefined (the positive value doesn't fit).

### Ending the program

| Name | Meaning |
|---|---|
| `exit(code)` | flush C streams, run `atexit` handlers, quit |
| `EXIT_SUCCESS` / `EXIT_FAILURE` | portable `0` / `1` |

⚠️ `exit()` does **not** run destructors of local objects still on the
stack — in C++ prefer returning from `main`, or throwing and catching in
`main`. Leaks reported by valgrind after an `exit` often come from that.

### Random numbers

| Function | Does |
|---|---|
| `srand(seed)` | seed the generator, **once**, at program start |
| `rand()` | pseudo-random `int` in `[0, RAND_MAX]` |

```cpp
#include <cstdlib>
#include <ctime>

std::srand(static_cast<unsigned int>(std::time(NULL)));
int dice = std::rand() % 6 + 1;     // 1..6
```

Calling `srand(time(NULL))` before every `rand()` gives the same number
within the same second. Used in CPP05 (robotomy 50%) and
CPP06 (`generate()`).

### Forbidden here

`malloc`, `calloc`, `realloc`, `free` live in `<cstdlib>` too, but the 42 C++
subjects forbid them: use `new` / `delete` ([`MEMORY.md`](MEMORY.md)).

---

## `<cstring>` — raw C strings and memory

Work on `char*` terminated by `'\0'`. In C++, `std::string` does all of this
more safely — you meet these mostly when talking to a C API, or on
`argv[i]`.

### Strings

| Function | Does | `std::string` equivalent |
|---|---|---|
| `strlen(s)` | length, `'\0'` excluded | `s.size()` / `s.length()` |
| `strcmp(a, b)` | `<0`, `0`, `>0` (lexicographic) | `a == b`, `a < b`, `a.compare(b)` |
| `strncmp(a, b, n)` | compare first `n` chars | `a.compare(0, n, b, 0, n)` |
| `strcpy(dst, src)` | copy, **no bounds check** | `dst = src` |
| `strncpy(dst, src, n)` | copy ≤ `n` chars, may **not** add `'\0'` | `dst = src.substr(0, n)` |
| `strcat(dst, src)` | append, **no bounds check** | `dst += src` |
| `strchr(s, c)` | first `c`, or `NULL` | `s.find(c)` |
| `strrchr(s, c)` | last `c`, or `NULL` | `s.rfind(c)` |
| `strstr(hay, needle)` | first substring, or `NULL` | `s.find(needle)` |

Gotchas:
- `strcmp` returns **0 when equal** — `if (strcmp(a, b))` means "different".
- `strlen` is O(n): it walks to `'\0'` each call. Don't put it in a loop condition.
- `strcpy` / `strcat` overflow `dst` if it's too small.

```cpp
if (argc == 2 && std::strcmp(argv[1], "--help") == 0)
    printUsage();
```

(or simply `std::string(argv[1]) == "--help"`).

### Memory blocks

| Function | Does |
|---|---|
| `memset(p, byte, n)` | fill `n` bytes with `byte` |
| `memcpy(dst, src, n)` | copy `n` bytes — zones must **not** overlap |
| `memmove(dst, src, n)` | same, overlap allowed |
| `memcmp(a, b, n)` | compare `n` bytes, `0` if equal |

⚠️ Never `memset`/`memcpy` an object that has constructors, virtual functions
or `std::string` members — it skips the constructor and smashes the vptr.
Use the copy constructor / `operator=` (see
[`../oop/ORTHODOX_CANONICAL_FORM.md`](../oop/ORTHODOX_CANONICAL_FORM.md)).

---

## Cheat sheet — "I want to…"

| I want to… | C++98 |
|---|---|
| test if a char is a digit | `std::isdigit(static_cast<unsigned char>(c))` |
| uppercase a char | `static_cast<char>(std::toupper(static_cast<unsigned char>(c)))` |
| skip whitespace | `while (std::isspace(static_cast<unsigned char>(s[i]))) ++i;` |
| parse an `int` safely | `std::strtol` + `end` + `errno` + range check |
| parse a `double` / `float` | `std::strtod`, cast for `float` |
| parse quick & dirty | `std::atoi` (no error detection) |
| compare two `char*` | `std::strcmp(a, b) == 0` |
| length of a `char*` | `std::strlen(s)` |
| `std::string` → `char*` | `s.c_str()` |
| random int in `[0, n)` | `std::rand() % n` after one `std::srand` |
| quit with an error code | `return EXIT_FAILURE;` from `main` |
