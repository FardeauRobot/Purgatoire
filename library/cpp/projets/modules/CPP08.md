# 🧰 CPP08 — Containers, Iterators, Algorithms: Stop Writing the Loop

> **TL;DR.** The STL is three families that snap together. **Containers** hold the data (`vector`, `list`, `deque`…). **Iterators** are generalised pointers that walk any container the same way. **Algorithms** (`find`, `sort`, `min_element`…) take a pair of iterators and don't care which container is underneath. CPP08 grades you on *using* that system: *"not using the STL is a bad grade even if the code works."* The exercises are `easyfind` (algorithm + iterator), `Span` (algorithms over a vector, plus a range insert) and `MutantStack` (open up an adapter to expose its iterators).

Related: [`STL.md`](../../notions/advanced/STL.md) · [`ALGORITHMS.md`](../../notions/advanced/ALGORITHMS.md) · [`TEMPLATES.md`](../../notions/advanced/TEMPLATES.md) · [`containers/INDEX.md`](../../notions/containers/INDEX.md) · [`VECTOR.md`](../../notions/containers/VECTOR.md) · [`STACK.md`](../../notions/containers/STACK.md) · [`DEQUE.md`](../../notions/containers/DEQUE.md) · [`TYPENAME.md`](../../lexique/TYPENAME.md) · [`INHERITANCE.md`](../../notions/oop/INHERITANCE.md) · Previous: [`CPP07.md`](CPP07.md)

---

## 1. The problem, in C you already know

```c
/* find 42 in an int array */
int *ft_find(int *arr, size_t n, int v)
{
    size_t i = 0;
    while (i < n && arr[i] != v)
        i++;
    return (i < n) ? &arr[i] : NULL;
}
/* ...and again for a linked list (t_list), with a completely different loop.
   ...and again for "find the smallest", "count", "sort"… per data structure. */
```

libft already made you write `ft_lstiter`, `ft_lstmap` and `ft_memchr` separately for each shape of data. **N algorithms × M data structures = N×M functions.**

**The STL's answer:** write each algorithm **once**, in terms of iterators. Give each container iterators. Now it's **N + M**.

```cpp
std::vector<int>::iterator it = std::find(v.begin(), v.end(), 42);
std::list<int>::iterator   jt = std::find(l.begin(), l.end(), 42);   // same algorithm
```

---

## 2. The mechanism, in one diagram

```
         ┌───────────────────┐        ┌───────────────────┐
         │    CONTAINERS     │        │    ALGORITHMS     │
         │ vector list deque │        │ find sort count   │
         │ map set stack*    │        │ min_element copy  │
         └─────────┬─────────┘        └─────────┬─────────┘
                   │ .begin() .end()            │ take (first, last)
                   ▼                            ▼
              ┌──────────────────────────────────────┐
              │              ITERATORS               │
              │   *it   ++it   it != end   (--it)    │
              └──────────────────────────────────────┘

 [ begin ...................................... end )
   ▲                                             ▲
   first element                                 ONE PAST the last. Never dereference it.
   "not found" is signalled by returning end().

 * stack, queue and priority_queue are ADAPTERS: they wrap another container
   and deliberately hide its iterators. ex02 is about opening that door.
```

An iterator is modelled on a pointer. For a `vector` it basically *is* one: `*it` reads, `++it` advances. The "half-open range" `[begin, end)` is the same convention as `for (i = 0; i < n; i++)` in C, where `n` is one past the last valid index.

### Which container, when (C++98)

| Container | Memory layout | Access by index | Insert/erase in the middle | Insert at the ends | Think of it as |
|---|---|---|---|---|---|
| `vector` | One contiguous block | O(1) `v[i]` | O(n), shifts everything | Back: amortised O(1) | `malloc`'d array that grows |
| `deque` | Chunks + index table | O(1) | O(n) | **Front and back** O(1) | Array that grows both ways |
| `list` | Doubly linked nodes | ❌ none | **O(1)** with an iterator | O(1) | `t_list` with `prev` |
| `map<K,V>` | Balanced tree (red-black) | O(log n) by key | O(log n) | — | Sorted dictionary |
| `set<K>` | Balanced tree | O(log n) lookup | O(log n) | — | Sorted, unique keys |
| `stack<T>` | Adapter over `deque` by default | ❌ top only | ❌ | push/pop top | LIFO wrapper |

Full pages: [`containers/INDEX.md`](../../notions/containers/INDEX.md).

---

## 3. ex00 — `easyfind`

### What the subject asks
- A function template `easyfind(T, int)`. `T` is a container of `int`s.
- Find the **first occurrence** of the value.
- If it isn't found, throw an exception or return an error value. Your choice, but look at how the STL does it.
- Associative containers (`map`, `set`) don't need to be handled.

