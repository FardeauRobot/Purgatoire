# `std::multimap` — Sorted Key → Value With Duplicate Keys

> **TL;DR.** `std::multimap<K, V>` is `std::map<K, V>` with the uniqueness constraint removed. Same red-black tree of `pair<const K, V>`, same O(log n) ops — but **the same key can appear many times**, each pointing to a (possibly different) `V`. Two consequences: there is **no `operator[]`** (which key would it return?), and `equal_range(k)` becomes the everyday tool for "give me all values associated with `k`."

Related: [`MAP.md`](MAP.md) · [`MULTISET.md`](MULTISET.md) · [`SET.md`](SET.md) · [`STL.md`](../advanced/STL.md) · [`INDEX.md`](INDEX.md)

Header: `#include <map>` *(yes — same header as `std::map`)*

---

## 1. Mental model

Same tree as `map`, but multiple nodes may share a key:

```
   multimap<std::string, int> after inserting:
     ("cat", 1), ("dog", 4), ("cat", 2), ("cat", 5), ("ant", 9)

                   ("cat", 1)
                   /          \
            ("ant", 9)    ("cat", 2)
                                \
                            ("cat", 5)
                                  \
                              ("dog", 4)

   In-order: ("ant", 9) ("cat", 1) ("cat", 2) ("cat", 5) ("dog", 4)
```

All `("cat", *)` entries cluster together in the in-order traversal. That's what `equal_range("cat")` returns.

---

## 2. The differences from `map`

### 2.1 No `operator[]`

```cpp
std::map<std::string, int>      m;  m["cat"] = 5;     // OK
std::multimap<std::string, int> mm; mm["cat"] = 5;    // ERROR — no operator[]
```

There's no way to define `mm[k]` sensibly when multiple `("cat", *)` entries exist. Use `insert` instead.

### 2.2 `insert` returns just an `iterator`

```cpp
std::map<K,V>::iterator       r1; std::pair<r1, bool> = m.insert(...);
std::multimap<K,V>::iterator  r2 =                      mm.insert(...);
```

Same as `set` → `multiset`: insert always succeeds, no boolean to return.

### 2.3 `erase(key)` removes ALL matching entries

```cpp
mm.insert(std::make_pair("cat", 1));
mm.insert(std::make_pair("cat", 2));
mm.insert(std::make_pair("cat", 5));
std::cout << mm.erase("cat") << "\n";    // 3
```

Just like `multiset`. To remove only one specific entry, use `erase(iterator)`:

```cpp
std::multimap<std::string, int>::iterator it = mm.find("cat");
if (it != mm.end()) mm.erase(it);        // removes only one
```

`find(k)` returns an iterator to **some** matching entry — typically the first in tree order, but the standard doesn't guarantee which one when there are multiple.

---

## 3. `equal_range` is the daily tool

The whole reason to use multimap: "give me every value associated with key K."

```cpp
std::pair<std::multimap<std::string, int>::iterator,
          std::multimap<std::string, int>::iterator>
    r = mm.equal_range("cat");

for (std::multimap<std::string, int>::iterator it = r.first;
     it != r.second; ++it)
    std::cout << it->first << " → " << it->second << "\n";
// cat → 1
// cat → 2
// cat → 5
```

`equal_range(k)` is `(lower_bound(k), upper_bound(k))` in one tree descent. Cost: O(log n) plus O(m) to walk the m matches.

---

## 4. Key API (the diffs from map in **bold**)

| Method                    | Returns / behavior                                            | Cost       |
|---------------------------|---------------------------------------------------------------|------------|
| `insert(pair)`            | **`iterator` (no bool — always succeeds).**                   | O(log n)   |
| `insert(it, pair)`        | Same with hint.                                               | Amortized O(1) if hint is right |
| `insert(first, last)`     | Bulk-insert range.                                            | O(m log n) |
| `erase(it)`               | Remove one entry.                                             | Amortized O(1) |
| `erase(key)`              | **Remove ALL matching. Returns count erased.**                | O(log n + m) |
| `find(key)`               | Iterator to **some** matching entry.                          | O(log n)   |
| `count(key)`              | **Number of entries with this key (can be > 1).**             | O(log n + m) |
| `equal_range(key)`        | **Iterator pair to the run of equal keys — the daily tool.**  | O(log n)   |
| `lower_bound(key)`        | First entry with key ≥ given.                                 | O(log n)   |
| `upper_bound(key)`        | First entry with key > given.                                 | O(log n)   |
| **`operator[]`**          | **Doesn't exist.**                                            | —          |
| **`at(k)`**               | **Doesn't exist** (and `at` is C++11 anyway).                 | —          |
| Other ops                 | Same as `map`.                                                | Same       |

---

## 5. Iterator invalidation — same as map / set

`insert` invalidates nothing; `erase(it)` invalidates only `it`. Iterators to other entries are stable across mutations. Same red-black tree.

---

## 6. Efficiency tips

### 6.1 Use `equal_range` when iterating one key's group

It's one tree descent for both endpoints — slightly cheaper than `lower_bound(k)` + `upper_bound(k)` separately.

### 6.2 Decide between `multimap<K, V>` and `map<K, vector<V> >`

