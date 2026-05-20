# `std::vector` — The Default Sequence Container

> **TL;DR.** A `vector<T>` is a **dynamically-resizable contiguous array** of `T`. It stores three pointers — `begin`, `end`, `end_of_storage` — and grows by reallocating to a bigger block (typically ×2) when it runs out of room. It is the right choice **by default**; pick `list`, `deque`, `set`, or `map` only when you have a measurable reason.

Related: [`STL.md`](../advanced/STL.md) · [`TEMPLATES.md`](../advanced/TEMPLATES.md) · [`MEMORY.md`](../fundamentals/MEMORY.md) · [`NEW.md`](../keywords/NEW.md) · [`INDEX.md`](INDEX.md)

Header: `#include <vector>`

---

## Table of Contents

1. [Mental model — what a vector really is](#1-mental-model--what-a-vector-really-is)
2. [Construction](#2-construction)
3. [Capacity — `size` vs `capacity`](#3-capacity--size-vs-capacity)
4. [Element access](#4-element-access)
5. [Modifiers](#5-modifiers)
6. [Iterators](#6-iterators)
7. [Complexity cheat sheet](#7-complexity-cheat-sheet)
8. [Iterator invalidation — the silent killer](#8-iterator-invalidation--the-silent-killer)
9. [Efficiency: ten tricks](#9-efficiency-ten-tricks)
10. [Algorithms that pair well with vector](#10-algorithms-that-pair-well-with-vector)
11. [`vector<bool>` — the cursed exception](#11-vectorbool--the-cursed-exception)
12. [When NOT to use vector](#12-when-not-to-use-vector)
13. [C++98 caveats](#13-c98-caveats)
14. [Gotchas](#14-gotchas)

---

## 1. Mental model — what a vector really is

A `vector<T>` is essentially a tiny struct holding three pointers (or a pointer + two sizes — implementations differ):

```
   struct vector_internals {
       T *begin_;         // first element
       T *end_;            // one past the last element
       T *end_of_storage_; // one past the allocated buffer
   };
```

Visually:

```
   vector<int> v with size = 4, capacity = 8

      begin_              end_                end_of_storage_
       │                   │                   │
       v                   v                   v
     ┌───┬───┬───┬───┬───┬───┬───┬───┐
     │ 3 │ 1 │ 4 │ 1 │ . │ . │ . │ . │   ← heap-allocated, contiguous
     └───┴───┴───┴───┴───┴───┴───┴───┘
       0   1   2   3
       └── size = 4 ──┘
       └────── capacity = 8 ──────┘
```

The whole point: **elements live in one contiguous block on the heap**, exactly like the C array `new T[N]` you used in CPP01 — but with automatic growth. Because it is contiguous, you get:

- O(1) random access (`v[i]` = `*(begin_ + i)`).
- Maximum CPU cache friendliness (the prefetcher loves sequential memory).
- A raw pointer to the data via `&v[0]` — interoperable with any C API that wants `T *`.

When you exceed `capacity`, the vector:
1. Allocates a **new, bigger** buffer (typically 2× the old capacity on libc++ / libstdc++).
2. **Copies** (or copy-constructs) every element into the new buffer.
3. Destroys the originals and frees the old buffer.
4. Updates the three pointers.

That reallocation is why `push_back` is *amortized* O(1), not strictly O(1). 99 % of pushes are pointer bumps; 1 % is a full copy. The doubling strategy guarantees the average cost per push stays constant.

### Coming from C — it's a `realloc`'d array, **not** a linked list

The intuition "vector = a mix of C array and linked list" is tempting because both grow on demand. It's misleading. A `std::vector<T>` is essentially a `realloc`-ed C array wrapped in a class — there are no nodes, no `next` pointers, nothing list-like.

If you wrote one in C using libft conventions, it would look almost exactly like this:

```c
typedef struct s_vec {
    int     *data;       // begin_     — heap buffer
    size_t  size;        // logical count
    size_t  capacity;    // physical count
}   t_vec;

void    vec_push_back(t_vec *v, int x)
{
    if (v->size == v->capacity)
    {
        v->capacity = v->capacity ? v->capacity * 2 : 1;
        v->data = realloc(v->data, v->capacity * sizeof(int));
        // ↑ realloc may MOVE the buffer to a new address.
        //   Any pointer you held into the old buffer now dangles.
    }
    v->data[v->size++] = x;
}
```

That's `std::vector<int>` in twelve lines of C. Every `push_back` is `realloc + assign`. Every `&v[0]` is `v->data`. Every `v[i]` is `v->data[i]`.

The growth *mechanism* is opposite to a linked list: a list grows by allocating **one new node** and pointing to it; a vector grows by allocating a **whole new bigger buffer** and copying everything into it.

| Property                                | C array            | libft `t_list *`           | `std::vector`                          |
| --------------------------------------- | ------------------ | -------------------------- | -------------------------------------- |
| Memory layout                           | Contiguous         | Scattered nodes            | **Contiguous** ← like array            |
| Random access `v[i]`                    | O(1)               | O(n)                       | **O(1)** ← like array                  |
| Cache friendliness                      | Excellent          | Poor (each node = a miss)  | **Excellent** ← like array             |
| Can grow on demand?                     | No                 | Yes                        | **Yes** ← like list                    |
| Insert at the **back**                  | N/A                | O(1)                       | Amortized O(1)                         |
| Insert in the **middle**                | N/A                | O(1) (relink)              | **O(n) — must shift** ← like array     |
| Pointer to element stays valid forever? | Yes (until `free`) | Yes (until you free node)  | **No — invalidated on growth**         |

Vector behaves like an array on every column except "can grow." That single linked-list-flavored property is itself implemented array-style (via `realloc`), not list-style (via node chaining).

If you actually want the linked-list analogue, that's [`std::list`](LIST.md) — the same `t_list *` you wrote in libft, just templated and with `prev` pointers added. The real "best of both worlds" is **`std::deque`**: a list of fixed-size arrays. Random access in O(1), push/pop at both ends in O(1), but not contiguous overall:

```
   std::deque<int> internals (conceptually)
   ┌──────────────┐
   │ block_ptrs[] │ → [block A] [block B] [block C] [block D]
   └──────────────┘     (4 ints)  (4 ints)  (4 ints)  (4 ints)
```

> **Mental model:** `std::vector<T>` is a C array on the heap that knows its `size` and `capacity` and can `realloc` itself. The contiguous layout makes it fast. The auto-grow is a convenience built on top. Every gotcha in §8 follows from this single fact.

---

## 2. Construction

C++98 gives you these constructors (and copy-assign):

```cpp
#include <vector>

std::vector<int> a;                            // empty,   size=0, capacity=0
std::vector<int> b(5);                         // 5 elems, value-initialized → 0
std::vector<int> c(5, 42);                     // 5 elems all == 42
std::vector<int> d(c);                         // copy of c
std::vector<int> e(c.begin(), c.end());        // range copy
std::vector<int> f(arr, arr + 10);             // from a C array

a = c;                                          // copy-assign — clears a, copies c's elements
a.assign(5, 7);                                 // replace contents with 5 sevens
a.assign(c.begin(), c.end());                   // replace contents with c's range
```

> **C++11+ only (forbidden in Mod08):** `std::vector<int> v = {1, 2, 3};` — brace-init-list ctor.
> In C++98 you build a small array first and use the range ctor: `int tmp[] = {1,2,3}; std::vector<int> v(tmp, tmp + 3);`.

---

## 3. Capacity — `size` vs `capacity`

This is *the* concept most beginners miss.

| Function       | Meaning                                                                 |
|----------------|-------------------------------------------------------------------------|
| `size()`       | How many elements you have logically.                                   |
| `capacity()`   | How many elements would fit before the buffer has to grow.              |
| `empty()`      | `size() == 0`. Use this — clearer and never wrong.                      |
| `max_size()`   | The theoretical max — usually `2^31` or `2^63`. Not useful in practice. |
| `reserve(n)`   | If `capacity < n`, reallocate to at least `n`. **Does not change size.**|
| `resize(n)`    | Force `size == n`. Default-constructs new elements, destroys excess.    |
| `resize(n, x)` | Same, but pads new elements with `x`.                                   |

```cpp
std::vector<int> v;
std::cout << v.size() << " " << v.capacity() << "\n";  // 0 0
v.reserve(100);
std::cout << v.size() << " " << v.capacity() << "\n";  // 0 100 (size unchanged!)
v.push_back(42);
std::cout << v.size() << " " << v.capacity() << "\n";  // 1 100 (no realloc — capacity was enough)
v.resize(5, 0);
std::cout << v.size() << " " << v.capacity() << "\n";  // 5 100 (added four zeros)
```

**Rule:** `reserve` is for performance — it skips reallocations. `resize` is for *semantics* — it actually creates / destroys elements.

---

## 4. Element access

| Form         | Bounds check?         | Returns                              |
|--------------|-----------------------|--------------------------------------|
| `v[i]`       | **No.** UB if `i ≥ size`. | `T&` (or `const T&` on const vector) |
| `v.at(i)`    | Yes — throws `std::out_of_range`. | Same |
| `v.front()`  | No — UB on empty.     | First element.                        |
| `v.back()`   | No — UB on empty.     | Last element.                         |
| `&v[0]`      | No — UB on empty.     | `T *` to the underlying buffer. The C-array escape hatch. |

```cpp
std::vector<int> v(3, 0);
v[0] = 10;
v.at(1) = 20;       // safe but slower (one branch per access)
int x = v.front();  // 10
int y = v.back();   // 0
int *raw = &v[0];   // pass to a C API: read(fd, raw, sizeof(int) * v.size())
```

> **C++11+:** `v.data()` returns the raw pointer even when the vector is empty (returns something convertible-to-null). In C++98, `&v[0]` on an empty vector is **UB** — always check `!v.empty()` first.

---

## 5. Modifiers

### 5.1 Add / remove at the back — fast path

```cpp
v.push_back(x);          // amortized O(1) — the workhorse
v.pop_back();            // O(1) — destroys the last element, decrements size
v.clear();               // O(n) — destroys all elements, size = 0, capacity UNCHANGED
```

**`clear()` does not free memory.** It only destroys elements. If you need to release the buffer, see the [swap trick](#9-efficiency-ten-tricks).

### 5.2 Insert / erase in the middle — slow path

```cpp
std::vector<int>::iterator it = v.begin() + 2;
v.insert(it, 99);                       // insert 99 at index 2, shift the rest right — O(n)
v.insert(it, 3, 99);                    // insert three 99s — O(n)
v.insert(it, src.begin(), src.end());   // insert a range — O(n + m)

it = v.begin() + 2;
it = v.erase(it);                       // erase one element, shift the rest left — O(n)
                                         // returns iterator to the element AFTER the erased one
v.erase(v.begin() + 1, v.begin() + 4);  // erase a range — O(n)
```

Every middle-insert / middle-erase **shifts every element after the change**. That's O(n) memory moves. If you do this in a loop, you've created O(n²).

### 5.3 Swap

```cpp
std::vector<int> a, b;
a.swap(b);              // O(1) — only swaps the three internal pointers
std::swap(a, b);        // same, specialised by the standard
```

Swap is constant time *and* never throws. It is the foundation of several efficiency idioms.

### 5.4 Assign

```cpp
v.assign(5, 0);                // replace contents with five 0s
v.assign(src.begin(), src.end());
```

Equivalent to `clear()` followed by `insert(end(), …)`, but the implementation may reuse existing storage.

---

## 6. Iterators

Every vector exposes four iterator types:

```cpp
std::vector<int>::iterator               it;
std::vector<int>::const_iterator         cit;
std::vector<int>::reverse_iterator       rit;
std::vector<int>::const_reverse_iterator crit;
```

Obtained via:

| Method            | Returns                                            |
|-------------------|----------------------------------------------------|
| `begin() / end()` | iterator (or const_iterator on const vector)       |
| `rbegin() / rend()` | reverse_iterator (walks `back()` → `front()`)    |

Vector iterators are **random-access**: you can do `it + 5`, `it2 - it1`, `it[3]`, `it < it2`. That's why `std::sort` (which needs random access) works on vectors but not on `std::list`.

```cpp
for (std::vector<int>::const_iterator it = v.begin(); it != v.end(); ++it) {
    std::cout << *it << " ";
}
```

**C++98 has no range-`for`** (`for (int x : v)`). You write the explicit loop, or use `std::for_each`.

---

## 7. Complexity cheat sheet

| Operation                       | Cost                | Note                                       |
|---------------------------------|---------------------|--------------------------------------------|
| `v[i]`, `v.at(i)`               | O(1)                | `at` is slightly slower (branch).          |
| `v.front()`, `v.back()`         | O(1)                |                                            |
| `v.push_back(x)`                | **Amortized** O(1)  | Occasional O(n) on reallocation.           |
| `v.pop_back()`                  | O(1)                |                                            |
| `v.insert(end(), x)`            | Amortized O(1)      | Same as `push_back`.                       |
| `v.insert(middle, x)`           | O(n)                | Shifts.                                    |
| `v.erase(middle)`               | O(n)                | Shifts.                                    |
| `v.clear()`                     | O(n)                | Destructors run; capacity unchanged.       |
| `v.size()`, `v.capacity()`, `v.empty()` | O(1)        |                                            |
| `v.reserve(n)`                  | O(n) when it grows, O(1) when it doesn't | One-shot cost. |
| `v.resize(n)`                   | O(n)                |                                            |
| `v.swap(w)`                     | O(1)                | Just swaps three pointers.                 |
| Find a value (`std::find`)      | O(n)                | Linear scan.                               |
| `std::sort(v.begin(), v.end())` | O(n log n)          | Introsort under the hood.                  |
| `std::lower_bound` on sorted v  | O(log n)            | Vector beats `std::set` here on small N.   |

---

## 8. Iterator invalidation — the silent killer

When you mutate a vector, **iterators, pointers, and references into it may become dangling**. Dereferencing them is undefined behavior — valgrind sometimes catches it, ASan reliably catches it, your eval might not.

| Operation                                           | What dies?                                                    |
|-----------------------------------------------------|---------------------------------------------------------------|
| `push_back` / `insert` **without** reallocation     | Iterators **at or after** the insertion point.                |
| `push_back` / `insert` **with** reallocation        | **All** iterators, pointers, references.                      |
| `pop_back`                                          | `end()` and iterator/ref to the removed last element.         |
| `erase(it)`                                         | `it` and everything after it.                                 |
| `clear()`                                           | All iterators, refs (but `end()` stays valid as the new end). |
| `reserve(n)` with `n > capacity`                    | All iterators, pointers, references.                          |
| `resize(n)` that grows past capacity                | All iterators, pointers, references.                          |
| `swap(other)`                                       | None of yours — but `end()` switches meaning.                 |

**The classic bug — pointer to an element kept around a `push_back`:**

```cpp
std::vector<int> v;
v.push_back(10);
int *p = &v[0];          // points into v's buffer
v.push_back(20);         // if capacity was 1, buffer moves — p now dangles
*p = 99;                 // UB
```

The fix: index, don't pointer. Or `reserve` enough up front so no reallocation happens during the lifetime of `p`.

**The other classic — erase in a loop:**

```cpp
// WRONG — erase returns the next valid iterator; you must use it.
for (std::vector<int>::iterator it = v.begin(); it != v.end(); ++it) {
    if (*it == 0)
        v.erase(it);     // it is now invalid; ++it is UB
}

// RIGHT — let erase advance you.
for (std::vector<int>::iterator it = v.begin(); it != v.end(); ) {
    if (*it == 0)
        it = v.erase(it);
    else
        ++it;
}

// BEST — erase–remove idiom (see §10).
v.erase(std::remove(v.begin(), v.end(), 0), v.end());
```

---

## 9. Efficiency: ten tricks

### 9.1 `reserve()` before bulk pushing

```cpp
std::vector<int> v;
v.reserve(1000);              // one allocation
for (int i = 0; i < 1000; ++i)
    v.push_back(i);
```

Without `reserve`, the vector reallocates ~10 times for 1000 elements, copying every element each time. With `reserve`, you get exactly one allocation. On non-trivial types this can be a 5–10× speedup.

### 9.2 Pass by **`const &`** (or `&` if you mutate)

```cpp
void print(const std::vector<int> &v);    // GOOD — no copy
void print(std::vector<int> v);            // BAD — copies the entire buffer
```

`vector` is value-type. Passing by value copies every element.

### 9.3 The **swap trick** — release excess capacity (C++98 substitute for `shrink_to_fit`)

```cpp
std::vector<int> v;
v.reserve(10000);
v.push_back(42);
// v.size() == 1, v.capacity() == 10000 — wasteful

std::vector<int>(v).swap(v);
// builds a *new* vector with capacity == size, swaps it in,
// the old (oversized) buffer is destroyed at end of full-expression
```

To go all the way to zero capacity:

```cpp
std::vector<int>().swap(v);   // v is now empty AND has freed its buffer
```

### 9.4 Prefer `push_back` and `pop_back` over middle insert / erase

If you can rephrase your algorithm to only touch the back of the vector, you've turned O(n) per op into O(1) per op. Sometimes a sort + linear pass is the better shape than "search + middle-erase."

### 9.5 The **erase–remove idiom** — remove all matches in O(n)

```cpp
v.erase(std::remove(v.begin(), v.end(), value), v.end());
```

What happens:
- `std::remove` doesn't actually erase — it shuffles "kept" elements to the front and returns an iterator to the new logical end.
- `v.erase(new_end, v.end())` then trims the tail in O(1) per remaining element.

Total: O(n) instead of O(n²) for "erase in a loop."

Same idea with `std::remove_if` and a functor:

```cpp
struct IsNegative {
    bool operator()(int x) const { return x < 0; }
};
v.erase(std::remove_if(v.begin(), v.end(), IsNegative()), v.end());
```

### 9.6 Sort once, then `lower_bound` — beats `std::set` for small N

If you build a collection and then query it many times without further inserts:

```cpp
std::sort(v.begin(), v.end());
std::vector<int>::iterator it = std::lower_bound(v.begin(), v.end(), key);
bool found = (it != v.end() && *it == key);
```

This gives O(log n) lookup with vector's cache locality. `std::set` is O(log n) too, but each node is a separate heap allocation — vector wins for `n < ~1000` in real-world benchmarks.

### 9.7 Build into a local, then `swap` into the destination

```cpp
void rebuild(std::vector<int> &target) {
    std::vector<int> tmp;
    tmp.reserve(target.size());
    // … fill tmp …
    target.swap(tmp);                     // O(1); old target's elements die with tmp
}
```

Strong exception guarantee for free — if construction of `tmp` throws, `target` is untouched.

### 9.8 Use indices, not iterators, when reallocation can happen

```cpp
for (std::size_t i = 0; i < v.size(); ++i) {
    if (some_condition(v[i]))
        v.push_back(derived(v[i]));       // safe: i still indexes correctly after realloc
}
```

An iterator-based version of this loop would dangle on the first reallocation.

### 9.9 `clear()` ≠ free. Use `swap` to actually return memory.

```cpp
v.clear();                                 // size = 0, capacity unchanged — buffer still held
std::vector<int>().swap(v);                // size = 0, capacity = 0 — buffer freed
```

### 9.10 Construct in place with the (count, value) or range ctor

```cpp
std::vector<int> v(1000, 0);               // one allocation, 1000 zeroes
// vs
std::vector<int> v;
for (int i = 0; i < 1000; ++i) v.push_back(0);   // multiple reallocs, slower
```

The standard library's constructors know the final size up front — they can pre-allocate exactly once.

---

## 10. Algorithms that pair well with vector

All of these live in `<algorithm>`. They work on iterators, so they're container-agnostic, but vector's contiguous random-access iterators are the cheapest possible — every algorithm runs at peak speed.

| Algorithm                      | Use                                                          |
|--------------------------------|--------------------------------------------------------------|
| `std::sort`                    | O(n log n) introsort. Random-access iterators required.      |
| `std::stable_sort`             | Like `sort` but preserves order of equal elements.           |
| `std::find` / `std::find_if`   | Linear search. Returns `end()` on not-found.                 |
| `std::count` / `std::count_if` | How many match.                                              |
| `std::remove` / `std::remove_if` | Pair with `erase` — see §9.5.                              |
| `std::unique`                  | Remove **adjacent** duplicates. Sort first if you want global unique. |
| `std::reverse`                 | In-place reverse.                                            |
| `std::lower_bound` / `upper_bound` | Binary search on a sorted vector.                        |
| `std::binary_search`           | Returns just a bool.                                         |
| `std::accumulate` (in `<numeric>`) | Sum / fold.                                              |
| `std::copy` / `std::copy_backward` | Copy ranges between iterators.                            |
| `std::fill` / `std::fill_n`    | Write a value across a range.                                |
| `std::min_element` / `std::max_element` | Iterators to the smallest / largest.                |
| `std::nth_element`             | Partial sort — only guarantees the n-th element is in place. O(n). |

Idiomatic example — unique-sort:

```cpp
std::sort(v.begin(), v.end());
v.erase(std::unique(v.begin(), v.end()), v.end());
```

---

## 11. `vector<bool>` — the cursed exception

`std::vector<bool>` is **not** a real vector of `bool`. It's a bit-packed specialization — each element takes 1 bit, not `sizeof(bool)` bytes. As a result:

- `&v[0]` is **not** `bool *` — you can't pass it to a C API.
- `v[i]` returns a **proxy reference**, not `bool &`. `bool &b = v[i];` won't compile.
- Iterators have surprising types; some generic code fails to instantiate.

**Workarounds:**

- `std::vector<char>` if you want a normal byte-per-element boolean array.
- `std::vector<int>` if you want it even bigger and value-stable.
- `std::deque<bool>` if you actually want a real container of `bool`.

This is widely considered the STL's biggest mistake. Avoid `vector<bool>` unless you specifically need the space saving and have audited every use.

---

## 12. When NOT to use vector

Vector is the default. Pick something else only when you have a *specific* reason:

| If you need…                                       | Pick…                                          |
|----------------------------------------------------|------------------------------------------------|
| Many inserts/erases in the **middle**              | `std::list` (O(1) splice / insert at iterator) |
| Fast push/pop at **both ends** + random access     | `std::deque`                                   |
| Iterator/reference stability across inserts        | `std::list`, `std::deque` (front/back only), or `std::map` |
| Key → value lookup in O(log n)                     | `std::map`                                     |
| Unique keys, ordered                               | `std::set`                                     |
| LIFO / FIFO with restricted interface              | `std::stack`, `std::queue`                     |
| A fixed-size buffer, never grows                   | C array or `T[N]` member                       |

For CPP09's exercises (`MutantStack`, `BitcoinExchange`, `RPN`): vector usually wins for RPN (push/pop back only), map wins for BitcoinExchange (sorted date lookup with `lower_bound`).

---

## 13. C++98 caveats

What you **don't** have, that newer code uses:

| Feature                | C++ version | C++98 alternative                                |
|------------------------|-------------|--------------------------------------------------|
| `auto`                 | C++11       | Spell the iterator type out.                     |
| Range-`for`            | C++11       | Index loop or explicit iterator loop.            |
| `emplace_back`         | C++11       | `push_back` — pays a copy but works.             |
| `shrink_to_fit()`      | C++11       | The **swap trick** (§9.3).                       |
| `data()` member        | C++11       | `&v[0]` (UB on empty — guard with `!empty()`).   |
| Initializer-list ctor `{1,2,3}` | C++11 | Build a C array first, then range-construct.    |
| Move semantics (`vector<X>` of move-only types) | C++11 | Store `X *` and own the lifetime manually. |
| `cbegin` / `cend`      | C++11       | `const_iterator` from a `const vector&`.         |
| `std::vector` over an `initializer_list` of pairs | C++11 | Build with `std::make_pair` and `push_back`. |

The 42 flags `-std=c++98 -Werror` will reject all the C++11 forms outright. Good — it forces you to learn what each one is actually doing under the hood.

---

## 14. Gotchas

- **`v.size() - 1` underflows when `v` is empty.** `size()` returns `size_t` (unsigned). `0u - 1u == SIZE_MAX`. Always check `!v.empty()` before computing `size() - 1`.
- **Signed/unsigned mismatch in loops.** `for (int i = 0; i < v.size(); ++i)` warns under `-Wall` because `i` is `int` and `v.size()` is `size_t`. Use `std::size_t` or cast deliberately.
- **`v[i]` does not bounds-check.** In debug builds libc++/libstdc++ sometimes do, in release they don't. Use `at` while developing, `[]` in tight loops.
- **A vector of objects is *not* a vector of pointers.** `std::vector<MyClass> v; v.push_back(MyClass());` calls the copy constructor. If `MyClass` is expensive to copy, store `MyClass *` instead — and remember to `delete` each pointer manually (no smart pointers in C++98 except `std::auto_ptr`, which is broken for use inside containers).
- **`v == w` is element-wise** and uses `operator==` on `T`. Make sure your class has one if you compare vectors.
- **Capacity growth factor is implementation-defined.** Don't write code that depends on it being exactly ×2 (libc++ uses 2, libstdc++ uses 2, MSVC uses 1.5).
- **`.front()` / `.back()` on an empty vector is UB**, not an exception. They don't bounds-check. Always `if (!v.empty())` first.
- **Self-insertion is UB.** `v.insert(v.end(), v.begin(), v.end())` may corrupt because the insert can reallocate, invalidating the source iterators mid-copy. Build a copy first.

---

## Worked example — the standard 42 idioms in one program

```cpp
#include <vector>
#include <algorithm>
#include <iostream>

int main(void)
{
    std::vector<int> v;
    v.reserve(10);                                  // §9.1

    for (int i = 0; i < 10; ++i)
        v.push_back(i % 4);

    std::sort(v.begin(), v.end());                  // §10

    v.erase(std::unique(v.begin(), v.end()),        // §10 — unique-sort
            v.end());

    v.erase(std::remove(v.begin(), v.end(), 2),     // §9.5 — erase–remove
            v.end());

    for (std::vector<int>::const_iterator it = v.begin();
         it != v.end(); ++it)
        std::cout << *it << " ";
    std::cout << "\n";

    std::vector<int>().swap(v);                     // §9.3 — release memory
    return 0;
}
```

Compile with the 42 flags:

```
c++ -Wall -Wextra -Werror -Wswitch -std=c++98 main.cpp -o demo
valgrind --leak-check=full ./demo
```

If you reserve, push_back, sort, and use erase–remove correctly, valgrind reports zero leaks and zero invalid reads. That's the goal.

---

> **One-line summary.** `std::vector` is `new T[]` that knows how to grow. Reserve up front, swap to shrink, never hold a pointer across a `push_back`, and use erase–remove instead of erasing in a loop. Everything else is a footnote.