### Core snippet

```cpp
#include <algorithm>
#include <stdexcept>

template <typename T>
typename T::iterator easyfind(T &container, int value)
{
    typename T::iterator it = std::find(container.begin(), container.end(), value);
    if (it == container.end())
        throw std::runtime_error("easyfind: value not found");
    return it;
}
```

Line by line:
- **`typename T::iterator`**: `T` is still unknown when the template is parsed, so `T::iterator` could be a type *or* a static member. `typename` tells the compiler "it's a type". Leave it out and clang reports `missing 'typename' prior to dependent type name 'T::iterator'`. Recent clang tolerates it in a few places as a *C++20 extension* warning, but with `-Werror` that's a hard error, and in C++98 it's simply wrong code. See [`TYPENAME.md`](../../lexique/TYPENAME.md).
- **`T &container`** is a non-const reference, so the returned iterator points into *your* container, not into a copy that dies when the function returns. The subject writes `easyfind(T, int)`, but passing by value would return a **dangling iterator**.
- **`std::find`** is the algorithm. It returns `end()` when nothing matches, just like `ft_strchr` returns `NULL`.
- Throwing on "not found" uses what CPP05 taught you. The STL's own convention is to return `end()`. Both are allowed, so be ready to justify your pick.

### Usage

```cpp
std::vector<int> v;
v.push_back(1);
v.push_back(42);
v.push_back(3);

std::list<int> l(v.begin(), v.end());

std::cout << *easyfind(v, 42) << std::endl;
std::cout << *easyfind(l, 3) << std::endl;
try
{
    easyfind(v, 7);
}
catch (std::exception &e)
{
    std::cerr << e.what() << std::endl;
}
```

`std::list<int> l(v.begin(), v.end())` is the **range constructor**. Every container has one, and it's the easiest way to build test data.

### ⚠️ Gotchas
- **A manual loop is graded as wrong.** The sheet: *"It HAS to use STL algorithms. If it does not (like manual search using iterators for example), count it as wrong."*
- To support `const` containers, add an overload that takes `T const &` and returns `typename T::const_iterator`.

---

## 4. ex01 — `Span`

### What the subject asks
- `Span(unsigned int N)` holds at most N `int`s.
- `addNumber(int)` throws when the Span is full.
- `shortestSpan()` returns the smallest distance between *any* two stored numbers. `longestSpan()` returns the largest. Both throw when fewer than 2 numbers are stored.
- Test with **at least 10,000 numbers**.
- Add a function that inserts a **whole range of iterators** in one call.

```cpp
Span sp = Span(5);
sp.addNumber(6);
sp.addNumber(3);
sp.addNumber(17);
sp.addNumber(9);
sp.addNumber(11);
std::cout << sp.shortestSpan() << std::endl;   // 2   (9 and 11)
std::cout << sp.longestSpan() << std::endl;    // 14  (17 - 3)
```

### The idea: sort, then only neighbours matter

```
 stored:  6   3   17   9   11
 sorted:  3   6   9   11   17
 gaps:      3   3    2    6       ← adjacent_difference
 shortest = min gap = 2                 longest = last - first = 17 - 3 = 14
```

Once the numbers are sorted, the closest pair **has to be adjacent**. A number in between would be closer to one of them. So n² comparisons become one `sort` plus one pass. The sheet warns: *"Finding the shortest span can't be done only by subtracting the two lowest numbers."* In the example, 3 and 6 are the two lowest, with a gap of 3, but the answer is 2.

### Core snippet

```cpp
class Span
{
private:
    unsigned int        _max;
    std::vector<int>    _numbers;

public:
    Span(unsigned int n);
    Span(Span const &other);
    Span &operator=(Span const &other);
    ~Span();

    void            addNumber(int n);
    unsigned int    shortestSpan() const;
    unsigned int    longestSpan() const;

    template <typename It>
    void addRange(It begin, It end)
    {
        if (std::distance(begin, end) > static_cast<long>(_max - _numbers.size()))
            throw std::length_error("Span: range does not fit");
        _numbers.insert(_numbers.end(), begin, end);
    }
};
```

```cpp
unsigned int Span::shortestSpan() const
{
    if (_numbers.size() < 2)
        throw std::logic_error("Span: need at least two numbers");
    std::vector<long> sorted(_numbers.begin(), _numbers.end());
    std::sort(sorted.begin(), sorted.end());
    std::vector<long> gaps(sorted.size());
    std::adjacent_difference(sorted.begin(), sorted.end(), gaps.begin());
    return static_cast<unsigned int>(*std::min_element(gaps.begin() + 1, gaps.end()));
}

unsigned int Span::longestSpan() const
{
    if (_numbers.size() < 2)
        throw std::logic_error("Span: need at least two numbers");
    long lo = *std::min_element(_numbers.begin(), _numbers.end());
    long hi = *std::max_element(_numbers.begin(), _numbers.end());
    return static_cast<unsigned int>(hi - lo);
}
```

