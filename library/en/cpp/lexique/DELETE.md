# `delete` — Run The Destructor **And** Release The Memory

> **TL;DR.** `delete` is the inverse of `new`: it runs the destructor (running invariants down to nothing) and then returns the memory to the allocator. The matching form (`delete` vs `delete[]`) must be correct, otherwise you get undefined behavior — usually a crash, sometimes silent corruption.

Related: [`NEW.md`](NEW.md) · [`MEMORY.md`](../notions/fundamentals/MEMORY.md) · [`ORTHODOX_CANONICAL_FORM.md`](../notions/oop/ORTHODOX_CANONICAL_FORM.md)

---

## 1. The two-step model

```cpp
delete p;
```

What actually happens, in order:

```
   step 1: run p->~T()                       — destructor cleans up the object
   step 2: ::operator delete(p, sizeof(T))   — return the bytes to the allocator
```

For `new[]`:

```cpp
delete[] arr;
```

```
   step 1: read the cookie at arr-N         — find the element count N
   step 2: run ~T() for each of the N       — in REVERSE order (N-1, N-2, ..., 0)
   step 3: ::operator delete[](arr, …)      — return the bytes to the allocator
```

Compare to C:

```c
free(p);     // step 2 only — no destructors
```

In C the heap doesn't know there are objects with cleanup logic on it. In C++, `delete` understands.

---

## 2. The form-matching rule

| Allocation | Deallocation |
|---|---|
| `new T(...)`            | `delete p`              |
| `new T[n]`              | `delete[] p`            |
| `new (buf) T(...)`      | `p->~T()` only — no delete (memory not from `new`) |
| `new (std::nothrow) T`  | `delete p`              |
| `malloc(...)`           | `free(...)` — never `delete` |

Mismatching is **undefined behavior**. Common forms of the bug:

```cpp
int *arr = new int[10];
delete arr;            // BUG — should be delete[]

int *p = new int(5);
delete[] p;            // BUG — should be delete

char *m = (char*)std::malloc(10);
delete[] m;            // BUG — should be std::free
```

The compiler can't always catch this. Some, like `delete[]` on a single object, blow up at runtime. Some corrupt the heap silently.

---

## 3. Hardware/runtime view

The allocator stores metadata in a hidden header before each block:

```
   what new returns to you:
                    ▼
   ┌─────────────┬─────────────────────────────────┐
   │  metadata   │     YOUR OBJECT (size bytes)    │
   │ (size etc.) │                                  │
   └─────────────┴─────────────────────────────────┘
```

For `new[]` of objects with non-trivial destructors, an additional cookie at the start records the count:

```
   what new[] returns to you:
                          ▼
   ┌─────────────┬─────┬─────────────────────────────────┐
   │  alloc meta │  N  │  obj | obj | obj | ... | obj    │
   └─────────────┴─────┴─────────────────────────────────┘
                          ↑
                          arr (the pointer you got)
```

When you call `delete[] arr`, the runtime reads `N` from `arr-sizeof(N)`, runs `~T()` `N` times in reverse order, then frees the block.

If you call `delete arr` (no brackets) on this:

- It tries to call `~T()` exactly once on `*arr`.
- It then calls `::operator delete(arr, ...)` — but the *real* allocation header is at `arr - cookie - alloc_meta`. The allocator now has a wrong pointer to free → corruption or crash.

Inverse problem: `delete[]` on a single `new`-allocated object reads "N" from memory that isn't a cookie → garbage `N` → tries to destruct N random bytes → crash.

This is why the form must match. The runtime cannot guess.

---

## 4. What the destructor does

```cpp
class Buffer {
    int *_data;
public:
    Buffer(int n) : _data(new int[n]) {}
    ~Buffer() { delete[] _data; }     // free what we owned
};
```

```cpp
Buffer *b = new Buffer(100);
delete b;
```

Sequence:

```
   delete b
       │
       ├── Buffer::~Buffer() runs
       │       │
       │       └── delete[] _data
       │              │
       │              └── frees the int[100] block
       │
       └── ::operator delete(b)
              │
              └── frees the Buffer block
```

Two `new`s, two `delete`s — balanced. RAII makes this automatic for the user of `Buffer`: they just `delete b;` and everything is cleaned up.

---

## 5. `delete` on a null pointer is safe

```cpp
int *p = 0;
delete p;           // no-op, no crash, no UB
delete[] p;         // also safe
```

The standard guarantees this. So defensive code like:

```cpp
if (p) delete p;
```

is unnecessary. Just `delete p;` works.

After deleting, set the pointer to 0 if you'll reuse the variable:

```cpp
delete p;
p = 0;              // optional but defensive — accidental reuse will be a clean nullptr deref
```

---

## 6. Double-free is a disaster

```cpp
int *p = new int(5);
delete p;
delete p;           // UB — block already freed; allocator metadata corrupted
```

After `delete p`, the block is back in the allocator's free list. A second `delete` sees the metadata in an unexpected state and can:

- Crash immediately (best case).
- Corrupt the free list (then a later `new` returns a bad pointer).
- Silently break some random other allocation hours later.

Set the pointer to 0 to prevent accidental double-frees, or design ownership to make double-free structurally impossible.

---

## 7. Use-after-free is also a disaster

```cpp
int *p = new int(5);
delete p;
*p = 10;            // UB — memory may be reused, may belong to something else now
```

