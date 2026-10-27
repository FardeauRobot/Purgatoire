# Iterators — The Glue of the STL

> **TL;DR.** An iterator is a **generalized pointer**: an object that marks a position in a container and answers to `*it`, `++it`, `it == other`. Every container hands out iterators through `begin()` / `end()`, every algorithm consumes them, and that is why one `std::find` works on a `vector`, a `list` and a C array alike. What an iterator *can* do depends on its **category** (input → forward → bidirectional → random-access), and when it *stops* being valid depends on the **container** you took it from.

Related: [`STL.md`](STL.md) · [`ALGORITHMS.md`](ALGORITHMS.md) · [`TEMPLATES.md`](TEMPLATES.md) · [`TYPENAME.md`](../../lexique/TYPENAME.md) · [`containers/INDEX.md`](../containers/INDEX.md) · [`CPP08.md`](../../projets/modules/CPP08.md)

Header: `#include <iterator>` (for the helpers and adaptors — the iterator types themselves come with each container's header)

---

## Table of Contents

1. [Mental model — a pointer that knows its container](#1.%20Mental%20model%20%E2%80%94%20a%20pointer%20that%20knows%20its%20container)
2. [The half-open range `[begin, end)`](#2.%20The%20half-open%20range%20%5Bbegin%2C%20end%29)
3. [The four nested types](#3.%20The%20four%20nested%20types)
4. [The canonical loop](#4.%20The%20canonical%20loop)
5. [Iterator categories](#5.%20Iterator%20categories)
6. [`<iterator>` helpers — `advance`, `distance`, `iterator_traits`](#6.%20%3Citerator%3E%20helpers%20%E2%80%94%20advance%2C%20distance%2C%20iterator_traits)
7. [Reverse iterators and `base()`](#7.%20Reverse%20iterators%20and%20base%28%29)
8. [Iterator adaptors — inserters and stream iterators](#8.%20Iterator%20adaptors%20%E2%80%94%20inserters%20and%20stream%20iterators)
9. [Iterator invalidation — per container](#9.%20Iterator%20invalidation%20%E2%80%94%20per%20container)
10. [Writing generic code against iterators (CPP08)](#10.%20Writing%20generic%20code%20against%20iterators%20%28CPP08%29)
11. [Writing your own iterator](#11.%20Writing%20your%20own%20iterator)
12. [C++98 caveats](#12.%20C%2B%2B98%20caveats)
13. [Gotchas](#13.%20Gotchas)
14. [Cheat sheet](#14.%20Cheat%20sheet)

---

## 1. Mental model — a pointer that knows its container

You already know iterators: you used them in C every time you walked an array with a pointer.

```c
int  arr[] = {3, 1, 4};
int *p;

for (p = arr; p != arr + 3; ++p)
    printf("%d\n", *p);
```

`p` marks a position, `*p` reads the element, `++p` moves to the next one, `p != arr + 3` checks if you are done. An iterator is exactly that contract, generalized so it also works when "next" is not "the next address in memory":

```
   vector<int>  (contiguous)              list<int>  (linked nodes)

   ┌───┬───┬───┬───┐                      ┌───┐    ┌───┐    ┌───┐
   │ 3 │ 1 │ 4 │ 1 │                      │ 3 │◄──►│ 1 │◄──►│ 4 │
   └───┴───┴───┴───┘                      └───┘    └───┘    └───┘
     ▲                                      ▲
     it          ++it = "address + 1"       it       ++it = "follow node->next"
```

For a `vector`, the iterator is basically a `T*` in a wrapper. For a `list`, it holds a node pointer and `operator++` does `node = node->next` — like walking libft's `t_list` with `lst = lst->next`, but hidden behind the same `++` syntax. **The algorithm never sees the difference.** That is the entire point.

```
    CONTAINERS  ──begin()/end()──►  ITERATORS  ──►  ALGORITHMS
    know how data is stored         know how to     know nothing about
                                    walk it          storage, only ++ * ==
```

---

## 2. The half-open range `[begin, end)`

Every STL range is **half-open**: `begin()` points at the first element, `end()` points **one past the last** — a sentinel, not an element.

```
     begin()                     end()
       │                           │
       v                           v
     ┌───┬───┬───┬───┐  ┌ ─ ─ ┐
     │ 3 │ 1 │ 4 │ 1 │    ???      ← never dereference end()
     └───┴───┴───┴───┘  └ ─ ─ ┘
```

Why this convention, and not `[first, last]`?

- **Empty range is natural:** `begin() == end()` means "nothing here". No special case.
- **Size for free** (random-access): `end() - begin() == size()`.
- **"Not found" has a value:** `std::find` returns `end()` when it fails — a position that can't be confused with a real element. This is the C++ equivalent of returning `NULL` from `ft_strchr`.

```cpp
std::vector<int>::iterator it = std::find(v.begin(), v.end(), 42);
if (it == v.end())
    std::cout << "not found\n";
```

---

## 3. The four nested types

Every container defines its iterator types as **nested typedefs**:

```cpp
std::vector<int>::iterator               it;    // read + write
std::vector<int>::const_iterator         cit;   // read only
std::vector<int>::reverse_iterator       rit;   // walks backward, read + write
std::vector<int>::const_reverse_iterator crit;  // walks backward, read only
```

| Member | On a non-const container returns | On a `const` container returns |
|---|---|---|
| `begin()` / `end()` | `iterator` | `const_iterator` |
| `rbegin()` / `rend()` | `reverse_iterator` | `const_reverse_iterator` |

The overload is picked by the **constness of the container**, not by what you assign to. That's why a function taking `const std::vector<int> &v` *must* use `const_iterator`:

```cpp
void print(const std::vector<int> &v)
{
    for (std::vector<int>::const_iterator it = v.begin(); it != v.end(); ++it)
        std::cout << *it << ' ';
}
```

### `const_iterator` ≠ `const iterator`

Same trap as `const int *` vs `int * const` in C:

```cpp
std::vector<int>::const_iterator a = v.begin();
*a = 5;      // ERROR — the element is read-only
++a;         // OK    — the iterator itself moves

const std::vector<int>::iterator b = v.begin();
*b = 5;      // OK    — the element is writable
++b;         // ERROR — the iterator is frozen
```

| C pointer | Iterator equivalent |
|---|---|
| `const int *p` | `const_iterator` |
| `int * const p` | `const iterator` |

An `iterator` converts implicitly to a `const_iterator` (never the reverse), so you can compare them with `==` / `!=`.

---

## 4. The canonical loop

```cpp
for (std::vector<int>::iterator it = v.begin(); it != v.end(); ++it)
    std::cout << *it << '\n';
```

Every piece is deliberate:

- **`it != v.end()`, not `it < v.end()`** — `<` exists only for random-access iterators. `!=` works for every category, so the same loop compiles on a `list` or a `map`.
- **`++it`, not `it++`** — postfix must return a copy of the old value. For a pointer that's free; for a class-type iterator (`list`, `map`) it's a wasted object. Prefix is never worse.
- **`*it`** gives a reference to the element; **`it->member`** works when the element is a struct/class — e.g. `it->first` / `it->second` on a `map`.

On a `map`, the element is a `std::pair<const K, V>`:

```cpp
for (std::map<std::string, int>::const_iterator it = m.begin(); it != m.end(); ++it)
    std::cout << it->first << " = " << it->second << '\n';
```

---

## 5. Iterator categories

Not every iterator supports every operation. A list node can't jump 5 nodes forward in one step, so a `list` iterator doesn't offer `it + 5`. The standard groups iterators into **five categories**:

```
   InputIterator          OutputIterator
   (read once, ++)        (write once, ++)
          │
          ▼
   ForwardIterator        ── read many times, ++
          │
          ▼
   BidirectionalIterator  ── + --
          │
          ▼
   RandomAccessIterator   ── + it+n, it-n, it[n], <, >, it2-it1
```

*Input* and *output* are two separate branches: a forward iterator refines **input**. A mutable forward iterator can *also* be used where an output iterator is expected, but that's not the same as being "below" it in the chain.

| Category | Operations added | Who provides it |
|---|---|---|
| Input | `*it` (read), `++`, `==`, `!=` — single pass | `std::istream_iterator` |
| Output | `*it = x` (write), `++` — single pass | `std::ostream_iterator`, `std::back_inserter` |
| Forward | multi-pass: copies of `it` can re-walk the same elements | *(no C++98 container stops here)* |
| Bidirectional | `--it` | `list`, `set`, `multiset`, `map`, `multimap` |
| Random-access | `it + n`, `it - n`, `it += n`, `it[n]`, `it2 - it1`, `<` `>` `<=` `>=` | `vector`, `deque`, `std::string`, **raw pointers** |

`stack`, `queue` and `priority_queue` are **adaptors**: they deliberately expose **no iterators at all** (that's what CPP08 ex02 works around).

### Why it matters: algorithms state their requirement

Each algorithm documents the weakest category it accepts. Pass something weaker and the compiler refuses — usually with a long error deep in `<algorithm>`.

```cpp
std::list<int> l;
std::sort(l.begin(), l.end());   // ERROR — sort needs random-access, list gives bidirectional
l.sort();                        // OK    — list's own member, written for linked nodes
```

| Algorithm | Needs at least |
|---|---|
| `find`, `count`, `accumulate`, `copy` (source) | Input |
| `copy` (destination), `fill_n` | Output |
| `min_element`, `max_element`, `replace`, `unique` | Forward |
| `reverse` | Bidirectional |
| `sort`, `nth_element`, `random_shuffle` | Random-access |

A raw pointer is a random-access iterator, which is why `std::sort(arr, arr + 3)` works on a C array.

---

## 6. `<iterator>` helpers — `advance`, `distance`, `iterator_traits`

Since `it + 5` only compiles on random-access iterators, `<iterator>` provides category-agnostic versions:

```cpp
#include <iterator>

std::list<int>::iterator it = l.begin();
std::advance(it, 3);                                       // moves it 3 steps (works on any category)
std::ptrdiff_t n = std::distance(l.begin(), l.end());     // counts steps between two iterators
```

They pick the best strategy for the category: on a `vector`, `advance(it, 3)` is `it += 3` (O(1)); on a `list` it is three `++it` (O(n)). Same call, different cost — know which container you're on.

- `advance` **modifies its argument** and returns `void`. There is no `std::next` / `std::prev` in C++98 — copy first, then advance:
  ```cpp
  std::list<int>::iterator third = l.begin();
  std::advance(third, 2);
  ```
- `distance(first, last)` requires `last` to be reachable from `first` with `++`. Swapping them is undefined behavior on non-random-access iterators.

### `std::iterator_traits`

How does `advance` know the category? Every iterator exposes five typedefs, read through `std::iterator_traits<It>`:

| Typedef | Meaning | For `std::vector<int>::iterator` |
|---|---|---|
| `value_type` | type of the element | `int` |
| `difference_type` | type of `it2 - it1` | `std::ptrdiff_t` |
| `pointer` | `value_type *` | `int *` |
| `reference` | `value_type &` | `int &` |
| `iterator_category` | a **tag type** | `std::random_access_iterator_tag` |

`iterator_traits` is specialized for raw pointers too, so generic code works on `int *` as well. Useful in your own templates when you need the element type of an arbitrary iterator:

```cpp
template <typename It>
typename std::iterator_traits<It>::value_type sum(It first, It last)
{
    typename std::iterator_traits<It>::value_type total = 0;
    for (; first != last; ++first)
        total += *first;
    return total;
}
```

Note the two `typename`: `value_type` is a *dependent* name, so the compiler needs to be told it's a type ([`TYPENAME.md`](../../lexique/TYPENAME.md)).

---

## 7. Reverse iterators and `base()`

`rbegin()` points at the **last** element, `rend()` at "one before the first". `++` on a reverse iterator moves **backward**:

```cpp
for (std::vector<int>::reverse_iterator rit = v.rbegin(); rit != v.rend(); ++rit)
    std::cout << *rit << ' ';      // prints in reverse order
```

```
              rend()   rbegin()
                 │        │
                 v        v
            ┌ ─ ┬───┬───┬───┬───┐
              ? │ 3 │ 1 │ 4 │ 1 │
            └ ─ ┴───┴───┴───┴───┘
                  ▲               ▲
               begin()          end()
```

A reverse iterator is an adaptor wrapping a normal iterator, stored **one position to the right**. `rit.base()` gives that wrapped iterator back:

```
   *rit  ==  *(rit.base() - 1)
```

This off-by-one matters when you want to `erase` through a reverse iterator — `erase` only accepts normal iterators:

```cpp
std::vector<int>::reverse_iterator rit = std::find(v.rbegin(), v.rend(), 4);
if (rit != v.rend())
    v.erase((++rit).base());       // ++ first, then base() lands on the right element
```

---

## 8. Iterator adaptors — inserters and stream iterators

`<iterator>` provides iterators that don't point into a container at all — they **do something** when you write through them.

### Inserters — growing a container from an algorithm

`std::copy` writes to `*dest++` and assumes room exists. Copying into an empty vector is a classic crash:

```cpp
std::vector<int> dst;
std::copy(src.begin(), src.end(), dst.begin());                 // UB — dst has size 0
std::copy(src.begin(), src.end(), std::back_inserter(dst));     // OK — calls dst.push_back()
```

| Adaptor | Each `*it = x` calls | Container needs |
|---|---|---|
| `std::back_inserter(c)` | `c.push_back(x)` | `vector`, `deque`, `list`, `string` |
| `std::front_inserter(c)` | `c.push_front(x)` (reverses order!) | `deque`, `list` |
| `std::inserter(c, pos)` | `c.insert(pos, x)` | any — the only one for `set` / `map` |

### Stream iterators — treating I/O as a range

```cpp
std::copy(v.begin(), v.end(), std::ostream_iterator<int>(std::cout, " "));
```

prints every element followed by a space — no loop.

```cpp
std::istringstream iss("3 1 4 1 5");
std::vector<int> v((std::istream_iterator<int>(iss)), std::istream_iterator<int>());
```

reads ints until extraction fails; a default-constructed `istream_iterator` is the "end of stream" sentinel. The **extra parentheses** around the first argument are mandatory in C++98: without them, the line parses as a *function declaration* (the "most vexing parse").

---

## 9. Iterator invalidation — per container

An iterator is only valid while what it points to stays where it was. Modify the container and some iterators become **dangling** — using them is undefined behavior, usually silent until it isn't. Valgrind or `-fsanitize=address` will often catch it.

| Container | Insert | Erase |
|---|---|---|
| `vector` | If it **reallocates** (size > capacity): **all** invalidated. Otherwise: those at/after the insertion point. | At/after the erased element. |
| `deque` | Anywhere: **all iterators** invalidated. (At the ends, references/pointers to elements stay valid.) | Middle: all. Ends: only the erased ones (+ `end()`). |
| `list` | **None.** | Only the erased element. |
| `set` / `map` (and multi-) | **None.** | Only the erased element. |

Mental model: `vector` moves its whole buffer when it grows (like `realloc`), so every pointer into it goes stale. `list` and `map` allocate one node per element and never move nodes, so iterators survive everything but their own node's death.

### Erasing while iterating

The wrong way — `it` is dead after `erase`, then `++it` uses it:

```cpp
for (std::vector<int>::iterator it = v.begin(); it != v.end(); ++it)
    if (*it % 2 == 0)
        v.erase(it);                // UB
```

`vector`, `deque` and `list` — `erase` returns the next valid iterator:

```cpp
for (std::vector<int>::iterator it = v.begin(); it != v.end(); )
{
    if (*it % 2 == 0)
        it = v.erase(it);
    else
        ++it;
}
```

`set` / `map` — **in C++98, `erase(it)` returns `void`**, so you step past the element *before* erasing it with post-increment:

```cpp
for (std::map<std::string, int>::iterator it = m.begin(); it != m.end(); )
{
    if (it->second == 0)
        m.erase(it++);              // it++ moves on, erase gets the old copy
    else
        ++it;
}
```

On a `vector`, the idiomatic bulk version is erase–remove:

```cpp
v.erase(std::remove(v.begin(), v.end(), 0), v.end());
```

Details for each container: [`VECTOR.md`](../containers/VECTOR.md) · [`DEQUE.md`](../containers/DEQUE.md) · [`LIST.md`](../containers/LIST.md) · [`MAP.md`](../containers/MAP.md) · [`SET.md`](../containers/SET.md).

---

## 10. Writing generic code against iterators (CPP08)

CPP08 is where you stop *using* iterators and start *writing code that accepts them*.

### ex00 — `easyfind`: getting the iterator type of `T`

```cpp
template <typename T>
typename T::iterator easyfind(T &container, int value)
{
    typename T::iterator it = std::find(container.begin(), container.end(), value);
    if (it == container.end())
        throw std::runtime_error("value not found");
    return it;
}
```

- `T::iterator` depends on `T`, so it needs `typename`.
- Taking `T &` (not `const T &`) is what makes `T::iterator` the right type. A `const` overload would return `typename T::const_iterator`.

### ex01 — `Span`: adding a range with a template member

```cpp
class Span
{
public:
    template <typename InputIt>
    void addNumbers(InputIt first, InputIt last)
    {
        if (std::distance(first, last) > static_cast<std::ptrdiff_t>(_max - _numbers.size()))
            throw std::length_error("Span is full");
        _numbers.insert(_numbers.end(), first, last);
    }
    // ...
};
```

Naming the parameter `InputIt` documents the weakest category you accept. Because it's a template, it takes a `vector<int>::iterator`, a `list<int>::iterator` or a plain `int *` alike.

### ex02 — `MutantStack`: exposing the hidden iterators

`std::stack` has no iterators, but it wraps a real container stored in a **protected** member `c` (a `deque` by default):

```cpp
template <typename T>
class MutantStack : public std::stack<T>
{
public:
    typedef typename std::stack<T>::container_type::iterator iterator;

    iterator begin() { return this->c.begin(); }
    iterator end()   { return this->c.end(); }
};
```

`this->c` is required: `c` lives in a dependent base class, so an unqualified `c` isn't looked up there. Don't forget `const_iterator`, `reverse_iterator` and the `const` overloads if you want a complete interface — plus the four canonical members.

---

## 11. Writing your own iterator

Rarely needed in CPP modules, but it shows there's no magic. A minimal **forward** iterator over a singly linked list:

```cpp
#include <iterator>

template <typename T>
struct Node
{
    T     value;
    Node *next;
};

template <typename T>
class NodeIterator : public std::iterator<std::forward_iterator_tag, T>
{
public:
    NodeIterator() : _node(NULL) {}
    explicit NodeIterator(Node<T> *node) : _node(node) {}
    NodeIterator(const NodeIterator &other) : _node(other._node) {}
    NodeIterator &operator=(const NodeIterator &other) { _node = other._node; return *this; }
    ~NodeIterator() {}

    T &operator*() const  { return _node->value; }
    T *operator->() const { return &_node->value; }

    NodeIterator &operator++()   { _node = _node->next; return *this; }
    NodeIterator  operator++(int) { NodeIterator old(*this); _node = _node->next; return old; }

    bool operator==(const NodeIterator &o) const { return _node == o._node; }
    bool operator!=(const NodeIterator &o) const { return _node != o._node; }

private:
    Node<T> *_node;
};
```

- Inheriting from `std::iterator<Category, T>` (C++98) provides the five typedefs `iterator_traits` reads — so `std::distance` and `std::find` work on it out of the box.
- `end()` would be `NodeIterator(NULL)`: the null `next` of the last node is your sentinel.
- `operator++(int)` — the dummy `int` marks the **postfix** form; note the copy it has to make (the reason to prefer `++it`).

---

## 12. C++98 caveats

| Modern feature | C++98 alternative |
|---|---|
| `auto it = v.begin();` | Spell the type: `std::vector<int>::iterator it = v.begin();` — or a `typedef` |
| `for (int x : v)` | The canonical loop (§4) |
| `v.cbegin()` / `v.cend()` | Assign to a `const_iterator`, or call `begin()` on a `const` reference |
| `std::next(it)` / `std::prev(it)` | Copy, then `std::advance(copy, ±1)` |
| `std::begin(arr)` / `std::end(arr)` | `arr` and `arr + sizeof(arr) / sizeof(arr[0])` |
| `map::erase(it)` returns the next iterator | Returns `void` — use `m.erase(it++)` |

Long type names are the C++98 tax. Tame them with a `typedef`:

```cpp
typedef std::map<std::string, int>::const_iterator WordIt;

for (WordIt it = counts.begin(); it != counts.end(); ++it)
    ...
```

---

## 13. Gotchas

- **Dereferencing `end()`** — UB. Always compare to `end()` before `*it`, especially after `find`.
- **`*v.begin()` on an empty container** — same thing: `begin() == end()`, so you dereferenced `end()`.
- **Comparing iterators from different containers** — `a.begin() != b.end()` compiles and is meaningless. Watch for it when a function returns a container **by value** and you call `.begin()` and `.end()` on two separate temporaries.
- **Using an iterator after a `push_back`** on a `vector` — a reallocation silently invalidates it (§9). Call `reserve()` first, or store indices.
- **`std::sort` on a `list`** — doesn't compile. Use `l.sort()`.
- **Modifying a key through a `set`/`map` iterator** — `*it` on a `set` is read-only, and a map's `it->first` is `const K`. Erase and reinsert instead.
- **Forgetting `typename`** before `T::iterator` in a template — "expected a type" error.
- **`it++` in hot loops on class iterators** — a copy for nothing. Use `++it`.
- **`std::copy` into an empty container** — writes past the end. Use `std::back_inserter`.

---

## 14. Cheat sheet

```cpp
// Types
C::iterator   C::const_iterator   C::reverse_iterator   C::const_reverse_iterator

// Ranges
c.begin()  c.end()         // [first, one-past-last)
c.rbegin() c.rend()        // backward

// Walk
for (C::iterator it = c.begin(); it != c.end(); ++it) { *it; it->member; }

// Category-agnostic moves (<iterator>)
std::advance(it, n);              // void, modifies it
std::distance(first, last);       // number of ++ steps

// Adaptors (<iterator>)
std::back_inserter(c)   std::front_inserter(c)   std::inserter(c, pos)
std::ostream_iterator<T>(std::cout, " ")   std::istream_iterator<T>(stream)

// Erase while iterating
it = c.erase(it);    // vector, deque, list
c.erase(it++);       // set, map (C++98: erase returns void)
```

| | `vector` / `deque` / `string` | `list` | `set` / `map` | `stack` / `queue` |
|---|---|---|---|---|
| Category | Random-access | Bidirectional | Bidirectional | — (none) |
| `it + n`, `<`, `[]` | ✅ | ❌ | ❌ | — |
| `--it` | ✅ | ✅ | ✅ | — |
| `std::sort` | ✅ | ❌ (`l.sort()`) | ❌ (already sorted) | — |
| Survive insert? | ❌ | ✅ | ✅ | — |

> **One-line summary.** An iterator is a pointer generalized to any container: `begin()`/`end()` give a half-open range, the **category** tells you which operations compile, and the **container** tells you how long the iterator stays valid.
