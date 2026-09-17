# 🎭 CPP06 — Casts: Saying Out Loud What the C Cast Hid

> **TL;DR.** In C, `(type)x` does *anything*: it converts numbers, throws away `const`, and turns an address into an integer, all with the same syntax. C++ splits it into **four named casts**, and each one can only do one kind of thing. That's why the grep-able name tells a reader (and your evaluator) exactly what you meant. CPP06 gives each exercise one cast: `static_cast` for value conversions (ex00), `reinterpret_cast` for raw bit reinterpretation (ex01), and `dynamic_cast` for asking an object what it really is at runtime (ex02). **Your choice of cast is graded.**

Related: [`CASTS.md`](../../lexique/CASTS.md) · [`STATIC.md`](../../lexique/STATIC.md) · [`POLYMORPHISM.md`](../../notions/oop/POLYMORPHISM.md) · [`VIRTUAL.md`](../../lexique/VIRTUAL.md) · [`TRY_CATCH_THROW.md`](../../lexique/TRY_CATCH_THROW.md) · [`TYPEID.md`](../../lexique/TYPEID.md) (banned here) · [`CMATH.md`](../../notions/fundamentals/CMATH.md) · [`STRING.md`](../../notions/fundamentals/STRING.md) · Previous: [`CPP05.md`](CPP05.md)

---

## 1. The problem, in C you already know

```c
double      d  = 42.9;
int         i  = (int)d;              /* value conversion: 42               */
const char *s  = "hello";
char       *w  = (char *)s;           /* drops const: writing w[0] is UB     */
uintptr_t   u  = (uintptr_t)&d;       /* address → integer                   */
Animal     *a  = (Animal *)some_ptr;  /* "trust me" downcast, never checked  */
```

All four lines use the same syntax, and the compiler accepts all of them. When one crashes, you can't find the dangerous cast with `grep`, because every cast looks alike.

**C++'s answer:** four keywords, each with a narrow job. If you use the wrong one, the compiler refuses.

---

## 2. The mechanism, in one table

| Cast | What it may do | Checked when? | CPP06 |
|---|---|---|---|
| `static_cast<T>(x)` | Conversions the language already knows: `double`→`int`, `int`→`char`, `void*`→`T*`, base↔derived pointers **without checking** | Compile time | **ex00** |
| `reinterpret_cast<T>(x)` | Read the **same bits** as another type: pointer ↔ integer, pointer ↔ unrelated pointer | Nothing checked. You promise | **ex01** |
| `dynamic_cast<T>(x)` | Base → derived, **checked at runtime** using the object's vtable. Requires a polymorphic type | Runtime | **ex02** |
| `const_cast<T>(x)` | Add or remove `const` / `volatile`. Nothing else | Compile time | (not used) |
| `(T)x` C cast | Tries `const_cast`, `static_cast`, then `reinterpret_cast`, and takes the first that compiles | — | **Avoid it**: it hides which one happened |

```
                how "trusting" is the cast?
 safe ◄─────────────────────────────────────────────────────► dangerous
 implicit    dynamic_cast       static_cast       const_cast     reinterpret_cast
 (promotion) (checked at run)   (unchecked)       (strip const)  (raw bits)
```

The grading sheet uses the same vocabulary: *"Accept the use of implicit casts for **promotion casts only**."* A promotion is a widening that can't lose information, such as `char`→`int` or `float`→`double`. Every *narrowing* conversion (`double`→`int`, `int`→`char`, `double`→`float`) must be an explicit `static_cast`.

---

## 3. The "class you can't instantiate" pattern (ex00 and ex01)

Both ex00 and ex01 ask for a class that is only a *namespace with a name*: static methods, and no objects allowed.

```cpp
class ScalarConverter
{
private:
    ScalarConverter();
    ScalarConverter(ScalarConverter const &other);
    ScalarConverter &operator=(ScalarConverter const &other);
    ~ScalarConverter();

public:
    static void convert(std::string const &literal);
};
```

- **Private constructors** mean that `ScalarConverter sc;` doesn't compile outside the class. The grading sheet asks exactly *"a class with a private constructor, and static methods?"*.
- **OCF still applies** (CPP02 → CPP09). Declaring the four members private satisfies it and blocks copies too.
- `static` means the method needs no `this`, so you call it as `ScalarConverter::convert(av[1])`. It's the class-scoped version of a plain C function.

> In C you'd have used a file with `static` functions. Here the *class* is the namespace, and the private ctor is the lock.

---

## 4. ex00 — `ScalarConverter` (static_cast)

### What the subject asks
`./convert <literal>` receives **one** C++ literal as a string. It detects its type, converts it to the actual type, then **explicitly** converts it to the other three types and prints all four.

