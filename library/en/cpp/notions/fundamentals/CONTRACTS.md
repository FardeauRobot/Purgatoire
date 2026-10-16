# 🤝 Contracts — The Hidden Backbone of C++

> **Keyword zoom-ins:** [`CONST`](../../lexique/CONST.md) · [`EXPLICIT`](../../lexique/EXPLICIT.md) · [`PUBLIC/PRIVATE/PROTECTED`](../../lexique/PUBLIC_PRIVATE_PROTECTED.md) · [`VIRTUAL`](../../lexique/VIRTUAL.md) · [`STATIC`](../../lexique/STATIC.md)
>
> **Topic crossroads:** [`BASICS`](BASICS.md) · [`MEMORY`](MEMORY.md) · [`oop/ORTHODOX_CANONICAL_FORM`](../oop/ORTHODOX_CANONICAL_FORM.md) · [`oop/POLYMORPHISM`](../oop/POLYMORPHISM.md) · [`io-errors/ERROR_MANAGEMENT`](../io-errors/ERROR_MANAGEMENT.md)

---

## TL;DR

A **contract** is a promise between two parties — the caller and the callee, the class and its users, you and the compiler. C++ is the language where the most contracts are **enforced at compile time**, before your program ever runs. Once you read C++ as a stack of contracts, the language stops feeling random and starts feeling deliberate.

```
   ┌─────────── CALLER ───────────┐         ┌─────────── CALLEE ───────────┐
   │  I PROMISE to honor your     │ ──────► │  I PROMISE the documented    │
   │  preconditions               │         │  behavior + postconditions   │
   │  I EXPECT  the result        │ ◄────── │  I EXPECT  valid arguments   │
   └──────────────────────────────┘         └──────────────────────────────┘
                       break it  ⇒  UB  /  compile error  /  b≥ug
```

---

## 1. What is a contract?

In software, a contract is the **complete agreement** between two pieces of code about how they cooperate. It has three parts:

| Part | Means | Example |
|---|---|---|
| **Preconditions** | What must be true *before* you call the function | "the pointer is not null" |
| **Postconditions** | What will be true *after* the function returns | "the vector contains the new element" |
| **Invariants** | What stays true the whole time | "size never exceeds capacity" |

This idea is older than C++ — Bertrand Meyer formalized it in the 80s as **Design by Contract**. C++ adopted it through *syntax*: instead of writing English comments, you encode the contract in the code itself, and the compiler does the policing.

---

## 2. Why C++ leans on contracts so hard

C and C++ both let you express dangerous things. The difference is:

```
   C:   "Here's a footgun. Try not to shoot yourself."
   C++: "Here's a footgun, plus 50 ways to ask the compiler
         to refuse to let you fire it in the wrong direction."
```

Most C++ syntax that looks like decoration (`const`, `explicit`, `private`, `virtual`, `noexcept`...) is actually a **contract you sign with the compiler**. In return, the compiler:

1. Refuses to compile code that breaks the contract.
2. Generates faster code because it can assume the contract holds.
3. Documents intent for the next human reader.

Java has runtime checks (`NullPointerException`). C++ has compile-time refusals plus undefined behavior — fast when you're right, brutal when you're wrong. **Contracts are how you stay on the right side of that line.**

---

## 3. The contracts hiding in plain syntax

A panoramic view. Each row is a real contract you sign or accept dozens of times a day.

| Syntax | The contract | Who's protected |
|---|---|---|
| `const T x` | "I will not reassign x." | Future-you, reviewer |
| `const T& param` | "I will not modify what you lent me." | Caller |
| `T method() const` | "Calling this won't change the object." | Caller, optimizer |
| `private:` | "Outside code may not touch this." | The class's invariants |
| `protected:` | "Subclasses may, strangers may not." | The hierarchy |
| `public:` | "This is part of my published interface." | Users |
| `virtual` | "Subclasses may override this." | Polymorphic call sites |
| `= 0` (pure virtual) | "Subclasses **must** implement this." | The abstraction |
| `explicit` ctor | "Don't call me via implicit conversion." | Caller, future-you |
| `noexcept` | "I will never throw." | Stack-unwind machinery |
| `static_cast<T>` | "I checked, this conversion is safe." | The reader |
| `dynamic_cast<T*>` | "Check at runtime; null on failure." | Polymorphism users |
| `T*` returned by `new` | "Caller now owns this and must `delete`." | The heap |
| Reference `T&` | "Bound, non-null, never reseated." | Anyone using it |
| Type system itself | "If shapes line up, types match." | Everyone |
| **Undefined behavior** | "Caller promises: no null-deref, no OOB, no overflow." | The optimizer |

---

