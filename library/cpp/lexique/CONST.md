# `const` — The Promise You Make to the Compiler

> **TL;DR.** `const` means *"I promise this won't change after initialization."* The compiler enforces the promise; the linker uses it to put data in read-only memory; the optimizer uses it to skip redundant reloads.

Related: [`BASICS.md`](../notions/fundamentals/BASICS.md#10.%20const%20%E2%80%94%20much%20more%20powerful%20than%20in%20C) · [`REFERENCE.md`](../notions/fundamentals/REFERENCE.md) · [`STATIC.md`](STATIC.md) · [`MUTABLE.md`](MUTABLE.md)

---

## 1. The mental model

`const` is **a contract**, not a storage class. It says: *"through this name, the value cannot be changed."*

It applies to whatever sits **immediately to its left** — and if nothing is to its left, to whatever is immediately to its right.

```cpp
const int   a = 1;   // const applies to 'int'         → "constant int"
int   const b = 1;   // identical: 'const' applies to 'int' on its left
const int  *p;       // pointer to const int           → can't change *p
int  *const q = &x;  // const pointer to int           → can't change q
const int *const r = &x; // const pointer to const int → neither can change
```

### Read it right-to-left

```
const int *const r;
       │     │
       │     └─ r itself is const
       └─────── what r points to is const
```

```
       int  ← what r points to  ← const ← * ← const ← r
       │                                          │
       └─ "r is a const pointer to a const int"
```

---

## 2. The four places `const` lives

```
   ┌───────────────────────────────────┐
   │ 1. const variable (file/local)    │   const int N = 10;
   ├───────────────────────────────────┤
   │ 2. const reference / pointee      │   void f(const std::string& s);
   ├───────────────────────────────────┤
   │ 3. const member function          │   int getX() const;
   ├───────────────────────────────────┤
   │ 4. const member variable          │   const int _id;
   └───────────────────────────────────┘
```

Each one tells the compiler a different thing. Internalize them.

---

## 3. `const` variables — where do they live?

```cpp
const int N = 10;     // file scope
int main() {
    const int M = 20; // local scope
}
```

| Where it lives | What the compiler usually does |
|---|---|
| **File-scope `const` with literal initializer** | Inlined into a read-only section of the binary (`.rodata`/`.rdata`). May not even occupy a runtime address. |
| **Local `const` with literal initializer** | Often optimized away — the value is propagated into instructions directly (constant folding). |
| **`const` with a runtime initializer** (`const int n = rand();`) | Lives on the stack like any local. The `const` is purely a compile-time check. |

```
Process memory layout (typical Linux/macOS)
┌──────────────────┐ high addr
│      stack       │  ← local 'const' lives here unless optimized away
├──────────────────┤
│       heap       │  ← 'new'-allocated objects
├──────────────────┤
│       BSS        │  ← uninitialized globals
├──────────────────┤
│      data        │  ← initialized globals (writable)
├──────────────────┤
│     rodata       │  ← string literals + 'const' globals
├──────────────────┤
│       text       │  ← machine code
└──────────────────┘ low  addr
```

Check it on Linux:
```bash
$ objdump -t a.out | grep " N$"
0000000000002004 g     O .rodata    0000000000000004  N
                            ^^^^^^^^   N is in .rodata — read-only
```

Writing through a `const_cast`-stripped pointer to such memory is **undefined behavior** and on most systems will SIGSEGV — the OS marks `.rodata` pages read-only at the page-table level.

```
        page table entry              MMU
        ┌────────────────────────┐    
        │ phys addr | R | W | X  │  → if W=0 and the CPU writes,
        │   …       │ 1 │ 0 │ 0  │    you get SIGSEGV (signal 11)
        └────────────────────────┘
```

---

## 4. `const` parameters & references — the workhorse

This is the single most common use of `const` in real C++:

```cpp
void print(const std::string& s);   // pass-by-const-reference
```

What this buys you:

- **No copy.** You pass an alias — typically a pointer the compiler hides — instead of duplicating the object.
- **The function can't mutate your data.** Compiler enforces it.
- **It binds to literals and temporaries.** A non-const reference cannot bind to `"hello"`; a `const std::string&` can (after a single conversion).

```cpp
void f(std::string& s);          // can't accept "hello"
void g(const std::string& s);    // can accept "hello", "x", x, std::string("y"), …
```

### Hardware view

```
caller                    callee
┌───────────────┐         ┌────────────────────┐
│ s = "long..." │ ◄─────  │ const std::string& │
│ (heap buffer) │  addr   │  s in callee       │
└───────────────┘         └────────────────────┘
   the heap-allocated buffer is shared. Only the
   address (8 bytes on x86_64) is passed in a register.
```

Compare with pass-by-value:

```
caller                    callee
┌───────────────┐         ┌────────────────────┐
│ s = "long..." │ ───┐    │ std::string s      │
│               │    └──► │  (full copy made)  │
└───────────────┘         └────────────────────┘
   a malloc + memcpy happens on every call.
```

**Rule of thumb:** if the type is bigger than a `void*` (8 bytes) and you don't need to copy it, pass `const T&`.

---

## 5. `const` member functions — the const at the end

```cpp
class Circle {
    double _r;
public:
    double area() const { return 3.14159 * _r * _r; }   // const member fn
    void   setRadius(double r) { _r = r; }              // non-const
};
```

What `const` after the parameter list does:

- Promises the function does not modify the object's observable state.
- Makes `this` have type `const Circle*` instead of `Circle*` inside the function.
- **Required** to call the function on a `const Circle` (or `const Circle&`, or `const Circle*`).

```cpp
const Circle c(5.0);
c.area();         // OK
c.setRadius(3);   // ERROR — non-const member fn on const object
```

### Why the rule exists — the vtable doesn't help here

This is purely a **type-system check**. Nothing changes at the machine-code level: the same instructions run for `const` and non-const member functions; only the compiler refuses to compile the call.

### `const` overloading

You can have two member functions with the same name and parameters that differ only in `const`:

```cpp
class Buffer {
    char _data[1024];
public:
    char&       operator[](std::size_t i) { return _data[i]; }   //mutable access
    
    const char& operator[](std::size_t i) const { return _data[i]; } //read-only};


Buffer       b;
const Buffer cb;

b[0] = 'x';        // calls non-const overload — OK
char c = cb[0];    // calls const overload — OK
cb[0] = 'x';       // ERROR — const overload returns const char&
```

### `const`-correctness chain reaction

If `getName()` is `const`, every function it calls on `*this` must also be `const`. One missing `const` ripples through your whole class. Add `const` to a getter at the start, not the end.

---

## 6. `const` member variables

```cpp
class Contact {
    const int _id;          // every Contact has its own _id, set once
public:
    Contact(int id) : _id(id) {}   // MUST use init list — assignment in body fails
};
```

Constraints:
- **Must be initialized in the initializer list.** Assignment in the constructor body would be a write to a `const`.
- **No default copy assignment.** The compiler-generated `operator=` would need to write `_id`. If you need assignment, write it yourself and either don't update `_id`, or rethink whether it should be `const`.

```cpp
Contact a(1), b(2);
a = b;  // ERROR: copy assignment implicitly deleted because _id is const
```

This is one of the most common reasons "OCF compiles for everyone except me" — a stray `const` member silently disables `operator=`.

---

## 7. The pointer/reference table

Memorize this. It is the source of 80% of `const` confusion.

| Declaration | Can rebind? | Can mutate target? |
|---|:-:|:-:|
| `int *p`              | yes | yes |
| `const int *p`        | yes | **no** |
| `int const *p`        | yes | **no** *(same as above)* |
| `int *const p`        | **no** | yes |
| `const int *const p`  | **no** | **no** |
| `int &r`              | n/a *(refs never rebind)* | yes |
| `const int &r`        | n/a | **no** |

A reference is *implicitly* "const-bound" — there is no syntax for `int &const r` because it would be redundant. Only the *target* of a reference can be `const`.

---

## 8. Constants vs `#define`

```cpp
#define MAX 100              // C-style — preprocessor textual replacement
const int MAX = 100;         // C++ — typed, scoped, debuggable
```

Why `const` wins:
- **Has a type.** `MAX` is `int`, not "whatever 100 looks like at the call site".
- **Has a scope.** Living inside a namespace or class restricts its reach.
- **Has an address.** You can take `&MAX` and pass it to a function (the compiler will materialize it if needed).
- **Survives the debugger.** `#define`'d names are gone by the time gdb sees the binary.

Use `#define` only for include guards and conditional compilation, not constants.

---

## 9. Tips & tricks

### 9.1 `const` everything you don't mutate

Aim for "const by default". The mental cost is low; the protection is real. If you find yourself wanting to remove a `const`, ask first whether you should change the data flow instead.

### 9.2 The `const_cast` escape hatch — almost never the answer

```cpp
void legacy_api(char *s);           // takes char*, doesn't actually write

const char *msg = "hello";
legacy_api(const_cast<char *>(msg)); // compiles, but: writing into msg is UB
```

Use `const_cast` only when interfacing with an old API that *promises* not to write but takes a non-const pointer. Never to "make the error go away."

### 9.3 `mutable` — the legitimate `const` escape

A member declared `mutable` can be modified from a `const` member function. Used for caches, mutexes, lazy initialization. See [`MUTABLE.md`](MUTABLE.md).

### 9.4 Return by `const` value? Don't.

```cpp
const std::string getName() const;   // the outer 'const' is useless in C++98
```

In C++98 it just blocks calls like `getName().clear()` — but it also blocks move semantics in C++11+. Skip it. Make the *member function* `const`, not the return value.

### 9.5 `const` does NOT mean "compile-time constant"

```cpp
const int n = readFromFile();   // legal — n's value is computed at runtime
int arr[n];                     // ILLEGAL in C++98 — needs a constant expression
```

For compile-time constants in C++98, use `enum { N = 10 };` or a `static const` integer initialized in the header. For compile-time constants in C++11+, use `constexpr`.

### 9.6 Top-level `const` on parameters is ignored for signature matching

```cpp
void f(int x);
void f(const int x);    // SAME function. Linker sees one symbol.
```

But:

```cpp
void f(int *p);
void f(const int *p);   // DIFFERENT functions. Pointee const matters.
```

Top-level on a value parameter is just a hint to the implementer. Pointee/referent const is part of the type.

---

## 10. Common compiler errors

| Error | Cause | Fix |
|---|---|---|
| `passing 'const X' as 'this' argument discards qualifiers` | Calling a non-const member fn on a const object | Add `const` to that member fn |
| `assignment of read-only location` | Writing to `*p` where `p` is `const T*` | Drop the `const` or stop writing |
| `uninitialized const member '_x'` | `const` member without initializer-list entry | Initialize in `: _x(value)` |
| `binding reference of type 'X&' to 'const X' discards qualifiers` | Passing a `const` object to a `T&` parameter | Make the parameter `const T&` |

---

## 11. Visual summary

```
                ┌────────────────────────────────────────┐
                │             const X                     │
                │   "this object cannot change"           │
                └─────────────────┬──────────────────────┘
                                  │
        ┌─────────────────────────┼─────────────────────────┐
        ▼                         ▼                         ▼
   const variable          const reference          const member fn
   (data in .rodata        (alias that won't        (this becomes
    or stack, can't be      mutate the target)        const T*; can be
    written through                                   called on const objects)
    that name)
                              │
                              ▼
                       const T&  →  pass-by-reference
                                    + no-copy
                                    + binds temporaries
```

---

## 12. Practice questions to test yourself

1. Why does `int& r = 42;` fail but `const int& r = 42;` work? *(answer: a non-const ref needs a real lvalue; a const ref creates a hidden temporary with extended lifetime.)*
2. Can two member functions overload on `const` only? *(yes — it's the standard pattern for getters that return mutable/immutable views.)*
3. Where does a file-scope `const int N = 10;` live in the binary? *(typically `.rodata` if its address is taken; otherwise inlined and never materialized.)*
4. Why is `const int *p` legal to declare without initializing, but `const int& r` is not? *(pointers can be null; references must bind to a real object.)*