After delete, the bytes are still there *for a microsecond* — the address-space mapping is unchanged. So a use-after-free might "work" in tiny tests and corrupt memory in production. Always invalidate pointers after delete.

---

## 8. Exception safety in destructors

A destructor should **not throw**. If it does, and another exception is already in flight (e.g., the destructor was running during stack unwinding), the program calls `std::terminate`.

```cpp
class C {
public:
    ~C() {
        if (problem()) throw std::runtime_error("bad");    // DON'T do this
    }
};
```

Instead, log and swallow, or signal via a different channel.

---

## 9. Custom `operator delete`

Mirror of `operator new`:

```cpp
class C {
public:
    static void operator delete(void *p, std::size_t size) {
        std::cout << "C::operator delete(" << size << ")\n";
        std::free(p);
    }
};
```

If you provide a custom `operator new`, you should provide a matching `operator delete`. Otherwise the default `delete` (calling `::operator delete`) won't pair with your custom `new`'s allocator.

---

## 10. Tips & tricks

### 10.1 Set pointer to 0 after delete (in long-lived classes)

```cpp
class Owner {
    Resource *_r;
public:
    ~Owner() { delete _r; _r = 0; }   // optional, but defensive
};
```

The class is going away, so `_r = 0` is technically wasted. The point is: if a destructor is non-trivial and could re-enter (signal handlers, weird flow), nulling avoids accidents.

### 10.2 The "rule of three" — see OCF

If your class has a non-trivial destructor (i.e., it `delete`s things), you almost certainly need a custom copy constructor and copy assignment too. Otherwise the default copies will share the pointer, and both objects will try to delete it.

```cpp
// BAD: rule-of-three violated
class Buffer {
    int *_data;
public:
    Buffer()  : _data(new int[100]) {}
    ~Buffer() { delete[] _data; }
};

void demo() {
    Buffer a;
    Buffer b = a;       // default copy ctor copies the pointer
}                       // a destructs → frees _data
                        // b destructs → tries to free same _data → crash
```

See [`ORTHODOX_CANONICAL_FORM.md`](../notions/oop/ORTHODOX_CANONICAL_FORM.md).

### 10.3 RAII makes manual delete rare

Inside a class destructor, `delete` is fine. Outside class code, manual `delete` is a smell — usually it should be inside a destructor of some owning class.

### 10.4 Ownership conventions

- *Raw pointer*: "I'm just looking, I don't own this." Don't delete.
- *Pointer in a member with `delete` in destructor*: "I own this; nothing else should delete it."

In C++11+ smart pointers (`std::unique_ptr`, `std::shared_ptr`) make ownership explicit. Not in 42 C++98 — you express it through naming and convention.

### 10.5 Don't `delete this` casually

Legal but treacherous:

```cpp
void C::release() {
    if (--_refs == 0) delete this;
}
```

After `delete this`, the function must not touch `*this` and the caller must not use the pointer. Used in some refcounted designs; almost never appropriate in 42 modules.

---

## 11. Common errors

| Error | Cause | Fix |
|---|---|---|
| Crash on `delete` | Mismatched form (`delete` vs `delete[]`) | Match: `new T[]` ↔ `delete[]`, `new T` ↔ `delete` |
| `double free or corruption` | Same pointer deleted twice | Null after delete; clarify ownership |
| Heap-use-after-free (asan) | Used a pointer after `delete` | Don't reuse; or null and check |
| Memory leak | Missing `delete` | Trace ownership; for every `new` find the `delete` |
| `terminate called recursively` | Destructor threw during stack unwinding | Don't throw from destructors |
| Double-delete during exception | Manual cleanup + RAII member trying to clean up the same thing | Pick one — let RAII own it |

---

## 12. Visual summary

```
              ┌──────────────────────────────────────────┐
              │   delete p;                              │
              └─────────────────────┬────────────────────┘
                                    │
                         ┌──────────┴──────────┐
                         ▼                     ▼
                  step 1: destruct      step 2: deallocate
                  ────────────────      ──────────────────
                  p->~T() runs.         ::operator delete(p)
                  members destructed.   returns bytes to the
                  base destructors      allocator's free list.
                  run after derived
                  in derived hierarchies.

              ┌──────────────────────────────────────────┐
              │   delete[] arr;                          │
              └─────────────────────┬────────────────────┘
                                    │
                                    ▼
                  read cookie → N. run ~T() N times in reverse.
                  ::operator delete[](arr) returns the block.

              must match new with delete:
                 new T            → delete p
                 new T[n]         → delete[] arr
                 placement new    → p->~T()  (no delete)
                 malloc           → free     (never delete)

              after delete, the pointer is dangling.
              don't read, don't write, don't delete again.
```

---

## 13. Practice

1. Why is `delete arr;` after `int *arr = new int[10];` undefined behavior even though it might not crash? *(`delete[]` looks at a cookie before the pointer; without it, the allocator gets a wrong pointer / wrong destruct count → corruption that may not surface immediately.)*
2. Why is `delete 0;` (null pointer) safe? *(The standard guarantees `delete` on a null pointer is a no-op.)*
3. Why does a destructor that throws cause `terminate`? *(If unwinding is already in progress for another exception, the runtime can't handle two simultaneous exceptions and terminates.)*
4. After `delete p; p = 0;`, why is the second statement defensive but not strictly required? *(Strictly, you "shouldn't reuse" `p` regardless. Setting to 0 ensures any accidental reuse fails loudly rather than silently corrupting memory.)*