These are two different shapes for the same idea. Trade-offs:

| Shape                          | Pros                                            | Cons                                       |
|--------------------------------|-------------------------------------------------|--------------------------------------------|
| `multimap<K, V>`               | Inserts are O(log n); each entry stable.        | Walking all values requires `equal_range`. Memory: one tree node per pair. |
| `map<K, std::vector<V> >`      | All values for a key are in one contiguous vector — cache-friendly to iterate. Clearer intent. | Each new value can trigger a vector reallocation. |

For "many keys, few values per key" → `multimap`. For "few keys, many values per key" → `map<K, vector<V> >`. Either works; the second is usually clearer in code review.

### 6.3 To erase one specific (key, value) pair, walk `equal_range`

```cpp
std::pair<It, It> r = mm.equal_range("cat");
for (It it = r.first; it != r.second; ) {
    if (it->second == 5) {
        mm.erase(it++);                 // post-increment trick — see §10
    } else {
        ++it;
    }
}
```

`erase(key)` would wipe all `("cat", *)`. Walking equal_range lets you target a specific value.

### 6.4 Pass by `const &`

Same as every other associative container.

---

## 7. When NOT to use multimap

| If you need…                                       | Pick…                                          |
|----------------------------------------------------|------------------------------------------------|
| Unique keys                                        | `map`                                          |
| `m[k]` syntax                                       | `map`                                          |
| Group-by where each group is large                 | `map<K, vector<V> >`                           |
| Counting (key → count)                             | `map<K, int>`                                  |
| Many keys, few-or-one values per key               | `map`                                          |

If you can express your problem as `map<K, vector<V> >`, that's almost always cleaner. Reserve `multimap` for when the tree-stability of each node matters (e.g. you hand out long-lived iterators to individual entries).

---

## 8. C++98 caveats

| Feature                | C++ version | C++98 alternative                           |
|------------------------|-------------|---------------------------------------------|
| `std::unordered_multimap` | C++11    | Use `std::multimap`.                        |
| `emplace`              | C++11       | `insert(make_pair(k, v))` — pays a copy.    |
| Initializer-list ctor  | C++11       | Build pair array, use range ctor.           |
| Move semantics         | C++11       | Pass by `const &`.                          |

---

## 9. Gotchas

- **No `operator[]`.** `mm[k]` is a compile error. Use `insert(make_pair(k, v))`.
- **`erase(key)` removes ALL matching entries.** Use `erase(iterator)` after `find` to remove just one — and even then, `find` returns *some* match, not a specific one.
- **`insert` returns just `iterator`.** Code expecting `pair<iterator, bool>` (set/map style) won't compile.
- **`find(k)` doesn't tell you which match it returned.** If you need a specific (key, value), use `equal_range` and search.
- **Order among duplicates is implementation-defined in C++98** (insertion order is the C++11 guarantee).
- **Each entry is a separate heap allocation.** For "many values per key," `map<K, vector<V> >` is usually more memory-efficient.
- **`erase(it)` returns `void` in C++98** (C++11 returns the next iterator). Use the post-increment idiom:
  ```cpp
  for (it = mm.begin(); it != mm.end(); ) {
      if (should_erase(it))
          mm.erase(it++);
      else
          ++it;
  }
  ```

---

## 10. Worked example — student → courses

```cpp
#include <map>
#include <string>
#include <iostream>

int main(void)
{
    std::multimap<std::string, std::string> enrolled;
    enrolled.insert(std::make_pair("alice", "algebra"));
    enrolled.insert(std::make_pair("alice", "history"));
    enrolled.insert(std::make_pair("alice", "biology"));
    enrolled.insert(std::make_pair("bob",   "algebra"));
    enrolled.insert(std::make_pair("bob",   "physics"));

    // All courses Alice is in
    std::pair<std::multimap<std::string, std::string>::iterator,
              std::multimap<std::string, std::string>::iterator>
        r = enrolled.equal_range("alice");

    std::cout << "Alice: ";
    for (std::multimap<std::string, std::string>::iterator it = r.first;
         it != r.second; ++it)
        std::cout << it->second << " ";
    std::cout << "\n";        // algebra biology history  (sorted by key, then in tree order)

    // How many courses does Bob take?
    std::cout << "Bob count: " << enrolled.count("bob") << "\n";    // 2

    // Drop Alice from biology
    for (std::multimap<std::string, std::string>::iterator it = enrolled.lower_bound("alice");
         it != enrolled.upper_bound("alice"); ) {
        if (it->second == "biology")
            enrolled.erase(it++);          // post-increment idiom
        else
            ++it;
    }

    std::cout << "After drop, Alice count: " << enrolled.count("alice") << "\n";  // 2
    return 0;
}
```

`equal_range` walks one student's courses; `count` answers "how many." The targeted erase-while-iterating pattern shows the C++98 safe form.

---

> **One-line summary.** `std::multimap` is `map` with duplicate keys allowed. No `operator[]`, `insert` returns just an iterator, `erase(key)` removes all matches, and `equal_range` is the daily tool. Often, `map<K, vector<V> >` is a clearer alternative.
