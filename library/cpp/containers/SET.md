# `std::set` — Sorted Unique Keys

> **TL;DR.** `std::set<T>` is a **sorted, key-only** associative container. Implementation: a **red-black tree**. Every operation (insert, erase, find) is O(log n). Iterators are **bidirectional, stable, and walk the tree in sorted order**. Keys are *immutable* through iterators — modifying a key in place would break the sort invariant. For small N or write-once-read-many workloads, a sorted `vector` + `lower_bound` often beats `set`.

Related: [`MULTISET.md`](MULTISET.md) · [`MAP.md`](MAP.md) · [`VECTOR.md`](VECTOR.md) · [`STL.md`](../advanced/STL.md) · [`INDEX.md`](INDEX.md)

Header: `#include <set>`

---

## 1. Mental model

A red-black tree is a **self-balancing binary search tree** — every path from root to leaf has roughly the same length, so every operation is O(log n) worst case (not just average).

```
   set<int> after inserting 5, 3, 8, 1, 4, 7, 9

                    5 (B)
                   / \
                3 (R) 8 (R)
                / \   / \
            1(B) 4(B) 7(B) 9(B)

   ── B = black, R = red. Color rules keep the tree balanced.
   ── In-order traversal (left → root → right) yields: 1 3 4 5 7 8 9
```

What this means for you:
- **Each element is a separate heap allocation** (one tree node per key).
- **Iterators walk the tree in sorted order** — `for (it = s.begin(); it != s.end(); ++it)` visits 1, 3, 4, 5, 7, 8, 9 in that order, regardless of insertion order.
- **No duplicates.** Inserting an existing key is a no-op (and `insert` returns `(iterator, false)` to tell you).
- **Keys are sorted using `operator<` by default** (or a comparator you supply).

---

## 2. Construction

```cpp
#include <set>
#include <functional>          // for std::greater

std::set<int> a;                                    // empty, default-sorted ascending
std::set<int> b(arr, arr + 10);                     // from a range — auto-deduplicates and sorts
std::set<int> c(b);                                  // copy

std::set<int, std::greater<int> > d;                // sorted descending

// Custom comparator
struct ByLength {
    bool operator()(const std::string &a, const std::string &b) const {
        return a.size() < b.size();
    }
};
std::set<std::string, ByLength> e;
```

**The comparator must be a strict weak ordering.** `comp(a,b)` and `comp(b,a)` may not both be true. If both `!comp(a,b)` and `!comp(b,a)`, `set` considers them equivalent — which means **at most one of them lives in the set**.

---

## 3. Key API

