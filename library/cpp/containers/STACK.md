# `std::stack` — LIFO Container Adapter

> **TL;DR.** `std::stack<T>` is **not a container** — it's a thin **adapter** over another container (`deque` by default; you can swap in `vector` or `list`). It exposes only `push`, `pop`, `top`, `size`, `empty`. The most common gotcha: **`pop()` returns `void`** — it removes the top element but does *not* hand it to you. Always `top()` first, then `pop()`.

Related: [`QUEUE.md`](QUEUE.md) · [`DEQUE.md`](DEQUE.md) · [`VECTOR.md`](VECTOR.md) · [`LIST.md`](LIST.md) · [`STL.md`](../advanced/STL.md) · [`INDEX.md`](INDEX.md)

Header: `#include <stack>`

---

## 1. Mental model

A stack is just a wrapper that hides everything except the back of an underlying container:

```
   std::stack<int>            (default = deque underneath)

      ┌──────────────┐
      │   stack<T>   │
      │              │
      │  push() ──── │──→ underlying.push_back()
      │  pop()  ──── │──→ underlying.pop_back()
      │  top()  ──── │──→ underlying.back()
      └──────────────┘
              │
              v
       std::deque<int>           ← the actual data lives here
       [a, b, c, d, e]
                    ↑
                  top
```

That's the entire implementation. The C++98 standard literally defines `std::stack` as:

```cpp
template <class T, class Container = std::deque<T> >
class stack {
protected:
    Container c;
public:
    void  push(const T& x) { c.push_back(x); }
    void  pop()             { c.pop_back(); }
    T&    top()             { return c.back(); }
    // size, empty, comparisons …
};
```

The "adapter pattern" — restrict an existing container's interface to enforce a discipline (LIFO).

---

## 2. Construction & choosing the underlying container

```cpp
#include <stack>
#include <vector>
#include <list>

std::stack<int> s;                          // default — deque underneath
std::stack<int, std::vector<int> > sv;      // backed by vector
std::stack<int, std::list<int> > sl;        // backed by list

std::deque<int> seed;                       // pre-populate from another container
seed.push_back(1); seed.push_back(2);
std::stack<int> s2(seed);                   // s2's top is 2
```

> **Note the space in `std::list<int> >`.** In C++98, `>>` is parsed as the right-shift operator. C++11 fixed this; in C++98 you must keep a space between adjacent template-closing brackets.

**Which underlying container?**

| Backing       | Pros                                | Cons                              |
|---------------|-------------------------------------|-----------------------------------|
| `deque` (default) | O(1) push/pop, never copies on growth, no big reallocation | Per-element overhead from blocks |
| `vector`      | Best cache locality; smallest memory for small N | Reallocates and copies all elements when capacity exhausted |
| `list`        | True O(1) push/pop, iterators stable across mutations (irrelevant for stack) | One `malloc` per push — slow |

**Default rule:** keep `deque`. Switch to `vector` only when you've measured that cache locality matters more than the rare reallocation cost.

---

## 3. Key API

| Method      | What it does                                       | Cost  |
|-------------|----------------------------------------------------|-------|
| `push(x)`   | Add `x` on top.                                    | Same as underlying `push_back`. |
| `pop()`     | Remove top. **Returns `void`.**                    | Same as underlying `pop_back`.  |
| `top()`     | Reference to top element. UB on empty.             | O(1)  |
| `size()`    | Element count.                                     | O(1)  |
| `empty()`   | `size() == 0`.                                     | O(1)  |
| `==`, `<`, etc. | Lexicographic compare via underlying container. | O(n)  |

**That's all.** No `begin`, no `end`, no `operator[]`, no iterator. By design.

---

## 4. The `pop()`-returns-`void` gotcha

This trips everyone exactly once:

```cpp
// WRONG — pop returns void; this won't compile.
int x = s.pop();

// RIGHT — read with top, then remove with pop.
int x = s.top();
s.pop();
```

**Why is `pop` void?** Because returning the popped element by value would require a copy. If the copy throws (e.g. `T` is a heavy type whose copy ctor allocates and `bad_alloc` is raised), the element would be **destroyed** but **never delivered to the caller** — the value would be lost. C++98 chose safety: separate the read (`top`) from the destroy (`pop`), so a throwing copy can't lose data.

