# `std::priority_queue` — The Heap-Backed Adapter

> **TL;DR.** `std::priority_queue<T>` is **not a queue** — it's a **binary heap** dressed up as one. The next item out is always the **largest** (max-heap by default). Backed by `vector` + the `<algorithm>` heap functions (`std::make_heap`, `std::push_heap`, `std::pop_heap`). Push and pop are O(log n), top is O(1). For a min-heap, pass `std::greater<T>` as the comparator.

Related: [`STACK.md`](STACK.md) · [`QUEUE.md`](QUEUE.md) · [`VECTOR.md`](VECTOR.md) · [`STL.md`](../advanced/STL.md) · [`INDEX.md`](INDEX.md)

Header: `#include <queue>` *(yes — same header as `std::queue`)*

---

## 1. Mental model

A binary heap stored as a vector. The vector is the storage; `<algorithm>`'s heap functions enforce the "parent ≥ children" invariant.

```
   priority_queue<int> with elements pushed in order: 3, 1, 4, 1, 5, 9, 2, 6

   the heap as a tree:                           the heap as a vector:

              9                                  index:  0  1  2  3  4  5  6  7
            /   \                                values: 9  6  4  3  5  1  2  1
           6     4
          / \   / \                              parent of i:  (i-1) / 2
         3   5 1   2                             children of i:  2*i+1, 2*i+2
        /
       1                                         top() = vector[0] = 9
```

Push: append at the end of the vector, then "sift up" (swap with parent until parent is bigger). Pop: swap root with last element, shrink the vector by one, then "sift down" the new root. Both are O(log n).

The C++98 standard defines it roughly as:

```cpp
template <class T,
          class Container = std::vector<T>,
          class Compare = std::less<T> >
class priority_queue {
protected:
    Container c;
    Compare   comp;
public:
    void  push(const T& x) {
        c.push_back(x);
        std::push_heap(c.begin(), c.end(), comp);
    }
    void  pop() {
        std::pop_heap(c.begin(), c.end(), comp);
        c.pop_back();
    }
    const T& top() const   { return c.front(); }
    // size, empty, no comparisons
};
```

Note the comparator: `std::less<T>` makes a **max-heap** because the heap functions place the "greatest" element at the front when the comparator is "less."

---

## 2. Construction & choosing the comparator

```cpp
#include <queue>
#include <vector>
#include <functional>          // for std::greater

std::priority_queue<int> max_pq;                 // default — max-heap

// Min-heap: change comparator to greater
std::priority_queue<int, std::vector<int>, std::greater<int> > min_pq;

// Pre-populate from a range — the ctor calls make_heap once
int data[] = {3, 1, 4, 1, 5, 9, 2, 6};
std::priority_queue<int> pq(std::less<int>(), std::vector<int>(data, data + 8));
```

**Backing container:** must support `front`, `push_back`, `pop_back`, **and** random-access iterators (the heap algorithms need them). That's `vector` (default) or `deque`. `list` is rejected.

> **C++98 ctor signature is awkward** — it takes the comparator first, then the container. C++11 added cleaner constructors. In practice you mostly use the default ctor and `push` items in.

---

## 3. Key API

| Method      | What it does                                             | Cost      |
|-------------|----------------------------------------------------------|-----------|
| `push(x)`   | Insert and re-heapify.                                   | O(log n)  |
| `pop()`     | Remove the top. **Returns `void`.**                      | O(log n)  |
| `top()`     | Reference to the largest element. UB on empty.           | O(1)      |
| `size()`    | Element count.                                           | O(1)      |
| `empty()`   | `size() == 0`.                                           | O(1)      |

**No iteration, no random access, no `begin`/`end`.** And uniquely among adapters: **no comparison operators** (`==`, `<`). The only thing you can do is push, peek, pop.

---

## 4. Max-heap by default — and how to flip it

This catches every newcomer. `std::less<T>` (the default comparator) means "less than" — but the heap stores the element such that **no other element compares greater than it** at the front. So the *front* is the *largest*. That's a max-heap.

```cpp
std::priority_queue<int> pq;
pq.push(3); pq.push(1); pq.push(4); pq.push(1); pq.push(5);
std::cout << pq.top() << "\n";                  // 5  (the maximum)
```

For a **min-heap** (smallest comes out first), use `std::greater<int>`:

```cpp
#include <functional>
std::priority_queue<int, std::vector<int>, std::greater<int> > pq;
pq.push(3); pq.push(1); pq.push(4); pq.push(1); pq.push(5);
std::cout << pq.top() << "\n";                  // 1  (the minimum)
```

Custom comparators work the same way — supply a functor whose `operator()(const T &a, const T &b) const` returns true when `a` should come **after** `b` in the priority ordering.

```cpp
struct ByLength {
    bool operator()(const std::string &a, const std::string &b) const {
        return a.size() < b.size();             // longest string is "greatest"
    }
};
std::priority_queue<std::string, std::vector<std::string>, ByLength> pq;
```

