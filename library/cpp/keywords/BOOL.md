# `bool` / `true` / `false` — Real Boolean Type, Not Just an Int

> **TL;DR.** `bool` is a built-in C++ type with two values: `true` and `false`. Conceptually one bit; in practice **one byte** of storage. Implicit conversion to/from integers is allowed (any nonzero → `true`, zero → `false`), and that's the source of most surprises.

Related: [`CONST.md`](CONST.md) · [`OPERATOR.md`](OPERATOR.md)

---

## 1. The basics

```cpp
bool b1 = true;
bool b2 = false;
bool b3 = (x > 0);           // any expression yielding true/false works

if (b1) { ... }              // direct test — clean
```

C had no `bool`. It used `int` and convention (`0` = false, anything else = true). C++ added a real boolean type along with the `true` and `false` literals.

---

## 2. Memory layout — why one byte, not one bit?

A `bool` could in principle be stored as a single bit. In practice, every C++ implementation makes `sizeof(bool) == 1` because:

- Bytes are individually addressable — bits are not. `&myBool` needs a real address.
- Aligning multiple bool variables on byte boundaries lets the CPU read each one in a single instruction.
- Mixed structs would need expensive bit-extraction logic if booleans were sub-byte.

```
   bool b;
   ┌──────┐
   │ 0x01 │     stored as a full byte (or 0x00 for false)
   └──────┘     1 byte = 8 bits, but only the LSB conventionally used
```

The bit pattern for `true` is implementation-defined, but **conventional** — usually 0x01. `false` is always 0x00.