The `top()` + `pop()` split is one of the standard's most-defended design choices and one of the most-grumbled-about by newcomers.

---

## 5. Efficiency tips

### 5.1 Default to `deque`. Switch to `vector` only with a reason.

For most uses, `deque` is fine. The reason vector can be tempting:

```cpp
std::stack<int, std::vector<int> > s;       // contiguous storage
```

Iteration speed (if you bypass the adapter — see §6) is better with vector. But for pure push/pop workloads, deque's amortized cost is usually identical.

### 5.2 Preallocate when using vector backing — but you can't reach `reserve`

The adapter hides `reserve`. To preallocate:

```cpp
std::vector<int> backing;
backing.reserve(1000);
std::stack<int, std::vector<int> > s(backing);   // copies — but capacity carries over
```

Or extend `std::stack` (the protected `c` member is *intentionally* protected to allow this). This is the classic CPP08 `MutantStack` exercise.

### 5.3 The `MutantStack` trick — exposing the underlying iterators

Stack hides iterators. To expose them, derive from `std::stack` and reach into the protected member:

```cpp
template <typename T>
class MutantStack : public std::stack<T> {
public:
    typedef typename std::stack<T>::container_type::iterator iterator;
    iterator begin() { return this->c.begin(); }   // c is protected, accessible from derived
    iterator end()   { return this->c.end();   }
};
```

Now you can iterate a stack — the standard adapter doesn't let you, but the underlying container does. This is the entire point of CPP08 ex02.

### 5.4 Pass by `const &`

Same rule as every other container — copying a stack copies every element underneath.

---

## 6. When NOT to use stack

If you need *anything* beyond push / pop / top — iteration, indexing, search — use the underlying container directly. The adapter actively prevents access to those operations.

Use stack when:
- You want the LIFO discipline enforced at the type level (so the compiler reminds you that "looking at element `i`" doesn't make sense for this collection).
- You're implementing a parser, expression evaluator, depth-first traversal, undo buffer, or anything else where LIFO is the algorithmic invariant.

Skip stack when:
- You'll need to inspect the contents later → use a vector.
- You'll need to splice across stacks → use a list.
- You're "kind of" using LIFO but also need other operations → use the underlying container.

---

## 7. C++98 caveats

| Feature        | C++ version | C++98 alternative                |
|----------------|-------------|----------------------------------|
| `emplace`      | C++11       | `push` — pays a copy.            |
| Move semantics | C++11       | Pass by `const &`.               |

---

## 8. Gotchas

- **`pop()` returns `void`.** You must `top()` first.
- **`top()` on empty stack is UB**, not an exception. Always `if (!s.empty())` first.
- **No iterators by default.** `MutantStack` is the workaround if you need them.
- **`std::list<int> >` needs a space** in C++98 — `>>` is parsed as right-shift.
- **Comparing two stacks compares element-by-element** via the underlying container's `operator==` — so make sure `T` defines it.
- **Stack assignment is a deep copy.** Copying a 1M-element stack copies all 1M elements.

---

## 9. Worked example — balanced parentheses

```cpp
#include <stack>
#include <string>
#include <iostream>

bool    is_balanced(const std::string &s)
{
    std::stack<char> st;
    for (std::size_t i = 0; i < s.size(); ++i)
    {
        char c = s[i];
        if (c == '(' || c == '[' || c == '{')
            st.push(c);
        else if (c == ')' || c == ']' || c == '}')
        {
            if (st.empty())
                return false;
            char open = st.top();
            st.pop();                       // §4 — top first, then pop
            if ((c == ')' && open != '(')
             || (c == ']' && open != '[')
             || (c == '}' && open != '{'))
                return false;
        }
    }
    return st.empty();
}

int main(void)
{
    std::cout << is_balanced("([{}])") << "\n";    // 1
    std::cout << is_balanced("([)]")   << "\n";    // 0
    return 0;
}
```

Classic LIFO discipline — every "open" pushes, every "close" pops + checks. The stack adapter forbids you from reading any element except `top`, which is exactly the constraint the algorithm needs.

---

> **One-line summary.** `std::stack` is a discipline, not a container. Push/top/pop, never `pop()`-as-an-expression, default backing is fine, and reach for `MutantStack` if you ever need iteration.
