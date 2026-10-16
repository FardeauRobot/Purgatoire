# `union` — One Memory Slot, Several Costumes

> **TL;DR.** A `union` is like a `struct`, except **all members share the same bytes**. Its size is the size of its largest member. Only the member you wrote last holds a valid value — reading a different one is type-punning, and in C++ that's undefined behavior (unlike C, which tolerates it).

Related: [`STRUCT.md`](STRUCT.md) · [`SIZEOF.md`](SIZEOF.md) · [`CASTS.md`](CASTS.md) (reinterpret_cast — the other punning tool)

---

## 1. The shape

```cpp
union Value {
    int    i;
    float  f;
    char   c[4];
};              // sizeof(Value) == 4 — they overlap
```

```
   struct { int i; float f; }      union { int i; float f; }
   ┌────────┬────────┐             ┌────────┐
   │   i    │   f    │  8 bytes    │  i / f │  4 bytes
   └────────┴────────┘             └────────┘
      side by side                   same bytes, one at a time
```

```cpp
Value v;
v.i = 42;          // v holds an int
v.f = 3.14f;       // now it holds a float — the int is gone
```

## 2. Why it exists

- **C interop / syscalls** — e.g. `epoll_data_t` in the kernel API is a union.
- **Memory-tight variants** — one field that is *either* an int *or* a pointer, tagged by hand:

```cpp
struct Token {           // the "tagged union" pattern
    enum Kind { NUMBER, WORD } kind;
    union {
        int   num;       // valid when kind == NUMBER
        char *word;      // valid when kind == WORD
    } as;
};
```

The `enum` tag is the discipline: check it before reading. The language does **not** check for you.

## 3. C++98 restrictions

- Members can't have constructors, destructors, or assignment operators — no `std::string` in a union. (POD types only.)
- Can't be a base class, can't have virtual functions.
- Reading a member other than the last written: **UB** in C++. The infamous float-bits trick —

```cpp
union { float f; int i; } u;
u.f = 1.0f;
std::cout << u.i;    // ⚠️ works everywhere in practice, UB on paper
```

— is how people inspect float bit patterns, but the sanctioned C++ tool is `memcpy`... which is off-limits at 42 anyway. Know it, don't ship it.

## 4. In 42 practice

You will read unions (kernel structs, old C code) far more than you'll write them. If you're tempted to write one to save memory in the CPP modules — don't; clarity wins at this scale. It exists in your toolbox for **C interop** and for understanding what `std::variant` (C++17) later civilized.
