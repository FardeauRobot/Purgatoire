%%  %%# `sizeof` — A Compile-Time Question About Type Layout

> **TL;DR.** `sizeof(expr)` returns the number of bytes that an object of `expr`'s type occupies in memory. It is a **compile-time** operator (the expression is not evaluated, only its type), and the result is a `std::size_t` (an unsigned integer type wide enough to address the largest object on the platform).

Related: [`CLASS.md`](CLASS.md) · [`STRUCT.md`](STRUCT.md) · [`MEMORY.md`](../fundamentals/MEMORY.md)

---

## 1. The two forms

```cpp
sizeof(int);             // operand is a TYPE — parens required
sizeof(myVar);           // operand is an EXPRESSION — parens optional but conventional
sizeof  myVar;           // legal; rare
sizeof(arr) / sizeof(arr[0]);   // common idiom: number of array elements
```

- [ ] The result type is `std::size_t` (an unsigned integer in `<cstddef>` or any standard header).

---

## 2. The compile-time evaluation

```cpp
sizeof(crash())          // crash() is NEVER called!
                         // sizeof asks the compiler "what type does crash() return?"
                         // and reports its size. The expression is not executed.
```

This is fundamental: `sizeof` works on the **static type**, computed at compile time. There's no runtime call, no overhead — just a constant inserted into the code.

```
   source:  int n = sizeof(int);

   compiler sees: int (4 bytes on this platform)

   emitted asm:  mov dword [n], 4         ; just an immediate constant
```

That's why `sizeof` works even on functions that would crash if called, on incomplete types declared but not defined, etc. — *as long as the type is known at the point of use*.

---

## 3. What `sizeof` measures

For built-in types: the byte size of the storage.

```
   sizeof(char)     == 1   (always — by definition)
   sizeof(bool)     == 1   (typical, but implementation-defined; could be larger)
   sizeof(short)    >= 2
   sizeof(int)      >= 2   (4 on most modern systems)
   sizeof(long)     >= 4   (8 on x86_64 Linux/macOS, 4 on Windows)
   sizeof(long long)>= 8
   sizeof(float)    == 4   (typically; IEEE 754 single)
   sizeof(double)   == 8   (typically; IEEE 754 double)
   sizeof(void*)    == 8   on x86_64; 4 on x86; varies on embedded
```

The standard guarantees: `sizeof(char) == 1` and `sizeof(short) <= sizeof(int) <= sizeof(long)`. Beyond that it's the platform's call.

For arrays: total bytes (number of elements × element size):

```cpp
int arr[10];
sizeof(arr);                      // 40   (10 * sizeof(int) = 10 * 4)
sizeof(arr) / sizeof(arr[0]);     // 10   (number of elements idiom)
```

For structs/classes: bytes of the layout (members + padding):

```cpp
struct Mixed { char c; int i; };
sizeof(Mixed);                    // 8   (1 + 3 padding + 4)
```

For pointers: pointer size, **not** the size of what it points at:

```cpp
int  arr[10];
int *p = arr;
sizeof(p);                        // 8 on x86_64 — pointer size
sizeof(*p);                       // 4 — int size
```

### sizeof(empty class) >= 1

```cpp
class Empty {};
sizeof(Empty);                    // 1 — never zero
```

So that distinct empty objects have distinct addresses. Empty base optimization may eliminate this in inheritance.

---

## 4. Padding and alignment — `sizeof` reflects them

```cpp
struct A {
    char  c;        // 1
    int   i;        // 4
};                   // sizeof == 8 (1 + 3 padding + 4)

struct B {
    int   i;        // 4
    char  c;        // 1
};                   // sizeof == 8 (4 + 1 + 3 tail padding)
```

```
   address →    0    1    2    3    4    5    6    7
   A:          ┌────┬────┬────┬────┬────┬────┬────┬────┐
               │ c  │ pd │ pd │ pd │ i.0│ i.1│ i.2│ i.3│
               └────┴────┴────┴────┴────┴────┴────┴────┘
   B:          ┌────┬────┬────┬────┬────┬────┬────┬────┐
               │ i.0│ i.1│ i.2│ i.3│ c  │ pd │ pd │ pd │
               └────┴────┴────┴────┴────┴────┴────┴────┘
```

The trailing padding ensures arrays of the struct work — every element starts at the right alignment.

---

## 5. `sizeof` and pointers — the array-decay trap

```cpp
int  arr[10];

void f(int parr[10]) {           // parr looks like an array but it's a pointer!
    sizeof(parr);                 // 8 — pointer size, NOT the array
}

sizeof(arr);                      // 40 in the function that owns the array
```

When you pass an array to a function, it **decays to a pointer**. The `sizeof` you see inside the function is the pointer size, not the array bytes. This is one of the oldest C/C++ gotchas.

To know the size inside a function, pass it explicitly:

```cpp
void f(int *p, std::size_t n) { ... }
```

In C++ pass `std::vector<int>&` and ask `.size()`.

---

## 6. `sizeof` cannot be used on incomplete types

```cpp
class Forward;            // forward declaration
sizeof(Forward);          // ERROR — class size unknown
```

Forward declarations are useful for pointers/references but not when you need the size.

```cpp
extern int huge[];        // size unknown here
sizeof(huge);             // ERROR
```

If the array's size is fixed elsewhere, you'd need to know it locally:

```cpp
extern int huge[100];
sizeof(huge);             // 400 — size is known
```

