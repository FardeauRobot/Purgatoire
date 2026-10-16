# `std::map` — Sorted Key → Value Lookup

> **TL;DR.** `std::map<K, V>` is a **sorted associative container** mapping unique `K` to `V`. Same red-black tree as `std::set`, but each node stores a `std::pair<const K, V>` instead of just a key. All ops are O(log n). Killer feature: range queries via `lower_bound` (the engine behind CPP09's BitcoinExchange). Killer trap: `m[k]` **silently inserts a default-constructed value** when `k` is missing — almost always a bug if you only meant to look up.

Related: [`SET.md`](SET.md) · [`MULTIMAP.md`](MULTIMAP.md) · [`STL.md`](../advanced/STL.md) · [`INDEX.md`](INDEX.md)

Header: `#include <map>`

---

## 1. Mental model

A red-black tree of `pair<const K, V>` nodes, ordered by `K`:

```
   map<std::string, int>  after inserting ("apple", 3), ("banana", 7), ("cherry", 2)

                  ("banana", 7)
                  /            \
       ("apple", 3)         ("cherry", 2)

   In-order traversal yields: ("apple", 3) ("banana", 7) ("cherry", 2)
   — sorted by key, regardless of insertion order.
```

The element type is **`std::pair<const K, V>`** — note the `const` on the key. The same reasoning as `set`: mutating the key would put the node in the wrong tree position. The value `V` is freely mutable.

```cpp
std::map<std::string, int>::iterator it = m.find("apple");
// it->first  is `const std::string &`  — read-only
// it->second is `int &`                 — modifiable
it->second = 99;        // OK
// it->first = "x";     // ERROR
```

---

## 2. Construction

```cpp
#include <map>

std::map<std::string, int> a;                              // empty

// From a range of pairs (one allocation per element)
std::pair<%% std %%::string, int> seed[] = {
    std::make_pair("apple", 3),
    std::make_pair("banana", 7)
};
std::map<std::string, int> b(seed, seed + 2);

std::map<std::string, int> c(b);                           // copy

// Custom comparator — must order by KEY only
std::map<std::string, int, std::greater<std::string> > d;  // descending key order
```

**No initializer-list constructor in C++98.** Build a temp array and use the range ctor.

---

## 3. The four ways to insert

| Form                                          | Returns                                | When to use                            |
|-----------------------------------------------|----------------------------------------|----------------------------------------|
| `m.insert(std::make_pair(k, v))`              | `pair<iterator, bool>` — bool tells if it was new | You want to know "did I overwrite or add new?" |
| `m.insert(it, std::make_pair(k, v))`          | `iterator` — uses hint                 | Bulk-inserting nearly-sorted pairs.    |
| `m[k] = v`                                    | `void` (well, ref to assigned slot)    | Quick assign, **inserts a default `V` if `k` was absent.** |
| `m[k]`                                        | `V &`                                  | "Get or default-construct." Same insert-on-miss trap. |

The first form is the safest. The bracket form is convenient but treacherous — see §5.

---

## 4. Key API

| Method                    | Returns / behavior                                            | Cost       |
|---------------------------|---------------------------------------------------------------|------------|
| `m[k]`                    | Reference to value for `k`. **Inserts default if absent.**    | O(log n)   |
| `m.find(k)`               | `iterator` to the pair, or `end()` if missing. Pure lookup.   | O(log n)   |
| `m.count(k)`              | 0 or 1 (map keys are unique).                                 | O(log n)   |
| `m.insert(pair)`          | `pair<iterator, bool>`. `bool == false` means key already present (no overwrite). | O(log n) |
| `m.erase(it)`             | Remove. Returns void in C++98.                                | Amortized O(1) |
| `m.erase(k)`              | Remove if present. Returns count erased (0 or 1).             | O(log n)   |
| `m.lower_bound(k)`        | First element whose key ≥ `k`.                                | O(log n)   |
| `m.upper_bound(k)`        | First element whose key > `k`.                                | O(log n)   |
| `m.equal_range(k)`        | Pair `(lower_bound, upper_bound)`. Range is 0 or 1 element.   | O(log n)   |
| `m.size()` / `m.empty()`  |                                                                | O(1)       |
| `m.clear()`               | Frees every node.                                             | O(n)       |
| `m.begin()` / `end()` / `rbegin()` / `rend()` | Bidirectional iterators in sorted-by-key order. | O(1)       |

> **`at(k)` is C++11.** In C++98, the closest pure-lookup is `find(k)` — and you must compare against `end()`.

---

## 5. The `m[k]` insert-on-miss trap

```cpp
std::map<std::string, int> counts;
counts["apple"] += 1;             // counts = {"apple" → 1}
counts["apple"] += 1;             // counts = {"apple" → 2}
                                   // perfect for histograms.

// But:
if (counts["banana"] == 0)         // ← THIS INSERTED ("banana", 0)
    std::cout << "no bananas\n";
std::cout << counts.size() << "\n";  // 2 — banana is now in the map
```

**Every `m[k]` access creates the entry if it doesn't exist.** That's deliberate — it's what makes `counts[word]++` work as a histogram. But if you only meant to *check*, you've polluted the map.

**The pure-lookup pattern:**

```cpp
std::map<std::string, int>::const_iterator it = counts.find("banana");
if (it == counts.end())
    std::cout << "no bananas\n";   // does NOT insert
else
    std::cout << it->second << "\n";
```

**Rule of thumb:** use `m[k]` to *write*; use `m.find(k)` to *read*. If you cross those wires, you'll insert ghosts.

### What does `m[k]` actually do?

```cpp
V& operator[](const K &k) {
    iterator it = lower_bound(k);
    if (it == end() || comp(k, it->first))
        it = insert(it, std::make_pair(k, V()));   // V's default ctor
    return it->second;
}
```

Two takeaways:
- `V` **must be default-constructible** to use `m[k]`. If `V` has no default ctor, the bracket form won't compile.
- Each `m[k]` lookup costs one tree descent — same as `find`. Don't re-bracket the same key twice if you're worried about cost.

---

## 6. Iterator invalidation — same as set

| Operation             | Iterators / references invalidated?              |
|-----------------------|--------------------------------------------------|
| `insert`              | None.                                            |
| `erase(it)`           | Only `it`.                                       |
| `erase(k)`            | Only the iterator to `k` (if it was there).     |
| `clear()`             | All.                                             |
| `swap(other)`         | None.                                            |

Iterators are stable across inserts. Same red-black tree as set.

---

## 7. Efficiency tips

### 7.1 Use `lower_bound` for "nearest preceding key" — the BitcoinExchange pattern

CPP09 ex00 asks: given a date, find the closest date *not later than* it in a price database. That's literally what `lower_bound` + a one-step backwards adjustment gives you:

```cpp
std::map<std::string, double>::const_iterator it = prices.lower_bound(query_date);
if (it == prices.end() || it->first != query_date) {
    if (it == prices.begin())
        return /* no earlier date */;
    --it;                           // step back to the strictly-earlier date
}
return it->second;
```

Sorted-by-key + log-n bisection = ideal for time-series lookup.

### 7.2 Use `insert` (not `[]`) when you want "add only if missing"

```cpp
// SAFE: leaves existing value untouched if "apple" already has one.
m.insert(std::make_pair("apple", 5));

// UNSAFE: overwrites silently
m["apple"] = 5;
```

### 7.3 Iterate in key order

The default iteration order **is** sorted by key. If that's what you want, no extra sort step. If you want insertion order, you need a `vector<pair<K,V>>`.

### 7.4 Build once + iterate many → consider a sorted `vector<pair<K,V>>`

Same trade-off as set vs sorted-vector. If the map is built once at startup and then queried many times, a sorted vector is faster (one allocation, contiguous memory, `lower_bound` works on it too).

### 7.5 Pass by `const &`

Map copies are O(n) and allocate one node per element. Always reference.

---

## 8. When NOT to use map

| If you need…                                       | Pick…                                          |
|----------------------------------------------------|------------------------------------------------|
| Duplicate keys                                     | `multimap`                                     |
| O(1) average lookup, no ordering needed            | `unordered_map` — **C++11, forbidden in 42 C++98 modules** |
| Insertion-order tracking                           | `vector<pair<K,V>>`                            |
| Counting (histogram)                               | `map<K, int>` is fine — that's the canonical use |
| Group-by                                           | `map<K, vector<V> >` is usually clearer than `multimap` |

---

## 9. C++98 caveats

| Feature                | C++ version | C++98 alternative                           |
|------------------------|-------------|---------------------------------------------|
| `std::unordered_map`   | C++11       | Use `std::map` (O(log n) instead of O(1) avg). |
| `at(k)`                | C++11       | `find(k)` + check `!= end()`.               |
| `emplace`              | C++11       | `insert(make_pair(k, v))` — pays a copy.    |
| Initializer-list ctor `{{a, 1}, {b, 2}}` | C++11 | Build pair array, use range ctor.       |
| Move semantics         | C++11       | Pass by `const &`.                          |

---

## 10. Gotchas

- **`m[k]` inserts on miss.** Use `find(k)` for pure lookup.
- **Element type is `pair<const K, V>`.** You cannot reassign a key through an iterator.
- **`V` must be default-constructible** to use `operator[]`.
- **`erase(it)` returns `void` in C++98** (C++11 returns the next iterator). The erase-while-iterating idiom needs the post-increment trick:
  ```cpp
  for (std::map<K, V>::iterator it = m.begin(); it != m.end(); ) {
      if (should_erase(it->first)) {
          std::map<K, V>::iterator dead = it++;
          m.erase(dead);
      } else {
          ++it;
      }
  }
  ```
- **`insert(make_pair(k, v))` does NOT overwrite** an existing entry. The boolean in the returned pair tells you if the insertion happened.
- **Each entry is a heap allocation.** Memory cost is significant for many small entries.
- **Custom comparators must define strict weak ordering on the key only.** They never see the value.

---

## 11. Worked example — word histogram + range query

```cpp
#include <map>
#include <string>
#include <iostream>

int main(void)
{
    std::map<std::string, int> counts;
    const char *words[] = {"the", "cat", "sat", "the", "mat", "the", "cat"};

    // Histogram via the bracket-insert pattern (this is what [] is good at)
    for (int i = 0; i < 7; ++i)
        counts[words[i]] += 1;

    // Iterate in sorted-by-key order
    for (std::map<std::string, int>::const_iterator it = counts.begin();
         it != counts.end(); ++it)
        std::cout << it->first << ": " << it->second << "\n";
    // cat: 2
    // mat: 1
    // sat: 1
    // the: 3

    // Pure lookup — DON'T use []
    std::map<std::string, int>::const_iterator q = counts.find("dog");
    std::cout << (q == counts.end() ? "no dog\n" : "found\n");
    std::cout << "size still: " << counts.size() << "\n";   // still 4

    // "Closest key not greater than" — the BitcoinExchange pattern
    std::map<std::string, int>::const_iterator lo = counts.lower_bound("mau");
    if (lo != counts.begin()) {
        if (lo == counts.end() || lo->first != "mau")
            --lo;
        std::cout << "nearest <= mau: " << lo->first << "\n";    // mat
    }
    return 0;
}
```

The histogram shows the bracket form's strength. The "nearest preceding key" shows `lower_bound`'s. Both are O(log n) per operation.

---

> **One-line summary.** `std::map` is sorted `K → V` with O(log n) ops. Use `m[k]` only for *write* (it inserts on miss); use `find(k)` for *read*. `lower_bound` is the engine behind sorted-time-series lookups. Element type is `pair<const K, V>` — keys are immutable.
