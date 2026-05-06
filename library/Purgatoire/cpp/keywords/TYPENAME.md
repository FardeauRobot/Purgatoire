# `typename` — "Trust Me, This Is A Type"

> **TL;DR.** Two unrelated jobs share this keyword: (1) declare a template type parameter (interchangeable with `class`); (2) tell the compiler that a *dependent name* inside a template refers to a type, not a value.

Related: [`TEMPLATE.md`](TEMPLATE.md) · [`TEMPLATES.md`](../advanced/TEMPLATES.md) · [`STL.md`](../advanced/STL.md)

---

## 1. Job #1 — declare a template parameter

```cpp
template <typename T>            // T is a type parameter
void f(T value);

template <class T>               // identical — 'class' is just an older spelling
void g(T value);
```

In a template parameter list, **`typename` and `class` mean the same thing**. The choice is style. Most modern code prefers `typename` (it doesn't lie about the kind of type — `T` could be a class, an int, a pointer, anything).

---

## 2. Job #2 — disambiguate dependent names

Here's the problem. Inside a template, the compiler is sometimes faced with a name like `T::foo`. Is `foo`:

- A nested type (e.g., `T::iterator` if `T` is `std::vector<int>`)?
- A static member variable / constant?
- A static member function?

Until you instantiate the template, the compiler doesn't know what `T` is — and therefore can't tell. By default, the C++ standard says: **assume it's a value, not a type, unless you use `typename`.**

```cpp
template <typename T>
void f() {
    T::iterator it;          // ERROR — compiler thinks T::iterator is a value
}
```

Tell it explicitly:

```cpp
template <typename T>
void f() {
    typename T::iterator it; // OK — promised: T::iterator is a type
}
```

---

## 3. What is a dependent name?

A *dependent name* is one whose meaning depends on a template parameter. `T` itself is dependent. So is anything reached through `T`:

```cpp
template <typename T>
struct X {
    typename T::value_type     v;        // ✓ — typename needed: type
    typename T::size_type      n;        // ✓
    T::static_constant;                  // would be a value (no typename)
    T t;                                 // ✓ — T is a type already known
};
```

Non-dependent names don't need `typename`:

```cpp
template <typename T>
void f() {
    int x;                               // 'int' is not dependent; no typename needed
    std::vector<int>::iterator i;        // not dependent on T; no typename needed
    typename std::vector<T>::iterator j; // dependent on T; typename required
}
```

The **rule of thumb**: if the name has a template parameter inside the path leading to it, prefix with `typename`.

---

## 4. Worked example — using STL inside a template

```cpp
template <typename Container>
void printAll(const Container& c) {
    typename Container::const_iterator it;       // typename — Container is template param
    for (it = c.begin(); it != c.end(); ++it) {
        std::cout << *it << ' ';
    }
}
```

Without `typename`, the compiler errors:

```
   error: need 'typename' before 'Container::const_iterator' because
          'Container' is a dependent scope
```

Add `typename` before any nested type accessed through a template parameter.

---

## 5. The companion case — `template` as a disambiguator

Just like `typename` for "this is a type," there's a sibling for "this is a member template":

```cpp
template <typename T>
void f(T t) {
    t.template doSomething<int>();   // 'template' tells compiler doSomething is a template
}
```

Without it, the compiler reads `t.doSomething<int>` as `(t.doSomething) < int >` — a comparison, not a template invocation. Rare, but you'll see it in heavy generic code.

---

## 6. Why does the rule exist?

```cpp
template <typename T>
void f() {
    T::foo * p;
}
```

What is this?

- `T::foo` (a type) `* p` (declaring a pointer named `p`)?
- `T::foo` (a value) `*` (multiplied by) `p` (some name)?

Without information, the compiler must pick one. The committee chose: **assume value**. So you must override that with `typename`:

```cpp
typename T::foo *p;        // declares p as a pointer to T::foo (a type)
```

This rule was adopted in 1998 and codified in C++03; you'll see older books written before this rule was widely supported.

---

## 7. Tips & tricks

### 7.1 Sprinkle `typename` whenever you reach into a template parameter for a type

```cpp
template <typename T>
struct Adapter {
    typename T::iterator   begin_;
    typename T::iterator   end_;
    typename T::size_type  size_;
};
```

Better to add unnecessary `typename` than miss a needed one. Most compilers tolerate it.

### 7.2 In C++20+, `typename` is no longer required in some contexts (not in 42 C++98)

The committee relaxed the rule for places where only a type makes sense (return types, member variable type, etc.). C++98/C++17 still requires `typename` in those positions.

### 7.3 Don't use `typename` outside templates

```cpp
typename std::vector<int>::iterator it;   // legal but pointless outside templates
                                           // simpler: std::vector<int>::iterator it;
```

`typename` is only meaningful where ambiguity could exist — inside templates.

### 7.4 Read carefully: `typename T::iterator` vs `typename T::iterator()`

```cpp
typename T::iterator i;       // declare a variable
typename T::iterator i();     // GOTCHA — declares a function returning T::iterator!
typename T::iterator i = T::iterator();   // construct a default value
```

Same "most vexing parse" trap that plagues regular C++.

### 7.5 Templates with templates as parameters

```cpp
template <template <typename> class Container>
void f() {
    Container<int> c;         // OK — Container is a template, so Container<int> is a type
}
```

Note the inner `class` (or `typename` in C++17+). This declares `Container` as a *template template parameter* — a template that accepts a single type. Used in some allocator/policy designs; rare in 42.

---

## 8. Common errors

| Error | Cause | Fix |
|---|---|---|
| `need 'typename' before 'T::iterator'` | Used a nested name from a template param without `typename` | Prepend `typename` |
| `expected ';' before 'p'` (in `T::foo *p`) | Compiler parsed it as multiplication | Use `typename T::foo *p;` |
| `expected primary-expression before 'class'` | Used `template <class T>` outside a template (e.g., in a function call) | Drop it; `class` only goes in template parameter lists |
| `'doSomething' was not declared in this scope` | Member template needs `template` disambiguator | `obj.template doSomething<int>()` |

---

## 9. Visual summary

```
              ┌─────────────────────────────────────────┐
              │  keyword: typename                       │
              │                                          │
              │  two jobs:                                │
              │     1. introduce a type parameter        │
              │        (= 'class' here)                  │
              │     2. disambiguate a dependent name     │
              │        as a type                         │
              └────────────────────┬───────────────────┘
                                    │
                ┌───────────────────┼───────────────────┐
                ▼                                       ▼

       template <typename T>             typename T::iterator it;
       template <class T>                ─────────  ─────────────
       (interchangeable                  required when reaching
        in this position)                through a template param
                                          to a nested type.

       (without it, compiler assumes the dependent name is a value.)
```

---

## 10. Practice

1. Why does `template <class T>` work where `template <typename T>` does? *(They're synonyms in template parameter lists.)*
2. Why is `typename T::iterator i;` needed inside a function template, but not outside? *(`T` is a template parameter; the compiler can't yet tell whether `T::iterator` is a type or a value. `typename` says "type.")*
3. What does `t.template foo<int>()` do that `t.foo<int>()` doesn't? *(In a template, `t.foo<int>` could be parsed as `(t.foo) < int`. The `template` keyword forces interpretation as a member template invocation.)*
4. Could you write `class T::iterator i;`? *(No — `class` is for declaring class types or in template parameter lists, not for disambiguating dependent names.)*
