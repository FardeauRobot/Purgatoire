# C++ Module 04 — Learning Guide

**Module theme:** *Subtype polymorphism — virtual dispatch, abstract
classes, and interfaces.*

Mod 03 gave you inheritance without polymorphism. This module adds
the missing piece: when you hold a `Dog` through an `Animal*`,
calling `makeSound()` should bark, not issue a generic "animal
noise". That's **dynamic dispatch**, and the mechanism behind it
(vtables, vptrs) is the heart of Mod 04.

See also: the per-module [`TOPICS.md`](../../../LEVEL4/C++1/Mod04/TOPICS.md)
and [`oop/POLYMORPHISM.md`](../oop/POLYMORPHISM.md) for the full
reference.

---

## The one sentence to memorise

> **`virtual`** makes method calls dispatch based on the object's
> **actual type** (the type you `new`'d), not the static type of
> the pointer or reference you're holding it through.

Drop `virtual` → the compiler picks the function at compile time
based on the variable's declared type (static dispatch). Add
`virtual` → the compiler emits code that reads a per-class
**vtable** at run time and picks the right override (dynamic
dispatch).

---

## The vtable, in one diagram

For any class with at least one virtual method:

```
class Animal              Animal's vtable
┌───────┐                 ┌──────────────────────┐
│ vptr ─┼──────────────►  │ &Animal::makeSound   │
│ type  │                 │ &Animal::~Animal     │
└───────┘                 └──────────────────────┘

class Dog : public Animal  Dog's vtable
┌───────┐                 ┌──────────────────────┐
│ vptr ─┼──────────────►  │ &Dog::makeSound      │   ← override
│ type  │                 │ &Dog::~Dog           │   ← override
│ brain │                 └──────────────────────┘
└───────┘
```

Every instance has a hidden `vptr` in its header (typically 8
bytes). `p->makeSound()` becomes *"load p's vptr; load slot N;
call through."* Two extra memory loads, usually cache-hot.

---

## Exercise 00 — Polymorphism

### What you're learning
**The `virtual` keyword and the difference between static and
dynamic dispatch**, observed side-by-side.

### Class shape
- `Animal` — `protected std::string type`; virtual `makeSound()`.
  Concrete (instantiable).
- `Dog`, `Cat` — set `type`, override `makeSound()`.
- `WrongAnimal` / `WrongCat` — same shape, **without `virtual`**.

### The money shot in `main`

```cpp
Animal const* i = new Dog();
Animal const* j = new Cat();
i->makeSound();   // "bark"  ← virtual dispatch works
j->makeSound();   // "meow"
delete i;
delete j;

WrongAnimal const* k = new WrongCat();
k->makeSound();   // generic "wrong animal sound"  ← static dispatch
delete k;
```

That contrast is the entire exercise.

### Tips
- **Make `~Animal()` virtual** even if ex00 doesn't need it — ex01
  does, and you'll forget if you don't build the habit.
- `c++98` has no `override` keyword. Signal overriding by matching
  the signature exactly. A typo in a parameter type silently
  creates a new method instead of overriding — run the code and
  test.
- `type` is `protected` so derived ctors can set it without going
  through a setter.

### Gotcha
**Don't forget the `const` on `makeSound() const`.** If the base is
`virtual void makeSound() const;` and the derived is `virtual void
makeSound();` (no const), the derived **does not override** — it's
a new method that shadows. Dispatch falls back to base.

---

## Exercise 01 — I don't want to set the world on fire

### What you're learning
**Deep copy of resource-owning classes**, and **why the virtual
destructor rule exists**.

### Class shape
- `Brain` — `std::string ideas[100]`. Fairly dumb, no pointers.
- `Dog` / `Cat` — own a private `Brain*`. Ctor does `new Brain()`;
  dtor does `delete brain`.
- Main: array of `Animal*`, half `Dog`, half `Cat`, then `delete`
  each through the base pointer.

### The virtual destructor rule

```cpp
Animal* a = new Dog();
delete a;
```

- If `~Animal()` is **virtual**, the compiler emits a vtable
  lookup → `~Dog()` runs first, which deletes the `Brain`, then
  chains into `~Animal()`. Clean.
- If `~Animal()` is **not virtual**, the compiler hardcodes a
  direct call to `~Animal()`. `~Dog()` never runs → `Brain` leaks.
  **Undefined behaviour** per the standard.

Rule: **when you intend to delete through a base pointer, the base
dtor must be virtual.** `-Wnon-virtual-dtor` warns about it;
valgrind catches the leak.

### Deep copy — why and how

```cpp
Dog basic;
Dog copy = basic;    // copy ctor
```

Default (compiler-generated) copy ctor does a member-wise copy —
both `basic` and `copy` point to the **same `Brain`**. When `copy`
dies, it deletes the brain. `basic`'s `brain` is now dangling. When
`basic` dies, double-free.

The fix — custom copy ctor + copy-assign:

```cpp
Dog::Dog(Dog const& src) : Animal(src), brain(new Brain(*src.brain)) {
    std::cout << "Dog copy constructed" << std::endl;
}

Dog& Dog::operator=(Dog const& other) {
    if (this != &other) {
        Animal::operator=(other);
        delete brain;                           // free old
        brain = new Brain(*other.brain);        // deep-copy new
    }
    return *this;
}
```

Three things to notice:
1. **Forward the base-slice copy** — `Animal(src)` / `Animal::operator=(other)`.
2. **Delete the old before allocating the new** in `operator=`.
   Otherwise leak.
3. **Self-assignment guard** — else `delete brain; brain = new
   Brain(*other.brain);` frees the thing you're about to copy.

### Tips
- **Test the deep-copy**: mutate `copy`'s ideas, print
  `basic`'s — they should differ. If they don't, your copy is
  shallow.
- **Valgrind**: `--show-leak-kinds=all ./animals`. Any leak or
  invalid-read means the virtual dtor or deep-copy pattern is
  wrong.
- The `Brain` class itself doesn't need custom OCF — an array of
  `std::string` already deep-copies per element.

### Gotcha
**Array of `Animal*` in main**. Create each with `new Dog()` or
`new Cat()`, store as `Animal*`. The upcast is implicit — no
casting needed. To `delete` them: `for (i=0; i<N; ++i) delete
zoo[i];`.

---

## Exercise 02 — Abstract class

### What you're learning
**Pure virtual functions and abstract classes.**

### The syntax

```cpp
class Animal {
public:
    virtual void makeSound() const = 0;   // pure virtual
};
```

`= 0` at the end of the declaration marks it pure. A class with
any pure virtual is **abstract** — you cannot instantiate it:

```cpp
Animal a;    // ← compile error: Animal is abstract
Animal* p = new Dog();    // ← fine
```

### The contract

Any concrete derived class **must override** every pure virtual
from the base. Miss one → the derived is also abstract → also
uninstantiable. Compile-time enforcement: you can't forget.

### Tips
- Rename `Animal` → `AAnimal` (the `A` prefix convention for
  abstract classes) if you want to match the ex03 `AMateria`
  naming. Optional but stylistically consistent.
- **Don't pure-virtualise the destructor.** You *can*
  (`virtual ~Animal() = 0;` + body), but it's an anti-pattern here.
  Plain virtual is fine.
- Everything from ex01 should still compile and behave the same —
  the `Dog`/`Cat` side is unchanged.

### Gotcha
Even abstract classes have vtables and vptrs. `sizeof(Animal)` is
unchanged from ex01. The language just refuses to let you
instantiate the abstract.

---

## Exercise 03 — Interface & recap (bonus)

### What you're learning
**Interfaces (pure abstract classes) and the Prototype pattern.**

### The cast

- **`AMateria`** — abstract. `getType()`, `clone()` (pure), `use()`.
- **`Ice`**, **`Cure`** — concrete. Override `clone()` and `use()`.
- **`ICharacter`** — pure abstract interface. `getName`, `equip`,
  `unequip`, `use`.
- **`Character`** — implements `ICharacter`. 4-slot inventory.
- **`IMateriaSource`** — pure abstract. `learnMateria`,
  `createMateria`.
- **`MateriaSource`** — implements `IMateriaSource`. Up to 4
  templates.

### The Prototype pattern

`MateriaSource::createMateria(type)` must return an `AMateria*`
of the right concrete type — but `MateriaSource` doesn't know the
derived types (no `if type == "ice" return new Ice()`). Solution:
hold a template of each type, call `clone()` on the template:

```cpp
AMateria* Ice::clone() const { return new Ice(); }

AMateria* MateriaSource::createMateria(std::string const& type) {
    for (int i = 0; i < 4; ++i)
        if (templates[i] && templates[i]->getType() == type)
            return templates[i]->clone();
    return 0;   // c++98: NULL is fine too
}
```

Adding a new materia type (e.g. `Fire`) = writing the class + its
`clone()` + registering an `Ice` template via `learnMateria`. No
changes to `MateriaSource`.

### Deep-copy of `Character`'s inventory

```cpp
Character& Character::operator=(Character const& other) {
    if (this != &other) {
        for (int i = 0; i < 4; ++i) {
            delete inventory[i];
            inventory[i] = other.inventory[i]
                         ? other.inventory[i]->clone()
                         : 0;
        }
        name = other.name;
    }
    return *this;
}
```

`clone()` is how you "copy a polymorphic thing without knowing
its type". This is **the** idiom for this kind of ownership.

### The ownership contract — `unequip` does not delete

```cpp
void Character::unequip(int idx) {
    if (idx < 0 || idx >= 4) return;
    inventory[idx] = 0;    // removes from inventory, NO delete
}
```

The caller now owns whatever the inventory used to hold. They can
re-equip elsewhere or drop it. If `unequip` deleted, the caller's
pointer becomes dangling — they can't tell.

### Tips
- **Null-safe iteration.** Inventory slots start at `0` (null).
  Always check `if (inventory[i])` before calling methods.
- **Full inventory / bad index** — no-op, no print (unless
  subject specifies otherwise).
- **`clone()` must not copy the type string.** Construct a fresh
  `Ice()` (which sets its own type in its ctor), not `new
  Ice(*this)` (which would copy the type via the base copy ctor).
- **`c++98` uses `0` or `NULL`** for null pointers — no `nullptr`.

### Gotcha
`MateriaSource` dtor must delete every non-null template.
`Character` dtor must delete every non-null inventory slot.
Floor'd materias (returned from `unequip`) are the main's
responsibility — tests check this.

---

## What this module leaves you with

- You can read a class hierarchy and predict which method runs on
  every call.
- Virtual destructor is reflex, not a chore.
- Deep-copy of polymorphic pointers is one pattern you can write
  without thinking (`clone()` + `delete` old + assign new).
- Pure virtual + abstract class + interface convention (`A*` /
  `I*` prefix) is second nature.

After Mod 04, C++ feels like C++ — not C with extra syntax. The
remaining modules (templates, STL, iterators, smart containers)
build on this foundation.
