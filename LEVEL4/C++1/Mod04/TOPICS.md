# C++ Module 04 — TOPICS

## Theme
**Subtype polymorphism** — `virtual` dispatch, abstract classes,
interfaces (pure abstract classes — C++98 has no `interface`
keyword). This is the module where inheritance starts to pay its
rent.

## Global rules
Same c++98 / OCF / no-STL / no-leaks constraints. Constructors and
destructors of **every** class must print **distinct** messages —
you'll need them to prove the right calls fire.

---

## Cross-cutting concepts (read before starting)

| Concept | Where |
|---|---|
| Virtual dispatch, vtables, pure virtual, object slicing | [library/cpp/oop/POLYMORPHISM.md](../../../library/cpp/oop/POLYMORPHISM.md) |
| Inheritance basics (prerequisite) | [library/cpp/oop/INHERITANCE.md](../../../library/cpp/oop/INHERITANCE.md) |
| OCF — deep-copy pattern for owned resources | [library/cpp/oop/ORTHODOX_CANONICAL_FORM.md](../../../library/cpp/oop/ORTHODOX_CANONICAL_FORM.md) |
| `new`/`delete` lifetimes | [library/cpp/fundamentals/MEMORY.md](../../../library/cpp/fundamentals/MEMORY.md) |
| Per-exercise walkthrough | [library/cpp/modules/CPP04.md](../../../library/cpp/modules/CPP04.md) |

### The one question Mod04 is asking

*When you call `p->makeSound()` on a `Animal* p`, which function
runs — `Animal::makeSound` or the most-derived override?*

- **Without `virtual`:** the answer is decided **at compile time** by
  the static type of `p`. `p` is declared `Animal*`, so
  `Animal::makeSound` runs. This is called **static dispatch**.
- **With `virtual`:** the answer is decided **at run time** by the
  actual object's type, looked up through a per-class table called
  the **vtable**. `p` might point to a `Dog`, so `Dog::makeSound`
  runs. This is **dynamic dispatch**.

That single distinction is the whole module. Every exercise adds a
wrinkle on top: ex00 shows the difference directly, ex01 exposes
memory ownership across polymorphic hierarchies, ex02 adds `= 0`
(pure virtual), ex03 builds a full interface-driven system.

### The vtable, briefly

Every class with at least one `virtual` method gets a **vtable** — a
table of function pointers, one per virtual method, laid out in
declaration order. Every *instance* of such a class carries a
hidden pointer (the **vptr**) to its class's vtable.

```
Dog object:
┌────────────────┐
│ vptr ──────────┼──► Dog's vtable ──► &Dog::makeSound
│ (inherited)    │                 ├──► &Animal::~Animal (virtual dtor)
│ Animal members │                 └──► ...
│ Dog members    │
└────────────────┘
```

`p->makeSound()` compiles to *"load `p`'s vptr; load the `makeSound`
slot; call through that pointer."* Two extra loads — cheap but not
free.

See [library/cpp/oop/POLYMORPHISM.md §4](../../../library/cpp/oop/POLYMORPHISM.md)
for the full layout.

---

## Exercise 00 — Polymorphism

### Goal
Two parallel hierarchies:

- `Animal` with `protected std::string type`; virtual
  `makeSound()`. Subclasses `Dog` / `Cat` set `type` and override
  `makeSound`.
- **`WrongAnimal`** / **`WrongCat`** — same shape, but
  `makeSound()` is **not** virtual.

A main that holds a `Dog` through an `Animal*` prints the dog's
sound. A main that holds a `WrongCat` through a `WrongAnimal*`
prints the wrong animal's sound. That contrast is the exercise.

### What the exercise drills

- **The `virtual` keyword** on a member function.
- **`override`** doesn't exist in c++98. You signal override by
  simply declaring the same signature in the derived class. Watch
  the signature — a typo silently creates a new method instead of
  overriding.