Walkthrough:
- **`addRange` is a member template**, so it accepts iterators from *any* container: `vector`, `list`, or even a raw `int[]`, since pointers are iterators. `vector::insert(pos, first, last)` does the copying.
- **`std::distance`** counts the range *before* inserting, so a Span never ends up half-filled. That's the same "check first, then act" idea as `operator=` in CPP07.
- **The constructor can call `_numbers.reserve(n)`.** It's one allocation up front instead of many as the vector grows. This is allowed in CPP08; the "no preventive allocation" rule belonged to CPP07's `Array`.
- **`shortestSpan` is `const`.** It sorts a *copy*, not the member, so the stored order is left alone.
- **`adjacent_difference`** writes `out[0] = in[0]`, then `out[i] = in[i] - in[i-1]`. That's why the minimum is taken from `gaps.begin() + 1`.
- **Headers:** `std::sort` and `std::min_element` come from `<algorithm>`, but `std::adjacent_difference` lives in `<numeric>` and `std::distance` in `<iterator>`. Apple's libc++ may let you forget them because other headers pull them in, and Linux's libstdc++ then fails to compile.

### ⚠️ Gotcha: the overflow trap (tested)
The span between `INT_MIN` and `INT_MAX` is 4294967295, which doesn't fit in an `int`. `adjacent_difference` computes in the **source** element type. A first version of this snippet sorted a `std::vector<int>`, and UBSan caught it:

```
runtime error: signed integer overflow: 2147483647 - -2147483648 cannot be represented in type 'int'
```

That's why the copy above is a `std::vector<long>`, which is 64 bits on macOS and 64-bit Linux, and why the return type is `unsigned int`. Put this case in your `main`, because evaluators like extreme values.

### Testing with 10,000+ numbers

```cpp
std::srand(static_cast<unsigned int>(std::time(NULL)));
std::vector<int> big(10000);
for (size_t i = 0; i < big.size(); i++)
    big[i] = std::rand();
Span huge(10000);
huge.addRange(big.begin(), big.end());
std::cout << huge.shortestSpan() << " " << huge.longestSpan() << std::endl;
```

`std::generate(big.begin(), big.end(), std::rand)` fills the vector with an algorithm instead of a loop, which is even more STL-flavoured.

---

## 5. ex02 — `MutantStack`

### What the subject asks
`std::stack` has no iterators. Write `MutantStack<T>`:
- It **is** a `std::stack`, with all of its member functions.
- It **adds iterators**.
- The subject's `main` must print the same thing when `MutantStack` is replaced by `std::list` (with `push_back`/`back`/`pop_back`).
- `std::stack<int> s(mstack);` must compile.

### How an adapter is built

```
 std::stack<T, Container = std::deque<T> >
 ┌────────────────────────────────────────────┐
 │ public:   push()  pop()  top()  size()  …  │  ← all implemented on top of `c`
 │ protected:                                 │
 │   Container c;  ──────────►  std::deque<T> │  ← the real storage, WITH iterators
 └────────────────────────────────────────────┘
                 ▲
                 │ derived class: protected members are visible inside it
 ┌────────────────────────────────────────────┐
 │ MutantStack<T>                             │
 │   typedef … container_type::iterator …     │
 │   begin() { return this->c.begin(); }      │
 │   end()   { return this->c.end(); }        │
 └────────────────────────────────────────────┘
```

The C++ standard says the underlying container is a **`protected` member named `c`**. That was deliberate, so a derived class can reach it. The exercise is really "read the standard's definition of `std::stack`".

### Core snippet

```cpp
#include <stack>

template <typename T>
class MutantStack : public std::stack<T>
{
public:
    MutantStack() {}
    MutantStack(MutantStack const &other) : std::stack<T>(other) {}
    MutantStack &operator=(MutantStack const &other)
    {
        std::stack<T>::operator=(other);
        return *this;
    }
    ~MutantStack() {}

    typedef typename std::stack<T>::container_type::iterator                iterator;
    typedef typename std::stack<T>::container_type::const_iterator          const_iterator;
    typedef typename std::stack<T>::container_type::reverse_iterator        reverse_iterator;
    typedef typename std::stack<T>::container_type::const_reverse_iterator  const_reverse_iterator;

    iterator                begin()         { return this->c.begin(); }
    iterator                end()           { return this->c.end(); }
    const_iterator          begin() const   { return this->c.begin(); }
    const_iterator          end() const     { return this->c.end(); }
    reverse_iterator        rbegin()        { return this->c.rbegin(); }
    reverse_iterator        rend()          { return this->c.rend(); }
    const_reverse_iterator  rbegin() const  { return this->c.rbegin(); }
    const_reverse_iterator  rend() const    { return this->c.rend(); }
};
```

