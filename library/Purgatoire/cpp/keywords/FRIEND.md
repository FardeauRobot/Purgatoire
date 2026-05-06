# `friend` — A Trapdoor Through Access Control

> **TL;DR.** `friend` declarations grant a specific function or class access to your `private` and `protected` members. It is **not transitive**, **not inherited**, and **not symmetric**. Use sparingly — typically for non-member operator overloads.

Related: [`PUBLIC_PRIVATE_PROTECTED.md`](PUBLIC_PRIVATE_PROTECTED.md) · [`OPERATOR_OVERLOADING.md`](../oop/OPERATOR_OVERLOADING.md) · [`CLASS.md`](CLASS.md)

---

## 1. The motivation — `operator<<`

```cpp
class Account {
    int _balance;
public:
    Account(int b) : _balance(b) {}
};

std::ostream& operator<<(std::ostream& os, const Account& a) {
    os << "Balance: " << a._balance;        // ERROR — _balance is private
    return os;
}
```

`operator<<` cannot be a member function of `Account` (the left operand is `std::ostream`, not `Account`). It must be a free function. But free functions can't see `_balance`.

The fix:

```cpp
class Account {
    int _balance;
    friend std::ostream& operator<<(std::ostream&, const Account&);
public:
    Account(int b) : _balance(b) {}
};

std::ostream& operator<<(std::ostream& os, const Account& a) {
    return os << "Balance: " << a._balance;     // now legal
}
```

`friend` says: "this function may name my private members." Nothing more, nothing less.

---

## 2. Three forms

### 2.1 Friend function — declared inside the class

```cpp
class Account {
    int _balance;
    friend std::ostream& operator<<(std::ostream&, const Account&);
};
```

The function is **not** a member of `Account`. It's a free function with permission to peek inside.

### 2.2 Friend class

```cpp
class Account {
    int _balance;
    friend class Bank;       // every member function of Bank can access Account's privates
};

class Bank {
public:
    void audit(const Account& a) {
        std::cout << a._balance;     // OK
    }
};
```

This is a heavier hammer — *every* method of `Bank` gets full access. Use it when two classes are tightly coupled (typically a class and its iterator).

### 2.3 Friend member function

```cpp
class Account;                      // forward decl

class Bank {
public:
    void audit(const Account& a);
};

class Account {
    int _balance;
    friend void Bank::audit(const Account&);    // only this one method of Bank
};
```

The most surgical option. Grants access to one specific method of one specific class.

---

## 3. The four facts about `friend`

```
              ┌─────────────────────────────────────────────┐
              │ 1. NOT a member.                             │
              │    A friend function isn't a member of      │
              │    the granting class. No 'this' pointer.   │
              ├─────────────────────────────────────────────┤
              │ 2. NOT transitive.                           │
              │    A's friend B does not become a friend     │
              │    of B's other friends.                     │
              ├─────────────────────────────────────────────┤
              │ 3. NOT symmetric.                            │
              │    A says "B is my friend" — that's one-way.│
              │    B does not get to read A's privates      │
              │    unless A says so.                        │
              ├─────────────────────────────────────────────┤
              │ 4. NOT inherited.                            │
              │    Subclasses of A do not inherit A's       │
              │    friendships (and vice versa).            │
              └─────────────────────────────────────────────┘
```

These rules exist because friendship is meant to be a deliberate, narrow exception — not a backdoor for general access.

---

## 4. Hardware/compiler view

`friend` is purely a **type-system mechanism**, like `private`. The compiler:

- Records the friendship at parse time.
- Skips access checks when a friend names a private/protected member.
- Generates **no extra code, no extra metadata, no runtime cost**.

```
   class Account {
     int _balance;
     friend f();
   };
   void f(Account &a) { a._balance = 100; }

   compiler:
     "f names a._balance. Is f a member? no.
      Is _balance accessible to f? yes — f is a friend.
      Allow."
   
   emitted machine code: identical to a public member access.
```

---

## 5. The two correct uses

### 5.1 Non-member operator overloads

The classic case:

```cpp
class Vec3 {
    float _x, _y, _z;
public:
    Vec3(float x, float y, float z) : _x(x), _y(y), _z(z) {}

    friend Vec3 operator+(const Vec3& a, const Vec3& b);
    friend std::ostream& operator<<(std::ostream& os, const Vec3& v);
};

Vec3 operator+(const Vec3& a, const Vec3& b) {
    return Vec3(a._x + b._x, a._y + b._y, a._z + b._z);
}

std::ostream& operator<<(std::ostream& os, const Vec3& v) {
    return os << '(' << v._x << ", " << v._y << ", " << v._z << ')';
}
```

Why a free function? So `2.0f * vec` can work — the left operand has to be the one-with-a-conversion-from. Member operators can't be flexible about their left operand.

### 5.2 Tightly coupled types — the iterator pattern

```cpp
class List {
    struct Node { int value; Node *next; };
    Node *_head;
    friend class Iterator;
public:
    class Iterator {
        Node *_node;
    public:
        Iterator(Node *n) : _node(n) {}
        int value() const { return _node->value; }
        Iterator& operator++() { _node = _node->next; return *this; }
    };

    Iterator begin() { return Iterator(_head); }
};
```

`Iterator` needs to walk through `Node*`s — which are private to `List`. Making `Iterator` a friend (or, equivalently, a nested class — nested types automatically have access to enclosing privates) is the norm.

---

## 6. The bad uses (don't)