---

## 5. Same `pop()` returns `void` rule

Identical to stack and queue:

```cpp
// WRONG
int x = pq.pop();

// RIGHT
int x = pq.top();
pq.pop();
```

Same reasoning: separating *read* from *destroy* protects against a throwing copy losing the element.

---

## 6. Efficiency tips

### 6.1 Bulk-construct beats N pushes

```cpp
// SLOW — N × O(log N) = O(N log N)
std::priority_queue<int> pq;
for (int i = 0; i < N; ++i) pq.push(data[i]);

// FAST — one make_heap call: O(N)
std::vector<int> v(data, data + N);
std::priority_queue<int> pq(std::less<int>(), v);   // ctor calls make_heap once
```

The range-based construction is asymptotically faster (`make_heap` is O(N), not O(N log N)).

### 6.2 Default backing (vector) is almost always right

`vector` gives the heap functions contiguous random-access memory — exactly what they want. `deque` works but adds block-boundary overhead per access. There's no good reason to prefer it for heap workloads.

### 6.3 Reserve indirectly

You can't `reserve` on a priority_queue (no such member). Workaround: build a vector first, reserve there, then construct the queue from it.

### 6.4 Pop in a loop = sort

```cpp
while (!pq.empty()) {
    std::cout << pq.top() << " ";
    pq.pop();
}
```

That's a heapsort. O(n log n), in descending order (max-heap) or ascending (min-heap). Useful when you want elements in priority order one at a time without sorting all up front.

### 6.5 Don't use it as a "sorted vector" — `std::set` or sorted vector + `lower_bound` are usually better

If you need sorted iteration, range queries, or to remove arbitrary elements, the heap is the wrong shape — it only knows how to find/remove the maximum. Picking the right tool: priority_queue for "always pull the next-best thing"; set for "sorted, by-key access."

---

## 7. When NOT to use priority_queue

- You need to delete arbitrary elements → use `std::set` (or implement a "lazy deletion" by ignoring stale tops on pop).
- You need to update an element's priority → not directly supported. Common workaround: push a duplicate with the new priority and skip stale ones on pop.
- You need stable order for equal-priority items → priority_queue makes no guarantee about ties. Embed an insertion sequence number in the comparator if you need it.
- You need to iterate the items → wrong adapter. Use a vector and `std::sort`/`std::partial_sort`.

---

## 8. C++98 caveats

| Feature              | C++ version | C++98 alternative                                |
|----------------------|-------------|--------------------------------------------------|
| `emplace`            | C++11       | `push` — pays a copy.                            |
| Move semantics       | C++11       | Pass by `const &`.                               |
| Lambda comparators   | C++11       | Define a functor struct with `operator()`.       |
| Cleaner ctor (just iterators) | C++11 | Awkward C++98 ctor takes `(comp, container)`.   |

---

## 9. Gotchas

- **Default is a *max*-heap, not min.** Pass `std::greater<T>` for a min-heap.
- **`pop()` returns `void`.** `top()` first.
- **No way to remove arbitrary elements.** The data structure doesn't support it; use `std::set` if you need to.
- **No iterators, no `begin`/`end`.** If you need them, the priority_queue isn't your tool.
- **The comparator is *strict weak ordering*.** Returning `true` for both `comp(a,b)` and `comp(b,a)` is UB.
- **C++98 ctor signature is `(Compare, Container)`** — order matters and is easy to swap by accident.
- **No comparison operators on priority_queues themselves** — `pq1 == pq2` does not compile. Compare the underlying containers if you need to.

---

## 10. Worked example — top K elements

```cpp
#include <queue>
#include <vector>
#include <functional>
#include <iostream>

int main(void)
{
    int data[] = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5};
    int N = sizeof(data) / sizeof(data[0]);
    int K = 3;

    // Min-heap of size K — keep the K LARGEST elements.
    // Trick: when full and a new element is bigger than the smallest kept, swap them.
    std::priority_queue<int, std::vector<int>, std::greater<int> > heap;

    for (int i = 0; i < N; ++i)
    {
        if ((int)heap.size() < K)
            heap.push(data[i]);
        else if (data[i] > heap.top())
        {
            heap.pop();
            heap.push(data[i]);
        }
    }

    // heap now contains the K largest elements (in min-heap order)
    while (!heap.empty())
    {
        std::cout << heap.top() << " ";        // 5 6 9
        heap.pop();
    }
    std::cout << "\n";
    return 0;
}
```

Time: O(N log K) instead of the O(N log N) you'd pay for a full sort. The min-heap-of-size-K is the canonical "top K" trick — and the priority_queue is the right tool because it always exposes the worst-of-the-best for cheap comparison.

---

> **One-line summary.** `std::priority_queue` is a max-heap on a vector with `push` / `top` / `pop`. Flip to min-heap with `std::greater<T>`. O(log n) push, O(1) top. No iteration, no arbitrary deletion — it does one job (pull the extreme) extremely well.
