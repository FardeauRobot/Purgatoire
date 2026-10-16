# `std::multiset` — Sorted Keys With Duplicates

> **TL;DR.** `std::multiset<T>` is `std::set<T>` minus the uniqueness constraint. Same red-black tree, same O(log n) ops, same iterator stability — but **inserting an existing key adds a duplicate** instead of being a no-op. Two consequences: `insert` returns just an `iterator` (no `bool`), and `erase(key)` removes **all matching elements**, not just one.

Related: [`SET.md`](SET.md) · [`MULTIMAP.md`](MULTIMAP.md) · [`MAP.md`](MAP.md) · [`STL.md`](../advanced/STL.md) · [`INDEX.md`](INDEX.md)

Header: `#include <set>` *(yes — same header as `std::set`)*

---

## 1. Mental model

Same tree as set, but a node may have siblings with equal keys:

```
   multiset<int> after inserting 5, 3, 8, 5, 4, 5

                    5
                   / \
                3    5
                 \    \
                  4    8
                       /
                      5

   In-order traversal yields: 3 4 5 5 5 8
   Notice: three 5s, all kept.
```

Equal elements are stored **in a stable order** (the order in which they were inserted) since C++11. In C++98, the order among equal elements is **implementation-defined** — usually insertion order in libstdc++/libc++, but don't rely on it.

---

## 2. The two API differences from `set`

### 2.1 `insert(x)` returns just an `iterator`

```cpp
std::set<int>      s; std::pair<std::set<int>::iterator, bool>  r1 = s.insert(5);
std::multiset<int> m; std::multiset<int>::iterator              r2 = m.insert(5);
```

There's no boolean to return — insert always succeeds (even if duplicates exist). The iterator points to the newly inserted node.

### 2.2 `erase(key)` removes **every** matching element

```cpp
std::multiset<int> m;
m.insert(5); m.insert(5); m.insert(5);
std::cout << m.erase(5) << "\n";      // 3 — erased all three
std::cout << m.size()    << "\n";      // 0
```

If you want to remove **just one**, use `erase(iterator)` after `find`:

```cpp
std::multiset<int>::iterator it = m.find(5);
if (it != m.end())
    m.erase(it);                        // erases just that one node
```

This is the silent killer when porting from `set` to `multiset`. `m.erase(5)` on a multiset of `{5, 5, 5, 7}` leaves `{7}`, not `{5, 5, 7}`.

---

## 3. `count` and `equal_range` actually matter here

For `set`, `count(key)` is always 0 or 1. For `multiset`, it's "how many of this key" — and `equal_range(key)` gives you the iterator pair to walk just those:

```cpp
std::multiset<int> m;
m.insert(1); m.insert(2); m.insert(2); m.insert(2); m.insert(3);

std::cout << m.count(2) << "\n";               // 3

std::pair<std::multiset<int>::iterator, std::multiset<int>::iterator>
    r = m.equal_range(2);
for (std::multiset<int>::iterator it = r.first; it != r.second; ++it)
    std::cout << *it << " ";                    // 2 2 2
std::cout << "\n";
```

`equal_range(k)` is just `(lower_bound(k), upper_bound(k))` packaged together. For `multiset` it gives you the contiguous run of equal keys in O(log n + m).

---

## 4. Key API (the diffs from set in **bold**)

| Method                    | Returns / behavior                                            | Cost       |
|---------------------------|---------------------------------------------------------------|------------|
| `insert(x)`               | **`iterator` (no bool — always succeeds).**                   | O(log n)   |
| `insert(it, x)`           | Same, with hint.                                              | Amortized O(1) if hint is right |
| `insert(first, last)`     | Bulk-insert range.                                            | O(m log n) |
| `erase(it)`               | Remove one element.                                           | Amortized O(1) |
| `erase(key)`              | **Remove ALL matching. Returns count erased.**                | O(log n + m) where m = matches |
| `find(key)`               | Iterator to **some** matching element (any of them).          | O(log n)   |
| `count(key)`              | **Number of matches (can be > 1).**                           | O(log n + m) |
| `lower_bound(key)`        | First ≥ `key`.                                                | O(log n)   |
| `upper_bound(key)`        | First > `key`.                                                | O(log n)   |
| `equal_range(key)`        | **Iterator pair to the run of equal keys.**                   | O(log n)   |
| Other operations          | Same as `set`.                                                | Same       |

