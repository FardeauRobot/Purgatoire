# C++ Module 03 — Learning Guide

**Module theme:** *Inheritance — "is-a" relationships, constructor
and destructor chains, and the diamond problem.*

This module drills one question four times: *when class B inherits
from class A, what exactly does B get, and in what order does it all
happen?* The vehicle is a silly robot family — `ClapTrap` and its
children — chosen so the mechanics stay loud (every ctor, dtor, and
action prints a trace, so you can watch the chain fire).

See also: the per-module [`TOPICS.md`](../../../LEVEL4/C++1/Mod03/TOPICS.md)
for exercise-by-exercise deep dives, and
[`oop/INHERITANCE.md`](../oop/INHERITANCE.md) for the complete
inheritance reference.

---

## The four rules you'll reach for constantly

1. **Public inheritance means "is-a".** Every `ScavTrap` *is-a*
   `ClapTrap`. You can pass it where a `ClapTrap&` is expected. You
   can store it in a `ClapTrap*`.
2. **Base construction runs first, destruction last.** Derived
   ctor body executes after the base subobject is fully alive.
   Derived dtor body executes before the base is torn down.
3. **Private members are not accessible from derived classes.** Use
   `protected` (still hidden from unrelated code, visible to
   derived) or provide accessors.
4. **Name-shadowing.** A method in the derived class with the same
   name as one in the base **hides** the base's — including all
   overloads. Bring them back with `using Base::method;`.

---

## Exercise 00 — Aaaaand… OPEN! (`ClapTrap`)

### What you're learning
**How to write a resource-managing class in full OCF** when the
resources are primitives (no heap yet). This is the base of every
class that follows.

### Class shape
- Private: `std::string name`, `unsigned int hitPoints` (start
  10), `energyPoints` (start 10), `attackDamage` (start 0).
- Public: OCF × 4, plus `attack(std::string const& target)`,
  `takeDamage(unsigned int)`, `beRepaired(unsigned int)`.
- **Every** ctor, dtor, and action **prints a trace**.

### The initialisation list is not optional

```cpp
ClapTrap::ClapTrap(std::string const& name)
    : name(name), hitPoints(10), energyPoints(10), attackDamage(0) {
    std::cout << "ClapTrap " << name << " constructed" << std::endl;
}
```

- Init list runs before the body.
- Order follows **declaration order in the class**, not the order
  you write them. Write them in declaration order anyway — else
  `-Wreorder` → compile error under `-Werror`.
- `const` members and reference members **must** be initialised
  in the init list (ex03 of Mod 01 hinted at this with `HumanA`'s
  reference member).

### Unsigned underflow — the one bug
`hitPoints -= 100;` when HP is 10 wraps to `4294967206`, not 0.
Clamp:

```cpp
hitPoints = (damage >= hitPoints) ? 0 : hitPoints - damage;
```

Same for any `unsigned int` arithmetic where the subtrahend might
exceed the minuend.

### Action guards — energy = 0 vs HP = 0