Walkthrough:
- **`: public std::stack<T>`** means `MutantStack` gets `push`, `pop`, `top`, `size` and `empty` for free. Public inheritance is also what makes `std::stack<int> s(mstack)` compile: a `MutantStack` *is a* `stack`, so the stack copy constructor accepts it.
- **`std::stack<T>::container_type`** is the typedef the standard gives for the wrapped container (`std::deque<T>`). Its `::iterator` is a *dependent type*, so `typename` is required again.
- **`this->c`, not plain `c`.** The base class depends on `T`, so C++ doesn't look names up in it automatically. Write plain `c` and you get `use of undeclared identifier 'c'`. `this->` postpones the lookup until the class is instantiated. This is the most common error in this exercise.
- **OCF:** everything delegates to `std::stack`'s own copy and assignment, which copy `c`.
- The member bodies are **inside the class template**. That's allowed, because the "no implementation in headers" rule exempts templates.

### Verified output of the subject's `main`

```cpp
MutantStack<int> mstack;
mstack.push(5);
mstack.push(17);
std::cout << mstack.top() << std::endl;
mstack.pop();
std::cout << mstack.size() << std::endl;
mstack.push(3);
mstack.push(5);
mstack.push(737);
mstack.push(0);
MutantStack<int>::iterator it = mstack.begin();
MutantStack<int>::iterator ite = mstack.end();
++it;
--it;
while (it != ite)
{
    std::cout << *it << std::endl;
    ++it;
}
std::stack<int> s(mstack);
```

```
17
1
5
3
5
737
0
```

The iteration runs **from bottom to top** (5 was pushed first), because it walks the deque front to back. To prove the `std::list` equivalence, write the list version side by side in your `main` and `diff` the two outputs.

### ⚠️ Gotchas
- `std::stack` has **no virtual destructor**. Never `delete` a `MutantStack` through a `std::stack *`. Keep it on the stack, the way the subject does.
- "Better tests" is its own line on the grading sheet. Good candidates: const iteration, reverse iteration, a `MutantStack<std::string>`, copy then modify, and the `std::list` comparison.

---

## 6. What the evaluator checks

From the official 42evalhub grading sheet:

**Prerequisites**
- [ ] `c++ -Wall -Wextra -Werror`, C++98.
- [ ] No function body in a header, **templates excepted**.
- [ ] No `*alloc`, `*printf` or `free`. No `using namespace`. No `friend`. No external library.
- [ ] No leaks.

**For every exercise:** a `main` with **enough tests**, and **every non-interface class in OCF**. Otherwise the exercise isn't graded.

**ex00**
- [ ] A function template `easyfind(T, int)` that does what the subject asks.
- [ ] It **uses an STL algorithm**. A manual iterator loop is counted as wrong.

**ex01**
- [ ] The class respects the subject's constraints. Members use **STL algorithms as much as possible**.
- [ ] `shortestSpan` isn't just "the two lowest numbers subtracted".
- [ ] There's a better way to add many numbers than calling `addNumber` repeatedly (a range insert).

**ex02**
- [ ] `MutantStack` **inherits from `std::stack`** and offers all its members.
- [ ] It has an iterator. At least the operations from the subject's example work.
- [ ] `main` has **more tests than the subject's**.

**Defense questions to prepare**
- "Why `vector` for Span?" → Contiguous memory, fast `sort`, cheap `reserve`.
- "Where does `c` come from?" → The standard makes the underlying container a protected member of `std::stack`.
- "Why `typename`?" / "Why `this->c`?" → Dependent names (§3, §5).

---

## 7. Cheat sheet

| Need | C++98 spelling |
|---|---|
| Find a value | `std::find(c.begin(), c.end(), v)` → `end()` if absent |
| Iterator type of a template parameter | `typename T::iterator` |
| Smallest / largest | `*std::min_element(b, e)` / `*std::max_element(b, e)` |
| Consecutive differences | `std::adjacent_difference(b, e, out)` |
| Count elements in a range | `std::distance(first, last)` |
| Append a range | `v.insert(v.end(), first, last)` |
| Build from a range | `std::list<int> l(v.begin(), v.end())` |
| Reach a template base's member | `this->c` |
| Container wrapped by an adapter | `std::stack<T>::container_type` |

**What this module leaves you with:** before writing a loop, you ask "which algorithm is this?". CPP09 takes the training wheels off: the containers are the tools, and *choosing* them is graded.

➡️ Next: [`CPP09.md`](CPP09.md). Three real programs, each with a container you must defend.