---

## 5. Iterator invalidation — same as set

Identical guarantees: `insert` invalidates nothing; `erase(it)` invalidates only `it`. You can hold iterators across mutations.

---

## 6. Efficiency tips

### 6.1 To erase one match, use `find` + `erase(iterator)`

```cpp
// WRONG if you only want to remove one — erases all matches
m.erase(5);

// RIGHT — erase just one
std::multiset<int>::iterator it = m.find(5);
if (it != m.end()) m.erase(it);
```

### 6.2 `equal_range` beats two separate `lower_bound`/`upper_bound` calls

Same complexity, but `equal_range` does both walks in one tree descent:

```cpp
std::pair<It, It> r = m.equal_range(key);     // one descent
// vs
It lo = m.lower_bound(key);
It hi = m.upper_bound(key);                    // two descents
```

### 6.3 If you mostly want "all values of key K," consider a different shape

A `std::map<Key, std::vector<Value> >` is often clearer and faster than a `multimap` (the `multiset`/`multimap` cousin). For pure counting, `std::map<Key, int>` (a histogram) is even better.

### 6.4 The hint form for sorted bulk insertion

Same trick as set: pass a correct hint to turn O(n log n) into O(n) bulk-insert.

---

## 7. When NOT to use multiset

| If you need…                                       | Pick…                                               |
|----------------------------------------------------|-----------------------------------------------------|
| Counting occurrences                               | `std::map<Key, int>` — clearer and same complexity. |
| Grouping values by key                             | `std::map<Key, std::vector<Value> >`.               |
| Fast lookup without ordering                       | (C++11 `unordered_multiset` — forbidden in C++98)   |
| Insertion-order tracking                           | `std::vector` of pairs, sort after.                 |

---

## 8. C++98 caveats

Same as `set`:

| Feature                | C++ version | C++98 alternative                           |
|------------------------|-------------|---------------------------------------------|
| `std::unordered_multiset` | C++11    | Use `std::multiset`.                        |
| `emplace`              | C++11       | `insert` — pays a copy.                     |
| Move semantics         | C++11       | Pass by `const &`.                          |
| Initializer-list ctor  | C++11       | Range ctor from a temp C array.             |

---

## 9. Gotchas

- **`erase(key)` removes ALL matches.** This is the multiset's #1 bug source. Use `erase(find(key))` to remove just one.
- **`insert` returns `iterator`, not `pair<iterator, bool>`.** Code that assumes `.second` won't compile when porting `set` → `multiset`.
- **`find(key)` returns *some* match, not a specific one.** If you need a particular position, use `equal_range` and walk.
- **Order among equal keys is implementation-defined in C++98.** Don't depend on insertion order — that guarantee is C++11+.
- **Same heap-per-node cost as set.** Memory overhead for many small keys is significant.
- **Keys are immutable through iterators** — same as set.

---

## 10. Worked example — anagram bucket

```cpp
#include <set>
#include <string>
#include <algorithm>
#include <iostream>

int main(void)
{
    // multiset stores all words; sorted by signature (sorted letters).
    // Custom comparator sorts by canonical form so anagrams cluster.
    struct ByCanonical {
        static std::string canonical(const std::string &s) {
            std::string c = s;
            std::sort(c.begin(), c.end());
            return c;
        }
        bool operator()(const std::string &a, const std::string &b) const {
            return canonical(a) < canonical(b);
        }
    };

    std::multiset<std::string, ByCanonical> bag;
    const char *words[] = {"listen", "silent", "enlist", "google", "gogole", "hello"};
    bag.insert(words, words + 6);

    // All anagrams of "listen" land in equal_range:
    std::pair<std::multiset<std::string, ByCanonical>::iterator,
              std::multiset<std::string, ByCanonical>::iterator>
        r = bag.equal_range("listen");

    for (; r.first != r.second; ++r.first)
        std::cout << *r.first << " ";
    std::cout << "\n";        // listen silent enlist  (anagrams of "listen")
    return 0;
}
```

The custom comparator sorts by "letter signature," so anagrams compare equivalent and cluster in the tree. `equal_range` then walks the cluster.

---

> **One-line summary.** `std::multiset` is `set` with duplicates allowed. Two API gotchas: `insert` returns just an iterator, and `erase(key)` removes **all** matches. `count`, `equal_range`, and `lower_bound`/`upper_bound` are the daily tools.
