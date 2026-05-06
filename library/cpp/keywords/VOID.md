# `void` — "Nothing", With Exceptions

> **TL;DR.** `void` is the type for "no value". Functions that return nothing return `void`. Pointers to "any type" use `void*`. You can never have a value of type `void`, never declare a `void` variable, and never apply most operators to a `void`.

Related: [`CASTS.md`](../advanced/CASTS.md) · [`NEW.md`](NEW.md)

---

## 1. The three uses

### 1.1 Function returning nothing

```cpp
void greet() {
    std::cout << "hi\n";
}                              // no return needed
```

You can write `return;` to exit early but must not return a value.

### 1.2 Function taking nothing

```cpp
void f();             // C++ — empty parens already mean "no parameters"
void f(void);         // C — required to mean "no parameters"
                       // legal in C++ but redundant
```

In C, `void f()` means "function with unspecified parameters" (KR-style). In C++, both forms mean "no parameters" — but write `void f()` for C++ idiomaticity.

### 1.3 Generic pointer: `void*`

```cpp
void *p;              // pointer to "any" type
int  n = 42;
p = &n;
int *pn = static_cast<int*>(p);    // recover the typed pointer
*pn = 100;
```

`void*` carries an address with no type. You **cannot** dereference it directly:

```cpp
*p;                   // ERROR — what type is the result?
p[0];                 // ERROR — same problem
```

To use it, cast it back to a typed pointer.

### 1.4 (Bonus) `(void)expr` — discard expression value

```cpp
(void)unused_param;   // tells the reader/compiler: "yes, I'm intentionally not using this"
```

Common in C-style codebases. In C++ you'd often comment-out the parameter name instead:

```cpp
void f(int /*unused*/) { ... }
```

---

## 2. What you can't do with `void`

```cpp
void v;               // ERROR — cannot declare a void variable
void arr[10];         // ERROR — cannot have an array of void
void f();
v = f();              // not even legal — f returns void; you can't assign void
sizeof(void);         // ERROR — void has no size
```

The only place `void` "exists" as a value-like thing is the return statement of a void function:

```cpp
void g() { return; }            // OK — explicit void return
void h() { return f(); }        // OK in C++ — propagating a void result is allowed
```

That last form is occasionally useful in templates.

---

## 3. `void*` — the C generic pointer

`void*` is the C-era escape hatch for "I have a pointer but I don't want to commit to a type." Used in:

- `malloc` / `free` / `realloc` (return / take `void*`).
- `qsort`, `bsearch` callbacks.
- `pthread_create` thread argument.
- C library callbacks with user-data.

```cpp
void *malloc(size_t n);
void  free(void *p);
```

In C, `void*` implicitly converts to and from any object pointer. In C++ it's stricter — conversion **to** `void*` is implicit, but **from** `void*` requires a cast:

```cpp
int *p = malloc(sizeof(int));               // ERROR in C++ (would compile in C)
int *p = static_cast<int*>(malloc(sizeof(int)));   // OK
int *q = (int*)malloc(sizeof(int));         // C-style cast, also works
```

The 42 norm prefers `static_cast` over C-style casts.

### Hardware view

A `void*` is just an address. On x86_64 it's 8 bytes. There's no metadata about what it points at — that knowledge lives in the programmer's head (or in a separate `size_t` parameter).

```
   void *p;
   ┌────────────────┐
   │  raw address   │  8 bytes — no type info
   └────────────────┘
```

This is exactly why C++ adds templates — typed generic code without `void*`-and-cast.

---

## 4. `void*` vs `T*` — type safety lost

```cpp
int    n = 42;
double d = 3.14;

void *p = &d;
int  *pi = static_cast<int*>(p);     // legal cast; *pi is now garbage
*pi = 100;                            // writes to d's memory as if it were int — UB
```

The compiler can't help you — once you go through `void*`, you've stripped type info, and the cast back to `int*` is a bare assertion you might be wrong about.

For 42 modules: avoid `void*` unless interfacing with a C API. Prefer templates or polymorphism for genericity.

---