`std::vector<bool>` is a famous **exception**: it packs booleans into bits, breaking the usual `std::vector<T>` interface (you can't take `&v[0]` for a true `bool*`). Don't rely on it acting like a normal container.

```cpp
struct Flags {
    bool a;        // 1 byte
    bool b;        // 1 byte
    bool c;        // 1 byte
    bool d;        // 1 byte
};                  // sizeof = 4 (compactly packed without padding for char-aligned)

struct Mixed {
    int  i;        // 4
    bool b;        // 1
};                  // sizeof = 8 (4 + 1 + 3 tail padding)
```

---

## 3. Implicit conversions — the trap

```cpp
int n = 42;
bool b = n;                  // OK — true (any nonzero int → true)
bool c = 0;                  // false (only zero → false)

if (n) { ... }               // legal — n != 0 means "true"

bool a = true;
int  i = a;                  // OK — i == 1
int  j = false;              // OK — j == 0
```

This is convenient *and* error-prone:

```cpp
bool ok = false;
ok = process();              // process returns int — caller never knows return value lost detail
```

Or:

```cpp
void f(bool flag);
void g(int  n);

f(42);                       // implicit int → bool → true. surprising?
g(true);                     // implicit bool → int → 1.
```

Most stylistic guidelines recommend testing explicitly:

```cpp
if (n != 0) { ... }          // explicit
if (p != 0) { ... }          // explicit (NULL pointer test)
```

But idiomatic C++ also accepts:

```cpp
if (n) { ... }
if (p) { ... }
```

Pick a style and be consistent.

---

## 4. Boolean operators

```cpp
&&    logical AND, short-circuits
||    logical OR,  short-circuits
!     logical NOT
```

Short-circuit means:

```cpp
if (p != 0 && p->isValid()) { ... }     // safe — p->isValid() not called if p is null
```

The right operand is only evaluated if needed. Don't break this with side-effecting expressions on the right side:

```cpp
if (cheap() || expensive()) { ... }     // expensive() skipped when cheap() is true
```

### `&&` / `||` are NOT bitwise

Easy to confuse with `&` and `|`:

```cpp
bool a, b;
a && b;          // logical: short-circuit, returns true/false
a &  b;          // bitwise: evaluates both, returns true/false (same result for bools but no short-circuit)
```

For `bool` the result is the same value, but the semantics differ. **Use `&&`/`||` for logic** and reserve `&`/`|` for actual bitwise integer operations.

---

## 5. Boolean arithmetic — yes, it's legal

```cpp
bool a = true;
bool b = false;

int sum = a + b + a;        // 1 + 0 + 1 = 2 — legal but weird
```

Adding bools as ints is technically valid because of implicit conversion. Don't write code like this — express intent clearly with explicit casts or counters.

A useful idiom though:

```cpp
int trueCount = 0;
trueCount += (x > 0);       // adds 1 if true, 0 if false
trueCount += (y > 0);
trueCount += (z > 0);
```

Compact way to count satisfied conditions.

---

## 6. Printing bools

```cpp
std::cout << true;            // 1
std::cout << false;           // 0

std::cout << std::boolalpha;  // switch to "true"/"false" mode
std::cout << true;            // true
std::cout << std::noboolalpha;
std::cout << true;            // 1 again
```

`std::boolalpha` from `<ios>` (included by `<iostream>`) toggles human-readable booleans. Sticky — applies to the stream until reset.

---

## 7. Boolean in conditions — the conversion contexts

The C++ standard talks about "contextual conversion to bool." Anywhere a condition is expected (`if`, `while`, `for`, ternary `?:`, `&&`, `||`, `!`), the operand is converted to `bool`. So:

```cpp
int *p = ...;
if (p) { ... }          // p converts to bool: nonzero pointer → true

std::string s = ...;
if (s.empty()) { ... }  // s.empty() returns bool — direct test
```

In your own classes, you can enable this with `operator bool()` (C++98) — but it's a slippery slope:

```cpp
class Smart {
public:
    operator bool() const { return _ptr != 0; }
};

Smart s;
if (s) { ... }          // OK
int n = s + 1;          // ALSO OK in C++98 — implicit bool→int→arithmetic
```

The implicit chain causes subtle bugs. C++11's `explicit operator bool()` fixes this; C++98 doesn't have explicit conversion operators. The 42-norm workaround is to expose a named method like `isValid()` instead.

---

## 8. Tips & tricks

### 8.1 `bool` parameters reduce readability

```cpp
sendEmail(addr, true, false);        // what do these flags mean?
```

Replace with named enums:

```cpp
enum Format { TEXT, HTML };
enum Priority { NORMAL, URGENT };

sendEmail(addr, HTML, NORMAL);       // self-documenting
```

### 8.2 Don't compare to true/false explicitly

```cpp
if (b == true)  { ... }     // verbose
if (b == false) { ... }     // verbose

if (b)  { ... }             // idiomatic
if (!b) { ... }             // idiomatic
```

### 8.3 Avoid `bool` in tight numeric loops

```cpp
bool flags[100];           // 100 bytes
unsigned char flags[100];  // also 100 bytes, but works with bitwise ops
```

For dense bit storage, use `std::bitset<N>` (compile-time size) or `std::vector<bool>` (runtime size — but watch the proxy interface).

### 8.4 Returning bool from `operator==` etc.

```cpp
bool operator==(const C&) const;
bool operator< (const C&) const;
bool operator!=(const C&) const;
```

Standard signatures. Always `bool`.

### 8.5 The `||` / `&&` short-circuit is a feature

```cpp
// safely walk a list
while (node && node->isValid()) {
    process(node);
    node = node->next;
}
```

This is one of the most idiomatic patterns in C and C++. Use it.

### 8.6 Don't overload `&&` / `||` / `,`

Overloaded versions don't short-circuit (both operands are evaluated). Surprising; almost always wrong. See [`OPERATOR.md`](OPERATOR.md).

---

## 9. Common errors

| Error | Cause | Fix |
|---|---|---|
| `bool` argument silently converts from int | Implicit int→bool conversion | Test explicitly (`if (n != 0)`) or use named enums |
| Output is `1`/`0` instead of `true`/`false` | No `std::boolalpha` | `std::cout << std::boolalpha << b;` |
| `vector<bool>` can't return a real `bool*` | Specialized to pack bits | Use `std::vector<char>` if you need a "vector of bools" with usual interface |
| Operator overload doesn't short-circuit | Custom `operator&&` / `operator||` | Don't overload them |

---

## 10. Visual summary

```
              ┌──────────────────────────────────────────────┐
              │   keyword: bool                              │
              │       values: true, false                    │
              │       size:   typically 1 byte               │
              └────────────────────┬─────────────────────────┘
                                   │
              ┌────────────────────┼─────────────────────────┐
              ▼                    ▼                         ▼
        operations              implicit conv         use cases
        ────────────            ─────────────         ──────────
        && ||  short-           bool ↔ int            flags, conditions,
        circuit logical         bool ↔ pointer        return types of
        !      negation         (any non-zero is      predicates,
        true/false              true)                 operator ==/!=/<
        literals
                                CAN be subtle —
                                use !=0 explicit
                                tests when in doubt.

       memory: 1 byte (8 bits)        std::vector<bool> packs to bits
       printing: 1/0 by default       std::boolalpha for true/false
       avoid bool params              prefer named enums
```

---

## 11. Practice

1. Why is `sizeof(bool) == 1` not `sizeof(bool) == 0` (a single bit)? *(Bytes are the smallest addressable unit; bools need addresses for `&b`, parameter passing, etc.)*
2. What does `std::cout << true;` print? *(`1` by default; `true` if you've set `std::boolalpha` on the stream.)*
3. Why does `if (n)` work where `n` is `int`? *(Contextual conversion to bool: zero → false, anything else → true.)*
4. Why is overloading `operator&&` rarely a good idea? *(Overloads lose the short-circuit semantics — both operands are evaluated, which surprises every reader.)*