## 4. Four scales of contract

```
                 ┌──────────────────────────┐
                 │   1. FUNCTION contract   │  smallest, per-call
                 │     parameters/return    │
                 └────────────┬─────────────┘
                              │
                 ┌────────────▼─────────────┐
                 │    2. CLASS contract     │  per-object lifetime
                 │   public API + invariants│
                 └────────────┬─────────────┘
                              │
                 ┌────────────▼─────────────┐
                 │  3. HIERARCHY contract   │  base promises kept by all
                 │   virtual + Liskov       │
                 └────────────┬─────────────┘
                              │
                 ┌────────────▼─────────────┐
                 │  4. LANGUAGE contract    │  you ↔ compiler
                 │  type system + UB rules  │
                 └──────────────────────────┘
```

### 4.1 Function-level

```cpp
// Pre:  v is non-empty
// Post: returns the smallest element
// Inv:  v is unchanged
int min(const std::vector<int>& v);
```

Reading the signature alone tells you most of it: `const std::vector<int>&` says "I won't modify it and I won't copy it." Only the "non-empty" precondition needs a comment.

### 4.2 Class-level

A class's contract is its **public interface plus its invariants**. The class promises invariants stay true; users promise to only touch it through the public API.

```cpp
class Stack {
    int *data_;
    size_t size_, cap_;
public:
    void  push(int x);     // public contract
    int   pop();
    size_t size() const;
    // Invariants (private contract with itself):
    //   1. 0 <= size_ <= cap_
    //   2. data_ owns cap_ ints (or is null when cap_ == 0)
};
```

If a user *could* poke `data_` directly, they could break invariant #2 and your destructor would `delete` garbage. `private:` is the lock; the invariant is what it protects.

### 4.3 Hierarchy-level (Liskov Substitution Principle)

Any function that works on a `Base*` must keep working when given a `Derived*`. Subclasses can **strengthen postconditions** and **weaken preconditions**, but never the other way.

```cpp
class Bird           { public: virtual void fly(); };
class Penguin : Bird { public: void fly() override { throw "no!"; } };
                                                   // 💥 contract broken
```