| Method                    | What it does                                                | Cost       |
|---------------------------|-------------------------------------------------------------|------------|
| `insert(x)`               | Insert if absent. Returns `pair<iterator, bool>`.           | O(log n)   |
| `insert(it, x)`           | Same, with a hint iterator (close to insertion point).      | Amortized O(1) if hint is right |
| `insert(first, last)`     | Bulk-insert a range.                                        | O(m log n) |
| `erase(it)`               | Remove the element pointed to.                              | Amortized O(1) |
| `erase(key)`              | Remove the element equal to `key`. Returns 1 if removed, 0 if absent. | O(log n) |
| `erase(first, last)`      | Remove a range.                                             | O(log n + m) |
| `find(key)`               | Returns iterator, or `end()` if not found.                  | O(log n)   |
| `count(key)`              | 0 or 1 for `set` (it's unique). Use `find` if you can — same cost, semantically clearer. | O(log n) |
| `lower_bound(key)`        | First element ≥ `key`.                                      | O(log n)   |
| `upper_bound(key)`        | First element > `key`.                                      | O(log n)   |
| `equal_range(key)`        | `pair(lower_bound, upper_bound)`. For `set`, the range is 0 or 1 element. | O(log n) |
| `clear()`                 | Destroys every node. O(n).                                  | O(n)       |
| `size()` / `empty()`      | O(1).                                                       | O(1)       |
| `begin()` / `end()` / `rbegin()` / `rend()` | Bidirectional iterators in sorted order.    | O(1)       |

**No `operator[]`, no `at`, no random access.** The set is sorted by *value*, not indexed by position.

---

## 4. The `pair<iterator, bool>` return of `insert`

This is the workhorse:

```cpp
std::set<int> s;
std::pair<std::set<int>::iterator, bool> r = s.insert(42);
if (r.second)
    std::cout << "newly inserted\n";
else
    std::cout << "was already present\n";
// either way, r.first points to the element with value 42
```

**Why a pair and not just a bool?** Because giving you the iterator costs nothing extra (the tree just walked there to decide), and you often want to immediately do something with the inserted/existing element.

---

## 5. Iterator stability — set's quiet superpower

| Operation             | Iterators / references invalidated?                             |
|-----------------------|-----------------------------------------------------------------|
| `insert(x)`           | **None.** All existing iterators stay valid.                    |
| `erase(it)`           | Only `it` itself.                                               |
| `erase(key)`          | Only the iterator to that key (if it existed).                  |
| `clear()`             | All iterators (every node freed).                               |
| `swap(other)`         | None.                                                           |

This is a stronger guarantee than vector's: in `set`, you can hold an iterator across **any number of inserts** and it keeps pointing to the same element. The tree's nodes never move — they just get re-linked.

---

## 6. Keys are immutable through iterators

```cpp
std::set<int>::iterator it = s.find(5);
// *it = 99;       // ERROR — *it is `const int &`
                    // (libstdc++/libc++ make it `const T` by spec)
```

If you could mutate the key, you'd violate the sort order — the element would be in the wrong position in the tree, and find/insert would give wrong answers. The standard prevents this by giving you `const` access through iterators.

To "modify" a set element: `erase` it, then `insert` the new value.

---

## 7. Efficiency tips

### 7.1 Use `lower_bound` / `upper_bound` for range queries

```cpp
// Find all elements in [10, 20]
std::set<int>::iterator it = s.lower_bound(10);
std::set<int>::iterator end = s.upper_bound(20);
for (; it != end; ++it)
    std::cout << *it << " ";
```

`find(key) == end()` checks "is exactly this present"; `lower_bound(key)` answers "where would this go" — also useful for "insert with hint."

### 7.2 Use the hinted `insert` for sorted bulk insertion

```cpp
std::set<int>::iterator hint = s.end();
for (std::size_t i = 0; i < sorted_data.size(); ++i)
    hint = s.insert(hint, sorted_data[i]);   // amortized O(1) if hint is correct
```

For data already known to be sorted, this turns a `n log n` bulk-insert into roughly O(n).

### 7.3 If you only build once and query many times — sorted vector wins

```cpp
std::vector<int> v(arr, arr + n);
std::sort(v.begin(), v.end());
v.erase(std::unique(v.begin(), v.end()), v.end());
// ...
bool found = std::binary_search(v.begin(), v.end(), key);
```

Same O(log n) lookup, but **one allocation** vs n allocations, and **contiguous memory** so cache hits dominate. For n < ~1000 this often beats `std::set` by 3–10× in real measurements.

Use `std::set` when you do interleaved inserts and lookups; use sorted vector when the build phase finishes before the query phase begins.

### 7.4 `count(key)` vs `find(key)` — prefer `find`

Both are O(log n). `find` is semantically clearer and gives you the iterator. Use `count` only when you literally just want the boolean (and even then `find(k) != end()` reads better).

### 7.5 Pass by `const &`

Copying a set copies every node — n allocations. Always pass by reference.

---

## 8. When NOT to use set

| If you need…                                       | Pick…                                          |
|----------------------------------------------------|------------------------------------------------|
| Duplicate keys                                     | `multiset`                                     |
| Key → value mapping                                | `map`                                          |
| O(1) lookup (and don't care about order)           | C++11+ `unordered_set` — **forbidden in C++98 / 42** |
| Sorted access but write-once / read-many           | sorted `vector` + `binary_search`              |
| Insertion-order traversal                          | `vector` (manually de-dup if needed)           |

---

## 9. C++98 caveats

| Feature                | C++ version | C++98 alternative                           |
|------------------------|-------------|---------------------------------------------|
| `std::unordered_set`   | C++11       | Use `std::set` (O(log n) instead of O(1) avg). |
| `emplace` / `emplace_hint` | C++11   | `insert` — pays a copy.                     |
| Move semantics         | C++11       | Pass by `const &`.                          |
| Initializer-list ctor  | C++11       | Build via range ctor from a temp C array.   |
| Lambda comparators     | C++11       | Functor struct with `operator()`.           |

---

## 10. Gotchas

- **Keys are `const` through iterators.** You can't `*it = x;`. Erase + insert if you need to modify.
- **Comparator must be strict weak ordering.** Returning true for both `comp(a,b)` and `comp(b,a)` is UB.
- **Two keys are "equivalent" if neither is `<` the other** — not necessarily equal by `operator==`. With a comparator like `ByLength`, `"abc"` and `"def"` are equivalent and only the first inserted survives.
- **`insert` returns `pair<iterator, bool>`.** Forgetting `.first` to get the iterator is a common bug.
- **Each node is a separate heap allocation.** Sets of millions of small keys have huge memory overhead vs sorted vector.
- **`erase(key)` returns the count erased** (always 0 or 1 for set), not an iterator. `erase(it)` returns `void` in C++98 (it returns the next iterator only from C++11). To erase while iterating, the safe C++98 pattern is:
  ```cpp
  for (std::set<int>::iterator it = s.begin(); it != s.end(); ) {
      if (should_erase(*it)) {
          std::set<int>::iterator dead = it++;
          s.erase(dead);
      } else {
          ++it;
      }
  }
  ```

---

## 11. Worked example — sorted unique words

```cpp
#include <set>
#include <string>
#include <iostream>

int main(void)
{
    const char *words[] = {"pear", "apple", "banana", "apple", "cherry", "pear"};
    std::set<std::string> unique_sorted(words, words + 6);

    for (std::set<std::string>::const_iterator it = unique_sorted.begin();
         it != unique_sorted.end(); ++it)
        std::cout << *it << "\n";
    // apple
    // banana
    // cherry
    // pear

    if (unique_sorted.find("apple") != unique_sorted.end())
        std::cout << "found apple\n";

    std::pair<std::set<std::string>::iterator, bool> r = unique_sorted.insert("date");
    std::cout << (r.second ? "added date\n" : "date was already there\n");
    return 0;
}
```

Build the set from a range (auto-dedups and sorts), iterate in sorted order, look up by key, observe the `pair<iterator, bool>` return. Every operation is O(log n).

---

> **One-line summary.** `std::set` is a sorted, unique-key, log-n container backed by a red-black tree. Iterators stay valid across inserts; keys are immutable through them. For small or build-once datasets, sorted vector + binary_search is often faster.
