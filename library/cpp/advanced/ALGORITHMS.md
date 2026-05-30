# STL Functions Reference — Containers & Algorithms

> **TL;DR.** Two halves. **Part 1** is every container *member function* worth knowing, as a cross-container matrix plus per-family notes — what `push_back`, `insert`, `find`, `erase`, `operator[]` do and where they exist. **Part 2** is the **`<algorithm>` library**, exhaustive for C++98, grouped by job (search → modify → sort → set ops → heap → min/max → permute). **Part 3** rounds it out with `<numeric>` and `<functional>`. Everything is annotated with complexity and the C++98 trap (because half the "standard" calls you'll see online are C++11+ and `-std=c++98 -Werror` rejects them).

Related: [`STL.md`](STL.md) (the conceptual hub) · [`TEMPLATES.md`](TEMPLATES.md) · [`MEMBER_FUNCTION_POINTERS.md`](MEMBER_FUNCTION_POINTERS.md) · containers deep-dives in [`../containers/INDEX.md`](../containers/INDEX.md) · [`../io-errors/STRING_FUNCTIONS.md`](../io-errors/STRING_FUNCTIONS.md)

Headers you'll include:
```cpp
#include <algorithm>   // sort, find, copy, remove, transform, ... (the bulk)
#include <numeric>     // accumulate, inner_product, partial_sum, adjacent_difference
#include <functional>  // less, greater, plus, bind2nd, ptr_fun, ...
#include <iterator>    // back_inserter, ostream_iterator, distance, advance
#include <utility>     // pair, make_pair
```

---

## Table of Contents

**Part 0 — How to read this**
- [The iterator contract every algorithm relies on](#the-iterator-contract-every-algorithm-relies-on)
- [The C++98 trap list (memorize this)](#the-c98-trap-list-memorize-this)

**Part 1 — Container member functions**
1. [Functions common to (almost) every container](#1-functions-common-to-almost-every-container)
2. [Sequence containers — `vector` / `deque` / `list`](#2-sequence-containers--vector--deque--list)
3. [`std::list`'s special members](#3-stdlists-special-members)
4. [Container adapters — `stack` / `queue` / `priority_queue`](#4-container-adapters--stack--queue--priority_queue)
5. [Associative containers — `set` / `map` / `multiset` / `multimap`](#5-associative-containers--set--map--multiset--multimap)
6. [`std::string` as a container](#6-stdstring-as-a-container)

**Part 2 — The `<algorithm>` library (exhaustive, C++98)**
7. [Non-modifying sequence operations](#7-non-modifying-sequence-operations)
8. [Modifying sequence operations](#8-modifying-sequence-operations)
9. [Partitioning](#9-partitioning)
10. [Sorting](#10-sorting)
11. [Binary search (on sorted ranges)](#11-binary-search-on-sorted-ranges)
12. [Merge & set operations (on sorted ranges)](#12-merge--set-operations-on-sorted-ranges)
13. [Heap operations](#13-heap-operations)
14. [Min / max](#14-min--max)
15. [Permutations & comparison](#15-permutations--comparison)

**Part 3 — Companions**
16. [`<numeric>`](#16-numeric)
17. [`<functional>` — functors & adaptors](#17-functional--functors--adaptors)
18. [`<iterator>` — the glue](#18-iterator--the-glue)
19. [Idiom cookbook](#19-idiom-cookbook)
20. [What's NOT in C++98](#20-whats-not-in-c98)

---

## The iterator contract every algorithm relies on

Every algorithm takes a **half-open range** `[first, last)` — `last` is *one past* the element you care about, exactly like `end()`. The element at `last` is never touched.

```
   [first, last)   means   first, first+1, ..., last-1     (last excluded)

     v.begin()                                  v.end()
        │                                          │
        v                                          v
      ┌───┬───┬───┬───┬───┐
      │ a │ b │ c │ d │ e │
      └───┴───┴───┴───┴───┘
```

An algorithm never knows the container — it only calls `*it`, `++it`, maybe `--it` or `it + n`. Which of those it's allowed to call is the iterator's **category** (see [`STL.md` §7](STL.md#7-iterator-categories)). The practical consequence:

| Category | Containers | Algorithms that need it |
|---|---|---|
| Random-access (`it + n`, `it[n]`, `it < it2`) | `vector`, `deque`, raw pointers, `string` | `sort`, `nth_element`, `partial_sort`, `random_shuffle`, the fast `lower_bound` |
| Bidirectional (`++`, `--`) | `list`, `set`, `map`, `multiset`, `multimap` | `reverse`, `next_permutation`, `inplace_merge`, `stable_partition` |
| Forward (`++` only, multi-pass) | `forward_list`* | `remove`, `unique`, `replace`, `rotate` |
| Input / Output (single pass) | stream iterators | `copy`, `find`, `count`, `transform`, `accumulate` |

\* `forward_list` is C++11 — not in your toolbox. Listed for completeness.

**The rule that bites everyone:** `std::sort` needs random-access iterators, so **you cannot `std::sort` a `std::list` or a `std::set`**. `list` has its own `.sort()` member; `set` is already sorted.

---

## The C++98 trap list (memorize this)

These look standard, compile on modern toolchains, and **fail under `-std=c++98 -Werror`**. The eval uses C++98.

| You might reach for… | Reality | C++98 substitute |
|---|---|---|
| `std::copy_if` | **C++11** | `std::remove_copy_if` with a negated predicate |
| `std::all_of` / `any_of` / `none_of` | **C++11** | `std::find_if` / `std::count_if` and compare |
| `std::find_if_not` | **C++11** | `std::find_if` with `std::not1(pred)` |
| `std::iota` | **C++11** (`<numeric>`) | hand loop, or `std::generate` with a counter functor |
| `std::is_sorted` / `is_heap` | **C++11** | `std::adjacent_find(b, e, greater)` for sorted-check |
| `std::min({a,b,c})` (init-list) | **C++11** | nest: `std::min(a, std::min(b, c))` |
| `map::at(k)` | **C++11** | `find` then check against `end()` |
| `it = s.erase(it)` on **set/map** | erase returns **`void`** in C++98 | `s.erase(it++);` (post-increment idiom) |
| `auto`, range-`for`, lambdas | **C++11** | spell the type; index/iterator loop; **functors** (Part 3) |
| `vector::data()` | **C++11** | `&v[0]` (UB on empty — guard with `!empty()`) |
| `emplace` / `emplace_back` | **C++11** | `insert` / `push_back` (pays a copy) |

> `std::erase(it)` on a **sequence** container (`vector`/`deque`/`list`) **does** return the next iterator in C++98 — only the **associative** containers differ. That asymmetry is the single most common C++98 surprise.

---

# Part 1 — Container member functions

## 1. Functions common to (almost) every container

These exist on essentially all standard containers (adapters excepted — they're deliberately restricted, see §4).

| Member | Returns | Cost | Notes |
|---|---|---|---|
| `size()` | `size_type` | O(1)† | Number of elements. †`list::size()` is O(n) on some old libstdc++ — true O(1) since C++11. |
| `empty()` | `bool` | O(1) | **Always prefer over `size() == 0`** — correct for every container, including the O(n)-size lists. |
| `max_size()` | `size_type` | O(1) | Theoretical ceiling. Rarely useful. |
| `swap(other)` | `void` | O(1) | Swaps internals (pointers / tree roots). Never throws, never copies elements. |
| `begin()` / `end()` | iterator | O(1) | `const_iterator` when the container is `const`. |
| `rbegin()` / `rend()` | reverse_iterator | O(1) | Walks back→front. |
| `clear()` | `void` | O(n) | Destroys all elements. **Does not free `vector` capacity** (see [`VECTOR.md` §9.9](../containers/VECTOR.md)). |
| `operator==`, `!=` | `bool` | O(n) | Element-wise; needs `T::operator==`. |
| `operator<`, `>`, `<=`, `>=` | `bool` | O(n) | Lexicographical; needs `T::operator<`. |

Construction is uniform too — every container supports default, copy, range, and (sequence ones) fill construction:

```cpp
std::vector<int> a;                       // empty
std::vector<int> b(other);                // copy
std::vector<int> c(first, last);          // range: copy [first, last) from anything iterable
std::vector<int> d(5, 42);                // fill: five 42s   (sequence containers only)
```

The **range constructor** is the universal converter — it copies from *any* matching iterator pair, including a C array or a different container type:

```cpp
int raw[] = {3, 1, 2};
std::set<int>    s(raw, raw + 3);         // array → set (sorts + dedups)
std::list<int>   l(s.begin(), s.end());   // set  → list
std::vector<int> v(l.begin(), l.end());   // list → vector
```

---

## 2. Sequence containers — `vector` / `deque` / `list`

Full per-container detail (mental model, invalidation, efficiency) lives in [`VECTOR.md`](../containers/VECTOR.md), [`DEQUE.md`](../containers/DEQUE.md), [`LIST.md`](../containers/LIST.md). This is the consolidated **what-exists-where** matrix.

| Member | `vector` | `deque` | `list` | Cost | Note |
|---|:---:|:---:|:---:|---|---|
| `operator[]` / `at()` | ✅ | ✅ | ❌ | O(1) | `at` bounds-checks → throws `out_of_range`; `[]` is UB out of range. No random access on `list`. |
| `front()` / `back()` | ✅ | ✅ | ✅ | O(1) | UB on empty (not an exception). |
| `push_back()` / `pop_back()` | ✅ | ✅ | ✅ | O(1)* | `vector` push_back is *amortized* O(1) (realloc); `deque`/`list` are true O(1). |
| `push_front()` / `pop_front()` | ❌ | ✅ | ✅ | O(1) | `vector` lacks it — front insert is O(n) there. |
| `insert(pos, x)` | ✅ | ✅ | ✅ | vec/deque O(n), list O(1) | Also `(pos, n, x)` and `(pos, first, last)` overloads everywhere. |
| `erase(pos)` / `erase(b, e)` | ✅ | ✅ | ✅ | vec/deque O(n), list O(1) | **Returns next iterator** (sequence containers). |
| `assign(n, x)` / `assign(first, last)` | ✅ | ✅ | ✅ | O(n) | Replace all contents; may reuse storage. |
| `resize(n[, x])` | ✅ | ✅ | ✅ | O(n) | Grow (default- or `x`-fill) / shrink (destroy tail). |
| `capacity()` / `reserve(n)` | ✅ | ❌ | ❌ | O(1) / O(n) | **`vector` only.** Pre-allocate to skip reallocs. |
| `swap(other)` | ✅ | ✅ | ✅ | O(1) | |
| `data()` | ✅ | ❌ | ❌ | O(1) | **C++11** — use `&v[0]` in C++98. |

```cpp
std::vector<int> v;
v.reserve(100);                 // vector-only: one allocation up front
v.push_back(10);                // amortized O(1)
v.insert(v.begin(), 5);         // O(n) — shifts everything right; deque/list don't

std::deque<int> d;
d.push_front(1);                // O(1) at both ends — deque's whole point
d.push_back(2);

std::list<int> l(v.begin(), v.end());
std::list<int>::iterator it = l.begin();
++it;
l.insert(it, 99);               // O(1) — no shifting in a linked list
it = l.erase(it);               // O(1), returns next (sequence-container rule)
```

> **Iterator invalidation differs sharply.** `vector`: any growth invalidates *everything*. `deque`: a push at either end invalidates iterators but **keeps references valid**. `list`: nothing is invalidated except the erased node. Each container's page has the full table.

---

## 3. `std::list`'s special members

`list` trades random access for O(1) structural surgery. These members exist *only* on `list` (and `forward_list`), and they're why you'd pick it. Full treatment in [`LIST.md`](../containers/LIST.md).

| Member | What it does | Cost |
|---|---|---|
| `splice(pos, other)` | Move **all** of `other`'s nodes into `*this` before `pos`. No copies, no allocations — just pointer relinking. `other` ends empty. | O(1) |
| `splice(pos, other, it)` | Move the single node `it` from `other`. | O(1) |
| `splice(pos, other, first, last)` | Move a range of nodes. | O(1)† |
| `remove(value)` | Erase **every** element `== value`. (Container member — distinct from `std::remove`, which can't actually erase.) | O(n) |
| `remove_if(pred)` | Erase every element satisfying `pred`. | O(n) |
| `unique()` | Collapse **consecutive** equal elements to one. Sort first for global dedup. | O(n) |
| `merge(other)` | Merge two **sorted** lists into one sorted list, splicing nodes (no copies). `other` ends empty. | O(n) |
| `sort()` | Sort in place (merge sort). Use this — **`std::sort` won't work on a list.** | O(n log n) |
| `reverse()` | Reverse the node order in place. | O(n) |

\† `splice` of a range is O(1) in C++98 if you don't ask for `size()` afterward on an O(n)-size implementation; treat as O(1) for the relink, O(distance) if size must be recomputed.

```cpp
std::list<int> a, b;
// ... fill both, sorted ...
a.merge(b);                     // a := sorted union (with dups), b := empty. Zero copies.
a.unique();                     // drop consecutive duplicates
a.remove_if(IsNegative());      // erase all negatives in one O(n) pass
```

> **Why `list::remove` and not `std::remove`?** The free `std::remove` (Part 2, §8) only *shifts* survivors and hands you a new logical end — it can't shorten the container, because algorithms don't know about containers. `list::remove` is a real member that unlinks and frees nodes.

---

## 4. Container adapters — `stack` / `queue` / `priority_queue`

Adapters **wrap** a sequence container and expose a deliberately tiny interface. **No iterators, no `begin()`, no random access.** Detail: [`STACK.md`](../containers/STACK.md), [`QUEUE.md`](../containers/QUEUE.md), [`PRIORITY_QUEUE.md`](../containers/PRIORITY_QUEUE.md).

| Member | `stack` (LIFO) | `queue` (FIFO) | `priority_queue` (max-heap) |
|---|:---:|:---:|:---:|
| `push(x)` | ✅ | ✅ | ✅ (O(log n)) |
| `pop()` | ✅ | ✅ | ✅ (O(log n)) — **returns `void`** |
| `top()` | ✅ | ❌ | ✅ (O(1)) |
| `front()` / `back()` | ❌ | ✅ / ✅ | ❌ |
| `size()` / `empty()` | ✅ | ✅ | ✅ |
| Default backing | `deque` | `deque` | `vector` |

```cpp
std::stack<int> s;
s.push(1); s.push(2);
int x = s.top();                // 2  — read BEFORE pop
s.pop();                        // void! it does NOT return the element

std::priority_queue<int> pq;    // max-heap by default
pq.push(3); pq.push(9); pq.push(1);
pq.top();                       // 9
// min-heap: pass the comparator (see §17)
std::priority_queue<int, std::vector<int>, std::greater<int> > minpq;
```

> **The cardinal rule:** `pop()` on all three adapters returns `void`. Always `top()` (or `front()`) *first*, then `pop()`. Calling `top()` on an empty adapter is UB — guard with `empty()`.

You can swap the backing container at the type level: `std::stack<int, std::vector<int> >`. `queue` forbids `vector` backing (it needs `pop_front`).

---

## 5. Associative containers — `set` / `map` / `multiset` / `multimap`

Sorted, key-based, O(log n), backed by a balanced tree. Iterators are **bidirectional** and walk in **sorted key order**. Detail: [`SET.md`](../containers/SET.md), [`MAP.md`](../containers/MAP.md), [`MULTISET.md`](../containers/MULTISET.md), [`MULTIMAP.md`](../containers/MULTIMAP.md).

| Member | `set` | `map` | `multiset` | `multimap` | Cost | Note |
|---|:---:|:---:|:---:|:---:|---|---|
| `insert(x)` | ✅ | ✅ | ✅ | ✅ | O(log n) | Unique versions return `pair<iterator,bool>`; multi versions return `iterator`. |
| `insert(hint, x)` | ✅ | ✅ | ✅ | ✅ | O(1) amortized | A correct hint makes bulk insert O(n) instead of O(n log n). |
| `erase(key)` | ✅ | ✅ | ✅ | ✅ | O(log n) | Returns **count erased** (`size_type`). |
| `erase(it)` | ✅ | ✅ | ✅ | ✅ | O(1) amortized | **Returns `void` in C++98** — use `erase(it++)`. |
| `find(key)` | ✅ | ✅ | ✅ | ✅ | O(log n) | Iterator to element, or `end()`. |
| `count(key)` | ✅ | ✅ | ✅ | ✅ | O(log n) | 0/1 for unique; actual count for multi. |
| `lower_bound(k)` | ✅ | ✅ | ✅ | ✅ | O(log n) | First element **not less than** `k`. |
| `upper_bound(k)` | ✅ | ✅ | ✅ | ✅ | O(log n) | First element **greater than** `k`. |
| `equal_range(k)` | ✅ | ✅ | ✅ | ✅ | O(log n) | `pair(lower_bound, upper_bound)` — the daily tool for multi-containers. |
| `operator[](k)` | ❌ | ✅ | ❌ | ❌ | O(log n) | **map only.** Inserts a default value on miss — the famous trap. |
| `key_comp()` / `value_comp()` | ✅ | ✅ | ✅ | ✅ | O(1) | The comparison objects, if you need to mirror the ordering. |

```cpp
std::map<std::string, int> ages;
ages["Alice"] = 30;                          // [] inserts if absent
ages.insert(std::make_pair("Bob", 25));      // insert won't overwrite existing

// Read WITHOUT risk of inserting:
std::map<std::string, int>::iterator it = ages.find("Carol");
if (it != ages.end())
    std::cout << it->first << " = " << it->second << "\n";   // pair<const K, V>

// multi-container: get all values for a key
std::multimap<std::string, int> mm;
std::pair<std::multimap<std::string,int>::iterator,
          std::multimap<std::string,int>::iterator> r = mm.equal_range("k");
for (std::multimap<std::string,int>::iterator i = r.first; i != r.second; ++i)
    std::cout << i->second << "\n";
```

> **Two C++98 traps specific to these containers:**
> 1. **`erase(it)` returns `void`.** The sequence-container habit `it = m.erase(it)` won't compile. Use the post-increment idiom:
> ```cpp
> for (std::map<K,V>::iterator it = m.begin(); it != m.end(); ) {
>     if (cond(it)) m.erase(it++);   // evaluate it, advance, THEN erase the old position
>     else          ++it;
> }
> ```
> 2. **Use the *member* `lower_bound`, not the `<algorithm>` one.** `std::lower_bound(s.begin(), s.end(), k)` *works* on a `set` but is **O(n)** — the free algorithm needs random access to binary-search, and tree iterators only step one at a time. `s.lower_bound(k)` walks the tree in true O(log n).

---

## 6. `std::string` as a container

`std::string` is `std::basic_string<char>` — a sequence container with extra text methods. It supports `operator[]`, `at`, `begin`/`end`, `push_back`, `insert`, `erase`, `size`, `empty`, `clear`, plus string-specific `find`, `substr`, `c_str`, `append`, `+`. Full method list: [`../io-errors/STRING_FUNCTIONS.md`](../io-errors/STRING_FUNCTIONS.md). Because its iterators are random-access, every `<algorithm>` works on it:

```cpp
std::string s = "hello";
std::sort(s.begin(), s.end());                              // "ehllo"
std::reverse(s.begin(), s.end());                           // "ollhe"
std::transform(s.begin(), s.end(), s.begin(), ::toupper);   // "OLLHE"
```

---

# Part 2 — The `<algorithm>` library (exhaustive, C++98)

Every algorithm below is in `<algorithm>` and exists in C++98. Anything C++11+ is called out in §20. Naming pattern to internalize:
- **`_if`** suffix → takes a *predicate* instead of a value (`find` → `find_if`).
- **`_copy`** suffix → writes results to a separate output range instead of mutating in place (`remove` → `remove_copy`).
- Many take an optional trailing **comparator / predicate** functor — that overload is listed inline.

## 7. Non-modifying sequence operations

These read a range; they never reorder or write it.

| Function | Signature shape | Returns | Cost | What it does |
|---|---|---|---|---|
| `for_each` | `(first, last, fn)` | a copy of `fn` | O(n) | Calls `fn(*it)` on each. Returns the functor — handy for **stateful** accumulators. |
| `find` | `(first, last, val)` | iterator | O(n) | First element `== val`, else `last`. |
| `find_if` | `(first, last, pred)` | iterator | O(n) | First element where `pred(*it)` is true. |
| `find_end` | `(first, last, s_first, s_last)` | iterator | O(n·m) | Last occurrence of the subrange. |
| `find_first_of` | `(first, last, s_first, s_last)` | iterator | O(n·m) | First element that equals *any* element in the second range. |
| `adjacent_find` | `(first, last[, pred])` | iterator | O(n) | First spot where two **consecutive** elements match. Negate for an `is_sorted` check. |
| `count` | `(first, last, val)` | `difference_type` | O(n) | How many `== val`. |
| `count_if` | `(first, last, pred)` | `difference_type` | O(n) | How many satisfy `pred`. |
| `mismatch` | `(first, last, first2[, pred])` | `pair<It1,It2>` | O(n) | First position where two ranges differ. |
| `equal` | `(first, last, first2[, pred])` | `bool` | O(n) | Are two ranges element-wise equal? (Second range must be ≥ as long.) |
| `search` | `(first, last, s_first, s_last)` | iterator | O(n·m) | First occurrence of a subrange (substring-style search). |
| `search_n` | `(first, last, count, val)` | iterator | O(n) | First run of `count` consecutive `val`. |

```cpp
// Linear find — works on ANY container (this is what easyfind wraps in CPP08):
std::vector<int>::iterator it = std::find(v.begin(), v.end(), 42);
if (it != v.end()) { /* found at *it */ }

// for_each with a stateful functor returns the functor back, holding the result:
struct Sum { long total; Sum() : total(0) {} void operator()(int n) { total += n; } };
Sum s = std::for_each(v.begin(), v.end(), Sum());
std::cout << s.total << "\n";

// count_if with a parametrized functor (see §17):
int big = std::count_if(v.begin(), v.end(), GreaterThan(100));
```

## 8. Modifying sequence operations

These write to or reorder a range. **The `remove`/`unique` family does NOT shrink the container** — see the erase-remove idiom (§19).

| Function | Signature shape | Returns | Cost | What it does |
|---|---|---|---|---|
| `copy` | `(first, last, d_first)` | output end iterator | O(n) | Copy forward into `d_first…`. Ranges must not overlap left-to-right. |
| `copy_backward` | `(first, last, d_last)` | output begin | O(n) | Copy back-to-front — use when ranges overlap and dest is *after* src. |
| `swap` | `(a, b)` | `void` | O(1) | Swap two objects. (In `<algorithm>` for C++98; moved to `<utility>` in C++11.) |
| `swap_ranges` | `(first, last, first2)` | end of 2nd | O(n) | Swap two equal-length ranges element-wise. |
| `iter_swap` | `(it_a, it_b)` | `void` | O(1) | Swap the two pointed-to elements. |
| `transform` | `(first, last, d_first, unary_fn)` | output end | O(n) | Write `fn(*it)` to dest. **Map operation.** |
| `transform` | `(first, last, first2, d_first, binary_fn)` | output end | O(n) | Combine two ranges: `fn(*a, *b)`. |
| `replace` | `(first, last, old, new)` | `void` | O(n) | Overwrite every `old` with `new` in place. |
| `replace_if` | `(first, last, pred, new)` | `void` | O(n) | Overwrite where `pred` holds. |
| `replace_copy` / `replace_copy_if` | `(…, d_first, …)` | output end | O(n) | Same, writing to a dest range. |
| `fill` | `(first, last, val)` | `void` | O(n) | Set every element to `val`. |
| `fill_n` | `(first, n, val)` | output end | O(n) | Set the first `n` elements. |
| `generate` | `(first, last, gen)` | `void` | O(n) | Assign `gen()` to each (functor called per element). |
| `generate_n` | `(first, n, gen)` | output end | O(n) | First `n` from `gen()`. |
| `remove` | `(first, last, val)` | new logical end | O(n) | **Shifts** non-`val` to the front; tail is garbage. Pair with `erase`. |
| `remove_if` | `(first, last, pred)` | new logical end | O(n) | Same, by predicate. |
| `remove_copy` / `remove_copy_if` | `(…, d_first, …)` | output end | O(n) | Copy out only the kept elements. `remove_copy_if` + `not1` is the C++98 `copy_if`. |
| `unique` | `(first, last[, pred])` | new logical end | O(n) | Collapse **consecutive** equal elements. Sort first for global dedup. |
| `unique_copy` | `(first, last, d_first[, pred])` | output end | O(n) | Copy out with consecutive dups removed. |
| `reverse` | `(first, last)` | `void` | O(n) | Reverse in place (needs bidirectional). |
| `reverse_copy` | `(first, last, d_first)` | output end | O(n) | Reversed copy into dest. |
| `rotate` | `(first, middle, last)` | `void` (C++98) | O(n) | Rotate so `middle` becomes the new first. |
| `rotate_copy` | `(first, middle, last, d_first)` | output end | O(n) | Rotated copy. |
| `random_shuffle` | `(first, last[, rng])` | `void` | O(n) | Shuffle. **Deprecated C++14 / removed C++17** — fine in C++98, but know it's a dead end; modern code uses `std::shuffle`. |

```cpp
// transform = map. Square every element into a new vector:
std::vector<int> out(v.size());
std::transform(v.begin(), v.end(), out.begin(), Square());

// transform with two inputs = zip-with. Element-wise sum of a and b into c:
std::transform(a.begin(), a.end(), b.begin(), c.begin(), std::plus<int>());

// fill / generate
std::fill(v.begin(), v.end(), 0);
std::generate(v.begin(), v.end(), Counter());   // 0,1,2,3,... if Counter() returns then ++

// remove does NOT erase — see §19 for the full idiom:
v.erase(std::remove(v.begin(), v.end(), 0), v.end());
```

> **Why `remove` can't remove (read this once, save hours):** algorithms only hold iterators, never the container, so they cannot call `erase` or change `size()`. `std::remove` does the only thing it can — slide the survivors forward and return where the "real" data now ends. The leftover tail still exists. You finish the job with the container's own `erase(new_end, end())`.

## 9. Partitioning

| Function | Signature | Returns | Cost | What it does |
|---|---|---|---|---|
| `partition` | `(first, last, pred)` | partition point | O(n) | Reorder so all `pred`-true precede all `pred`-false. **Order within groups not preserved.** |
| `stable_partition` | `(first, last, pred)` | partition point | O(n log n) | Same, but **preserves relative order** within each group. |

The returned iterator points at the first element of the second (false) group:

```cpp
std::vector<int>::iterator mid =
    std::stable_partition(v.begin(), v.end(), IsEven());
// [v.begin(), mid) are even (original order kept); [mid, v.end()) are odd.
```

## 10. Sorting

| Function | Signature | Cost | What it does |
|---|---|---|---|
| `sort` | `(first, last[, cmp])` | O(n log n) | In-place introsort. **Not stable.** Needs random-access iterators. |
| `stable_sort` | `(first, last[, cmp])` | O(n log n)† | Preserves order of equal elements. †O(n log²n) if it can't grab scratch memory. |
| `partial_sort` | `(first, middle, last[, cmp])` | O(n log k) | Sorts only enough that `[first, middle)` holds the k smallest, sorted. Rest unspecified. |
| `partial_sort_copy` | `(first, last, d_first, d_last[, cmp])` | O(n log k) | Top-k into a separate (possibly smaller) dest. |
| `nth_element` | `(first, nth, last[, cmp])` | **O(n)** average | Places the element that *would* be at `nth` if sorted; everything before is ≤ it, after is ≥ it. The fast way to a median / percentile. |

```cpp
std::sort(v.begin(), v.end());                          // ascending
std::sort(v.begin(), v.end(), std::greater<int>());     // descending (functor, §17)
std::sort(v.begin(), v.end(), ByLength());              // custom: strict weak ordering

// "Top 3 smallest, sorted; I don't care about the rest" — cheaper than a full sort:
std::partial_sort(v.begin(), v.begin() + 3, v.end());

// Median without sorting the whole thing — O(n):
std::nth_element(v.begin(), v.begin() + v.size()/2, v.end());
int median = v[v.size()/2];
```

> **Comparator contract:** `cmp(a, b)` must be a **strict weak ordering** — return `true` only when `a` strictly precedes `b`, and `cmp(a, a)` must be `false`. A `<=` comparator (returning true on equal) violates it and can **segfault** or loop. When in doubt, model it on `std::less`.

## 11. Binary search (on sorted ranges)

These require the range to already be **sorted by the same criterion**. All are O(log n) *comparisons* — but only O(log n) *total* on random-access iterators; on a `list` they degrade to O(n) because advancing the iterator is linear (use the container's member version on `set`/`map`).

| Function | Returns | What it does |
|---|---|---|
| `lower_bound(first, last, val[, cmp])` | iterator | First position where `val` could insert without breaking order — i.e. first element **not less than** `val`. |
| `upper_bound(first, last, val[, cmp])` | iterator | First element **strictly greater than** `val`. |
| `equal_range(first, last, val[, cmp])` | `pair<It,It>` | `(lower_bound, upper_bound)` — the whole run equal to `val`. |
| `binary_search(first, last, val[, cmp])` | `bool` | Just "is it present?" — no position. |

```cpp
std::sort(v.begin(), v.end());
std::vector<int>::iterator it = std::lower_bound(v.begin(), v.end(), key);
bool found = (it != v.end() && *it == key);   // lower_bound alone doesn't confirm equality

// This is the BitcoinExchange (CPP09) pattern: "value at the closest date <= query".
// lower_bound gives first >= query; step back one for "closest not after".
```

## 12. Merge & set operations (on sorted ranges)

Both input ranges must be **sorted**. Output is sorted. These are how you do unions/intersections without building a `set`.

| Function | Returns | What it does |
|---|---|---|
| `merge(f1,l1, f2,l2, d_first[, cmp])` | output end | Merge two sorted ranges into one sorted dest. |
| `inplace_merge(first, middle, last[, cmp])` | `void` | Merge two consecutive sorted subranges `[first,middle)` and `[middle,last)` in place. (Backbone of merge sort / PmergeMe.) |
| `includes(f1,l1, f2,l2[, cmp])` | `bool` | Is the second sorted range a subset of the first? |
| `set_union(f1,l1, f2,l2, d_first[, cmp])` | output end | A ∪ B (each element max(countA, countB) times). |
| `set_intersection(…)` | output end | A ∩ B. |
| `set_difference(…)` | output end | A \ B (in A, not in B). |
| `set_symmetric_difference(…)` | output end | In exactly one of A, B. |

```cpp
// a, b are sorted vectors. Compute the intersection into c:
std::vector<int> c;
std::set_intersection(a.begin(), a.end(),
                      b.begin(), b.end(),
                      std::back_inserter(c));   // back_inserter: grow c as we go (§18)
```

## 13. Heap operations

A heap here is a max-heap stored in a random-access range (a `vector`, usually). This is what `std::priority_queue` uses under the hood — reach for these only when you need the array form directly.

| Function | Signature | Cost | What it does |
|---|---|---|---|
| `make_heap(first, last[, cmp])` | | O(n) | Rearrange the range into a valid heap; max at `first`. |
| `push_heap(first, last[, cmp])` | | O(log n) | After you `push_back`ed a new element, sift it into place. |
| `pop_heap(first, last[, cmp])` | | O(log n) | Move the max to `last-1`; then you `pop_back` it. |
| `sort_heap(first, last[, cmp])` | | O(n log n) | Turn a heap into a sorted range (destroys heap property). |

```cpp
std::vector<int> v;  // ...filled...
std::make_heap(v.begin(), v.end());           // v[0] is now the max
v.push_back(99);
std::push_heap(v.begin(), v.end());           // restore heap including the new 99
std::pop_heap(v.begin(), v.end());            // max moved to back
int max = v.back();  v.pop_back();            // and removed
```

> `std::is_heap` / `std::is_heap_until` are **C++11** — not available.

## 14. Min / max

| Function | Returns | Cost | Note |
|---|---|---|---|
| `min(a, b[, cmp])` | smaller value | O(1) | Two args only in C++98 — **no init-list** `min({a,b,c})`. Nest: `min(a, min(b, c))`. |
| `max(a, b[, cmp])` | larger value | O(1) | Same restriction. |
| `min_element(first, last[, cmp])` | iterator | O(n) | Iterator to smallest. `end()` if range empty. |
| `max_element(first, last[, cmp])` | iterator | O(n) | Iterator to largest. |

```cpp
int lo = *std::min_element(v.begin(), v.end());
int hi = *std::max_element(v.begin(), v.end());
// longestSpan() in CPP08's Span exercise is exactly hi - lo.
```

> `std::minmax` and `std::minmax_element` (one pass for both) are **C++11**.

## 15. Permutations & comparison

| Function | Returns | Cost | What it does |
|---|---|---|---|
| `next_permutation(first, last[, cmp])` | `bool` | O(n) | Rearrange to the next lexicographically greater permutation. Returns `false` and wraps to the first (sorted) permutation when it was the last. |
| `prev_permutation(first, last[, cmp])` | `bool` | O(n) | The previous permutation. |
| `lexicographical_compare(f1,l1, f2,l2[, cmp])` | `bool` | O(n) | Dictionary-order `<` between two ranges. The engine behind container `operator<`. |

```cpp
std::vector<int> v;  // must be SORTED to enumerate all permutations
std::sort(v.begin(), v.end());
do {
    /* use this permutation of v */
} while (std::next_permutation(v.begin(), v.end()));
```

---

# Part 3 — Companions

## 16. `<numeric>`

Four numeric folds. **`std::iota` is C++11 — not here.**

| Function | Signature | Returns | What it does |
|---|---|---|---|
| `accumulate` | `(first, last, init[, binop])` | the fold result | Sum, or fold with a custom binary op. |
| `inner_product` | `(first1, last1, first2, init[, op1, op2])` | the result | Dot product, or generalized fold over two ranges. |
| `partial_sum` | `(first, last, d_first[, binop])` | output end | Running totals: `out[i] = in[0]+…+in[i]`. |
| `adjacent_difference` | `(first, last, d_first[, binop])` | output end | `out[0]=in[0]`, `out[i]=in[i]-in[i-1]`. Inverse of `partial_sum`. |

```cpp
int total = std::accumulate(v.begin(), v.end(), 0);                 // sum
long prod  = std::accumulate(v.begin(), v.end(), 1L, std::multiplies<long>());  // product
double dot = std::inner_product(a.begin(), a.end(), b.begin(), 0.0);            // a·b
```

> **Pick the right `init` type.** `accumulate(v.begin(), v.end(), 0)` over a `vector<double>` sums into an **`int`** and truncates — the accumulator type *is* the type of `init`. Use `0.0` (or `0L`) deliberately.

## 17. `<functional>` — functors & adaptors

C++98 has no lambdas. A **functor** is a class with `operator()`; you pass an *instance* where a callable is expected. The standard ships ready-made ones. (Deeper dive: [`STL.md` §10](STL.md#10-functors--c98s-answer-to-lambdas), and `operator()` itself in [`MEMBER_FUNCTION_POINTERS.md`](MEMBER_FUNCTION_POINTERS.md).)

**Ready-made functors** (each templated on `T`):

| Group | Functors |
|---|---|
| Arithmetic | `plus`, `minus`, `multiplies`, `divides`, `modulus`, `negate` |
| Comparison | `equal_to`, `not_equal_to`, `greater`, `less`, `greater_equal`, `less_equal` |
| Logical | `logical_and`, `logical_or`, `logical_not` |

```cpp
std::sort(v.begin(), v.end(), std::greater<int>());                  // descending
std::transform(a.begin(), a.end(), b.begin(), out.begin(), std::plus<int>());
std::priority_queue<int, std::vector<int>, std::greater<int> > minheap;  // min-heap
```

**Adaptors** — turn ordinary functions / binary functors into the unary predicate an algorithm wants. (All **deprecated in C++11**, but they're the C++98 toolkit. The modern replacement is lambdas, which you don't have.)

| Adaptor | Turns… | …into |
|---|---|---|
| `not1(pred)` | a unary predicate | its negation |
| `not2(pred)` | a binary predicate | its negation |
| `bind1st(f, x)` | binary `f` | unary `f(x, ·)` |
| `bind2nd(f, x)` | binary `f` | unary `f(·, x)` |
| `ptr_fun(fp)` | a free function pointer | a functor (so you can `bind`/`not` it) |
| `mem_fun(&C::m)` | a member function | a functor taking a `C*` |
| `mem_fun_ref(&C::m)` | a member function | a functor taking a `C&` |

```cpp
// "count elements > 5" using bind2nd + greater, no custom functor:
int n = std::count_if(v.begin(), v.end(),
                      std::bind2nd(std::greater<int>(), 5));

// "copy out the elements that are NOT spaces" — C++98 copy_if:
std::remove_copy_if(s.begin(), s.end(), std::back_inserter(out),
                    std::bind2nd(std::equal_to<char>(), ' '));

// call a member on every element of a vector<Widget*>:
std::for_each(ptrs.begin(), ptrs.end(), std::mem_fun(&Widget::draw));
```

In practice these adaptors get unreadable fast (nested `bind2nd(not2(...))`). For anything non-trivial, **write a named functor** — it's clearer and the standard idiom in the 42 cursus:

```cpp
struct GreaterThan {
    int n;
    GreaterThan(int v) : n(v) {}
    bool operator()(int x) const { return x > n; }
};
int n = std::count_if(v.begin(), v.end(), GreaterThan(5));
```

To make a functor `bind`-able you derive from `std::unary_function<Arg, Result>` or `std::binary_function<Arg1, Arg2, Result>` (they just supply the `typedef`s the adaptors look for).

## 18. `<iterator>` — the glue

Algorithms that *write* (`copy`, `transform`, `remove_copy`, `set_*`) need a destination iterator. If the dest container is empty, you can't hand them `dest.begin()` (there's nowhere to write). **Insert iterators** solve this by calling `push_back` / `insert` under the hood.

| Helper | What it does |
|---|---|
| `back_inserter(c)` | Output iterator that `push_back`s into `c`. The workhorse. |
| `front_inserter(c)` | `push_front`s (so `deque`/`list`, not `vector`). |
| `inserter(c, pos)` | `insert`s at `pos` (works for associative containers too). |
| `ostream_iterator<T>(os, sep)` | Output iterator that writes to a stream — print a range in one line. |
| `istream_iterator<T>(is)` | Input iterator that reads `T`s from a stream until EOF. |
| `distance(first, last)` | Number of steps between two iterators (O(1) random-access, O(n) otherwise). |
| `advance(it, n)` | Move `it` by `n` (the only portable way to `+n` a non-random-access iterator). |

```cpp
// Grow the destination as the algorithm produces output:
std::vector<int> evens;
std::remove_copy_if(v.begin(), v.end(), std::back_inserter(evens), IsOdd());

// Print a range without a loop:
std::copy(v.begin(), v.end(), std::ostream_iterator<int>(std::cout, " "));
std::cout << "\n";

// Read all ints from stdin into a vector:
std::vector<int> nums((std::istream_iterator<int>(std::cin)),
                       std::istream_iterator<int>());   // note: extra parens dodge the
                                                        // "most vexing parse"
```

---

## 19. Idiom cookbook

The handful of compound patterns that show up constantly. Memorize the shapes.

**Erase–remove — delete all matches in O(n):**
```cpp
v.erase(std::remove(v.begin(), v.end(), value), v.end());          // by value
v.erase(std::remove_if(v.begin(), v.end(), IsBad()), v.end());     // by predicate
```
`std::remove` shifts survivors forward and returns the new end; `erase` trims the dead tail. Without this you write an erase-in-a-loop that's O(n²) and easy to get wrong. (On a `list`, prefer the member `l.remove_if(IsBad())`.)

**Sort + unique — global dedup:**
```cpp
std::sort(v.begin(), v.end());
v.erase(std::unique(v.begin(), v.end()), v.end());
```
`unique` only kills *consecutive* dups, so you sort first.

**Shrink a vector to fit (no `shrink_to_fit` in C++98):**
```cpp
std::vector<int>(v).swap(v);     // copy holds exactly size() capacity; swap it in
std::vector<int>().swap(v);      // or fully release: empty + capacity 0
```

**Delete pointers owned by a container, then clear:**
```cpp
for (std::vector<T*>::iterator it = v.begin(); it != v.end(); ++it)
    delete *it;
v.clear();
```
No smart pointers in C++98 (`std::auto_ptr` is broken inside containers — never store it). You own the `delete`. See [`../fundamentals/MEMORY.md`](../fundamentals/MEMORY.md).

**Erase-while-iterating an associative container (the `void`-return workaround):**
```cpp
for (std::map<K,V>::iterator it = m.begin(); it != m.end(); ) {
    if (drop(it)) m.erase(it++);   // post-increment: it advances, old copy gets erased
    else          ++it;
}
```

**Print any range in one line:**
```cpp
std::copy(c.begin(), c.end(), std::ostream_iterator<T>(std::cout, " "));
```

---

## 20. What's NOT in C++98

Quick reject-list — if you see these in a tutorial, find the C++98 form above.

| Symbol | Introduced | Use instead |
|---|---|---|
| `copy_if` | C++11 | `remove_copy_if` + `not1` / negated predicate |
| `all_of`, `any_of`, `none_of` | C++11 | `find_if` / `count_if` + compare |
| `find_if_not` | C++11 | `find_if` + `not1(pred)` |
| `is_sorted`, `is_sorted_until` | C++11 | `adjacent_find` with a `>` comparator |
| `is_heap`, `is_heap_until` | C++11 | — (just trust your own `make_heap`) |
| `min`/`max` with `{init list}` | C++11 | nest two-arg calls |
| `minmax`, `minmax_element` | C++11 | separate `min_element` + `max_element` |
| `iota` | C++11 (`<numeric>`) | hand loop or `generate` + counter functor |
| `shuffle` | C++11 | `random_shuffle` (itself dead after C++17) |
| `move`, `move_backward` | C++11 | `copy` / `copy_backward` |
| `emplace`, `emplace_back`, `emplace_front` | C++11 | `insert` / `push_back` / `push_front` |
| `cbegin`, `cend` | C++11 | `const_iterator` from a `const` container |
| `auto`, range-`for`, lambdas | C++11 | spell the type; explicit loop; functors (§17) |
| `std::function`, `std::bind` | C++11 | function pointers + `bind1st`/`bind2nd`, or named functors |
| `unordered_set`, `unordered_map` | C++11 | `set` / `map` (O(log n), not O(1), but sorted) |

---

> **One-line summary.** Containers give you `push_back` / `insert` / `find` / `erase` (mind which ones return `void` in C++98); `<algorithm>` gives you everything that operates on `[first, last)` — search, modify, sort, set-ops, heap, min/max; glue them with functors and insert iterators. When a function looks standard but won't compile under `-std=c++98`, check §20.