- **Upcasting**. `Animal* a = new Dog();` is safe, implicit, and
  the polymorphic handle in `main`.
- **Static vs dynamic dispatch, observed.** With `Animal`, the
  `Dog` version runs. With `WrongAnimal`, the base version runs.
  That's the whole test.

### `type` as protected

`type` is **protected** so `Dog`'s and `Cat`'s constructors can set
it directly: `Dog::Dog() { type = "Dog"; }`. Protected members are
accessible from derived classes but still hidden from unrelated code.

### Hardware-level notes

- `Animal` and `Dog` each have a vtable. At `new Dog()`, the ctor
  sets the instance's vptr to `Dog`'s vtable. When the `Dog` ctor
  starts running, the vptr is briefly `Animal`'s (because the
  `Animal` subobject is constructed first and its ctor sets the
  vptr to `Animal`'s). Then `Dog`'s ctor body changes it to
  `Dog`'s. This matters: **calling a virtual method from a ctor
  dispatches as the type being constructed at that moment**, not
  the most-derived type.
- `sizeof(Animal)` on libstdc++ typically = 8 (vptr) + `sizeof(std::string)` ≈ 32-40 bytes.
- `WrongAnimal` has no virtual methods (yet) → no vtable → no
  vptr. `sizeof(WrongAnimal)` is smaller than `sizeof(Animal)` by
  the vptr (usually 8 bytes).

### Tips & gotchas

- **`virtual` in the base is sufficient.** c++98 inherits
  virtuality automatically. Adding `virtual` in `Dog::makeSound()`
  is optional but **best practice** — documents intent.
- **Virtual destructor.** Even in ex00, make `Animal`'s destructor
  virtual. You won't `delete` polymorphically here, but ex01 will.
  Build the habit.
- **`std::string type` not copied explicitly.** Compiler-generated
  copy ctor + copy-assign are fine here — `std::string` deep-copies
  itself.

### Search terms
`"c++ virtual function example"`,
`"c++ virtual vs non-virtual dispatch"`,
`"c++ static binding vs dynamic binding"`.

### Cross-reference
- [library/cpp/oop/POLYMORPHISM.md §1-3](../../../library/cpp/oop/POLYMORPHISM.md)
- [library/cpp/modules/CPP04.md §ex00](../../../library/cpp/modules/CPP04.md)

---

## Exercise 01 — I don't want to set the world on fire

### Goal
Add a `Brain` class (holding `std::string ideas[100]`). `Dog` and
`Cat` each own a `Brain*` — `new Brain()` in the ctor, `delete`
in the dtor. In `main`, build an array of `Animal*` — half `Dog`,
half `Cat` — and `delete` each through the `Animal*` base pointer.
That requires the `Animal` destructor to be **virtual**.

Deep copy must work: copy a `Dog`, mutate the copy's ideas, verify
the original is unchanged.

### What the exercise drills

- **The virtual destructor rule.** If you `delete animal_ptr` and
  the dtor isn't virtual, only `~Animal()` runs — `Brain*` leaks
  because `~Dog()` never fires. Valgrind catches it, but the point
  is understanding **why** without needing valgrind to tell you.
- **Deep copy of a resource-owning class.** Default copy ctor does
  a member-wise copy → two `Dog`s with the *same* `Brain*`. When
  the first one dies, the second dereferences a freed pointer.
  Double-free when the second dies. You **must** write a copy ctor
  and copy assignment that `new Brain(*other.brain)`.
- **Resource ownership through inheritance.** `Brain` is owned by
  `Dog` and `Cat`, not by `Animal`. But when you destroy through
  `Animal*`, the chain has to reach `~Dog()` for the delete to
  happen. Virtual dtor is the hinge.

### The brain-owning OCF for `Dog`

Roughly:

