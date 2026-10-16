# `mutable` — The Member That Can Change Through `const`

> **TL;DR.** `mutable` lets a member be modified even when accessed through a `const` member function (or a `const` instance). Use it for things that change but don't change the **observable state** — caches, mutexes, lazy values.

Related: [`CONST.md`](CONST.md) · [`STATIC.md`](STATIC.md)

---

## 1. The motivation

Sometimes a `const` member function needs to write to *something*:

```cpp
class Date {
    int _year, _month, _day;
public:
    std::string toString() const {
        // we want to cache the formatted string so we don't recompute it
        // ... but this is a const member function. we can't write _cache.
    }
};
```

The user's mental model is: *"calling `toString()` doesn't change the date."* True — it doesn't change `_year`, `_month`, `_day`. But internally we want to **memoize**. That's where `mutable` comes in.

```cpp
class Date {
    int _year, _month, _day;
    mutable std::string _cached;       // can be written even from const member fns
    mutable bool        _isCached;
public:
    Date() : _year(0), _month(0), _day(0), _isCached(false) {}

    const std::string& toString() const {
        if (!_isCached) {
            _cached  = format(_year, _month, _day);
            _isCached = true;
        }
        return _cached;
    }
};
```

`toString()` is still **logically** const — two consecutive calls return the same string and nobody outside can tell the cache is involved.

---

## 2. The rule

Inside a `const` member function:

```
   non-mutable members   →   read-only  (this is const T*)
   mutable members       →   read/write
```

A `mutable` member is treated as if `this` were non-const, *only for that member*.

```cpp
class X {
    int           _a;      // const obj → can't write
    mutable int   _b;      // const obj → CAN write
public:
    void f() const {
        _a = 1;   // ERROR — _a is not mutable
        _b = 1;   // OK    — _b is mutable
    }
};
```

---

## 3. Memory layout — `mutable` changes nothing

`mutable` is a **type-system annotation** for the compiler. At the hardware level there's no difference:

```
class Date          memory layout (no padding shown):
┌───────────────────────────────────────────────────────────┐
│ _year (int)  │ _month (int) │ _day (int) │ _cached (str)  │
│              │              │            │ _isCached (b)  │
└───────────────────────────────────────────────────────────┘
       all of these are at fixed offsets,
       all are real read/write memory.
       'mutable' doesn't move them; it's a compiler-only hint.
```

Even `const Date d;` lives in writable memory if it's on the stack — the page is read/write at the OS level. The compiler simply refuses to compile writes through the `const` interface. `mutable` carves out an exception.

If you put a `const Date` in `.rodata` (e.g., a global with literal init) and then write to a `mutable` member, you get a SIGSEGV at runtime — page tables don't care about `mutable`. Don't do this.

---

## 4. The legitimate use cases

### 4.1 Lazy initialization / memoization

```cpp
class Polynomial {
    std::vector<double> _coeffs;
    mutable bool        _hasRoots;
    mutable std::vector<double> _roots;
public:
    const std::vector<double>& roots() const {
        if (!_hasRoots) {
            _roots    = computeRoots(_coeffs);
            _hasRoots = true;
        }
        return _roots;
    }
};
```

Computing roots is expensive. Caching is internal. Caller never sees the cache.

### 4.2 Mutex protection in const operations

```cpp
class ThreadSafeCounter {
    int _count;
    mutable pthread_mutex_t _mu;
public:
    int get() const {
        pthread_mutex_lock(&_mu);            // mu is mutable → ok
        int c = _count;
        pthread_mutex_unlock(&_mu);
        return c;
    }
};
```

Locking a mutex is a write, even on a "read" operation. `mutable` is the standard answer.

### 4.3 Reference-counting / debug counters

```cpp
class Tracked {
    mutable int _accessCount;
public:
    int value() const { ++_accessCount; return _v; }
    int accessCount() const { return _accessCount; }
private:
    int _v;
};
```

Whether this counts as "observable state" is a judgment call. The 42 norm doesn't forbid it.

---

## 5. The illegitimate use cases

`mutable` is **not** for:

- **Defeating `const`-correctness because you got lazy.** If you find yourself making half your members mutable, your design is wrong.
- **Storing user-visible state.** If two const calls return different values for reasons the caller cares about, that's not const.
- **Working around an API you don't want to change.** Add a non-const overload instead.

A test:
> *Could a sane caller write a property-based test that says "two calls in a row return the same thing"?*

If yes — caching state is internal — `mutable` is fine.
If no — observable state changes — your function shouldn't be `const`.

---

## 6. Tips & tricks

### 6.1 `mutable` does NOT remove `const` everywhere

```cpp
class X {
    mutable int _n;
public:
    void f() const {
        _n = 1;            // OK
        modify(*this);     // ERROR if modify takes X& — *this is still const X&
    }
    void modify(X& x);
};
```

`mutable` only loosens access to that **specific member**. The `this` pointer is still `const X*` everywhere else.

### 6.2 `mutable` and references

```cpp
class X {
    mutable int& _ref;     // legal — ref binds at construction, can write through it
};
```

Note: a `mutable` reference doesn't let you rebind it (refs never rebind), only write through it.

### 6.3 `mutable` cannot be applied to

- **`const` members** — `mutable const int x;` is a contradiction; the compiler rejects it.
- **`static` members** — they aren't part of the per-instance layout; `mutable` is meaningless on them.
- **References to const data** — `mutable const T&` is also rejected.

### 6.4 Don't combine with `volatile` casually

`mutable volatile T` is valid but rare — typically a hardware register accessed from a "logical const" wrapper. If you're not writing a driver, you don't need it.

### 6.5 Document why each `mutable` exists

In review, `mutable` is a yellow flag. A one-line `// caches result of expensive computeRoots()` comment makes it green.

---

## 7. Common errors

| Error | Cause | Fix |
|---|---|---|
| `assignment of member 'X::_n' in read-only object` | Writing to a non-mutable member from a const member fn | Add `mutable` if it's a cache; otherwise drop `const` from the function |
| `'mutable' is incompatible with 'const'` | `mutable const int x;` | Pick one — `mutable` and `const` together are nonsense |
| Crash / SIGSEGV when writing a mutable member of a `const` global | The object is in `.rodata`; OS rejects the write | Make the global non-`const`, or move the cache out |

---

## 8. Visual summary

```
                          class X {
                              int       _a;        ← honor const, observable
                              mutable int _b;      ← const exception, internal
                          };

                                 │
            void f() const  ─────┘
                 │
                 │   compiler view of *this:
                 │   ┌────────────────────────────────────┐
                 ▼   │   _a:   const int   (cannot write) │
                     │   _b:         int   (can write)    │
                     └────────────────────────────────────┘

                   "logical const" but with
                  internal state allowed to mutate
```

---

## 9. Practice

1. Why is `mutable static int n;` rejected? *(Static members aren't per-instance, so the const-this exception doesn't apply.)*
2. Could you implement a thread-safe `const` getter without `mutable`? *(Yes — make the mutex external, or pass it in. But `mutable` is the standard idiom.)*
3. Will writing to a `mutable` member of a global `const X g;` work at runtime? *(Sometimes — depends on whether the linker placed `g` in `.rodata`. If it did, you'll SIGSEGV. Don't combine `const` global + `mutable` member.)*
