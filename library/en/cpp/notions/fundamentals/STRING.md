# `std::string` — How the Type Really Works

> **TL;DR.** A `std::string` is a `basic_string<char>`: an object that **owns a resizable, null-terminated buffer of `char`**. Think of it as a `vector<char>` that guarantees a trailing `'\0'` so `c_str()` is free. It manages its own memory — you never `new`/`delete` the buffer, and copying a string deep-copies its contents. On your machine (Apple Silicon → **libc++**) it uses the **small-string optimization**: short strings live *inside* the object with no heap allocation at all.

Related: [`STRING_FUNCTIONS.md`](../io-errors/STRING_FUNCTIONS.md) (full API cheat sheet) · [`VECTOR.md`](../containers/VECTOR.md) · [`MEMORY.md`](MEMORY.md) · [`REFERENCE.md`](REFERENCE.md) · [`BASICS.md`](BASICS.md)

Header: `#include <string>`

> This note is the *how it works* companion. For the **exhaustive method list** (every `find`/`insert`/`replace`/`compare` overload), use [`STRING_FUNCTIONS.md`](../io-errors/STRING_FUNCTIONS.md). Here we cover the mental model, memory layout, growth, and the C++98 gotchas.

---

## Table of Contents

1. [Mental model — what a string really is](#1.%20Mental%20model%20%E2%80%94%20what%20a%20string%20really%20is)
2. [`std::string` vs `char*` — the C bridge](#2.%20std%3A%3Astring%20vs%20char%20%E2%80%94%20the%20C%20bridge)
3. [The buffer: SSO vs COW](#3.%20The%20buffer%3A%20SSO%20vs%20COW)
4. [How it grows](#4.%20How%20it%20grows)
5. [The null terminator, `c_str()` and `data()`](#5.%20The%20null%20terminator%2C%20c_str%28%29%20and%20data%28%29)
6. [Value semantics — copies are deep](#6.%20Value%20semantics%20%E2%80%94%20copies%20are%20deep)
7. [Complexity cheat sheet](#7.%20Complexity%20cheat%20sheet)
8. [Invalidation — when pointers and iterators go stale](#8.%20Invalidation%20%E2%80%94%20when%20pointers%20and%20iterators%20go%20stale)
9. [Efficiency tips](#9.%20Efficiency%20tips)
10. [C++98 caveats](#10.%20C%2B%2B98%20caveats)
11. [Gotchas](#11.%20Gotchas)

---

## 1. Mental model — what a string really is

`std::string` is not a keyword and not a primitive. It is an ordinary class from the standard library — a `typedef`:

```cpp
namespace std {
    typedef basic_string<char> string;   // roughly
}
```

Conceptually the object holds three things:

```
   ┌─────────────────────────────────────┐
   │  std::string                         │
   │    pointer  ──────────►  heap buffer │   "hello\0"
   │    size      = 5                     │
   │    capacity  = 22                    │
   └─────────────────────────────────────┘
```

- **size** — how many `char`s are logically stored (what `size()`/`length()` return).
- **capacity** — how many `char`s fit before it must reallocate (`capacity()`).
- **pointer** — where the bytes live.

This is *exactly* the `vector<T>` model from [`VECTOR.md`](../containers/VECTOR.md), specialised to `char`, with one extra promise: **there is always a `'\0'` one past the last character**, so handing the buffer to a C function is O(1).

The whole point: you get a growable text buffer that **cleans up after itself**. When the string goes out of scope, its destructor frees the buffer. That is RAII (see [`MEMORY.md`](MEMORY.md)) — the reason you never leak a `std::string` the way you leak a hand-rolled `char*` in C.

---

## 2. `std::string` vs `char*` — the C bridge

You already know the C way. Line these up:

| Task | C (`char*`) | C++ (`std::string`) |
|---|---|---|
| Declare + init | `char *s = strdup("hi");` | `std::string s = "hi";` |
| Length | `strlen(s)` — O(n) scan | `s.size()` — O(1), it's stored |
| Concatenate | `strcat` into a big-enough buffer | `s += " there";` — grows automatically |
| Compare | `strcmp(a, b) == 0` | `a == b` |
| Copy | `malloc` + `strcpy`, remember to free | `std::string b = a;` — deep copy, auto-freed |
| Free | `free(s);` (and never double-free) | *nothing* — destructor does it |
| Substring | manual pointer math + null-term | `s.substr(pos, len)` |

Two things move from *your problem* to *the library's problem*: **memory ownership** and **length bookkeeping**. That is the entire value proposition. The trade is that a `std::string` is a fatter object (24 bytes here, vs an 8-byte pointer) and may own a heap allocation.

You can always drop back to C with `s.c_str()` when calling a C API (`open`, `execve`, etc. — relevant in webserv).

---

## 3. The buffer: SSO vs COW

Here is the subtlety that trips people up: **the C++ standard does not say how `std::string` stores its bytes.** Two historical strategies exist, and *which one you get depends on your standard library, not on `-std=c++98`.*

### Small-String Optimization (SSO) — what libc++ does (your machine)

The string object reserves a small inline buffer *inside itself*. Short strings are stored there, with **zero heap allocation**. Only when the text outgrows the inline buffer does it allocate on the heap.

Measured on your setup (`c++ -std=c++98`, Apple Clang + libc++):

```
sizeof(std::string) == 24 bytes
empty string capacity() == 22      ← 22 chars fit inline, no malloc
```

```
   Short string "hello" — lives ENTIRELY inside the 24-byte object:

   ┌───────────────────────────────────────────────┐
   │ h e l l o \0 . . . . . . . . . . . . . . . [sz]│   no heap, no new
   └───────────────────────────────────────────────┘
              (inline buffer, ~22 usable chars)

   Long string (>22 chars) — object holds a pointer to the heap:

   ┌──────────────────────────┐
   │ ptr ─► ┌──────────────────────────────────┐   │
   │ size   │ this is a long string ...... \0   │   │  heap-allocated
   │ cap    └──────────────────────────────────┘   │
   └──────────────────────────┘
```

Consequence: building small strings (typical in the C++ modules) often costs **no allocation at all** — great for performance, and it means valgrind won't show a heap block for them.

### Copy-On-Write (COW) — the old libstdc++ way

Older GCC/libstdc++ (pre-C++11 ABI) used reference-counted, copy-on-write strings: copying a string just bumped a refcount and shared the buffer; the deep copy happened lazily on the first write. You may read about this in old tutorials. **libc++ never did this**, and modern libstdc++ dropped it. You don't have it — but knowing the name explains why some old advice ("copying strings is free!") no longer holds.

> **Didactic point for eval defense:** if an evaluator asks "does `std::string b = a;` allocate?" the honest answer is *"it depends on the implementation and the length — on libc++ a short string copies inline with no allocation; a long one deep-copies the heap buffer."* That nuance is exactly the kind of thing that shows you understand the type rather than memorised a slogan.

---

## 4. How it grows

When you append past `capacity()`, the string does the same dance as `vector`:

1. allocate a bigger buffer,
2. copy the existing chars over,
3. free the old buffer,
4. update pointer/size/capacity.

Measured growth on your machine (pushing chars one at a time, printing capacity at each reallocation):

```
22 → 47 → 95 → 191 → 383 → ...
```

That is **geometric growth (~×2)**, rounded so the allocation lands on a 16-byte boundary (libc++ reports `capacity = allocated_bytes - 1`, reserving one byte for the `'\0'`). Geometric growth is what makes `push_back`/`+=` **amortised O(1)**: most appends are a cheap write, and the occasional expensive reallocation is rare enough to average out.

If you know the final size, tell the string up front to skip the reallocations:

```cpp
std::string s;
s.reserve(1024);        // one allocation, no growth churn afterwards
for (int i = 0; i < 1000; ++i)
    s += 'x';           // never reallocates
```

`reserve(n)` sets capacity to at least `n` without changing `size()`. Handy in webserv when you're accumulating a response body of known length.

---

## 5. The null terminator, `c_str()` and `data()`

A `std::string` keeps a `'\0'` just past its last character so it can hand a valid C-string to legacy APIs in O(1):

```cpp
std::string path = "index.html";
int fd = open(path.c_str(), O_RDONLY);   // c_str() → const char*
```

Two access points, both `const char*` in C++98:

- **`c_str()`** — guaranteed null-terminated. Use this for C APIs.
- **`data()`** — pointer to the internal bytes. In C++98 it was *not* guaranteed null-terminated (that promise was added in C++11); prefer `c_str()` when you need the `'\0'`.

Critical rule: **the returned pointer is only valid until the next non-const operation on the string.** Any `+=`, `insert`, `resize`, etc. may reallocate and leave your pointer dangling:

```cpp
const char *p = s.c_str();
s += "more";               // may reallocate — p is now DANGLING
std::cout << p;            // undefined behaviour
```

Never cache `c_str()`; call it at the point of use.

Also note: a `std::string` **can contain embedded `'\0'` bytes** (it stores an explicit size, unlike a C-string). But `c_str()` will look terminated at the first `'\0'` to a C function. Keep that in mind if you ever store binary data in a string.

---

## 6. Value semantics — copies are deep

`std::string` behaves like a value, not a reference/handle:

```cpp
std::string a = "hello";
std::string b = a;     // b is an INDEPENDENT copy
b[0] = 'H';
std::cout << a;        // "hello"  — a is untouched
```

This is the opposite of a C `char*`, where `b = a` aliases the same buffer. Because copies are deep, **passing strings by value copies the whole buffer** — so pass by `const&` when you don't need a copy (see [`REFERENCE.md`](REFERENCE.md)):

```cpp
void log(const std::string& msg);   // no copy
void log(std::string msg);          // copies the buffer every call
```

This is why every function in the C++ modules that just *reads* a string takes `const std::string&`.

---

## 7. Complexity cheat sheet

| Operation | Complexity | Notes |
|---|---|---|
| `size()`, `length()`, `empty()` | O(1) | stored, not scanned |
| `operator[]`, `at()` | O(1) | random access |
| `c_str()`, `data()` | O(1) | already null-terminated |
| `push_back`, `+=`, `append` | amortised O(1) | O(n) on the reallocation |
| `insert`, `erase` (middle) | O(n) | shifts the tail |
| `find`, `rfind`, `find_first_of` | O(n·m) | naive substring search |
| `substr` | O(len) | allocates a new string |
| `compare`, `==`, `<` | O(n) | lexicographic |
| copy construct / assign | O(n) | deep copy (O(1) if SSO short) |
| `swap` | O(1) | swaps the internals |

---

## 8. Invalidation — when pointers and iterators go stale

Any operation that **changes size or capacity** may reallocate the buffer, which invalidates:

- pointers/references from `operator[]`, `at()`, `front()`, `back()`,
- pointers from `c_str()` / `data()`,
- all iterators.

```cpp
std::string s = "abc";
char& first = s[0];
s += "xyz...long enough to reallocate...";
first = 'Z';               // DANGLING reference — UB
```

Rule of thumb: **treat any reference/pointer/iterator into a string as invalid after you mutate that string.** Same discipline as `vector` ([`VECTOR.md`](../containers/VECTOR.md) §8).

---

## 9. Efficiency tips

1. **Pass by `const std::string&`** to avoid deep copies (§6).
2. **`reserve()` when the final size is known** to skip repeated reallocations (§4).
3. **Prefer `+=` / `append` over chained `operator+`** in loops — `a + b + c` builds temporaries; `s += a; s += b;` reuses one buffer.
4. **Use `empty()` not `size() == 0`** — clearer, and always O(1).
5. **Compare with `==`, not `strcmp(a.c_str(), b.c_str())`** — the operator is direct and knows the sizes.
6. **`swap` to shrink**: in C++98 there's no `shrink_to_fit`; `std::string(s).swap(s)` forces a right-sized copy (rarely needed).

---

## 10. C++98 caveats

The 42 subject is **C++98 only**. These common conveniences are **C++11+ and forbidden**:

| Not available in C++98 | Do this instead |
|---|---|
| `std::to_string(n)` | `std::ostringstream oss; oss << n; oss.str();` |
| `std::stoi(s)` / `stol` / `stod` | `std::istringstream iss(s); iss >> n;` |
| `s.front()` / `s.back()` | `s[0]` / `s[s.size() - 1]` |
| `s.pop_back()` | `s.erase(s.size() - 1)` |
| range-`for` (`for (char c : s)`) | classic iterator or index loop |
| `data()` guaranteed null-terminated | use `c_str()` |
| `s += {…}` / brace init | plain `+=` / constructor calls |

Number-to-string and back go through `<sstream>` — see [`FSTREAM_GUIDE.md`](../io-errors/FSTREAM_GUIDE.md).

---

## 11. Gotchas

- **`operator[]` does not bounds-check** — out-of-range is UB. Use `at()` (throws `std::out_of_range`) when the index may be invalid.
- **`find` returns `size_t`, and "not found" is `std::string::npos`** — always compare to `std::string::npos`, never to `-1`.
- **`size()` is unsigned.** `s.size() - 1` on an empty string is a huge number, not `-1`. Guard with `if (!s.empty())` before subtracting.
- **`substr(pos)` throws if `pos > size()`** — but `pos == size()` is legal and returns `""`.
- **Two string literals can't be concatenated with `+`:** `"a" + "b"` is pointer arithmetic and won't compile. At least one operand must be a `std::string`: `std::string("a") + "b"`.
- **Don't cache `c_str()`** across a mutation — the pointer dangles (§5).
- **A copy is deep and may allocate.** Passing by value in a hot path silently copies the buffer; use `const&`.

---

### See also

- [`STRING_FUNCTIONS.md`](../io-errors/STRING_FUNCTIONS.md) — every method, grouped and with signatures.
- [`FSTREAM_GUIDE.md`](../io-errors/FSTREAM_GUIDE.md) — `stringstream` for number↔string in C++98.
- [`VECTOR.md`](../containers/VECTOR.md) — same buffer/growth model, generalised.
- [`REFERENCE.md`](REFERENCE.md) — why you pass strings by `const&`.
