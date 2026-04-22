# The STL — Complete Guide

The **Standard Template Library** is the C++ standard library's collection of ready-made, generic, efficient data structures and algorithms. Learn it once and you never hand-roll a linked list or a sort again.

The STL is built on three legs:

```
    CONTAINERS         ITERATORS           ALGORITHMS
    (store data)  ◄──►  (walk data)  ◄──►  (process data)

    vector              begin/end            sort
    list                ++/--                find
    map                  *                   copy
    stack               -> and .             accumulate
    ...                 ...                  ...
```

The genius is that algorithms don't know which container they're working on — they talk to **iterators**, which every container provides. One `sort` works on every container whose iterators are random-access. One `find` works on almost anything.

---

## Table of Contents

1. [The philosophy](#1-the-philosophy)
2. [Containers overview](#2-containers-overview)
3. [Sequence containers](#3-sequence-containers)
4. [Container adapters](#4-container-adapters)
5. [Associative containers](#5-associative-containers)
6. [Iterators](#6-iterators)
7. [Iterator categories](#7-iterator-categories)
8. [Algorithms](#8-algorithms)
9. [`std::pair` and `std::make_pair`](#9-stdpair-and-stdmake_pair)
10. [Functors — C++98's answer to lambdas](#10-functors--c98s-answer-to-lambdas)
11. [In CPP08 — iterators + templated containers](#11-in-cpp08--iterators--templated-containers)
12. [In CPP09 — picking the right container](#12-in-cpp09--picking-the-right-container)
13. [Cheatsheet: which container when](#13-cheatsheet-which-container-when)
14. [Rules of thumb](#14-rules-of-thumb)
15. [Gotchas](#15-gotchas)

---

## 1. The philosophy

The STL separates three things that languages usually conflate:

- **What you store** — the container.
- **How you move through it** — the iterator.
- **What you do with it** — the algorithm.

A `std::sort` doesn't care whether it's sorting an array, a `vector`, or a `deque`. It calls `*it`, `++it`, `swap`. Any iterator that supports those works. That's the unit of composition.

```cpp
std::vector<int> v; v.push_back(3); v.push_back(1); v.push_back(2);
std::sort(v.begin(), v.end());       // 1, 2, 3

int arr[] = {3, 1, 2};
std::sort(arr, arr + 3);              // raw pointers are iterators too
```

This is why templates matter: the STL works because every container exposes types and operations the algorithms can count on. See [TEMPLATES.md](TEMPLATES.md).

---

## 2. Containers overview

Three families:

| Family | Examples | Access by |
|---|---|---|
| **Sequence** | `vector`, `list`, `deque`, `string` | Position (index, iterator) |
| **Container adapter** | `stack`, `queue`, `priority_queue` | Restricted operations on a sequence |
| **Associative** | `set`, `map`, `multiset`, `multimap` | Key |

All of them require:
```cpp
#include <vector>     // and so on — one header per container
```

---

## 3. Sequence containers

### `std::vector<T>` — the default

A dynamic array. Contiguous memory, fast random access, amortized O(1) push_back.

```cpp
#include <vector>

std::vector<int> v;
v.push_back(10);
v.push_back(20);
v[0];                            // 10 — O(1)
v.size();                        // 2
v.begin(); v.end();              // iterators

std::vector<int> w(5, 42);       // [42, 42, 42, 42, 42]
std::vector<int> x(v);           // copy
std::vector<int> y(v.begin(), v.end());   // range copy
```

**When to use:** your default. Fast, cache-friendly, simple.

Key operations:

| Op | Cost |
|---|---|
| `v[i]` | O(1) |
| `v.push_back(x)` | Amortized O(1) |
| `v.pop_back()` | O(1) |
| `v.insert(it, x)` | O(n) — shifts |
| `v.erase(it)` | O(n) — shifts |
| `v.size()`, `v.empty()` | O(1) |

**Gotcha:** inserting/erasing invalidates iterators (and sometimes all of them, if capacity has to grow).

### `std::list<T>` — doubly-linked list

Fast insert/erase anywhere, no random access.

```cpp
#include <list>

std::list<int> l;
l.push_back(1);
l.push_front(0);
l.size();                        // 2
// l[0];                         // NO — list has no operator[]

std::list<int>::iterator it = l.begin();
++it;
l.insert(it, 99);                // insert before position — O(1)
l.erase(it);                     // O(1)
```

**When to use:** many inserts/erases in the middle; you don't need random access; you need iterator/reference stability after insert.

### `std::deque<T>` — double-ended queue

Random-accessible like a vector, but push/pop at both ends is O(1). Implemented as a sequence of blocks — *not* contiguous.

```cpp
#include <deque>

std::deque<int> d;
d.push_back(1);
d.push_front(0);
d[0];                            // 0 — O(1)
```

**When to use:** you need fast push/pop at both ends **and** random access. Otherwise vector.

### `std::string`

Technically a sequence container (it's `std::basic_string<char>`). See [STRING_FUNCTIONS.md](STRING_FUNCTIONS.md).

---

## 4. Container adapters

These **wrap** a sequence container and expose a restricted interface.

### `std::stack<T>` — LIFO

```cpp
#include <stack>

std::stack<int> s;
s.push(1);
s.push(2);
s.top();                         // 2
s.pop();                         // removes top; returns void (NOT the element!)
s.size();
s.empty();
```

No iterators. No random access. `pop` doesn't return — you call `top()` first, then `pop()`.

**Default underlying container:** `std::deque`. You can change it:
```cpp
std::stack<int, std::vector<int> > sv;
```

### `std::queue<T>` — FIFO

```cpp
#include <queue>

std::queue<int> q;
q.push(1);
q.push(2);
q.front();                       // 1 — oldest
q.back();                        // 2 — newest
q.pop();                         // removes front
```

### `std::priority_queue<T>` — max-heap by default

```cpp
#include <queue>

std::priority_queue<int> pq;
pq.push(3);
pq.push(1);
pq.push(5);
pq.top();                        // 5 — largest first
pq.pop();
```

Min-heap: pass `std::greater<T>` as the comparator (see §10).

---

## 5. Associative containers

These keep elements **sorted by key** (using `<` by default) and give O(log n) find/insert/erase. Implemented as balanced BSTs (red-black trees, typically).

### `std::set<T>` — unique, sorted values

```cpp
#include <set>

std::set<int> s;
s.insert(3);
s.insert(1);
s.insert(3);                     // ignored — already present
// Iteration walks in sorted order:
for (std::set<int>::iterator it = s.begin(); it != s.end(); ++it)
    std::cout << *it << ' ';     // "1 3"

s.find(3);                       // iterator to 3, or s.end() if missing
s.count(3);                      // 0 or 1
s.erase(3);                      // by value
```

### `std::map<Key, Value>` — unique keys, sorted by key

```cpp
#include <map>

std::map<std::string, int> ages;
ages["Alice"] = 30;
ages["Bob"]   = 25;

std::map<std::string, int>::iterator it = ages.find("Alice");
if (it != ages.end())
    std::cout << it->first << " = " << it->second << '\n';

// Iteration walks keys in sorted order
for (std::map<std::string, int>::iterator it = ages.begin(); it != ages.end(); ++it)
    std::cout << it->first << " = " << it->second << '\n';
```

**Element type is `std::pair<const Key, Value>`.** That's why you do `it->first` (key) and `it->second` (value).

**Gotcha:** `ages["nobody"]` **inserts a default-constructed entry** if the key isn't present. Use `find` if you want to avoid that.

### `std::multiset<T>` and `std::multimap<Key, Value>`

Like `set` / `map`, but duplicates are allowed.

```cpp
std::multiset<int> ms;
ms.insert(1); ms.insert(1); ms.insert(2);
ms.count(1);                     // 2
```

---

## 6. Iterators

An iterator is a "generalized pointer." It responds to the same operations a pointer does: `*it`, `++it`, `it1 == it2`.

Every container has these nested types:
```cpp
std::vector<int>::iterator       it;
std::vector<int>::const_iterator cit;
std::vector<int>::reverse_iterator rit;
```

Every container exposes:
```cpp
c.begin();       // iterator to first element
c.end();         // iterator to "one past the last" — NOT a valid element
c.rbegin();      // reverse iterator starting at last element
c.rend();        // reverse iterator at "one before first"
```

### The canonical loop

```cpp
for (std::vector<int>::iterator it = v.begin(); it != v.end(); ++it)
    std::cout << *it << '\n';
```

- `!=` not `<` — many iterator categories don't support `<`.
- `++it` not `it++` — prefix avoids a needless copy.
- `*it` dereferences to the element. `it->member` works when the element is a class.

### Iterator or pointer?

Raw pointers **are** iterators for C arrays:
```cpp
int arr[] = {3, 1, 2};
std::sort(arr, arr + 3);          // arr and arr+3 are iterators
```

Any container's `.data()` (C++11) gives you a pointer, but in C++98, iterate with `begin()`/`end()`.

---

## 7. Iterator categories

Not every iterator supports every operation. There are five **categories**, each a superset of the one before:

```
InputIterator        ── read once, ++
   │
OutputIterator       ── write once, ++
   │
ForwardIterator      ── read/write, ++ (list, set)
   │
BidirectionalIterator ── ++ and --          (list, map, set)
   │
RandomAccessIterator ── +n, -n, [], <       (vector, deque, raw pointers)
```

Algorithms specify which category they need. `std::sort` needs random-access; that's why you can't `std::sort` a `std::list`. (`list` has its own `.sort()` member for exactly this reason.)

---

## 8. Algorithms

`#include <algorithm>` and `#include <numeric>`. The algorithms take iterator ranges `[first, last)` — `last` is one past the end, same as `end()`.

### The ones you'll actually use

| Algorithm | What it does |
|---|---|
| `std::sort(first, last)` | Sort in place (random-access) |
| `std::sort(first, last, cmp)` | Sort with custom comparator |
| `std::find(first, last, val)` | Find first occurrence; returns iterator or `last` |
| `std::find_if(first, last, pred)` | Find first element matching predicate |
| `std::count(first, last, val)` | Count occurrences |
| `std::copy(first, last, dest)` | Copy range to output iterator |
| `std::fill(first, last, val)` | Fill range with a value |
| `std::transform(first, last, dest, fn)` | Apply `fn` to each; write results to `dest` |
| `std::for_each(first, last, fn)` | Apply `fn` to each (result ignored) |
| `std::accumulate(first, last, init)` | Sum (or fold with a custom binary op) — in `<numeric>` |
| `std::min_element(first, last)` | Iterator to smallest |
| `std::max_element(first, last)` | Iterator to largest |
| `std::reverse(first, last)` | Reverse in place |
| `std::unique(first, last)` | Remove consecutive duplicates; returns new end |
| `std::remove_if(first, last, pred)` | Move non-matching to front; return new end |

### The erase-remove idiom

`remove` and `remove_if` don't actually remove — they **shift** the keepers to the front and return the new logical end. You must follow with `erase`:

```cpp
v.erase(std::remove_if(v.begin(), v.end(), is_odd), v.end());
```

This gets easier when you've seen it three times. Until then, it's confusing.

---

## 9. `std::pair` and `std::make_pair`

A `std::pair<T1, T2>` holds two values. `map` elements are pairs.

```cpp
#include <utility>

std::pair<std::string, int> p("Alice", 30);
p.first;                         // "Alice"
p.second;                        // 30

std::pair<std::string, int> q = std::make_pair("Bob", 25);   // types deduced
```

**`make_pair` deduces both types**, sparing you writing them twice. This is why it exists.

---

## 10. Functors — C++98's answer to lambdas

No lambdas in C++98. The alternative: **functors** — classes with `operator()`:

```cpp
struct IsEven {
    bool operator()(int n) const { return n % 2 == 0; }
};

std::vector<int> evens;
std::copy_if(v.begin(), v.end(), std::back_inserter(evens), IsEven());
//                                                           ^^^^^^^
//                                               instantiate, pass as callable
```

(`copy_if` is C++11; in C++98 use `std::remove_copy_if` with the negation.)

### Built-in functors

`<functional>` has many: `std::less<T>`, `std::greater<T>`, `std::plus<T>`, `std::equal_to<T>`, ...

```cpp
std::sort(v.begin(), v.end(), std::greater<int>());   // sort descending
```

### Functors with state

Unlike plain function pointers, functors can carry data:

```cpp
struct GreaterThan {
    int threshold;
    GreaterThan(int t) : threshold(t) {}
    bool operator()(int n) const { return n > threshold; }
};

std::count_if(v.begin(), v.end(), GreaterThan(10));   // how many are > 10
```

This is exactly what lambdas do in C++11 — capture state in an unnamed functor. But you're in C++98, so you write the class.

---

## 11. In CPP08 — iterators + templated containers

- **ex00 — `easyfind`** — a function template `easyfind(container, value)` that returns an iterator to the first occurrence, or throws if missing. Works on any container providing `begin()`, `end()`, and `iterator`. This is your first taste of writing generic code *against* the STL.

- **ex01 — `Span`** — a class storing up to `N` numbers with:
  - `addNumber(int)` — throws if full.
  - `shortestSpan()` — minimum |difference| between any two numbers.
  - `longestSpan()` — max minus min.
  - An overload that adds a range via iterators: `void addNumbers(InputIt first, InputIt last)`.

  Hint: sort once, then `shortestSpan` is a single linear scan over adjacent differences. Use `std::min_element` / `std::max_element` for `longestSpan`.

- **ex02 — `MutantStack`** — inherit from `std::stack<T>` (yes, really) and add iterators. `std::stack` doesn't expose iterators by default, but the underlying container does. You expose them through:
  ```cpp
  template <typename T>
  class MutantStack : public std::stack<T> {
  public:
      typedef typename std::stack<T>::container_type::iterator iterator;
      iterator begin() { return this->c.begin(); }
      iterator end()   { return this->c.end(); }
  };
  ```
  `this->c` is the `protected` underlying container the standard promises is there. `typename` is required (see [TEMPLATES.md §5](TEMPLATES.md)).

---

## 12. In CPP09 — picking the right container

- **ex00 — Bitcoin Exchange** — read a CSV of dates and prices; read input queries; output value at that date (or closest earlier). This is **`std::map<std::string, float>`** territory — O(log n) lookup and sorted iteration.

- **ex01 — Reverse Polish Notation** — evaluate RPN expressions like `"8 9 * 9 - 9 - 9 - 4 - 1 +"`. Classic **`std::stack`** exercise.

- **ex02 — PmergeMe (Ford-Johnson)** — implement merge-insertion sort using **two different containers** (vector + deque, or vector + list). The point is to see how the same algorithm performs on different storage.

By the end of CPP09 you've chosen the right container for three different problem shapes. That's the skill.

---

## 13. Cheatsheet: which container when

| You need… | Reach for |
|---|---|
| Default, fast random access | `std::vector` |
| Many middle inserts/erases, iterator stability | `std::list` |
| Fast push/pop at both ends | `std::deque` |
| LIFO | `std::stack` |
| FIFO | `std::queue` |
| Always grab the min or max | `std::priority_queue` |
| Unique, sorted values | `std::set` |
| Lookup by key | `std::map` |
| Duplicates allowed by key | `std::multimap` / `std::multiset` |
| Fixed size known at compile time | C array or `T[N]` as a member |
| A character string | `std::string` |

---

## 14. Rules of thumb

- **Default to `std::vector`.** Only switch when you can name the specific operation that's slow.
- **Prefer `++it` over `it++` in loops.** Prefix is cheaper for iterator types.
- **Range-based loops don't exist in C++98.** You write `for (It it = c.begin(); it != c.end(); ++it)` — get used to it.
- **`it != end()`, never `it < end()`.** Not all iterators support `<`.
- **Don't store iterators across modifications.** Inserts/erases can invalidate them.
- **`map::find` over `map::operator[]`** when you just want to check presence — `[]` inserts.
- **Pass containers by `const T&`** — they own heap memory; copying is expensive.
- **`typename` is mandatory** when naming dependent types in templates (`typename std::vector<T>::iterator it`).

---

## 15. Gotchas

- **`end()` is not an element.** Dereferencing it is Undefined Behavior. It marks "one past the last."
- **`erase` returns the next iterator** for sequence containers (and for `map`/`set` in C++11). Write `it = v.erase(it);`, not `v.erase(it); ++it;`.
- **`push_back` can invalidate iterators** into a `vector` if capacity grows. If you need stable iterators, use `list`.
- **`map::operator[]` inserts a default value** when the key is missing. Use `find` to check presence without inserting.
- **`std::sort` needs random-access iterators.** Use `list::sort()` (member function) on lists.
- **Comparators must define a strict weak ordering.** `!(a < b) && !(b < a)` means equal. Inconsistent comparators can loop forever or segfault.
- **C++98 has no `std::unique_ptr`** — only `std::auto_ptr`, which is weird (see [RAII.md](RAII.md)) and avoid-unless-necessary.
- **Don't derive publicly from STL containers** except for narrow needs like MutantStack. STL containers don't have virtual destructors; polymorphic deletion leaks.
- **`remove` doesn't remove.** Follow with `erase`. The idiom is `v.erase(std::remove(v.begin(), v.end(), x), v.end());`.