## 5. `void` in templates

```cpp
template <typename T>
T identity(T x) { return x; }

void v = identity<void>(?);     // can't — no value of type void
```

`void` cannot appear in a context that requires a value. Templates that might end up with `T = void` need specialization (e.g., `std::enable_if`, SFINAE — out of scope for 42).

A safer rule: design templates that don't need `T = void`.

---

## 6. The `(void)expr` cast

In C, `(void)expr` is the standard way to say "evaluate this expression and discard the result, suppress warnings about unused values":

```cpp
(void)printf("hi");     // ignore printf's return value (which is num chars written)
```

In C++ you usually don't bother — most compilers don't warn for this, and the cast is verbose. Useful in macro definitions when you want a "no-op statement":

```cpp
#define ASSERT(cond) ((void)0)     // disable in release builds
```

---

## 7. Tips & tricks

### 7.1 Avoid `void*` in C++

If you find yourself reaching for `void*`, ask:
- Could a template do this with type safety?
- Could a base class with virtual functions do this with polymorphism?
- Is this a C interop boundary? (Then it's fine.)

### 7.2 `void` return is not "no return"

A `void` function still returns control to the caller — it just doesn't return a value. Don't confuse it with `[[noreturn]]` (C++11) or functions that loop forever / call `exit`.

### 7.3 Use `void` to mark "no parameters" in C++

```cpp
void f();         // canonical C++
void f(void);     // C-flavored, redundant in C++
```

### 7.4 Consistent return-from-void

In a `void` function:

```cpp
void f(int n) {
    if (n < 0)
        return;             // early exit — fine
    // ...
}
```

Bare `return;` (no value) is legal and often the cleanest way to exit early.

### 7.5 Don't use `void*` to "hide" a class type

If you want to hide implementation details, use the **Pimpl idiom** — a forward-declared `Impl` type accessed through a pointer. Type-safe, exception-safe, debugger-friendly. Way better than `void*`.

```cpp
// MyClass.hpp
class MyClass {
    class Impl;            // forward decl
    Impl *_impl;
};
```

---

## 8. Common errors

| Error | Cause | Fix |
|---|---|---|
| `cannot convert 'void*' to 'T*' without a cast` | C-style implicit conversion not allowed in C++ | `static_cast<T*>(...)` |
| `void value not ignored as it ought to be` | Used a `void`-returning expression where a value is expected | Don't assign / return it where a typed value is required |
| `array of void` / `void variable` | Tried to declare `void v;` or `void arr[10];` | Pick a real type |
| Misuse of `void*` causing memory corruption | Pointer cast to wrong type | Avoid `void*` in C++; use templates or virtual dispatch |

---

## 9. Visual summary

```
              ┌────────────────────────────────────────────┐
              │   keyword: void                             │
              └─────────────────────┬──────────────────────┘
                                    │
              ┌─────────────────────┼─────────────────────┐
              ▼                     ▼                     ▼
        return type            no parameters         generic pointer
        ─────────────          ──────────────        ──────────────
        function returns       void f();             void *p;
        nothing.               (in C++; C uses       address only,
        plain return; or       'void f(void)')       no type info.
        omit return.                                  must cast back
                                                      to a typed pointer
                                                      to use.

        cannot:
          - declare 'void' variable
          - take sizeof(void)
          - apply * (deref) to void*
          - have an array of void

        in C++ prefer templates and polymorphism over void*.
        in C interop, void* is unavoidable.
```

---

## 10. Practice

1. Why does `*p` not work when `p` is `void*`? *(The result type is unknown — `void` has no size and no operations.)*
2. Why does C++ require a cast from `void*` to `T*` while C does it implicitly? *(C++ is stricter about type safety; the cast forces the programmer to acknowledge they're asserting the type.)*
3. Why is `void f(void)` redundant in C++? *(In C++, `void f()` already means "no parameters"; the C-style `(void)` is for compatibility.)*
4. Can a function return `void`? Can a variable have type `void`? *(Yes / no. `void` is "absence of a value" — you can return it from a function but can't store it.)*