| Type | Accepted literals |
|---|---|
| `char` | `'c'`, `'a'`… Non-displayable chars are never used as input. A single digit counts as an `int` |
| `int` | `0`, `-42`, `42` |
| `float` | `0.0f`, `-4.2f`, `4.2f`, plus the pseudo-literals **`-inff`, `+inff`, `nanf`** |
| `double` | `0.0`, `-4.2`, `4.2`, plus the pseudo-literals **`-inf`, `+inf`, `nan`** |

When a conversion makes no sense or overflows, print `impossible`. When a char isn't printable, print `Non displayable`.

```
$> ./convert 0                 $> ./convert nan             $> ./convert 42.0f
char: Non displayable          char: impossible             char: '*'
int: 0                         int: impossible              int: 42
float: 0.0f                    float: nanf                  float: 42.0f
double: 0.0                    double: nan                  double: 42.0
```

### The pipeline

```
 "42.0f"
    │
    ▼
 ┌───────────────── detect ─────────────────┐
 │ nanf  +inff  -inff         → FLOAT       │
 │ nan   +inf   -inf          → DOUBLE      │
 │ length 1 && !isdigit       → CHAR        │
 │ [+-]?digits                → INT (*)     │
 │ [+-]?digits.digits f       → FLOAT       │
 │ [+-]?digits.digits         → DOUBLE      │
 │ anything else              → error, stop │
 └──────────────────────────────────────────┘
    │  e.g. FLOAT
    ▼
 parse ONCE into the real type:
    float f = static_cast<float>(std::strtod(s, NULL));
    │
    ▼
 static_cast to the three OTHER types, each guarded:
    char   ← nan or outside [0,127]?     → "impossible"
             not printable?              → "Non displayable"
    int    ← nan or outside int range?   → "impossible"
    double ← static_cast<double>(f)        (always fits)
```

(*) `2147483648` matches the INT pattern but doesn't fit in an `int`. Decide what you do with it (treat it as a `double`, or print `impossible` for char and int) and say so at the defense.

The subject insists on **detect first, then convert from the real type**. Some students parse everything as a `double` and print from there. The sheet says *"please don't be too uncompromising… if the spirit of the exercise is respected"*, but be ready to explain what you did.

### Core snippet: the guards on the output side

```cpp
static void printChar(double d)
{
    std::cout << "char: ";
    if (d != d || d < 0 || d > 127)
        std::cout << "impossible" << std::endl;
    else if (!std::isprint(static_cast<int>(d)))
        std::cout << "Non displayable" << std::endl;
    else
        std::cout << "'" << static_cast<char>(d) << "'" << std::endl;
}

static void printInt(double d)
{
    std::cout << "int: ";
    if (d != d || d < static_cast<double>(INT_MIN) || d > static_cast<double>(INT_MAX))
        std::cout << "impossible" << std::endl;
    else
        std::cout << static_cast<int>(d) << std::endl;
}
```

Line by line:
- **`d != d`** is the C++98 test for NaN. NaN is the only value that isn't equal to itself. (`std::isnan` is C++11 in `<cmath>`. C99's `isnan` macro often works, but it isn't guaranteed in `-std=c++98`.)
- **The range check comes *before* the cast.** `static_cast<int>(1e10)` is **undefined behaviour**, not a clean overflow. Check first, cast second.
- **`static_cast<char>(d)` only runs once we know `d` fits.** This is the cast the grading sheet looks for.
- `±inf` fails both range checks on its own, so it needs no special case.
- These helpers take a `double` to stay short. If your real type is `int`, passing it in is an implicit `int`→`double`, which is **not** a promotion: write `printChar(static_cast<double>(i))`.

### Printing `42.0f` instead of `42f`

`std::cout << 42.0f` prints `42`, because the stream drops trailing zeros. You have to add the `.0` yourself when the value is whole:

```cpp
float f = static_cast<float>(d);
std::cout << "float: " << f;
if (f == std::floor(f) && std::fabs(f) < 1e6)
    std::cout << ".0";
std::cout << "f" << std::endl;
```

- `f == std::floor(f)` is true when there's no fractional part. It's false for `nan` and true for `inf`…
- …so the `std::fabs(f) < 1e6` guard stops `inf.0f`. It also stops `1e+10.0f`, because the default precision (6) switches big numbers to scientific notation.
- `nan` prints as `nan`, and `+ "f"` turns it into `nanf`. That's exactly the pseudo-literal the subject expects.

The alternative is `std::fixed << std::setprecision(1)`. It always prints one decimal, but it silently loses digits: `4.25` prints as `4.2`, and `4.35` as `4.3`. The default stream loses digits too, just later: 6 significant digits, so `3.14159265` prints as `3.14159`. Both approaches are accepted. Pick one and justify it.

