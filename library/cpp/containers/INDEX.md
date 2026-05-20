# `cpp/containers/` — One Page Per STL Container

Deep-dive notes for every C++98 STL container. Use this as the entry point; for the high-level "what's the STL even about" overview, see [`../advanced/STL.md`](../advanced/STL.md).

> Each page follows the same shape: TL;DR → mental model (with ASCII diagram) → construction → key API → complexity table → iterator invalidation → efficiency tips → when not to use → C++98 caveats → gotchas → worked example.

---

## The 10 containers, sorted by family

### Sequence containers — *position-indexed storage*

| Container | File | One-line | Default? |
|---|---|---|---|
| `std::vector<T>` | [`VECTOR.md`](VECTOR.md) | Dynamic contiguous array. The default. `realloc`-style growth, O(1) random access. | ✅ |
| `std::list<T>` | [`LIST.md`](LIST.md) | Doubly-linked list of heap nodes. O(1) splice, stable iterators, no random access. | |
| `std::deque<T>` | [`DEQUE.md`](DEQUE.md) | Block-list — O(1) at both ends *and* random access. References stable across push. | |

`std::string` (technically a sequence container) lives at [`../io-errors/STRING_FUNCTIONS.md`](../io-errors/STRING_FUNCTIONS.md).

### Container adapters — *restricted interface over a sequence container*

| Container | File | One-line | Default backing |
|---|---|---|---|
| `std::stack<T>` | [`STACK.md`](STACK.md) | LIFO. `push` / `top` / `pop`. Only `pop()` is `void` — read with `top()` first. | `deque` |
| `std::queue<T>` | [`QUEUE.md`](QUEUE.md) | FIFO. Same shape as stack but `front()` + `pop()`. Vector backing forbidden. | `deque` |
| `std::priority_queue<T>` | [`PRIORITY_QUEUE.md`](PRIORITY_QUEUE.md) | Binary max-heap. O(log n) push/pop, O(1) top. `std::greater<T>` for min-heap. | `vector` |

### Associative containers — *sorted, key-based access (red-black tree)*

| Container | File | One-line | Allows duplicates? |
|---|---|---|---|
| `std::set<K>` | [`SET.md`](SET.md) | Sorted unique keys. O(log n) insert/erase/find. Stable iterators. | No |
| `std::multiset<K>` | [`MULTISET.md`](MULTISET.md) | Like `set` but duplicates allowed. `equal_range` is the daily tool. | Yes |
| `std::map<K, V>` | [`MAP.md`](MAP.md) | Sorted `K → V`. `m[k]` inserts on miss (the trap). Iterator type is `pair<const K, V>`. | No (keys) |
| `std::multimap<K, V>` | [`MULTIMAP.md`](MULTIMAP.md) | Sorted `K → V` with duplicate keys. No `operator[]`. | Yes |

---

## Decision tree — "which container should I pick?"

```
   Need key-based lookup?
   ├── Yes → ordered, log n, key-sorted iteration?
   │         ├── Yes, value too?         → map / multimap
   │         └── Yes, key only?          → set / multiset
   └── No  → access pattern?
             ├── Push/pop one end only?  → stack (LIFO) or queue (FIFO)
             ├── Always pull the max?    → priority_queue
             ├── Push/pop BOTH ends + random access?  → deque
             ├── Many middle inserts/erases?          → list
             └── Otherwise (the default)              → vector
```

If in doubt: **`std::vector`**. Pick something else only when you have a measured reason.

---

## Cross-cutting concerns covered in the deep-dives

Topics that recur across multiple files — each container's page covers the version specific to that container, but here's what to look for:

- **Iterator invalidation rules** — every container has them; vector's are the strictest, list's are the loosest. See each file's "Iterator invalidation" section.
- **C++98 caveats** — what's *missing* (no `emplace`, no `auto`, no `shrink_to_fit`, no `unordered_*`) and the C++98 substitute for each.
- **The `pop()` returns `void` rule** — applies to all three adapters (stack, queue, priority_queue). Always `top()` / `front()` *before* `pop()`.
- **Pass by `const &`** — every container is value-type; copies are expensive. Always reference unless you mean to copy.
- **The hint form of `insert`** — for ordered containers (set/multiset/map/multimap), passing a correct hint turns O(n log n) bulk-insert into O(n).

---

## Where each container shows up in the 42 cursus

| Container | Mod | Why |
|---|---|---|
| `vector` | CPP08 ex01 (Span), CPP09 ex01 (RPN) | Random-access, push_back-heavy. |
| `list`   | rare              | Comes up when iterator stability is the explicit subject. |
| `deque`  | CPP08 ex02 (MutantStack underneath) | Default backing for `std::stack`. |
| `stack`  | CPP08 ex02 (MutantStack), CPP09 ex01 (RPN) | LIFO discipline. |
| `queue`  | rare in cursus, common in algorithms | BFS, scheduling. |
| `priority_queue` | rare in cursus       | Heap-based selection. |
| `set`    | CPP08 ex01 (Span)              | Auto-sort + dedup. |
| `map`    | CPP09 ex00 (BitcoinExchange)   | Time-series lookup via `lower_bound`. |
| `multiset`/`multimap` | rare              | When duplicates are semantically meaningful. |

---

## Reading order if you're starting from zero

1. **`VECTOR.md`** — establishes the mental model; everything else compares against it.
2. **`STL.md` (in `../advanced/`)** — high-level overview of containers + iterators + algorithms.
3. **`LIST.md`** — the linked-list contrast; teaches iterator stability.
4. **`MAP.md`** — the most common associative container; introduces `pair<const K, V>` and `lower_bound`.
5. **`STACK.md` / `QUEUE.md`** — the adapter pattern and the `pop()`-is-`void` discipline.
6. **`SET.md`, `DEQUE.md`, `PRIORITY_QUEUE.md`** — fill in as you need them.
7. **`MULTISET.md`, `MULTIMAP.md`** — read once you've mastered their unique-key cousins.