---

## 7. `sizeof` an expression — type only, no evaluation

```cpp
int *p = 0;
sizeof(*p);               // 4 — even though *p would dereference null!
                          //     no actual deref happens, only type is examined.

int a;
sizeof(a++);              // 4 — a is NOT incremented.
```

`sizeof` looks at the type of the expression and reports its size. The expression itself is **discarded** — no side effects, no function calls, no actual access.

This makes `sizeof` safe to use on any well-typed expression, even ones that wouldn't run.

---

## 8. `sizeof...` (variadic templates) — C++11+, not 42

In C++11:

```cpp
template <typename... Ts>
void f(Ts... args) {
    std::cout << sizeof...(args);     // count of arguments
}
```

Different operator (note the dots). Not in C++98.

---

## 9. Hardware/codegen view

```
   sizeof is folded by the compiler into a constant.
   no runtime cost. no symbol references.

   source:    char buf[sizeof(int) * 10];
   asm:       sub rsp, 40                 ; just a stack reservation
                                          ; the compiler computed 40 at compile time
```

The size depends on **target architecture**. Cross-compiling for ARM, embedded, or 32-bit gives different `sizeof` values. Code that hardcodes `sizeof(int) == 4` may break on platforms where it isn't.

---

## 10. Tips & tricks

### 10.1 Element count of an array

```cpp
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

int arr[10];
ARRAY_SIZE(arr);          // 10
```

Caveat: works only on **arrays**, not pointers. Inside a function with a pointer parameter, it gives the wrong answer (1 or 2 depending on element size).

In modern C++ use `std::size(arr)` (C++17) or `std::extent` for arrays.

### 10.2 Compute struct layout for serialization

```cpp
struct PacketHeader {
    uint8_t  version;
    uint32_t length;
};

std::cout << "header bytes: " << sizeof(PacketHeader);   // padding-included size
```

If you're shipping bytes over a network, `sizeof` includes platform-dependent padding — you usually want to define a packed layout explicitly (see [`STRUCT.md`](STRUCT.md)).

### 10.3 Generic memory allocation

```cpp
T *p = new T[n];                          // C++ way — uses sizeof(T) internally
T *q = (T*)std::malloc(n * sizeof(T));    // C-style; rare in C++
```

### 10.4 Never use `sizeof` to count chars in a string

```cpp
const char *s = "hello";
sizeof(s);                // 8 — pointer size
sizeof("hello");          // 6 — array size: 'h','e','l','l','o','\0'
strlen(s);                // 5 — characters until NUL
```

For string length use `std::strlen` (C-string) or `.length()` (`std::string`).

### 10.5 The "char[1] == sizeof's anchor" trick

```cpp
char arr[sizeof(MyClass)];        // a buffer big enough for one MyClass
new (arr) MyClass();              // placement new
```

Pattern used in optional-types, small-buffer optimizations, etc. Niche but powerful.

### 10.6 sizeof bitfields — undefined for bitfields directly

```cpp
struct Flags {
    unsigned int a : 1;
    unsigned int b : 1;
};

sizeof(Flags);            // 4 — width of the underlying int
sizeof(Flags::a);         // ERROR — can't take sizeof of a bitfield
```

---

## 11. Common errors

| Error | Cause | Fix |
|---|---|---|
| `sizeof` of a function-parameter array gives 8 (pointer size) | Array decays to pointer when passed | Pass size separately, or use a vector / std::array |
| `sizeof(incomplete type)` | Used on a forward-declared class | Include the full definition |
| Wrong byte count between platforms | `sizeof(int)` differs on 16/32/64-bit targets | Use `<cstdint>` types: `int32_t`, `int64_t` etc. |
| `sizeof(void)` | `void` has no size | You can't — `sizeof(void)` is illegal in standard C++ |

---

## 12. Visual summary

```
              ┌────────────────────────────────────────────┐
              │   sizeof(type)   or   sizeof(expression)    │
              │                                              │
              │   compile-time. result: std::size_t.        │
              │   no evaluation; expression's TYPE is read. │
              └────────────────────┬───────────────────────┘
                                   │
              ┌────────────────────┼───────────────────────┐
              ▼                    ▼                       ▼
        scalar types            arrays                  user types
        ─────────────────       ──────────────          ──────────
        sizeof(int)   ≈ 4       sizeof(int[10])         layout-driven:
        sizeof(double)≈ 8       = 40                    members + padding
        sizeof(void*) ≈ 8       sizeof(arr)/            polymorphic classes
                                sizeof(arr[0])          add 8 (vptr)

        gotchas:
        - array decays to pointer when passed → sizeof(arg) == 8
        - empty class sizeof >= 1
        - cannot take sizeof of incomplete type
        - cannot take sizeof of bitfield
```

---

## 13. Practice

1. Why is `sizeof(crash())` legal even though calling `crash()` would terminate the program? *(`sizeof` doesn't evaluate the operand; it only looks at the type.)*
2. Why does `sizeof(arr)` inside a function (with `int arr[]` parameter) return the pointer size rather than the array size? *(Array-to-pointer decay on parameter passing.)*
3. Why does `sizeof(class C{})` give 1 rather than 0? *(So distinct empty objects have distinct addresses.)*
4. Why does the layout of `struct {char c; int i;}` give 8 bytes, not 5? *(Padding for alignment: 1 byte for `c`, 3 padding, 4 for `i`. Tail padding may also apply.)*