```cpp
Dog::Dog() : Animal(), brain(new Brain()) { /* trace */ }

Dog::Dog(Dog const& src) : Animal(src), brain(new Brain(*src.brain)) {
    /* trace */
}

Dog& Dog::operator=(Dog const& other) {
    if (this != &other) {
        Animal::operator=(other);
        delete brain;
        brain = new Brain(*other.brain);
    }
    return *this;
}

Dog::~Dog() { delete brain; /* trace */ }
```

Three things to notice:

1. Copy ctor calls `Animal(src)` — forwards base-slice copy.
2. Copy-assign calls `Animal::operator=(other)` — forwards too.
3. Copy-assign **deletes the old brain before allocating a new
   one**. Leak otherwise.

### Why deep copy matters — the test

The subject's main mutates the copy's ideas:

```cpp
Dog basic;
{
    Dog tmp = basic;             // copy ctor
    // tmp's brain is a separate allocation
}                                 // tmp dies, its brain is freed
// basic still has its own, untouched brain
```

If your copy is shallow, both `basic` and `tmp` share one brain.
When `tmp` dies, it frees the brain. `basic` now holds a dangling
pointer. Later access = UB.

### Hardware-level notes

- `Brain` is **100 `std::string`s**, each 24-32 bytes → ~3 KB on
  the heap per `Brain`. Deep-copying N animals allocates N *
  3 KB, plus N separate `new Brain()` allocator calls. That's why
  the subject keeps N small.
- `delete` through a base pointer with a non-virtual dtor is
  **undefined behaviour** per the C++ standard. On practical
  implementations it leaks the derived's resources silently.
  Valgrind shows the leak; `g++ -Wnon-virtual-dtor` warns at
  compile time.
- The **vptr** in the object is what makes virtual dispatch
  possible. On `delete base_ptr`, the compiler generates
  *"look up the dtor slot in the vtable; call it"* — that ends up
  at `Dog::~Dog()`, which chains into `~Animal()`. Without
  `virtual`, the compiler hardcodes `Animal::~Animal` and
  `Dog::~Dog` is never called.

### Tips & gotchas

- **`Animal::operator=` exists and must be called** from
  `Dog::operator=` if `Animal` has anything to copy. In this
  exercise `Animal` holds `type` — so yes, forward.
- **Array of `Animal*` in main.** `Animal* zoo[10];` then
  `zoo[i] = (i < 5) ? (Animal*)new Dog() : (Animal*)new Cat();`.
  The upcast is implicit — no cast actually needed.
- **Delete loop in main.** `for (i=0; i<10; ++i) delete zoo[i];` —
  this is what exercises the virtual-dtor chain.
- **Don't forget `~Brain()`.** It's implicit (default dtor is
  enough for an array of `std::string`), but the subject may want
  a trace. Re-read.

### Search terms
`"c++ virtual destructor why"`,
`"c++ deep copy vs shallow copy example"`,
`"c++ rule of three example"`,
`"-Wnon-virtual-dtor"`.

### Cross-reference
- [library/cpp/oop/POLYMORPHISM.md §7](../../../library/cpp/oop/POLYMORPHISM.md) — virtual destructor rule
- [library/cpp/oop/ORTHODOX_CANONICAL_FORM.md](../../../library/cpp/oop/ORTHODOX_CANONICAL_FORM.md) — deep-copy pattern

---

## Exercise 02 — Abstract class

### Goal
Make `Animal` **uninstantiable**. At least one method becomes
**pure virtual** (signature `= 0`). Everything from ex01 should
still compile and behave the same.

### What the exercise drills

- **Pure virtual functions.** `virtual void makeSound() const = 0;`
  — no body in the base (you *can* provide one, but it's not
  required). The `= 0` is what marks the method pure.
- **Abstract classes.** A class with at least one pure virtual is
  **abstract** — you cannot instantiate it. `Animal a;` becomes a
  compile error; `Animal* p = new Dog();` still works.
- **Concrete classes must override all pure virtuals.** If `Dog`
  forgets to override `makeSound`, `Dog` is also abstract — also
  uninstantiable. This is how abstract classes enforce a contract
  at compile time.