### ⚠️ Gotchas
- **`'a'` vs `a`.** The subject shows char literals with quotes, but the shell strips them unless you write `./convert "'a'"`. Most students accept a single non-digit character. Decide, and say so at the defense.
- **`"0"` is an int, not a char.** Its char conversion is `Non displayable` because the *value* 0 is NUL. The character `'0'` is the value 48, which prints fine.
- **`2147483648`** looks like an int but overflows. Parse with `strtol` or `strtod`, then range-check against `INT_MIN`/`INT_MAX`. Don't rely on `errno == ERANGE` from `strtol`: `long` is 64 bits on your Mac, so it doesn't overflow there. Never parse it with `atoi`, which is UB on overflow.
- `+inf` prints `inf`, not `+inf`. That's fine: the subject's own example only shows `nan`.
- Include `<cctype>` for `isprint`, `<cmath>` for `floor` and `fabs`, `<climits>` for `INT_MAX`, and `<cstdlib>` for `strtod`.

---

## 5. ex01 — `Serializer` (reinterpret_cast)

### What the subject asks
- A non-instantiable class `Serializer` with:
  - `static uintptr_t serialize(Data *ptr);`
  - `static Data *deserialize(uintptr_t raw);`
- A **non-empty** `Data` struct with real members.
- A test showing that `deserialize(serialize(&data)) == &data`.

### Core snippet

```cpp
#include <stdint.h>

uintptr_t Serializer::serialize(Data *ptr)
{
    return reinterpret_cast<uintptr_t>(ptr);
}

Data *Serializer::deserialize(uintptr_t raw)
{
    return reinterpret_cast<Data *>(raw);
}
```

### What's happening in memory

```
 stack                                      same 8 bytes, read differently
 ┌──────────────────────┐
 │ Data data            │ ◄── lives at 0x7ffee40e23b0
 │   int id = 42        │
 │   std::string name   │
 └──────────────────────┘

 Data *ptr = &data        →  0x7ffee40e23b0      (typed: "points to a Data")
          │ reinterpret_cast<uintptr_t>
          ▼
 uintptr_t raw            →  140732724552624    (just a number: you can print it, store it, send it)
          │ reinterpret_cast<Data *>
          ▼
 Data *back               →  0x7ffee40e23b0      back == ptr ✓   back->id == 42 ✓
```

- **Why `uintptr_t`?** It's the unsigned integer type guaranteed to be large enough to hold a pointer. `unsigned int` is 32 bits on your Mac, while pointers are 64, so a round trip through it would truncate.
- **Why not `static_cast`?** A pointer and an integer aren't "related" types, so `static_cast<uintptr_t>(ptr)` doesn't compile. `reinterpret_cast` is the one cast that says "same bits, different type".
- **"Serialise" here only means turning the pointer into a number.** Nothing is copied: the `Data` has to still be alive when you deserialise. It's not real serialisation (no bytes are written to a file). The exercise is about the cast.

### ⚠️ Gotchas
- **Use `<stdint.h>` rather than `<cstdint>`.** `<cstdint>` is officially C++11. Apple's libc++ lets it through, but a stricter setup might not.
- **The sheet wants `reinterpret_cast` exactly twice**, once in each direction. If you add a C cast somewhere, it gets flagged.
- An empty `struct Data {};` fails the exercise. Put at least one real member in it, and print it after the round trip to prove the struct is still usable.

---

## 6. ex02 — Identify the real type (dynamic_cast)

### What the subject asks
- `Base` has **only a public virtual destructor**. `A`, `B` and `C` inherit publicly from `Base` and are empty. These four classes are exempt from the OCF.
- Three free functions:
  - `Base *generate(void);` returns a random `A`, `B` or `C`.
  - `void identify(Base *p);` prints `A`, `B` or `C`.
  - `void identify(Base &p);` prints the same. **A pointer is forbidden inside this function.**
- **Including `<typeinfo>` is forbidden**, so `typeid` is off the table.

### Why `dynamic_cast` works: the vtable knows

```
 Base *p = new B;

 heap object B                       B's vtable (built by the compiler)
 ┌──────────────────┐               ┌────────────────────────────┐
 │ vptr ────────────┼─────────────► │ type info: "I am a B,      │
 ├──────────────────┤               │  I derive from Base"       │
 │ (no members)     │               │ &B::~B                     │
 └──────────────────┘               └────────────────────────────┘

 dynamic_cast<A *>(p)  → reads the vtable: B is not an A  → NULL
 dynamic_cast<B *>(p)  → reads the vtable: yes            → p, typed as B*
```

Only classes with **at least one virtual function** have a vtable. That's why the subject gives `Base` a virtual destructor, and why `dynamic_cast` on a non-polymorphic class is a **compile error** (`'Base' is not polymorphic`). The same virtual destructor also makes `delete p` run `~B`, which you learnt in CPP04.

### Core snippet: pointer version

