# Getters and Setters in C++ — Complete Guide

> **Keyword zoom-ins:** [`PUBLIC/PRIVATE/PROTECTED`](../../lexique/PUBLIC_PRIVATE_PROTECTED.md) · [`CONST`](../../lexique/CONST.md) · [`THIS`](../../lexique/THIS.md) · [`MUTABLE`](../../lexique/MUTABLE.md) · [`EXPLICIT`](../../lexique/EXPLICIT.md)

## Table of Contents
1. [What are getters and setters?](#What%20are%20getters%20and%20setters%3F)
2. [The mental model: encapsulation and class invariants](#The%20mental%20model%3A%20encapsulation%20and%20class%20invariants)
3. [Minimal example](#Minimal%20example)
4. [Anatomy of a good getter](#Anatomy%20of%20a%20good%20getter)
   - [Return by value, reference, or const reference?](#Return%20by%20value%2C%20reference%2C%20or%20const%20reference%3F)
   - [The trailing `const`](#The%20trailing%20const)
   - [Computed (derived) getters](#Computed%20%28derived%29%20getters)
5. [Anatomy of a good setter](#Anatomy%20of%20a%20good%20setter)
   - [Pass parameters by const reference](#Pass%20parameters%20by%20const%20reference)
   - [Validation strategies](#Validation%20strategies)
6. [The `const` story (deep dive)](#The%20const%20story%20%28deep%20dive%29)
7. [When NOT to write a getter/setter](#When%20NOT%20to%20write%20a%20getter%2Fsetter)
8. [Common pitfalls](#Common%20pitfalls)
9. [Naming conventions](#Naming%20conventions)
10. [Worked example — PhoneBook `Contact`](#Worked%20example%20%E2%80%94%20PhoneBook%20Contact)
11. [Decision tree / cheat sheet](#Decision%20tree%20%2F%20cheat%20sheet)

---

## What are getters and setters?

A **getter** and a **setter** are public member functions whose only job is to
read or write a private data member. They are the *controlled doors* between
the outside world and the internals of a class.

```
Outside world  ──► setter()  ──► private member
Outside world  ◄── getter()  ◄── private member
```

In C, you read and write struct fields directly. In idiomatic C++, fields
are `private` and accessed only through methods. Getters and setters are the
simplest case of that pattern.

> **In short:** the class owns its data; getters and setters are how it
> chooses to expose that data to callers.

---

## The mental model: encapsulation and class invariants

Encapsulation is not just "make the field private". The real reason for it
is **class invariants** — properties of the object that must hold at all
times for the object to be valid.

Examples:
- `Date` — month must be `1..12`, day must be valid for that month.
- `Account` — balance must equal sum of past deposits minus withdrawals.
- `Contact` (PhoneBook) — phone number must contain only digits.

If the field is `public`, *any* code anywhere can break the invariant. The
class can no longer guarantee anything about itself, because it has no
say in who writes what.

If the field is `private` and modified only via a setter, the setter
**enforces** the invariant on every write. The class becomes *self-defending*.

> **Why "getter/setter" and not "public field"?** Because `public field`
> means "no invariant possible". The cost of a getter/setter pair is one
> level of indirection; the benefit is that the class controls its own
> validity. For non-trivial fields, the trade is almost always worth it.

---

## Minimal example

```cpp
class Contact {
private:
    std::string m_name;          // private — outside code cannot touch this

public:
    void               setName(const std::string& name) { m_name = name; }
    const std::string& getName() const { return m_name; }
};
```

```cpp
Contact c;
c.setName("Alice");                     // goes through the setter
std::cout << c.getName() << std::endl;  // goes through the getter
// c.m_name = "Bob";                    // ERROR — private, won't compile
```

Three tiny details in this example are doing real work — all explained in
the next two sections:

1. The setter takes its parameter `const std::string&` (const reference).
2. The getter returns `const std::string&` (const reference).
3. The getter is marked `const` after the parameter list.

---

## Anatomy of a good getter

### Return by value, reference, or const reference?

This is the single most important decision when writing a getter. The
"right" answer depends on the type of the member.

```cpp
//  A) Return BY VALUE  — caller gets a copy
std::string getName() const { return m_name; }

//  B) Return BY CONST REFERENCE — caller gets a read-only handle, no copy
const std::string& getName() const { return m_name; }

//  C) Return BY NON-CONST REFERENCE — caller can modify the member directly
std::string& getName() { return m_name; }   // (almost always wrong)
```

| Form | When to use | Cost |
|------|-------------|------|
| `T` (by value) | Primitives (`int`, `bool`, `char`, ...), or when the value is **computed** and doesn't outlive the call. | Cheap for primitives, a copy for heavy types. |
| `const T&` | Heavy types (`std::string`, containers, custom classes) where the member already exists in the object. | Zero copy. The reference is valid as long as the object is. |
| `T&` | **Almost never.** Hands the caller a leash to mutate your private member with no validation. Defeats encapsulation. Acceptable only if the class deliberately exposes mutable internals (e.g., `std::vector::operator[]`). |

> **Rule of thumb:** for `int`/`bool`/`char` → return by value.
> For `std::string`/containers → return by `const T&`.
> Return by non-const reference only when you really mean "this field is
> public; I'm just spelling it `obj.foo()` instead of `obj.foo`".

### The trailing `const`

```cpp
const std::string& getName() const { return m_name; }
//                            ^^^^^
//   "this method does not modify *this"
```

This `const` on the method has two effects:

1. The compiler **enforces** that the method does not modify any non-`mutable`
   field. If you accidentally write `m_name = "x";` in the body, it fails to
   compile. It's a contract checked at compile time.
2. It makes the method **callable on `const` objects**:

   ```cpp
   void print(const Contact& c) {
       std::cout << c.getName();   // works ONLY if getName() is const
   }
   ```

   If `getName()` were not marked `const`, the call would fail because
   `c` is `const Contact&` — a `const` object can only call `const` methods.

**Always mark getters `const`.** A getter that doesn't is either lying about
not modifying state, or it'll silently exclude itself from being usable on
`const` objects (which means `const` references to your class become useless).

### Computed (derived) getters

A getter doesn't have to return a stored field — it can compute the value:

```cpp
class Person {
private:
    std::string m_firstName;
    std::string m_lastName;

public:
    std::string getFullName() const {           // ← BY VALUE, not by reference
        return m_firstName + " " + m_lastName;
    }
};
```

> **Pitfall:** *never* return a `const T&` to a value computed inside the
> function. The temporary that holds the concatenation is destroyed at the
> end of the function, and the caller would receive a reference to garbage:
>
> ```cpp
> const std::string& getFullName() const {                 // ⚠ DANGLING
>     return m_firstName + " " + m_lastName;               //   temporary dies here
> }
> ```
>
> Computed values must be returned **by value**. The compiler usually
> elides the copy (return value optimisation), so the cost is often zero.

---

## Anatomy of a good setter

### Pass parameters by const reference

```cpp
void setName(const std::string& name) { m_name = name; }
//           ^^^^^^^^^^^^^^^^^^^
```

For any non-trivial type, take the parameter by `const T&`:
- `const` — the setter promises not to modify the caller's argument.
- `&` — the argument is not copied on entry; the function works directly on
  the caller's object.

For primitives (`int`, `bool`, `char`, pointers), pass by value — references
to a register-sized value are no faster than the value itself.

### Validation strategies

This is where getter/setter style earns its keep. A setter is the
choke-point through which all writes flow — it's the natural place to
enforce the class invariant.

There are four common ways to handle a bad input:

```cpp
// 1. SILENT REJECTION — caller has no idea the call was a no-op.
//    Easy but error-prone; useful for "best-effort" updates.
void setAge(int age) {
    if (age < 0 || age > 150) return;
    m_age = age;
}

// 2. RETURN A BOOL — caller chooses how to react.
//    Common in C++98 code that wants to avoid exceptions.
bool setAge(int age) {
    if (age < 0 || age > 150) return false;
    m_age = age;
    return true;
}

// 3. THROW ON BAD INPUT — caller must handle or propagate.
//    Best when "wrong" is genuinely exceptional; aligns with std-lib style.
void setAge(int age) {
    if (age < 0 || age > 150)
        throw std::invalid_argument("Contact: age out of range");
    m_age = age;
}

// 4. PRECONDITION CONTRACT — no run-time check, doc says "must be in [0,150]".
//    Cheapest; suitable when the caller is always trusted code.
void setAge(int age) {
    m_age = age;   // UB if precondition violated
}
```

There is no single right answer — pick one and **be consistent across the
class**. For 42 piscine projects, options (1) and (3) are both common:
silent reject for things like a UI input loop, throw for programmer errors.

> **Useful pattern:** when several setters share validation logic, factor
> the check into a private helper (`bool isValidName(const std::string&)
> const`) and call it from both the setter and any constructor that
> accepts the same field.

---

## The `const` story (deep dive)

`const` is everywhere in C++ getter/setter code. Worth slowing down.

```cpp
class Foo {
public:
    int                f1() const;            // method is const
    void               f2(const int& x);      // parameter is const reference
    const std::string& f3() const;            // return type is const ref, method is const
private:
    int m_x;
};
```

Three independent uses of `const`, each on a different "slot":

| Slot | Meaning |
|------|---------|
| After the parameter list (`f1() const`) | The method does not modify `*this`. |
| Before a parameter type (`const int& x`) | The method does not modify the caller's argument. |
| Before the return type (`const std::string&`) | The caller cannot modify what the return reference points to. |

### `const` overloading

You can have **two getters with the same name**, one `const` and one not:

```cpp
class Buffer {
public:
          int& operator[](size_t i)       { return m_data[i]; }
    const int& operator[](size_t i) const { return m_data[i]; }
private:
    int m_data[100];
};
```

The compiler picks the right overload based on whether the object is
`const`. On a non-const `Buffer`, you get the mutable reference; on a
`const Buffer`, you get the read-only one. This is exactly how
`std::vector::operator[]` works.

### Logical vs bitwise `const`

`const` on a method is "logical" — it's *checked* by the compiler at the
bitwise level (no fields can be assigned), but you can opt out with
[`mutable`](../../lexique/MUTABLE.md) for fields like caches or mutexes that
don't represent the object's observable state.

```cpp
class Cache {
public:
    int compute() const {
        if (!m_done) { m_value = expensive(); m_done = true; }
        return m_value;
    }
private:
    mutable int  m_value;   // can be modified inside const methods
    mutable bool m_done;
};
```

> **Pointer member caveat:** `const` on a method protects `m_ptr` itself
> from being reseated, but **not** what `m_ptr` points to. A `const`
> getter can still call `*m_ptr = 42;`. To prevent that, the pointer must
> be `const T*` or its target must be const.

---

## When NOT to write a getter/setter

This is the part most beginner guides skip, and it's the most important.

If every member of a class has a `getX`/`setX` pair, the class is **anemic**:
it's a bag of data with no behaviour, and the logic that *should* live in
the class has leaked into its callers. Whenever you see this:

```cpp
account.setBalance(account.getBalance() - 50);    // smell
```

…the class isn't doing its job. Behaviour belongs **inside** the class:

```cpp
account.withdraw(50);                              // method does the work
```

The principle is **"tell, don't ask"** — instead of asking the object for
its data, computing something, and writing it back, *tell* the object what
you want it to do.

A healthy class typically has:
- A handful of getters for fields the outside world genuinely needs to read.
- **Few or zero** raw setters; instead, methods named after operations
  (`deposit`, `rename`, `validate`, `addContact`) that mutate state in
  domain-meaningful ways.
- Setters only for fields that are honestly "just data" with no class-level
  invariants tying them to others.

> **Rule of thumb before writing `setX`:** ask "is there a higher-level
> operation here?" If yes, write that operation instead. If genuinely not,
> the setter is fine.

---

## Common pitfalls

| # | Pitfall | Symptom | Fix |
|---|---------|---------|-----|
| 1 | Returning `const T&` to a temporary computed in the function | Use of garbage memory; sometimes works, sometimes crashes | Return by value for computed results |
| 2 | Returning `T&` (non-const) to a private member | Caller bypasses validation; encapsulation lost | Return `const T&` instead |
| 3 | Forgetting `const` on the method | Call fails on `const Contact&` callers; warns about "this is const" | Add `const` after the parameter list |
| 4 | Passing `std::string` by value into setter | One unnecessary copy per call | Use `const std::string&` |
| 5 | Validating in setters but not in the constructor | Class can be built in an invalid state | Apply the same check in both, factor to a private helper |
| 6 | Setter silently rejects bad input, caller assumes it worked | Stale data, confusing bugs | Return `bool`, throw, or assert |
| 7 | Public field "for performance" | Invariant breaks the moment someone else writes to it | Profile first; in practice the cost of `const T&` getter is zero |
| 8 | Writing `setX/getX` for *every* member | Class becomes a bag of data with no behaviour | "Tell, don't ask" — model operations, not fields |

---

## Naming conventions

| Style       | Getter        | Setter        | Notes |
|-------------|---------------|---------------|-------|
| Classic     | `getName()`   | `setName()`   | Most universal; C++/Java tradition |
| Qt-style    | `name()`      | `setName()`   | Favoured in modern C++ where the noun *is* the read |
| 42-style    | `F_GetName()` | `F_SetName()` | Subject-document holdover; you'll see it in the piscine |

For 42 work either is fine. Within a single project, pick one and **stick
to it** — mixing styles is the worst option.

Member naming (private fields):

```cpp
std::string m_name;     // m_ prefix — common in 42 / older C++
std::string _name;      // leading underscore — also common (but reserved at file scope)
std::string name_;      // trailing underscore — Google style
std::string name;       // no prefix — relies on `this->name` for disambiguation
```

The point of the prefix is to distinguish the field from a parameter or
local of the same name (`setName(const std::string& name) { m_name = name; }`).

---

## Worked example — PhoneBook `Contact`

A `Contact` for module 00's PhoneBook, showing all the patterns above in
context.

### `Contact.hpp`

```cpp
#ifndef CONTACT_HPP
# define CONTACT_HPP

# include <string>

class Contact {
private:
    std::string m_firstName;
    std::string m_lastName;
    std::string m_nickname;
    std::string m_phone;
    std::string m_secret;

    bool isPrintable(const std::string& s) const;

public:
    Contact();
    Contact(const std::string& first, const std::string& last,
            const std::string& nick,  const std::string& phone,
            const std::string& secret);
    Contact(const Contact& other);
    Contact& operator=(const Contact& other);
    ~Contact();

    // --- getters: const refs for std::string, all const methods ---
    const std::string& getFirstName() const;
    const std::string& getLastName()  const;
    const std::string& getNickname()  const;
    const std::string& getPhone()     const;
    const std::string& getSecret()    const;

    // --- setters: const-ref params, validation that throws on bad input ---
    void setFirstName(const std::string& name);
    void setLastName (const std::string& name);
    void setNickname (const std::string& nick);
    void setPhone    (const std::string& phone);
    void setSecret   (const std::string& secret);

    // --- a real operation, not a setter ---
    bool isEmpty() const;
};

#endif
```

### `Contact.cpp` (excerpts)

```cpp
#include "Contact.hpp"
#include <stdexcept>
#include <cctype>

bool Contact::isPrintable(const std::string& s) const {
    for (size_t i = 0; i < s.size(); ++i)
        if (!std::isprint(static_cast<unsigned char>(s[i]))) return false;
    return !s.empty();
}

const std::string& Contact::getFirstName() const { return m_firstName; }
const std::string& Contact::getPhone()     const { return m_phone; }
// ... other getters

void Contact::setFirstName(const std::string& name) {
    if (!isPrintable(name))
        throw std::invalid_argument("Contact: first name not printable");
    m_firstName = name;
}

void Contact::setPhone(const std::string& phone) {
    for (size_t i = 0; i < phone.size(); ++i)
        if (!std::isdigit(static_cast<unsigned char>(phone[i])))
            throw std::invalid_argument("Contact: phone has non-digit");
    m_phone = phone;
}

bool Contact::isEmpty() const {
    return m_firstName.empty() && m_lastName.empty()
        && m_nickname.empty()  && m_phone.empty()
        && m_secret.empty();
}
```

Things to notice:
- Every getter is `const`, returns `const std::string&`. Zero copies.
- Every setter takes `const std::string&`, validates, throws on bad input.
- `isPrintable` is `private` and `const` — a helper used by setters.
- `isEmpty()` is **not** a getter — it's a class-level question, computed
  from multiple fields. That's the kind of method "tell, don't ask" leads
  to.

---

## Decision tree / cheat sheet

```
Does outside code need to READ this field?
├── No  → no getter
└── Yes → add getter
         ├── Field is primitive (int, bool, char, ...)?
         │   └── return by value:        T    getX() const;
         ├── Field is heavy (string, container, class)?
         │   └── return by const ref:    const T& getX() const;
         └── Value is COMPUTED (not stored)?
             └── return by value:        T    getX() const;       (NEVER const T&)

Does outside code need to WRITE this field?
├── No  → no setter (the field is set in constructor / not at all)
└── Yes → is there a higher-level operation that should own this write?
         ├── Yes → write THAT operation, not a raw setter
         └── No  → add setter
                  ├── Param: const T& if heavy, T if primitive
                  ├── Validate the new value (or document the precondition)
                  └── Pick one error strategy (silent / bool / throw) and
                      use the same one across the whole class
```

> **One-line summary:** every getter is `const`. Heavy types are returned
> by `const T&`. Computed values are returned by value. Setters validate.
> If you find yourself writing `setX/getX` for every field, ask whether
> the class needs *operations* instead.
