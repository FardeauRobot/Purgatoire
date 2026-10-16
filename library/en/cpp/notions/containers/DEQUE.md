# `std::deque` — Double-Ended Queue (the "block list")

> **TL;DR.** A `std::deque<T>` is **a list of fixed-size arrays** (called *blocks* or *chunks*). It gives you O(1) push/pop at **both ends** *and* O(1) random access — but the storage is **not contiguous**, so `&d[0]` is not a useful pointer to a buffer. Iterators are random-access but slightly more expensive than a vector's because they may have to hop block boundaries.

Related: [`VECTOR.md`](VECTOR.md) · [`LIST.md`](LIST.md) · [`STACK.md`](STACK.md) · [`QUEUE.md`](QUEUE.md) · [`STL.md`](../advanced/STL.md) · [`INDEX.md`](INDEX.md)

Header: `#include <deque>`

---

## 1. Mental model

A deque holds a small array of pointers to fixed-size blocks. Each block stores N elements (typically 512 bytes / `sizeof(T)`, but implementation-defined). The deque tracks where the live region begins inside the first block and where it ends inside the last block.

```
   std::deque<int> d, after a few push_back/push_front

   block_map (small array of pointers)
   ┌───┬───┬───┬───┐
   │ A │ B │ C │ D │     ← four block pointers
   └─┬─┴─┬─┴─┬─┴─┬─┘
     │   │   │   │
     v   v   v   v
   [...][...][...][...]   ← four heap-allocated blocks of N ints each
       └─ live region ─┘

   front_iterator points into block A at some offset
   back_iterator  points into block D at some offset
```

When you `push_front` and the leading block fills up, the deque allocates a new block and prepends its pointer to the block_map. When the block_map itself fills up, *that* gets reallocated — but the **data blocks themselves don't move**. That's why mid-element references stay (mostly) stable.

For random access `d[i]`: compute `(block_index, offset_in_block)` and dereference. Two array lookups instead of one — a hair slower than `vector`, but still O(1).

---

## 2. Construction

Identical menu to `vector`:

```cpp
#include <deque>

std::deque<int> a;
std::deque<int> b(5);                        // five 0s
std::deque<int> c(5, 42);                    // five 42s
std::deque<int> d(c);                        // copy
std::deque<int> e(c.begin(), c.end());       // range
std::deque<int> f(arr, arr + 10);            // from C array

a = c;
a.assign(5, 7);
```

There is no `reserve` and no `capacity()` — deque doesn't have a single growable buffer to reserve into.

---

## 3. Key API

| Method                              | Cost          | Note                                   |
|-------------------------------------|---------------|----------------------------------------|
| `push_back(x)` / `push_front(x)`    | Amortized O(1) | Allocates a new block when needed.    |
| `pop_back()` / `pop_front()`        | O(1)          | May free a block when it empties.      |
| `d[i]`, `d.at(i)`                   | O(1)          | Two indirections (block + offset).     |
| `front()` / `back()`                | O(1)          | UB on empty.                           |
| `insert(it, x)` / `erase(it)`       | O(n) middle, O(1) at ends | Shifts the shorter half.    |
| `clear()`                           | O(n)          | Frees every block.                     |
| `size()` / `empty()`                | O(1)          |                                        |
| `resize(n)` / `resize(n, x)`        | O(\|Δ\|)      |                                        |
| `swap(other)`                       | O(1)          | Swaps the block_map pointer.           |
| `begin()` / `end()` / `rbegin()` / `rend()` | random-access | Slightly heavier than vector iterators. |

**No `data()`, no `reserve`, no `capacity`.** Deque actively manages multiple blocks; "the buffer" doesn't exist as a single allocation.

---

## 4. Iterator invalidation — the deque trap

This is where most beginners get burned. Deque's invalidation rules are **stricter than they look**:

| Operation                    | Iterators                          | References / pointers                             |
|------------------------------|------------------------------------|---------------------------------------------------|
| `push_back` / `push_front`   | **All iterators invalidated.**     | References to existing elements **stay valid**.   |
| `pop_back` / `pop_front`     | Iterators to the popped end + `end()` | References to other elements stay valid.       |
| `insert(middle)` / `erase(middle)` | All iterators invalidated.   | All references invalidated.                       |
| `clear()`                    | All iterators                      | All references                                    |
| `swap(other)`                | None of yours                      | None of yours                                     |

