# `struct` — A `class` That Hangs Out The Window

> **TL;DR.** In C++, `struct` and `class` create the same kind of type. The **only** differences are: members are `public` by default in `struct`, `private` in `class`; inheritance is `public` by default in `struct`, `private` in `class`. Pick `struct` for plain-old-data; pick `class` when you want encapsulation.

Related: [`CLASS.md`](CLASS.md) · [`PUBLIC_PRIVATE_PROTECTED.md`](PUBLIC_PRIVATE_PROTECTED.md) · [`INHERITANCE.md`](../notions/oop/INHERITANCE.md)

---

## 1. The two-line summary

```cpp
class A {
    int x;            // private (default for class)
};

struct B {
    int x;            // public (default for struct)
};
```

That's the only language-level difference. Both can have constructors, destructors, virtual functions, member functions, access specifiers, inheritance — everything.

```cpp
struct Counter {
    int  count;
    Counter() : count(0) {}
    void inc() { ++count; }
    virtual ~Counter() {}     // yes, virtual destructors work in struct too
};
```

---

## 2. The convention (when to use which)

By **convention** in modern C++:

| Use `struct` when… | Use `class` when… |
|---|---|
| All data is public | Data is private; access via methods |
| It's pure data (POD-ish) | There are invariants to maintain |
| It's a simple aggregate (`Point{x,y}`) | There's behavior, not just data |
| It's a tag type / type traits | There's a copy ctor / OCF |

This convention is enforced by readers, not the compiler. The compiler treats them identically (modulo the default-access rule).

```cpp
struct Point { int x, y; };                          // simple data

class  PhoneBook {                                   // has invariants
    Contact _contacts[8];
    int     _count;
public:
    void add(const Contact&);
    Contact get(int idx) const;
};
```

---

## 3. Inheritance default

The other place the keyword matters:

```cpp
class  Derived1 : Base { … };       // private inheritance (default for class)
struct Derived2 : Base { … };       // public inheritance (default for struct)

class  Derived3 : public Base { … };       // explicit — preferred for clarity
struct Derived4 : public Base { … };       // explicit — explicit even though redundant
```

**Always write the access specifier explicitly** when you inherit. Don't rely on the keyword default — it bites you the moment you change `class` to `struct` or vice versa.

See [`INHERITANCE.md`](../notions/oop/INHERITANCE.md).

---

## 4. C-compatibility — the `struct` superpower

A `struct` containing only **trivial** members (no constructors, no virtuals, no member access specifiers, no inheritance) is a **standard-layout type** — it has the same memory layout as the equivalent C struct. You can:

- Pass it to a C library by pointer.
- `memcpy`/`memmove` it freely.
- Read/write it from a binary file.

```cpp
// In C
struct sockaddr_in { ... };

// In C++
extern "C" {
    #include <netinet/in.h>
}

sockaddr_in addr;        // identical layout
bind(fd, (sockaddr*)&addr, sizeof(addr));
```

The moment you add a constructor, virtual function, or non-default member, you may break C compatibility — the compiler is free to reorder things or insert a vtable pointer.

---

## 5. POD (Plain Old Data) — the C-compatibility frontier

In C++98, a **POD** is a type that:

- Has no user-declared constructors, destructor, copy assignment.
- Has no virtual functions.
- Has no virtual base classes.
- Has only POD non-static members.
- Has no private/protected non-static members.
- Has no references as members.

```cpp
struct PodPoint {
    int x;
    int y;
};                        // POD — safe to memcpy, fread/fwrite, etc.

struct NonPodPoint {
    int x, y;
    NonPodPoint() : x(0), y(0) {}    // user ctor → not POD anymore
};
```

For PODs, all of these are guaranteed:
- `memcpy` between two of them works.
- Default-constructed values are *uninitialized* (no implicit zero-init).
- Layout matches a C struct of the same members.

Non-PODs may still be safely copied — but only via the copy constructor, not `memcpy`.

In C++11+ this got refined into "trivially copyable" and "standard-layout" — but for C++98/42 the POD term is what you'll see.

---

## 6. Memory layout — same as `class`

A `struct` lays out members in declaration order with alignment padding, exactly like [`class`](CLASS.md):

```cpp
struct Pixel {
    unsigned char r;     // 1
    unsigned char g;     // 1
    unsigned char b;     // 1
    unsigned char a;     // 1
};                        // sizeof = 4 — perfectly packed
```

```
   address →   0    1    2    3
              ┌────┬────┬────┬────┐
              │ r  │ g  │ b  │ a  │
              └────┴────┴────┴────┘
   no padding because all members are 1-byte aligned
```

Compare to a struct with mixed alignment:

```cpp
struct Mixed {
    char  flag;          // 1
    int   value;         // 4 (must align to 4)
};                        // sizeof = 8 (1 + 3 pad + 4)
```

```
   address →   0    1    2    3    4    5    6    7
              ┌────┬────┬────┬────┬────┬────┬────┬────┐
              │flag│ pd │ pd │ pd │  value (32-bit)   │
              └────┴────┴────┴────┴───────────────────┘
```

Reorder for size:

```cpp
struct Mixed2 {
    int   value;         // 4
    char  flag;          // 1
};                        // sizeof = 8 (4 + 1 + 3 tail pad — same total here)
```

Tail padding rounds up to the alignment of the strictest member, so `sizeof` is a multiple of that alignment. This makes arrays of the struct work without per-element padding.

### Inspect padding

