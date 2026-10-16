# `std::list` — The Doubly-Linked List

> **TL;DR.** A `std::list<T>` is a **doubly-linked list of heap-allocated nodes**. Each node holds a value and two pointers (`prev`, `next`). It gives you O(1) insert/erase **anywhere** when you already hold an iterator, and **iterators stay valid** across any insert or erase except on the element you removed. The price: no random access (`l[i]` doesn't exist), and every traversal is a cache miss per node.

Related: [`VECTOR.md`](VECTOR.md) · [`DEQUE.md`](DEQUE.md) · [`STL.md`](../advanced/STL.md) · [`INDEX.md`](INDEX.md)

Header: `#include <list>`

---

## 1. Mental model

It's `t_list *` from libft, but **doubly linked** and templated:

```
   list<int> with three elements: [10, 20, 30]

       ┌─────┐      ┌─────┐      ┌─────┐
   …──→│ 10  │←────→│ 20  │←────→│ 30  │←──…
       │prev │      │prev │      │prev │
       │next │      │next │      │next │
       └─────┘      └─────┘      └─────┘
         ▲                          ▲
         │                          │
       front()                    back()

   Each node = one heap allocation, scattered anywhere in RAM.
```

In C with libft conventions:

```c
typedef struct s_node {
    int             value;
    struct s_node   *prev;
    struct s_node   *next;
}   t_node;
```

That's the whole secret. Every `push_back` is `malloc(sizeof(t_node))` + relink two pointers. There is no resize, no realloc, no copying of the whole structure. That's why iterators stay valid: the node a given iterator points to never moves.

---

## 2. Construction

```cpp
#include <list>

std::list<int> a;                            // empty
std::list<int> b(5);                         // five default-constructed elements (0)
std::list<int> c(5, 42);                     // five 42s
std::list<int> d(c);                         // copy
std::list<int> e(c.begin(), c.end());        // range copy
std::list<int> f(arr, arr + 10);             // from a C array

a = c;
a.assign(5, 7);
a.assign(c.begin(), c.end());
```

Same constructor set as `vector`. No surprises here.

---

## 3. Key API

| Method                        | What it does                                                  |
|-------------------------------|---------------------------------------------------------------|
| `push_back(x)` / `push_front(x)` | Allocate one node, link in. Both O(1).                     |
| `pop_back()` / `pop_front()`  | Unlink + delete the end node. O(1).                           |
| `insert(it, x)`               | Insert before `it`. O(1) **given** `it`.                      |
| `erase(it)`                   | Unlink + delete. O(1). Returns iterator to next.              |
| `clear()`                     | Destroys every node. O(n) — actually frees memory (no buffer). |
| `front()` / `back()`          | Reference to first / last value. O(1). UB on empty.           |
| `size()` / `empty()`          | O(1) in C++11+; **O(n) in some C++98 implementations** of libstdc++ pre-4.x. Treat `empty()` as the safe one. |
| `begin()` / `end()`           | Bidirectional iterators (no `+`, `-`, no `<`).                |
| `rbegin()` / `rend()`         | Reverse iterators.                                            |

**No `operator[]`. No `at()`.** Random access is O(n) by definition — the standard refuses to expose it.

### List-only special members

These exist on `std::list` because they exploit the linked structure to be cheaper than the generic algorithm:

| Method              | What it does                                                      | Cost |
|---------------------|-------------------------------------------------------------------|------|
| `splice(pos, other)` | Move all of `other` into `*this` before `pos`. **No copies, no allocations.** | O(1) |
| `splice(pos, other, it)` | Move one element. | O(1) |
| `splice(pos, other, first, last)` | Move a range. | O(1) in C++98 (size not maintained) |
| `merge(other)`      | Merge two **sorted** lists into one sorted list. | O(n+m) |
| `sort()`            | Sort in place. List can't use `std::sort` (needs random-access iterators). | O(n log n) |
| `reverse()`         | Reverse the chain by swapping `prev`/`next` pointers. | O(n) |
| `unique()`          | Remove consecutive duplicates (sort first if you want global unique). | O(n) |
| `remove(val)`       | Erase every node equal to `val`. | O(n) |
| `remove_if(pred)`   | Erase every node satisfying `pred`. | O(n) |

`std::sort(l.begin(), l.end())` **does not compile** for a list — list iterators are bidirectional, `std::sort` requires random-access. Use the member: `l.sort();`.

---

## 4. Complexity cheat sheet

| Operation                     | Cost   | Note                                          |
|-------------------------------|--------|-----------------------------------------------|
| `push_back` / `push_front`    | O(1)   | One `malloc`. No reallocation ever.           |
| `pop_back` / `pop_front`      | O(1)   |                                                |
| `insert(it, x)`               | O(1)   | But finding `it` may have cost O(n).          |
| `erase(it)`                   | O(1)   |                                                |
| `find(begin, end, x)`         | O(n)   | No tree, no hash — full scan.                 |
| `l.sort()`                    | O(n log n) | Member function; uses merge sort.         |
| `l.splice(pos, other)`        | O(1)   | The killer feature. Move whole list, no copies. |
| `size()`                      | O(1) (C++11+) / O(n) (some C++98)  | Use `empty()` when you only need the boolean. |
| `clear()`                     | O(n)   | Frees every node.                             |
| Random access `l[i]`          | **N/A** | Doesn't exist. Manually advance an iterator with `++`, that's O(n). |

---

## 5. Iterator invalidation — list's superpower

This is where `list` shines:

| Operation                | Iterators invalidated?                                     |
|--------------------------|------------------------------------------------------------|
| `push_back` / `push_front` | **None.** Existing iterators stay valid.                 |
| `insert(it, x)`          | **None.** `it` still points where it did.                  |
| `erase(it)`              | Only `it` itself.                                          |
| `splice(...)`            | **None.** The moved nodes keep their identity (and their iterators!), even when transferred to another list. |
| `clear()`                | All iterators (every node freed).                          |
| `swap(other)`            | `end()` swaps meaning, but iterators to elements stay valid. |

This is the killer reason to pick `list` over `vector`: if you need long-lived iterators across many inserts, list is the only sequence container that gives them to you.

---

## 6. Efficiency tips

### 6.1 Use `splice` to move nodes between lists for free

```cpp
std::list<int> a; a.push_back(1); a.push_back(2);
std::list<int> b; b.push_back(99);

b.splice(b.begin(), a);      // a is now empty, b is [1, 2, 99]
                              // ZERO allocations, ZERO copies — just pointer relinking
```

This is impossible in `vector` (would require O(n) copy). It's the single best reason `std::list` exists.

### 6.2 Use the member `sort` and `merge`, not the `std::` versions

```cpp
l.sort();                    // member — works
l.merge(other);              // member — assumes both sorted; merges in O(n+m)

std::sort(l.begin(), l.end()); // ERROR — bidirectional iterators reject random-access op
```

Why list has its own sort: the `std::sort` algorithm needs `it + n` (random access). List can't provide that without scanning. Member sort uses merge-sort which only walks `++` / `--`.

### 6.3 Don't compute `size()` in a loop

In strict C++98, some `list` implementations (libstdc++ before 4.7 with the old ABI) make `size()` O(n) — they walk the whole list to count. So:

```cpp
// Possibly O(n²) on old libstdc++
for (std::size_t i = 0; i < l.size(); ++i) { … }

// Always O(n)
for (std::list<int>::iterator it = l.begin(); it != l.end(); ++it) { … }
```

Modern libc++ and libstdc++ store size in the list header (O(1)). Still, prefer iteration over indexed loops on a list.

### 6.4 Avoid `list` for short-lived collections — vector wins

`malloc` is expensive — easily 100× slower than incrementing a pointer. For a thousand `push_back`s, `vector` (with one `reserve`) does one allocation; `list` does a thousand. List is for **long-lived** collections with frequent middle inserts/erases, not for fast accumulation.

### 6.5 `remove`/`remove_if` is the erase–remove idiom, built-in

```cpp
l.remove(0);                 // erase every 0 — O(n)
l.remove_if(IsNegative());    // erase every negative — O(n)
```

For vector you write `v.erase(std::remove(v.begin(), v.end(), 0), v.end())`. List bakes that into a member call.

---

## 7. When NOT to use list

| If you need…                                       | Pick…                                          |
|----------------------------------------------------|------------------------------------------------|
| Random access by index                             | `vector` or `deque`                            |
| Iterating millions of elements fast                | `vector` (cache locality dominates)            |
| Simple LIFO / FIFO                                 | `stack` / `queue`                              |
| Sorted unique keys                                 | `set`                                          |
| Key → value lookup                                 | `map`                                          |

**Default to `vector`.** Pick `list` only when you've measured that middle-insert/erase is your bottleneck, or when you genuinely need long-lived iterators across mutations.

---

## 8. C++98 caveats

| Feature                | C++ version | C++98 alternative                           |
|------------------------|-------------|---------------------------------------------|
| `emplace` / `emplace_back` | C++11   | `insert` / `push_back` — pays a copy.       |
| `std::forward_list`    | C++11       | Use `std::list` (slightly heavier nodes).   |
| Move semantics         | C++11       | Use `splice` (which moves nodes for free). |
| Initializer-list ctor  | C++11       | Range ctor from a temp C array.             |

---

## 9. Gotchas

- **`std::sort(l.begin(), l.end())` does not compile.** Use `l.sort()`.
- **`size()` may be O(n)** on some C++98 implementations. Use `empty()` when you only need to know "is there anything?"
- **No `operator[]`, no `at`.** If you find yourself wanting random access on a list, switch to `vector` or `deque`.
- **Each node is a separate heap allocation** — list of 1M ints is ~24 MB of overhead on top of the 4 MB of data. Cache-misses dominate iteration cost.
- **`splice` from another list does not invalidate the moved iterators** — they keep working through the new list. This is the strongest iterator guarantee in the entire STL.
- **`erase(it)` returns the next iterator**, just like vector. The "erase in a loop" pattern is the same: `it = l.erase(it);`.

---

## 10. Worked example — splice for free

```cpp
#include <list>
#include <iostream>

int main(void)
{
    std::list<int> hot;       // recently used
    std::list<int> cold;      // archived

    for (int i = 0; i < 5; ++i)
        hot.push_back(i);     // hot = [0, 1, 2, 3, 4]

    // "Archive" the front element — move one node from hot to cold.
    cold.splice(cold.end(), hot, hot.begin());
    // hot = [1, 2, 3, 4], cold = [0]
    // ZERO allocations: the node was unlinked from hot and linked into cold.

    // Print both
    for (std::list<int>::const_iterator it = hot.begin(); it != hot.end(); ++it)
        std::cout << *it << " ";
    std::cout << "| ";
    for (std::list<int>::const_iterator it = cold.begin(); it != cold.end(); ++it)
        std::cout << *it << " ";
    std::cout << "\n";        // 1 2 3 4 | 0
    return 0;
}
```

Compile with the 42 flags:

```
c++ -Wall -Wextra -Werror -Wswitch -std=c++98 main.cpp -o demo
valgrind --leak-check=full ./demo
```

Every `push_back` is one `malloc`; the destructor of `std::list` chases the chain and `free`s every node. Valgrind reports zero leaks.

---

> **One-line summary.** `std::list` is libft's `t_list` with `prev` pointers and a templated value. Pick it for **long-lived iterators across middle inserts/erases**, or to **splice nodes between lists in O(1)**. Otherwise, vector wins.