The subject is specific:
- **`attack`** — costs 1 energy. Refuse if energy or HP is 0.
- **`beRepaired`** — costs 1 energy. Refuse if energy is 0 (HP
  doesn't matter; a 0-HP ClapTrap is already dead in spirit, but
  the subject doesn't require an HP check here — re-read).
- **`takeDamage`** — does NOT cost energy. Just reduces HP.

### Tips
- `ClapTrap()` default ctor: subject allows defaulting the name to
  `"default"` or similar. The name-taking ctor is the main one.
- Copy ctor and copy-assign can be written as `*this = src;` and
  member-wise copy respectively. For primitives + `std::string`,
  the defaults are correct — but you write them anyway for the
  traces.

### Gotcha
Ctor/dtor messages must match the subject's wording **character
for character**, including quotation marks, exclamation points,
and whether there's a trailing period. The moulinette diffs output.

---

## Exercise 01 — Serena, my love! (`ScavTrap`)

### What you're learning
**Public inheritance, base ctor in the init list, and the
ctor/dtor chain.**

```cpp
class ScavTrap : public ClapTrap { ... };
```

### Calling the base ctor

```cpp
ScavTrap::ScavTrap(std::string const& name) : ClapTrap(name) {
    hitPoints = 100;
    energyPoints = 50;
    attackDamage = 20;
    std::cout << "ScavTrap " << name << " constructed" << std::endl;
}
```

Two choices here:

1. **Reassign in the body** (shown above) — simpler, one extra
   write per stat. Requires the stats to be `protected` (or
   accessible via setters).
2. **Protected `ClapTrap(name, hp, ep, atk)`** — more efficient,
   but adds a non-public ctor. Perfectly fine; matter of taste.

### Ctor/dtor trace order

```
ClapTrap Steve constructed
ScavTrap Steve constructed
        ← work happens here
ScavTrap Steve destroyed
ClapTrap Steve destroyed
```

Base-first-on-construction, derived-first-on-destruction.

### Overriding `attack()` without `virtual`

`ScavTrap::attack` hides `ClapTrap::attack`. Calling via a
`ScavTrap&` gets the derived version. Calling via a `ClapTrap&`
(that actually points to a `ScavTrap`) gets the **base** version —
this is **static dispatch**. Mod 04 will introduce `virtual` to
fix it.

### Tips
- `guardGate()` is a method on `ScavTrap` only. Through a
  `ClapTrap*`, it's invisible — compile error.
- Copy ctor forwards to base: `ScavTrap(ScavTrap const& src) :
  ClapTrap(src) { ... }`. Implicit slicing at parameter binding is
  safe because the base ctor only needs the base-subobject view.
- Don't forget `ScavTrap::operator=`. The usual shape:
  ```cpp
  ScavTrap& operator=(ScavTrap const& other) {
      ClapTrap::operator=(other);
      // ... copy ScavTrap's own members, none here
      return *this;
  }
  ```

### Gotcha
If you left `ClapTrap`'s fields `private` in ex00 with no
setters, the ScavTrap ctor can't touch them from its body. Fix:
promote to `protected`, or add setters.

---

## Exercise 02 — Repetitive work (`FragTrap`)

### What you're learning
**Repetition to build muscle memory.** Same shape as ex01 with
different constants and a different signature method
(`highFivesGuys()`). No new concepts.

### Tips
- Copy-paste `ScavTrap.*` → `FragTrap.*`, rename, adjust stats,
  update messages. 5 minutes of work.
- **Read what you're writing, don't auto-pilot.** ex03 is about to
  throw a multi-inheritance wrench in, and if ex02's pattern didn't
  actually register, ex03 will feel like magic.

### Gotcha
The subject expects traces for copy ctor and copy-assign too.
Don't skip them because they "feel redundant" — moulinette tests
`FragTrap b(a);` and diffs the output.

---

## Exercise 03 — Now it's weird! (`DiamondTrap`, bonus)

### What you're learning
**The diamond problem and `virtual` inheritance.**

```
        ClapTrap
         /    \
   ScavTrap  FragTrap
         \    /
       DiamondTrap
```

### The problem without `virtual`

Declaring `class DiamondTrap : public FragTrap, public ScavTrap {}`
gives you **two** `ClapTrap` subobjects — one inside each
intermediate. Two `name`s, two `hp`s, two ctors fire, two dtors
fire. Any reference to `ClapTrap::x` is ambiguous.

### The fix

Both intermediates inherit **virtually**:

```cpp
class ScavTrap  : virtual public ClapTrap { ... };
class FragTrap  : virtual public ClapTrap { ... };
class DiamondTrap : public ScavTrap, public FragTrap { ... };
```

Now there is **one shared `ClapTrap`**.

### The rule that comes with `virtual`

> The most-derived class must call the virtual base's constructor
> directly.

```cpp
DiamondTrap::DiamondTrap(std::string const& name)
    : ClapTrap(name + "_clap_name"),   // virtual base, here
      ScavTrap(name),
      FragTrap(name),
      m_name(name) {
    hitPoints    = FragTrap::hitPoints;     // or just 100
    energyPoints = ScavTrap::energyPoints;  // or just 50
    attackDamage = FragTrap::attackDamage;  // or just 30
    std::cout << "DiamondTrap " << name << " constructed" << std::endl;
}
```

Without virtual inheritance, the `ClapTrap(name + "_clap_name")`
call would be illegal (can't call a grandparent's ctor directly).
With virtual, it's mandatory.

### The name-shadowing trick

`DiamondTrap` has its own private `std::string name` — same
identifier as `ClapTrap::name`. Two members, same name, different
stores.

- `DiamondTrap::name` — human's name, e.g. `"Stevie"`.
- `ClapTrap::name` — robot designation, e.g. `"Stevie_clap_name"`.

`whoAmI()` prints both:

```cpp
void DiamondTrap::whoAmI() {
    std::cout << "I am " << m_name
              << " and my ClapTrap name is " << ClapTrap::name
              << std::endl;
}
```

Qualified name (`ClapTrap::name`) reaches past the shadow.

### `attack` dispatch

Subject wants `DiamondTrap::attack` to use `ScavTrap::attack`:

```cpp
using ScavTrap::attack;   // bring ScavTrap's attack into scope
```

Or write a wrapper:

```cpp
void DiamondTrap::attack(std::string const& target) {
    ScavTrap::attack(target);
}
```

### Tips
- **Ctor/dtor count check.** With virtual inheritance, `ClapTrap`'s
  ctor fires **once** during `DiamondTrap` construction. That's the
  moulinette's smoke test — if you see it twice, the `virtual` isn't
  wired.
- **-Wshadow hint.** The shadowing of `name` is intentional; if
  your build has `-Wshadow`, silence selectively or accept the
  warning for this file.
- **OCF on `DiamondTrap`.** Yes, all four. Copy ctor and
  copy-assign forward to both parents (which both share the
  single virtual base, so no double-work).

### Gotcha
`ScavTrap(name)` and `FragTrap(name)` each try to initialise
`ClapTrap` with `name` too (they don't know `DiamondTrap` already
handled it). With `virtual public ClapTrap`, their `ClapTrap(name)`
calls are **ignored** — only `DiamondTrap`'s call counts. This is
why the most-derived-class rule exists.

---

## What this module leaves you with

- You know what a **subobject** is and how it lives inside a
  derived object.
- Ctor/dtor chains are automatic mental imagery — you can predict
  trace order without running the code.
- You understand **name hiding** and **static dispatch** well
  enough that `virtual` (Mod 04) will feel like a fix, not
  a surprise.
- You've held a diamond in your hand and resolved it.

Everything in Mod 04 assumes this — especially the virtual-destructor
rule, which lands immediately in ex01.
