# `public` / `private` / `protected` — Access Control

> **TL;DR.** Three keywords that label sections of a class. They control **who can see member names** at compile time. There is no runtime enforcement, no extra storage, no slowdown — just a compiler-side promise about who's allowed to touch what.

Related: [`CLASS.md`](CLASS.md) · [`STRUCT.md`](STRUCT.md) · [`FRIEND.md`](FRIEND.md) · [`INHERITANCE.md`](../notions/oop/INHERITANCE.md) · [`GETTERS_SETTERS.md`](../notions/oop/GETTERS_SETTERS.md)

---

## 1. The three labels

```cpp
class C {
public:                          // visible to everyone
    int  publicData;
    void publicFn();

protected:                       // visible to C and to derived classes
    int  protectedData;
    void protectedFn();

private:                         // visible only inside C itself
    int  privateData;
    void privateFn();
};
```

Each label changes the access for **everything that follows** until the next label. You can have any number of labels in any order.

---

## 2. The access matrix

```
                 │   accessed by    │ accessed by      │ accessed by
                 │   the class      │ a derived class  │ outside code
                 │   itself         │ (its subclass)   │ (a stranger)
─────────────────┼──────────────────┼──────────────────┼─────────────────
  public         │   yes            │   yes            │   yes
─────────────────┼──────────────────┼──────────────────┼─────────────────
  protected      │   yes            │   yes            │   no
─────────────────┼──────────────────┼──────────────────┼─────────────────
  private        │   yes            │   no             │   no
```

Memorize this table. Every confusing access error you ever hit is a row in it.

---

## 3. Worked example

```cpp
class Animal {
public:
    void speak() const { _doSound(); }

protected:
    virtual void _doSound() const = 0;          // pure virtual

private:
    int _hunger;                                 // base's own state
};

class Dog : public Animal {
protected:
    void _doSound() const { /* bark */ }         // OK: protected → accessible in subclass
    // can also touch... well, not _hunger (private to Animal).
};

void someoneOutside(const Animal &a) {
    a.speak();          // OK   — public
    // a._doSound();    // ERROR — protected
    // a._hunger;       // ERROR — private
}
```

---

## 4. Compile-time only — there's no security boundary

Access control is enforced **purely by the compiler**. The bytes of a private member are at a known offset and on the same memory page as everything else.

```cpp
class Box {
private:
    int _secret;
};

Box b;
int *p = (int*)&b;
*p = 42;            // perfectly legal; b._secret is now 42 (UB strictly speaking, but
                    //                                       portable on real compilers)
```

So `private` is **not** a security feature. It's a **design** feature — it tells you "the maintainer didn't want you to touch this; if you reach in, you own the consequences."

```
   compile time              runtime
   ────────────              ───────
   class Box {               memory of a Box object:
     int _secret;            ┌─────────────┐
   };                        │  _secret    │   ← regular int, regular memory,
                             └─────────────┘     regular addressable.
   compiler:                                     no checks at runtime.
     "_secret" is private.
     reject any code that
     tries to name it from
     outside Box.
```

---

## 5. The 42 norm class layout

The order in 42 norm is conventionally:

```cpp
class MyClass {
private:
    // member variables (state)
    std::string _name;
    int         _age;

public:
    // constructors / destructor first
    MyClass();
    MyClass(const std::string& name, int age);
    MyClass(const MyClass& other);
    MyClass& operator=(const MyClass& other);
    ~MyClass();

    // accessors next
    const std::string& getName() const;
    int                getAge() const;
    void               setName(const std::string& name);
    void               setAge(int age);

    // other public behavior last
    void greet() const;
};
```

Some codebases flip it (public first, private last). Either is fine. Pick one and be consistent.

---

## 6. `protected` — the inheritance gateway

`protected` is the *least common* of the three. Use it when a derived class needs to read or modify part of the base's state directly:

```cpp
class Animal {
protected:
    std::string _type;          // the type identifier — set by subclass

public:
    Animal() : _type("Animal") {}
    virtual ~Animal() {}
    const std::string& getType() const { return _type; }
};

class Dog : public Animal {
public:
    Dog() { _type = "Dog"; }    // protected access — OK
};
```

If `_type` were private, `Dog` would need a setter on `Animal` — sometimes that's the right answer, sometimes it's busywork. Decide case by case.

### Rule of thumb

- `private` first. Most members.
- `protected` only when a subclass clearly needs it.
- `public` for the class's contract.

A class with everything `protected` is suspicious; you've made the inheritance interface as wide as the public interface.

---

## 7. Inheritance access — `class D : public B` vs `class D : private B`

There's a **second** axis where these keywords appear: the inheritance specifier.

```cpp
class B { public: void f(); protected: void g(); private: void h(); };

class Pub  : public    B { /* … */ };       // standard "is-a" inheritance
class Prot : protected B { /* … */ };       // rare
class Priv : private   B { /* … */ };       // "implemented in terms of"
```