### Optional: the `A` prefix

The subject hints at renaming `Animal` → `AAnimal` (the `A`
convention for abstract classes). Not required for passing, but
good practice — aligns with the AMateria / ICharacter / IMateriaSource
naming in ex03.

### Hardware-level notes

- Abstract classes **still have a vtable**. The pure virtual slot
  in that vtable points to a special stub (`__cxa_pure_virtual` on
  Itanium ABI) that calls `std::terminate()` if ever invoked.
- The abstract class's vptr exists in the object layout just like
  a normal virtual class's — `sizeof(Animal)` is unchanged from
  ex01. The prohibition is **purely a language-level check**; at
  runtime a partially-constructed abstract is indistinguishable
  from a concrete one (during construction, the vptr temporarily
  points to the abstract's vtable).
- **Calling a pure virtual from a ctor/dtor** invokes that stub
  → immediate crash. The subject avoids this pattern; you should
  too.

### Tips & gotchas

- **Pick the right method to pure-virtualise.** `makeSound` is the
  obvious one — "an animal that can't make a sound isn't really an
  animal". Subject is lax about which method.
- **Keep the destructor virtual but not pure.** You could declare
  `virtual ~Animal() = 0;` *and* provide a body, but the usual
  choice is non-pure virtual dtor (since you usually want
  derived classes to be free of the override burden for `~Animal`).
- **Dog/Cat still need full OCF.** Making the base abstract does
  not excuse you from OCF on the concrete classes.

### Search terms
`"c++ pure virtual function"`,
`"c++ abstract class example"`,
`"c++ = 0 syntax"`.

### Cross-reference
- [library/cpp/oop/POLYMORPHISM.md §5](../../../library/cpp/oop/POLYMORPHISM.md) — pure virtual
- [library/cpp/oop/POLYMORPHISM.md §6](../../../library/cpp/oop/POLYMORPHISM.md) — interfaces

---

## Exercise 03 — Interface & recap (bonus)

### Goal
A small RPG-ish system exercising **pure abstract classes as
interfaces**, plus a non-trivial inventory model.

- **`AMateria`** — abstract. Protected `type`. Public
  `AMateria(std::string const& type)`, `std::string const& getType()
  const`, `virtual AMateria* clone() const = 0`,
  `virtual void use(ICharacter& target);`.
- **`Ice`** and **`Cure`** — concrete. `clone()` returns a new
  instance of the same concrete type. `use()` prints the flavour
  message.
- **`ICharacter`** — pure abstract interface: `getName`,
  `equip(AMateria*)`, `unequip(int idx)`, `use(int idx, ICharacter&
  target)`.
- **`Character : public ICharacter`** — 4-slot inventory,
  starts empty. `equip` fills the first empty slot. `unequip`
  **does not delete** (caller's responsibility); full inventory
  or bad index = no-op. Deep copy of the inventory.
- **`IMateriaSource`** — interface: `learnMateria(AMateria*)`,
  `createMateria(std::string const&)`.
- **`MateriaSource`** — holds up to 4 templates, clones one by
  type name in `createMateria`, returns `0` if unknown.

### What the exercise drills

- **Interfaces as pure-virtual-only classes.** C++98 has no
  `interface` keyword, but the convention is a class where every
  non-destructor method is pure virtual, typically no data members.
  The `I` prefix marks them visually.
- **The Prototype pattern.** `clone()` lets you "make another one
  like this" without knowing the concrete type. `MateriaSource`
  uses it: it holds an `AMateria*` of some concrete type and
  calls `->clone()` when asked to create a new instance.
- **Ownership through pointers.** `Character`'s inventory holds
  `AMateria*`. Copying a character must deep-copy each materia
  (via `clone()`), else two characters share inventory and
  double-free on destruction.
- **The "floor" concept.** `unequip(i)` removes the pointer from
  the inventory but **does not delete** it. The caller now owns
  the materia, typically to re-equip it or drop it somewhere.
  This is explicit ownership transfer.

### Prototype pattern in 10 lines

```cpp
AMateria* Ice::clone() const { return new Ice(); }
AMateria* Cure::clone() const { return new Cure(); }

AMateria* MateriaSource::createMateria(std::string const& type) {
    for (int i = 0; i < 4; ++i)
        if (templates[i] && templates[i]->getType() == type)
            return templates[i]->clone();
    return 0;
}
```

`createMateria` doesn't need to know about `Ice` or `Cure` —
polymorphism does the work. Add a new materia type by registering
its template once; the factory Just Works.

### Deep copy of `Character`

```cpp
Character& Character::operator=(Character const& other) {
    if (this != &other) {
        for (int i = 0; i < 4; ++i) {
            delete inventory[i];           // delete old
            inventory[i] = other.inventory[i]
                         ? other.inventory[i]->clone()
                         : 0;
        }
        name = other.name;
    }
    return *this;
}
```

`clone()` is the key — it's how you copy through a base pointer
without knowing the concrete type.

### Hardware-level notes

- **Each `AMateria` subclass has its own vtable.** `clone()`
  dispatches through the vtable — `materia_ptr->clone()` produces
  an `Ice*` or `Cure*` depending on the actual object.
- **`Character`'s inventory is 4 `AMateria*`** — 32 bytes of
  pointer storage, plus 4 separately-allocated materia objects.
  Four calls to `new` per full character.
- **`MateriaSource`'s templates** are just 4 pointers. Each
  `learnMateria` stores a pointer; each `createMateria` calls
  `clone()`, allocating a fresh object. The source owns the
  templates; the character owns the clones.
- **No leaks means:** `Character` dtor deletes each non-null
  inventory slot; `MateriaSource` dtor deletes each template.
  Floor'd materias (from `unequip`) are the test harness's
  responsibility.

### Tips & gotchas

- **`unequip` does not delete.** Test harnesses rely on this —
  unequipping a materia should leave it callable elsewhere. Easy
  to get wrong "defensively".
- **Bad index / full inventory / null materia** — all no-ops. No
  crashes, no errors, no prints (unless the subject specifies).
- **`clone()` must not copy the type string.** The subject says:
  "Copying a materia must not copy its type." Interpretation:
  `clone()` constructs a fresh `Ice()` (which sets its own type),
  not `new Ice(*this)` (which would copy the type via base).
- **The copy ctor / operator= for `AMateria`** — subject wants
  them but doesn't let them copy the type. So: default-construct
  the type in the copy ctor, leave it alone in operator=.
- **`MateriaSource::createMateria` returns `0` on unknown types.**
  Not `NULL`, not `nullptr` (c++11) — `0` is fine in c++98.

### Search terms
`"c++ prototype pattern clone method"`,
`"c++ interface pure abstract class"`,
`"c++ deep copy polymorphic pointer"`,
`"c++98 NULL vs 0"`.

### Cross-reference
- [library/cpp/oop/POLYMORPHISM.md §5-6](../../../library/cpp/oop/POLYMORPHISM.md)
- [library/cpp/modules/CPP04.md §ex03](../../../library/cpp/modules/CPP04.md)

---

## Module-level sanity checks before moulinette

- **Virtual destructors everywhere** in hierarchies you `delete`
  through a base pointer. If you forget, you leak. The compiler
  warns with `-Wnon-virtual-dtor`.
- **`valgrind ./program`** clean for every exercise. Mod 04 leaks
  easy if OCF is sloppy.
- **Distinct ctor/dtor messages** for every class — subject is
  explicit.
- **ex02:** `Animal a;` must fail to compile. Add that line in a
  comment as a test, uncomment to verify.
- **ex03:** verify `unequip(0); equip(mat);` puts `mat` back in
  slot 0 — not slot 1. And verify a deleted character doesn't
  take floor'd materias down with it.

You can pass the module without ex03.
