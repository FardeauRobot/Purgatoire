# C++ Module 03 — Quiz (15 questions)

> Inheritance, ctor/dtor chains, name hiding, the diamond.

---

### 1. The "is-a" relationship
What does `class ScavTrap : public ClapTrap` say about a `ScavTrap` object's memory layout? (one sentence)

>  

---

### 2. Construction order
For `ScavTrap s("Steve");`, list the **four** steps the compiler emits, in order.

> 

---

### 3. Destruction order
For the same `s` going out of scope, list the **four** steps in destruction order.

> 

---

### 4. Why this order?
Why must the *base* constructor run **before** the derived constructor's body — not after? Phrase it in terms of what the derived might rely on.

> 

---

### 5. Code prediction — full trace
```cpp
ClapTrap::ClapTrap(std::string n)  { std::cout << "ClapTrap "  << n << " ctor\n"; }
ClapTrap::~ClapTrap()              { std::cout << "ClapTrap "  << name << " dtor\n"; }
ScavTrap::ScavTrap(std::string n) : ClapTrap(n) { std::cout << "ScavTrap " << n << " ctor\n"; }
ScavTrap::~ScavTrap()              { std::cout << "ScavTrap "  << name << " dtor\n"; }

int main() { ScavTrap s("Steve"); }
```
What does it print? (4 lines)

> 

---

### 6. Access specifiers
For each access mode, who can read the member?
- `private` — 
- `protected` — 
- `public` — 

Which one do you typically use for fields a derived class needs to touch directly?

> 

---

### 7. `private` field bug
The `ScavTrap` ctor body tries to write `hp = 100;` and gets a compile error. Why? Two ways to fix it.

> 

---

### 8. Init-list ordering
This compiles with `-Wreorder` (which becomes an error under `-Werror`):
```cpp
class ClapTrap {
    std::string name;
    unsigned int hp;
    unsigned int ep;
    unsigned int atk;
public:
    ClapTrap(std::string n)
        : atk(0), ep(10), hp(10), name(n) {}   // <-- warning here
};
```
What's wrong, and what is the fix?

> 

---

### 9. Override without `virtual`
In Mod 03, `ScavTrap::attack` overrides `ClapTrap::attack` *non-virtually*. What dispatch happens for `ClapTrap& ref = scav; ref.attack("x");` and what is this called?

> 

---

### 10. The diamond — naïve layout
Without `virtual` inheritance, sketch the layout of a `DiamondTrap` (just the subobjects, no fields needed). How many `ClapTrap` subobjects does it contain?

> 

---

### 11. Why the diamond is a problem
Name **two** concrete problems caused by having two `ClapTrap` subobjects.

> 

---

### 12. The `virtual` fix
Where exactly do you put `virtual` to make the diamond collapse to a single base? Write the three class headers.

> 

---

### 13. Most-derived initialises the virtual base
With virtual inheritance in place, who is responsible for calling `ClapTrap`'s ctor explicitly in the init list? Why?

> 

---

### 14. Name shadowing — the `name` trick
`DiamondTrap` has its own private `name` and so does `ClapTrap`. In `whoAmI()` you want to print **both**. Show the two expressions you use, and explain how each one resolves.

> 

---

### 15. Trace count — proof the virtual is wired right
With `virtual` inheritance correctly in place, **how many times** does `ClapTrap`'s constructor fire when you create one `DiamondTrap`? How many without `virtual`?

> 