Note the asymmetry: pushing to a deque **does not move existing elements** (the data blocks don't reallocate), so **pointers and references stay valid** — but the iterator implementation may need to recompute internal block-map state, so iterators are conservatively all invalidated.

In practice: if you hold a `T*` or `T&` to an element of a deque, it's safe across `push_back` / `push_front` (unique to deque). If you hold an iterator, it's not. **Use indices** in mutation loops, just like vector.

---

## 5. Efficiency tips

### 5.1 Pick deque when you genuinely push at both ends

```cpp
std::deque<int> d;
d.push_front(1);                  // O(1) — vector would be O(n)
d.push_back(2);                   // O(1) — same as vector
d.pop_front();                    // O(1) — vector would be O(n)
```

If your algorithm is "build a sliding window" or "queue with random-access," deque is the right tool.

### 5.2 Don't pick deque just because vector "wastes capacity"

Deque allocates per block, so an empty deque already owns one block (~512 bytes overhead). For small collections, vector with `reserve(n)` is leaner.

### 5.3 Iteration is slower than vector — measure if it matters

Each iterator dereference may need to check whether you've crossed a block boundary. For tight numeric loops over millions of elements, vector wins. For mixed-access patterns, the difference disappears in noise.

### 5.4 `swap` for shrinking — same trick as vector

```cpp
std::deque<int>(d).swap(d);       // shrink to fit
std::deque<int>().swap(d);        // free all blocks
```

`clear()` frees blocks lazily (some implementations keep one); the swap idiom forces full release.

---

## 6. When NOT to use deque

| If you need…                                         | Pick…                                     |
|------------------------------------------------------|-------------------------------------------|
| A buffer to hand to a C API (`read`, `write`, `memcpy`) | `vector` — only it gives you a pointer to a single contiguous block. |
| Maximum iteration speed                              | `vector`                                  |
| Many middle inserts                                  | `list`                                    |
| Sorted access by key                                 | `set` / `map`                             |
| Pure LIFO / FIFO with restricted API                 | `stack` / `queue` (which uses deque internally by default) |

**Deque's sweet spot is narrow:** push/pop at both ends + random access. Outside that, vector is better.

---

## 7. C++98 caveats

| Feature                | C++ version | C++98 alternative                           |
|------------------------|-------------|---------------------------------------------|
| `emplace_*`            | C++11       | `push_*` — pays a copy.                     |
| `shrink_to_fit()`      | C++11       | The swap trick (§5.4).                      |
| `data()` member        | Never (deque is non-contiguous) | N/A — by design.        |
| Move semantics         | C++11       | Not available; deque copies are O(n).       |

---

## 8. Gotchas

- **`&d[0]` is *not* a pointer to the whole data**, only to the first block. Don't pass it to `read`/`write`/`memcpy` expecting `n * sizeof(T)` bytes.
- **`push_front` invalidates all iterators**, even though it doesn't reallocate data blocks. The block_map may grow.
- **References are more stable than iterators.** `T&` / `T*` to existing elements survives `push_back` / `push_front`. Iterators don't.
- **Deque has more per-element overhead than vector** for small N — at least one block (often 512 bytes) plus the block_map. For N < ~32, vector almost always wins.
- **No `reserve`.** You can't preallocate a number of elements; growth is automatic per block.

---

## 9. Worked example — sliding window

```cpp
#include <deque>
#include <iostream>

int main(void)
{
    std::deque<int> window;
    int data[] = {3, 1, 4, 1, 5, 9, 2, 6};
    int N = sizeof(data) / sizeof(data[0]);
    int K = 3;

    for (int i = 0; i < N; ++i)
    {
        window.push_back(data[i]);
        if ((int)window.size() > K)
            window.pop_front();             // O(1) — the deque sweet spot

        if ((int)window.size() == K)
        {
            std::cout << "window: ";
            for (std::size_t j = 0; j < window.size(); ++j)
                std::cout << window[j] << " ";
            std::cout << "\n";
        }
    }
    return 0;
}
```

Both ends move every step. With a `vector`, every `pop_front` would be O(K). Deque keeps it O(1).

---

> **One-line summary.** `std::deque` is a list of small arrays. Use it when you push at **both** ends *and* want random access. Otherwise vector.