A function that calls `bird->fly()` was promised it works. Penguin breaks the deal. The fix: redesign the hierarchy (`Bird` shouldn't promise `fly`).

### 4.4 You ↔ the compiler

The deepest contract. The C++ standard is a list of things you promise not to do (UB) in exchange for performance.

```cpp
int *p = nullptr;
*p = 42;          // YOU broke the contract
                  // compiler is allowed to do anything — including nothing
```

UB is not "the program crashes." It's "the program is allowed to do anything," because the compiler optimized as if you would never do that.

---

## 5. The cleanest example, line by line

Four words, four contracts:

```cpp
int sum(const std::vector<int>& v);
//   ↑       ↑           ↑     ↑
//   │       │           │     └── caller: "you lend me this vector"
//   │       │           └──────── callee: "I won't COPY it (& = cheap)"
//   │       └──────────────────── callee: "I won't MODIFY it (const)"
//   └──────────────────────────── callee: "I'll return an int"
```

Every promise is enforced by the compiler:
- Try to modify `v` inside `sum` → compile error.
- Pass a `std::list<int>` → compile error.
- Forget to return an `int` → compile error.

That's why C++ programmers obsess over signatures. The signature *is* the contract.

---

## 6. The Orthodox Canonical Form: a lifecycle contract

📚 [`oop/ORTHODOX_CANONICAL_FORM.md`](../oop/ORTHODOX_CANONICAL_FORM.md)

When you write the four canonical functions, you're signing the lifecycle contract:

```
   ┌──────────────────────────────────────────────────────────┐
   │  1. Default constructor   "I can be born from nothing."  │
   │  2. Copy constructor      "I can be cloned safely."      │
   │  3. Copy assignment       "I can be replaced safely."    │
   │  4. Destructor            "I clean up after myself."     │
   └──────────────────────────────────────────────────────────┘
```

If you skip one and the compiler synthesizes a wrong default (shallow copy of an owned pointer, for instance), you've made a promise you can't keep — and the bug surfaces miles from where it was caused.

---

## 7. Contracts and ownership

📚 [`fundamentals/MEMORY.md`](MEMORY.md)

`new` and `delete` are an **ownership contract** between caller and callee:

```cpp
Zombie* spawn();        // returns a heap pointer
                        // contract: caller must delete the result
```

Nothing in the type system enforces this in C++98 — it's expressed in the **name and documentation**. That's exactly why ownership bugs are common in C and were a primary motivator for smart pointers in modern C++. In C++98 you compensate with discipline:

- Document who owns every pointer that crosses a function boundary.
- Prefer **values and references** over pointers when ownership doesn't transfer.
- Pair every `new` with a clear destructor that runs (RAII).

---

## 8. Contracts and exceptions

📚 [`io-errors/ERROR_MANAGEMENT.md`](../io-errors/ERROR_MANAGEMENT.md)

Exceptions exist for **broken contracts the local code can't fix**.

```cpp
int Stack::pop() {
    if (size_ == 0)
        throw std::out_of_range("pop on empty stack");
    return data_[--size_];
}
```

The function's precondition was "stack is non-empty." Caller broke it. Throwing is the loud way of saying "your half of the contract failed; I refuse to silently produce garbage."

The four **exception safety guarantees** are themselves contracts the function offers:

| Level | Contract |
|---|---|
| **No-throw** | "I won't throw, ever." |
| **Strong** | "I either succeed or leave the world unchanged." |
| **Basic** | "I won't leak or corrupt — but state may differ." |
| **None** | "I make no promises. Avoid me." |

---

## 9. How to *read* C++ as contracts

When a line looks confusing, parse it as a contract. Three quick wins:

```cpp
void log(const std::string& msg) const noexcept;
//        ──────────────────────  ─────  ────────
//        won't modify msg        won't  won't throw
//                                modify
//                                *this
```

Parse from the outside in: each qualifier is one promise. The signature alone tells you what's safe to do, what's safe to assume, and what guarantees you can rely on at the call site.

---

## 10. How to *write* C++ as contracts

Three habits that compound:

1. **Default to `const`.** Strip it only when you genuinely need to mutate. Most parameters, most methods, most variables → `const`.
2. **Make the public API minimal and the private invariants explicit.** Comment the invariant once at the top of the class. Every method either preserves or restores it.
3. **Trust signatures, validate boundaries.** Inside your codebase, trust that callers honor the contract. At the edges (user input, file I/O, network) — validate, then trust.

```
   ┌────────────────────── BOUNDARY ──────────────────────┐
   │ validate here  │ trust everywhere   │ validate here  │
   │ (user, file)   │ inside             │ (network out)  │
   └──────────────────────────────────────────────────────┘
```

Inside the trust zone, contracts replace defensive code. That's how C++ code stays both fast and correct.

---

## 11. Common contract bugs (and how to spot them)

| Smell | The broken contract | The fix |
|---|---|---|
| `delete` called twice | Two owners → both think they're responsible | One owner; the rest hold non-owning pointers |
| Use-after-free | Reader assumed lifetime ≥ usage | Tie lifetime to a scope (RAII) |
| `const` cast away | Promise made then revoked | Don't lie — make it non-const if it must mutate |
| Slicing | Base contract was kept, but derived parts vanished | Pass by reference/pointer, not by value |
| Missing `override` | Subclass *thought* it overrode but didn't | Use `override` (C++11) or check signatures carefully in C++98 |
| Unhandled exception across ABI | `noexcept` contract violated | Wrap the boundary in `try/catch` |

---

## 12. Visual summary

```
                  ┌────────────────────────────────┐
                  │      Every C++ symbol is       │
                  │       a contract somewhere     │
                  └────────────────┬───────────────┘
                                   │
        ┌──────────────────────────┼──────────────────────────┐
        │                          │                          │
   ┌────▼─────┐              ┌─────▼─────┐              ┌─────▼─────┐
   │  const   │              │  private  │              │  virtual  │
   │ "I won't │              │  "Hands   │              │ "Override │
   │ change"  │              │   off."   │              │  if you   │
   └──────────┘              └───────────┘              │  wish."   │
                                                        └───────────┘
        │                          │                          │
   ┌────▼─────┐              ┌─────▼─────┐              ┌─────▼─────┐
   │ explicit │              │  ~Class() │              │   = 0     │
   │ "Don't   │              │ "I'll     │              │ "Override │
   │ convert  │              │  clean up │              │  you must"│
   │ secretly"│              │  myself." │              └───────────┘
   └──────────┘              └───────────┘

         All of them: enforced at compile time → no runtime cost.
```

---

## 13. Practice questions

1. What contract does `int& at(size_t i) const;` make? What does the `const` apply to — the reference, the parameter, or the method?
2. A function returns `std::string&`. What invariant must the caller assume? Where could this go wrong?
3. You see `void f(T* p);` with no comment. What three different ownership contracts could be in play, and how would you decide which?
4. Why does undefined behavior exist instead of having the standard mandate "this should crash"?
5. A class has 12 setters and 12 getters and no invariants — what contract is missing, and what does that mean for the class?

> Answers in your head, not on paper. If three out of five feel fuzzy, re-read sections 3–5 — that's where the leverage is.