```cpp
#include <cstddef>      // for offsetof
struct S { char a; int b; };
std::cout << offsetof(S, a);      // 0
std::cout << offsetof(S, b);      // 4
std::cout << sizeof(S);           // 8
```

### `#pragma pack` — break the alignment rules

For wire-protocol parsing, you sometimes need a struct with no padding:

```cpp
#pragma pack(push, 1)
struct PacketHeader {
    char     version;
    uint32_t length;     // unaligned!
};
#pragma pack(pop)
```

Now the struct is 5 bytes (no padding). Reading `length` may be slow on hardware that can't handle unaligned access (some ARM cores), and may be UB on some platforms. Don't do this without a real protocol reason.

---

## 7. Aggregates and brace-initialization

A `struct` (or `class`) with no user-declared constructors, no private/protected non-static members, and no base classes is an **aggregate** — and can be brace-initialized:

```cpp
struct Point { int x; int y; };
Point p = {3, 4};                  // OK — aggregate init
Point q = {3};                     // OK — y default-initialized to 0
```

Once you add a user-defined constructor, this stops working:

```cpp
struct PointC { int x, y; PointC() : x(0), y(0) {} };
PointC p = {3, 4};                 // ERROR in C++98
PointC p(3, 4);                    // requires a matching ctor
```

Aggregate init is a useful pattern for config structs and bag-of-data types.

---

## 8. `struct` vs `class` style guide

Use `struct` for:
- Plain data carriers (`Point`, `Color`, `Date`).
- C-compatibility layers.
- Tag types (`std::random_access_iterator_tag`).
- Function-object-only types (`struct Less { bool operator()(int a, int b) const { return a<b; } };`).

Use `class` for:
- Anything with invariants.
- Anything with a non-trivial OCF.
- Most "real" classes in 42 modules (Contact, PhoneBook, Animal, etc.).

The compiler doesn't care; **readers** parse the choice as a hint.

---

## 9. C-style `typedef struct` is unnecessary

C:

```c
typedef struct s_node {
    int   data;
    struct s_node *next;
} t_node;
```

C++:

```cpp
struct Node {
    int   data;
    Node *next;       // 'Node' is already a type — no 'struct Node'
};
```

You don't need `typedef` and you don't need the trailing tag. Just `struct Name { … };`.

---

## 10. Tips & tricks

### 10.1 Forward declare with `struct` or `class` — the linker doesn't care

```cpp
struct Foo;             // forward
class  Foo;             // also forward, refers to same type
```

But pick one and be consistent in your codebase. Some compilers warn on the mismatch.

### 10.2 Trivial types are zero-cost

```cpp
struct Vec2 { float x, y; };
Vec2 a, b;
Vec2 c = a;             // single 8-byte copy — same speed as struct copy in C
```

The C++ overhead you sometimes hear about (vtables, copy ctors, etc.) only kicks in when you opt into those features.

### 10.3 Returning small structs is fine

```cpp
struct Point { int x, y; };
Point makePoint() { Point p = {1, 2}; return p; }
```

Modern compilers do **return value optimization (RVO)** — the caller's destination is passed in as a hidden pointer, the function fills it, no copy happens.

### 10.4 Anonymous structs (don't, in C++98 standard)

```cpp
struct {
    int x, y;
} g;             // works in C; technically non-standard in C++98 but compilers accept it
```

You won't see this in 42 norm. Avoid.

### 10.5 Bit-fields work in `struct` and `class`

```cpp
struct Flags {
    unsigned int red    : 1;
    unsigned int green  : 1;
    unsigned int blue   : 1;
    unsigned int alpha  : 5;
};                            // 1 byte total (8 bits)
```

Bit-field layout is implementation-defined — never put a bit-field struct on the wire without testing.

---

## 11. Common errors

| Error | Cause | Fix |
|---|---|---|
| `class S has no member named 'x'` after switching `struct`→`class` | Members became private | Add `public:`, or write `class S { public: int x; };` |
| `cannot access private member ...` after switching base from `struct` to `class` | Inheritance default flipped to private | Write `: public Base` explicitly |
| `member 'x' is uninitialized` | POD struct with `S s;` is uninitialized | Brace-init: `S s = {};` |
| `cannot initialize aggregate of type 'S' with brace list` | Class has user-declared ctor → no longer an aggregate | Remove the ctor, or use ctor args |

---

## 12. Visual summary

```
                ┌──────────────────────────────────────┐
                │   keywords: struct / class           │
                │                                      │
                │   identical except for:              │
                │     • default member access          │
                │     • default inheritance access     │
                └──────────────────┬───────────────────┘
                                   │
              ┌────────────────────┼────────────────────┐
              ▼                    ▼                    ▼
        struct conventions     class conventions    practical rule
        ─────────────────      ─────────────────    ──────────────
        plain data             encapsulated         when in doubt,
        public members         private state        write the access
        no invariants          OCF / behavior       specifier
        C-compatible           "real" object        explicitly so
                                                    the keyword choice
                                                    becomes pure style.
```

---

## 13. Practice

1. What's the only language-level difference between `class` and `struct`? *(Default access of members and bases — `private` for `class`, `public` for `struct`.)*
2. Can a `struct` have a constructor and destructor? *(Yes — both can have everything `class` can.)*
3. Why does `Point p = {3, 4};` work for a struct without a ctor but break when you add one? *(Aggregate initialization requires no user-declared ctor.)*
4. Why does `sizeof(struct{char a;int b;})` give 8, not 5? *(Padding for `int` alignment plus tail padding so arrays work.)*