### 6.1 "I just want this function to access privates"

If a free function regularly needs to see private state, ask whether it should be a member function. `friend` is for cases where it *cannot* be (operators on the wrong side, two-class coupling) — not because writing a getter feels like extra typing.

### 6.2 "Friend my whole namespace"

You can't, and you shouldn't want to. Friendship is per-name.

### 6.3 Mass friending

```cpp
class A {
    friend class B;
    friend class C;
    friend class D;        // smell
    friend class E;
    friend class F;
};
```

If five classes need internals, your encapsulation is wrong. Refactor — a shared interface, a public read-only view, etc.

---

## 7. Forward declarations and friends

If you friend a class, the granting class doesn't need to know its full definition:

```cpp
class Bank;                          // forward decl is enough

class Account {
    int _balance;
    friend class Bank;
};
```

If you friend a specific member function, you need the full definition of that member's class first:

```cpp
class Bank;                          // forward decl insufficient
void Bank::audit(const Account&);    // needs Bank's definition
```

In practice, declare classes in dependency order or use forward declarations carefully.

---

## 8. `friend` in a template

```cpp
template <typename T>
class Box {
    T _value;
    friend std::ostream& operator<<(std::ostream& os, const Box<T>& b) {
        return os << b._value;       // defined inline, needs no separate forward decl
    }
};
```

This is a common pattern: define the friend `operator<<` *inside* the template's body. Each template instantiation gets its own friend.

If you want to declare the friend separately, you need template machinery that gets a bit involved — for 42 modules, the inline form above is fine.

---

## 9. Tips & tricks

### 9.1 Default to non-friend

Every time you reach for `friend`, ask: "could this be a public member instead?" Often yes.

### 9.2 Friend declarations don't care about access labels

```cpp
class A {
private:
    friend class B;            // works
public:
    friend class C;            // also works — same effect
};
```

The compiler treats `friend` declarations identically regardless of which section they're in. Convention is to put them at the top of the class definition or right next to the related members, depending on style.

### 9.3 Friendship is not inherited — both ways

```cpp
class Base { friend class Buddy; };
class Derived : public Base {};

class Buddy {
    void f(Derived &d) {
        // can access Base's privates inherited by Derived (since Buddy is Base's friend)
        // CANNOT access Derived's own privates — Derived didn't grant friendship
    }
};
```

And:

```cpp
class A { int x; friend class B; };
class B {};
class C : public B {};               // C is NOT a friend of A
```

### 9.4 Stream operator pattern in 42

```cpp
// Account.hpp
class Account {
    int _balance;
public:
    Account(int b);

    int  getBalance() const;       // public getter
};

std::ostream& operator<<(std::ostream& os, const Account& a);  // free function decl
```

```cpp
// Account.cpp
std::ostream& operator<<(std::ostream& os, const Account& a) {
    return os << "Balance: " << a.getBalance();    // uses the public getter — no friend needed
}
```

If you have a public getter, you don't need `friend`. Some 42 evaluators prefer this — friendship-free, with the operator using the public interface.

If you don't have a getter (and don't want one), use `friend`.

### 9.5 Friend functions defined inside the class are still free functions

```cpp
class Box {
    int _v;
public:
    friend bool operator==(const Box& a, const Box& b) {     // body inline
        return a._v == b._v;
    }
};
```

`operator==` is **not** a member of `Box` even though defined here. It's only **found** via argument-dependent lookup when at least one operand is a `Box`. This is fine and idiomatic.

---

## 10. Common errors

| Error | Cause | Fix |
|---|---|---|
| `'_x' is private` from inside `operator<<` | Friend declaration missing or misspelled | Add `friend` declaration in the class |
| `friend declaration ... not a member of any class` | Wrote `friend void f();` outside a class | Friendship can only be granted from inside a class definition |
| Inheritance doesn't grant friend | Tried to access base privates from a subclass that thought it inherited friendship | Friendship is not inherited; refactor |
| Linker error on a friend function | Declared the friend but never defined it | Provide the definition in a `.cpp` (or inline in the class) |

---

## 11. Visual summary

```
                  ┌────────────────────────────────────────┐
                  │     class Granter {                     │
                  │         int _secret;                    │
                  │                                         │
                  │         friend class Receiver;          │  ─── permission
                  │         friend void f(...);             │
                  │     };                                  │
                  └─────────────────────┬───────────────────┘
                                        │
                                        ▼
                  ┌────────────────────────────────────────┐
                  │     class Receiver {                    │
                  │         void f(Granter& g) {            │
                  │             g._secret = 42;             │  ✓ allowed
                  │         }                               │
                  │     };                                  │
                  └────────────────────────────────────────┘

                  ✗ NOT inherited      ✗ NOT transitive
                  ✗ NOT symmetric     ✗ NOT a member
```

---

## 12. Practice

1. Why must `operator<<` for a custom class be a free function (typically a friend)? *(Its left operand is `std::ostream`; the function can't be a member of the user's class because `*this` would be a stream.)*
2. If `class A { friend class B; }; class A2 : public A {};` — does `A2` grant `B` friendship? *(No — friendship is not inherited.)*
3. If `friend bool operator==(const Box&, const Box&) { ... }` is defined inside `Box`, is it a member of `Box`? *(No — it's a free function. Just defined inside Box's body for convenience.)*
4. When could using a public getter eliminate the need for `friend`? *(Whenever the friend function only needs read access. Public getter + free function = no friendship needed.)*