What changes for the **derived class itself**: nothing — `private` members of `B` are still inaccessible from `Pub`, `Prot`, or `Priv`.

What changes for **the rest of the world looking at the derived class**:

| Inheritance | `B::f` (public) becomes… | `B::g` (protected) becomes… | `B::h` (private) becomes… |
|---|---|---|---|
| `: public B`    | public in the derived    | protected in the derived    | inaccessible |
| `: protected B` | protected in the derived | protected in the derived    | inaccessible |
| `: private B`   | private in the derived   | private in the derived      | inaccessible |

`public` inheritance is the **only** one that models "is-a." Use it for polymorphism; the others are advanced/unusual.

42 modules use almost exclusively `: public Base`. Do likewise.

---

## 8. `friend` overrides access (briefly)

A `friend` declaration in class A says: "this function or class can access my private/protected members." See [`FRIEND.md`](FRIEND.md).

```cpp
class Account {
    int _balance;
    friend std::ostream& operator<<(std::ostream&, const Account&);
};
```

`friend` is the only legal way to give an outside name access to private state.

---

## 9. Hardware/compiler view — what these keywords actually generate

Nothing.

```
   class A { public:    int x; };
   class B { private:   int x; };

   sizeof(A) == sizeof(B) == 4
   the layout, alignment, and code generated for A and B are identical.
   the compiler simply REFUSES to compile certain access expressions
   for B that it ACCEPTS for A.
```

There is no per-object metadata, no extra fields, no runtime checks. Pure type-system enforcement.

---

## 10. Tips & tricks

### 10.1 Default to `private`

Make members private; expose them through `public` member functions if needed. This is encapsulation in one sentence. Adding access later is cheap; removing it is a breaking change.

### 10.2 Keep your public surface small

A class is easier to maintain when its public methods are few and well-named. If you find yourself writing `getX/setX` for every member, consider whether the data should be wrapped differently — passing struct, returning a copy, etc. (See [`GETTERS_SETTERS.md`](../notions/oop/GETTERS_SETTERS.md).)

### 10.3 Sections can repeat

```cpp
class C {
public:    void f();
private:   int  _a;
public:    void g();           // legal — but readers prefer one section each
private:   int  _b;
};
```

Avoid this in 42 norm. One `private:` section, one `public:` section, in a consistent order.

### 10.4 `public:` after `class` is implicit for `struct`

```cpp
struct S {
    int x;            // public — no label needed
};
```

You can still add `private:` and `protected:` to a struct if you want some hidden state.

### 10.5 Forward-declared classes have no access info

```cpp
class C;            // forward decl
C *p;               // OK
p->member;          // ERROR — type is incomplete; can't check access
```

You need the full definition before access checks fire.

### 10.6 The "law of Demeter" — don't reach through getters

```cpp
// C++ smell:
phonebook.getContact(0).getAddress().getZip();   // exposing chains of internals

// better:
phonebook.zipOfContact(0);                       // a single intent-revealing method
```

When a method exists only to feed another method, ask whether the chain can collapse into one operation.

### 10.7 Access-control bugs are usually design bugs

If you're tempted to write `friend` for a function in your *own* code (not for `operator<<` etc.), step back: maybe the function should be a member, or maybe the data should be exposed through a different abstraction.

---

## 11. Common errors

| Error | Cause | Fix |
|---|---|---|
| `'x' is private within this context` | Touching private from outside | Add a getter/setter or a `friend` |
| `'x' is protected within this context` | Touching protected from outside (a non-derived caller) | If the access is needed, consider why; otherwise add a getter |
| Slicing surprise: derived-only fields disappear after copy to base | (Not access-related, but adjacent) | Pass by reference, or use virtual cloning |
| `cannot access private member declared in base class` after `class D : private B` | Members of B are not exposed via D's public interface | Use `: public B`, or write public member fns in D that forward |

---

## 12. Visual summary

```
                ┌──────────────────────────────────────────┐
                │  class C {                                │
                │     private:    /* internal only */       │
                │                                            │
                │     protected:  /* + subclasses */        │
                │                                            │
                │     public:     /* + everyone */          │
                │  };                                        │
                └─────────────────────┬──────────────────────┘
                                      │
                ┌─────────────────────┼─────────────────────┐
                ▼                     ▼                     ▼
            visibility            inheritance         no runtime
            of members            mode                cost
            ──────────            ──────────          ──────────
            who can name          how members         enforced at
            X::y in code          look from           compile time
                                  outside the         only.
                                  derived class       memory layout
                                                      is unchanged.
```

---

## 13. Practice

1. Why is `private` not a security boundary? *(It's compile-time enforced; the bytes are still in regular memory and reachable via casts.)*
2. What inheritance mode is "is-a"? *(Public.)*
3. Why does flipping `class C` to `struct C` change the meaning of code that uses `C`'s members? *(Default access changes — what was `private` in `class` is now `public` in `struct`.)*
4. Can you have `private:` then `public:` then `private:` again? *(Legal, but bad style.)*
