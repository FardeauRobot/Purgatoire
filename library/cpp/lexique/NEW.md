# `new` — Allocate Memory **And** Construct An Object On It

> **TL;DR.** `new` does **two** things: it asks the runtime for raw memory, then it runs the constructor on that memory. `malloc` only does the first half. Every `new` must be paired with a matching `delete` (or `delete[]` for `new[]`).

Related: [`DELETE.md`](DELETE.md) · [`MEMORY.md`](../notions/fundamentals/MEMORY.md) · [`BASICS.md`](../notions/fundamentals/BASICS.md#6.1%20new%20%2F%20delete%20instead%20of%20malloc%20%2F%20free)

---

## 1. The two-step model

```cpp
MyClass *p = new MyClass(42);
```

What actually happens, in order:

```
   step 1: ::operator new(sizeof(MyClass))  → raw bytes from the heap
   step 2: placement-construct: MyClass(42) at that address
   step 3: return a typed pointer to it
```

Compare to C:

```c
MyClass *p = malloc(sizeof(*p));   // step 1 only — bytes are uninitialized
                                    // (and there's no constructor in C anyway)
```

In C++, after `new MyClass(42)`:
- Memory is allocated.
- The object is fully constructed — invariants hold, members initialized.
- Virtual table pointer is set (if applicable).
- You get a typed pointer; the compiler knows what's there.

---

## 2. Hardware/runtime view — what the heap is

```
   process address space
   ┌─────────────────┐ high
   │      stack      │   ← function locals; managed by the CPU (rsp register)
   ├─────────────────┤
   │       …         │
   ├─────────────────┤
   │      heap       │   ← grown via brk()/mmap(); managed by the malloc allocator
   ├─────────────────┤
   │     globals     │
   ├─────────────────┤
   │      code       │
   └─────────────────┘ low
```

`new` calls `::operator new(size_t)` which (typically) calls the C library's `malloc` underneath, which (eventually) syscalls `brk` or `mmap` to get pages from the kernel. The first call to `new` you make may take microseconds; subsequent ones reuse memory from the allocator's free lists and take nanoseconds.

```
   first new:    program → libc malloc → maybe a syscall → kernel allocates pages
   later new:    program → libc malloc → free list pop → return cached chunk
```

What you get back is an address aligned to at least `alignof(std::max_align_t)` (usually 16 bytes on x86_64). The allocator stores its own metadata in a hidden header just before your block — that's why "writing one byte before the pointer" corrupts the heap.

---

## 3. Five forms of `new`

### 3.1 Single object

```cpp
MyClass *p = new MyClass();           // calls default ctor
MyClass *q = new MyClass(1, 2);       // calls matching ctor
delete p;
delete q;
```

### 3.2 Array

```cpp
int *arr = new int[10];                // 10 ints, default-initialized (uninitialized for built-ins)
MyClass *objs = new MyClass[5];        // 5 default-constructed MyClass objects
delete[] arr;
delete[] objs;
```

In C++98, you cannot pass arguments to the array constructors with `new[]`. Each element gets the default constructor.

### 3.3 `new[]` storage layout — the cookie

When you `new[]` an array of objects with non-trivial destructors, the runtime stores the element count just before your pointer:

```
       hidden cookie     your pointer
       ─────────────     ────────────
            ▼                ▼
            ┌─────┬─────┬─────┬─────┬─────┬─────┐
            │  N  │ obj │ obj │ obj │ obj │ obj │
            └─────┴─────┴─────┴─────┴─────┴─────┘
```

That's why `delete[]` knows how many destructors to run. Mixing `new[]` with `delete` (no brackets) → it skips the cookie, calls one destructor, and frees a wrong address. Undefined behavior, often a crash.

```cpp
int *arr = new int[10];
delete arr;              // BUG — should be delete[]
```

### 3.4 Nothrow `new`

```cpp
#include <new>
int *p = new (std::nothrow) int[1000000000];
if (p == 0) {
    // allocation failed — gracefully handle
}
```

Default `new` **throws `std::bad_alloc`** on failure. `new (std::nothrow)` returns null pointer instead. 42 norm rarely uses this — exceptions are the C++ way to signal allocation failure.

### 3.5 Placement `new` — construct in pre-existing memory

```cpp
#include <new>

char buffer[sizeof(MyClass)];
MyClass *p = new (buffer) MyClass(42);     // construct in 'buffer'

p->~MyClass();                             // destructor must be called manually
                                            // do NOT 'delete p' — buffer wasn't malloc'd
```

Used by container implementations (e.g., `std::vector`'s `reserve`+`push_back` pattern) to separate allocation from construction. Advanced; you'll mostly avoid it.

---

## 4. What `new` does NOT do

- It does **not** zero memory unless the type has a constructor that does.
- It does **not** check pointer validity later — once you have a pointer, the runtime forgets about it; mismatching `delete` is your problem.
- It does **not** track ownership. Two pointers to the same `new` block, two `delete`s — double free, crash.

```cpp
int *a = new int(5);
int *b = a;
delete a;
delete b;             // BUG — double free, undefined behavior
```

---

## 5. Constructor failure — `new` cleans up its own allocation

If the constructor throws an exception, `new` releases the memory:

```cpp
class C { public: C() { throw std::runtime_error("nope"); } };

try {
    C *p = new C();          // memory allocated, ctor throws
                              // 'new' deallocates the memory before propagating
}
catch (...) {
    // p was never assigned to — and the memory is gone.
}
```

This guarantee is "exception-safe `new`": you don't leak memory on a throwing constructor. (Member subobjects already constructed are also destructed in reverse order — see exception safety.)

---

## 6. `new` vs `malloc` — the differences in one table

| | `malloc(sizeof(T))` | `new T()` |
|---|---|---|
| Allocates memory | yes | yes |
| Calls constructor | **no** | **yes** |
| Initializes | leaves garbage | runs ctor; built-in types may still be uninitialized |
| Type-safe | returns `void*`, requires cast | returns `T*` |
| Failure mode | returns NULL | throws `std::bad_alloc` (or returns NULL with `nothrow`) |
| Pair with | `free` | `delete` |
| Honors alignment | up to `alignof(max_align_t)` | yes |
| Custom per-class | no | yes — overload `operator new` |

In 42 CPP modules: **always use `new`/`delete`**, never `malloc`/`free`.

---

## 7. Custom `operator new` (per-class)

A class can define its own allocation:

```cpp
class C {
public:
    static void *operator new(std::size_t size) {
        std::cout << "C::operator new(" << size << ")\n";
        void *p = std::malloc(size);
        if (!p) throw std::bad_alloc();
        return p;
    }
    static void operator delete(void *p) {
        std::cout << "C::operator delete\n";
        std::free(p);
    }
};

C *c = new C();         // calls C::operator new
delete c;               // calls C::operator delete
```

Used for object pools, custom allocators, performance tuning. Out of scope for 42 norm but useful to recognize.

---

## 8. Tips & tricks

### 8.1 Initializer choice

```cpp
int *a = new int;             // uninitialized (built-in types)
int *b = new int();           // value-initialized to 0  ← prefer this
int *c = new int(42);         // initialized to 42
```

For built-in types, `new int()` zero-initializes. `new int` does not. This is a nasty gotcha.

### 8.2 Always pair the form

| Allocation     | Deallocation |
|----------------|--------------|
| `new T`        | `delete p` |
| `new T[n]`     | `delete[] p` |
| `new (buf) T`  | `p->~T();` (no delete!) |

Mismatching is undefined behavior.

### 8.3 Never `new` for things that don't need it

```cpp
int *p = new int(5);   // don't
int  v = 5;             // do
```

Heap allocation is always slower than the stack. Use `new` when you need:
- Object lifetime that exceeds the current scope.
- Polymorphism (you need a pointer to a base class).
- Sizes only known at runtime.

### 8.4 RAII wraps `new`/`delete`

The 42 way is to put `new` in a constructor and `delete` in the destructor:

```cpp
class Buffer {
    int *_data;
    int  _size;
public:
    Buffer(int n) : _data(new int[n]), _size(n) {}
    ~Buffer()    { delete[] _data; }
private:
    Buffer(const Buffer&);              // disabled — copying would double-free
    Buffer& operator=(const Buffer&);   // disabled
};
```

Or implement OCF properly to allow copying. See [`MEMORY.md`](../notions/fundamentals/MEMORY.md) and [`ORTHODOX_CANONICAL_FORM.md`](../notions/oop/ORTHODOX_CANONICAL_FORM.md).

### 8.5 Track every allocation in your destructor

If your class allocates with `new` in its constructor, count the allocations. Your destructor must free exactly that many, with the matching `delete` form.

### 8.6 Use valgrind to verify

```bash
$ valgrind --leak-check=full ./prog
==== LEAK SUMMARY:
====    definitely lost: 0 bytes in 0 blocks
====    indirectly lost: 0 bytes in 0 blocks
```

If you see "definitely lost," you have a `new` without a matching `delete`. See [`MEMORY.md`](../notions/fundamentals/MEMORY.md).

### 8.7 `nullptr` is a C++11 thing — for C++98, use `0` or `NULL`

```cpp
int *p = 0;          // C++98 idiom
int *q = NULL;       // also fine
int *r = nullptr;    // C++11+ — won't compile under -std=c++98
```

After `delete p; p = 0;` is a defensive habit — sets the pointer to a known invalid value so accidental reuse is more likely to crash visibly.

---

## 9. Common errors

| Error | Cause | Fix |
|---|---|---|
| Memory leak | Missing `delete` for a `new` | Match every `new` with a `delete` (preferably via RAII) |
| `pointer being freed was not allocated` | Mismatched `new[]`/`delete`, or freeing a stack pointer | Use `delete[]` for arrays; never `delete &localVar` |
| `double free or corruption` | Two `delete`s for the same allocation | Set pointer to 0 after delete; design ownership clearly |
| `std::bad_alloc` thrown | Out of memory (or insane size) | Check sizes; consider `new (std::nothrow)` if you can recover |
| Crash inside ctor → leak (if not exception-safe) | Constructor allocates members manually before its own throw | Use member objects that own their resources (RAII members) |

---

## 10. Visual summary

```
              ┌──────────────────────────────────────────┐
              │   T *p = new T(args);                    │
              └─────────────────────┬────────────────────┘
                                    │
                         ┌──────────┴──────────┐
                         ▼                     ▼
                  step 1: alloc         step 2: construct
                  ─────────────         ──────────────────
                  ::operator new        T::T(args) runs in
                  (size = sizeof(T))    that memory.
                  returns aligned       vptr set if needed.
                  raw bytes from heap.  invariants now hold.

              ┌──────────────────────────────────────────┐
              │   forms                                   │
              ├──────────────────────────────────────────┤
              │   new T            ←→  delete p          │
              │   new T[n]         ←→  delete[] p        │
              │   new (buf) T      ←→  p->~T(); (no del) │
              │   new (nothrow) T  ←→  delete p (after   │
              │                        null check)        │
              └──────────────────────────────────────────┘

              every new has exactly one matching delete,
              with the matching form — or you have UB.
```

---

## 11. Practice

1. Why does `new int` leave the int uninitialized but `new int()` zero-initializes? *(Default- vs value-initialization. The parens trigger value-init for built-in types.)*
2. What's stored in the "cookie" before a `new[]` array? *(The element count, used by `delete[]` to know how many destructors to run.)*
3. Why is `delete[]` required for arrays? *(Per-element destructors must run; the array form reads the cookie to know how many.)*
4. What happens to memory if a constructor throws inside `new`? *(`new` deallocates it before propagating the exception. No leak — assuming members were RAII-correct.)*
