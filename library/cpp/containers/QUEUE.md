# `std::queue` — FIFO Container Adapter

> **TL;DR.** `std::queue<T>` is the FIFO sibling of [`std::stack`](STACK.md): a thin **adapter** over an underlying container. Default backing is `deque` (since the queue needs both `push_back` and `pop_front` in O(1)). You **cannot use `vector`** as the backing — `vector` has no `pop_front`. Same `pop()` returns `void` gotcha as stack.

Related: [`STACK.md`](STACK.md) · [`PRIORITY_QUEUE.md`](PRIORITY_QUEUE.md) · [`DEQUE.md`](DEQUE.md) · [`LIST.md`](LIST.md) · [`STL.md`](../advanced/STL.md) · [`INDEX.md`](INDEX.md)

Header: `#include <queue>`

---

## 1. Mental model

```
   std::queue<int>            (default = deque underneath)

   front ────►  ┌───┬───┬───┬───┬───┐  ◄──── back
                │ 1 │ 2 │ 3 │ 4 │ 5 │
                └───┴───┴───┴───┴───┘
                  ▲                 ▲
                  │                 │
              pop here           push here

      push() ──→ underlying.push_back()
      pop()  ──→ underlying.pop_front()
      front()──→ underlying.front()
      back() ──→ underlying.back()
```

The C++98 standard defines `std::queue` as essentially:

```cpp
template <class T, class Container = std::deque<T> >
class queue {
protected:
    Container c;
public:
    void  push(const T& x) { c.push_back(x); }
    void  pop()             { c.pop_front(); }
    T&    front()           { return c.front(); }
    T&    back()            { return c.back(); }
    // size, empty, comparisons …
};
```

That `pop_front` is why **vector cannot back a queue**. Vector's `erase(begin())` is O(n), and there is no `pop_front` member at all. The standard requires the backing container to provide a true `pop_front` — only `deque` and `list` qualify.

---

## 2. Construction & choosing the underlying container

```cpp
#include <queue>
#include <list>

std::queue<int> q;                          // default — deque
std::queue<int, std::list<int> > ql;        // backed by list (mind the space before >)

std::deque<int> seed;
seed.push_back(1); seed.push_back(2);
std::queue<int> q2(seed);                   // q2.front() == 1, q2.back() == 2
```

| Backing  | Pros                                    | Cons                              |
|----------|-----------------------------------------|-----------------------------------|
| `deque`  | O(1) at both ends; cache-decent within blocks | Overhead of one block at minimum |
| `list`   | True O(1) push/pop; iterator stability irrelevant for queue | One `malloc` per push — slow |
| `vector` | **Not allowed** — no `pop_front`        | —                                 |

**Default rule:** keep `deque`. Don't change without a measured reason.

---

## 3. Key API

| Method      | What it does                                    | Cost  |
|-------------|-------------------------------------------------|-------|
| `push(x)`   | Add `x` at the back.                            | Same as underlying `push_back`. |
| `pop()`     | Remove the front. **Returns `void`.**           | Same as underlying `pop_front`. |
| `front()`   | Reference to the oldest element. UB on empty.   | O(1)  |
| `back()`    | Reference to the newest element. UB on empty.   | O(1)  |
| `size()`    | Element count.                                  | O(1)  |
| `empty()`   | `size() == 0`.                                  | O(1)  |
| `==`, `<`, etc. | Lexicographic compare via underlying.       | O(n)  |

**No iterators, no indexing.** Same restricted interface as stack — by design.

---

## 4. The same `pop()` returns `void` gotcha

Identical reasoning to [`STACK.md` §4](STACK.md#4-the-pop-returns-void-gotcha): C++98 separates *read* from *destroy* so a throwing copy can't lose data.

```cpp
// WRONG
int x = q.pop();

// RIGHT
int x = q.front();
q.pop();
```

---

## 5. Efficiency tips

### 5.1 Default to `deque`. `list` is rarely worth it.

Deque's amortized cost is O(1) per push and O(1) per pop, with cache locality inside each block. List adds one `malloc`/`free` per push/pop — easily 10× slower in tight workloads.

### 5.2 Pass by `const &`

A queue copy copies every element. Don't do that by accident.

### 5.3 If you need to peek at the middle, you've outgrown the adapter

Use `std::deque<T>` directly. The whole point of the adapter is to *prevent* mid-element access; if you need it, the abstraction isn't a fit.

### 5.4 Need priority over insertion order? See `PRIORITY_QUEUE.md`

`std::queue` is strictly FIFO. If "next item out" depends on a value-based priority rather than arrival time, use [`std::priority_queue`](PRIORITY_QUEUE.md) instead.

---

## 6. When NOT to use queue

- You need to inspect or modify elements other than `front` / `back` → use `deque`.
- You need to rearrange the order → use `vector` and sort, or `priority_queue`.
- You need many producers/consumers across threads → `std::queue` is **not thread-safe**. C++98 has no built-in concurrent queue; you'd wrap with a mutex (and `<thread>` is C++11+, forbidden in 42 cursus until later).

Use queue when:
- The algorithm is "process in arrival order" — BFS, task scheduler, event loop, work pool.
- You want the FIFO discipline enforced by the type.

---

## 7. C++98 caveats

| Feature        | C++ version | C++98 alternative                |
|----------------|-------------|----------------------------------|
| `emplace`      | C++11       | `push` — pays a copy.            |
| Move semantics | C++11       | Pass by `const &`.               |
| `std::queue` overloads from initializer-list | C++11 | Build the deque first, then construct from it. |

---

## 8. Gotchas

- **`pop()` returns `void`** — `front()` first, then `pop()`.
- **Cannot use `vector`** as the backing container. The compiler error is cryptic ("no member named `pop_front`"). The standard allows only `deque` and `list`.
- **`front()` / `back()` on empty queue is UB.** Always `if (!q.empty())` first.
- **No iterators** — same restriction as stack.
- **Not thread-safe.** Two threads pushing concurrently is UB.
- **`std::list<int> >` needs a space** between the brackets in C++98.

---

## 9. Worked example — BFS (breadth-first search) skeleton

```cpp
#include <queue>
#include <vector>
#include <iostream>

void    bfs(int start, const std::vector<std::vector<int> > &graph)
{
    std::vector<bool> seen(graph.size(), false);
    std::queue<int>   frontier;

    frontier.push(start);
    seen[start] = true;

    while (!frontier.empty())
    {
        int node = frontier.front();
        frontier.pop();                     // §4 — front first, then pop
        std::cout << node << " ";

        for (std::size_t i = 0; i < graph[node].size(); ++i)
        {
            int next = graph[node][i];
            if (!seen[next])
            {
                seen[next] = true;
                frontier.push(next);
            }
        }
    }
    std::cout << "\n";
}
```

BFS is the canonical use of a queue: process nodes in the order they were discovered. A stack here would give you DFS (depth-first) instead — same skeleton, different container.

---

> **One-line summary.** `std::queue` is `std::stack`'s FIFO twin. Default backing is `deque` (vector is forbidden), `pop()` is `void`, no iterators. Use it for arrival-order processing.