```cpp
void identify(Base *p)
{
    if (dynamic_cast<A *>(p))
        std::cout << "A" << std::endl;
    else if (dynamic_cast<B *>(p))
        std::cout << "B" << std::endl;
    else if (dynamic_cast<C *>(p))
        std::cout << "C" << std::endl;
}
```

A failed pointer cast **returns NULL**. The grading sheet checks for exactly that: *"should check if the cast return is NULL"*.

### Core snippet: reference version

A reference can't be NULL, so a failed reference cast **throws** instead:

```cpp
void identify(Base &p)
{
    try
    {
        (void)dynamic_cast<A &>(p);
        std::cout << "A" << std::endl;
        return;
    }
    catch (std::exception &) {}
    try
    {
        (void)dynamic_cast<B &>(p);
        std::cout << "B" << std::endl;
        return;
    }
    catch (std::exception &) {}
    try
    {
        (void)dynamic_cast<C &>(p);
        std::cout << "C" << std::endl;
    }
    catch (std::exception &) {}
}
```

- The exception thrown is `std::bad_cast`, but **`std::bad_cast` is declared in `<typeinfo>`**, which is banned. Catch its base class `std::exception &` (from `<exception>`) instead. The ban on `<typeinfo>` turns this into a CPP05 review: catch by base reference.
- `(void)` discards the cast result. You only care whether it threw.
- **Don't write `identify(&p)` inside it.** That creates a pointer, which the subject forbids.

### `generate`

```cpp
Base *generate(void)
{
    switch (std::rand() % 3)
    {
        case 0:  return new A;
        case 1:  return new B;
        default: return new C;
    }
}
```

Call `std::srand(static_cast<unsigned int>(std::time(NULL)))` **once**, in `main`. The cast matters in this module: `time_t` → `unsigned int` is a narrowing conversion, not a promotion. Remember to `delete` what `generate` returns. The virtual destructor makes that correct.

### ⚠️ Gotchas
- `grep -r typeinfo .` before you push. The grading sheet: *"the header `<typeinfo>` must not appear anywhere."*
- Don't identify the type through a virtual `name()` method. The exercise is about `dynamic_cast`, and `Base` must have *only* the destructor.

---

## 7. What the evaluator checks

From the official 42evalhub grading sheet:

**Prerequisites**
- [ ] `c++ -Wall -Wextra -Werror`, C++98, no C++11 features.
- [ ] No function body in a header (templates excepted). The Makefile uses `c++` with the flags.
- [ ] No `*alloc`, `*printf` or `free`. No `using namespace`. No `friend`. No external library.
- [ ] No leaks.

**ex00: static_cast**
- [ ] A class with a **private constructor** and **static methods**.
- [ ] Conversions use **`static_cast`**. Implicit casts are only accepted for promotions.
- [ ] The program works as required. The evaluator is lenient on exact formatting "if the spirit is respected".

**ex01: reinterpret_cast**
- [ ] A class with a private constructor and static methods.
- [ ] `reinterpret_cast` is used **twice**: `Data*` → `uintptr_t`, then `uintptr_t` → `Data*`.
- [ ] The resulting `Data` is usable.

**ex02: dynamic_cast**
- [ ] The real type is identified with `dynamic_cast`.
- [ ] `identify(Base*)` checks for **NULL**.
- [ ] `identify(Base&)` uses **try/catch**.
- [ ] **`<typeinfo>` appears nowhere.**

**Defense questions to prepare**
- "Why `static_cast` and not a C cast?" → It's explicit, grep-able, and refuses dangerous conversions (§2).
- "Why is `reinterpret_cast` needed in ex01?" → Pointer ↔ integer isn't a value conversion.
- "What would happen without the virtual destructor in `Base`?" → `dynamic_cast` wouldn't compile, and `delete` through a `Base*` would be UB.

---

## 8. Cheat sheet

| Need | C++98 spelling |
|---|---|
| Narrowing numeric conversion | `static_cast<int>(d)` after a range check |
| NaN test without C++11 | `d != d` |
| Pointer → integer and back | `reinterpret_cast<uintptr_t>(p)`, `reinterpret_cast<T *>(u)` with `<stdint.h>` |
| "Is this Base really a B?" (pointer) | `if (B *b = dynamic_cast<B *>(p))` → NULL on failure |
| Same, with a reference | `dynamic_cast<B &>(r)` inside `try`, `catch (std::exception &)` |
| Class that can't be instantiated | Private ctor, copy ctor, `operator=` and dtor, plus `static` methods |
| Remove const | `const_cast<T *>(p)` (writing through it is UB if the object was originally const) |

**What this module leaves you with:** every conversion in your code now states its *intent*. When a review asks "what did you mean here?", the name of the cast answers.

➡️ Next: [`CPP07.md`](CPP07.md). Instead of converting between types, you write code that doesn't care about the type at all.
