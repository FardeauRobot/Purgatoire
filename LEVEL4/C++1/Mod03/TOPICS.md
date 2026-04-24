# C++ Module 03 — TOPICS

## Theme
**Inheritance** — public inheritance, constructor/destructor chains,
name hiding, and the **diamond problem** resolved with `virtual`
inheritance. The module is structured as a cumulative hierarchy
around a silly robot family.

## Global rules
Same as previous modules — c++98, `-Wall -Wextra -Werror`, no STL,
no leaks. **Every class must follow Orthodox Canonical Form.**

---

## Cross-cutting concepts (read before starting)

| Concept | Where |
|---|---|
| Inheritance: modes, ctor/dtor order, memory layout, diamond | [library/cpp/oop/INHERITANCE.md](../../../library/cpp/oop/INHERITANCE.md) |
| OCF — four members, deep vs shallow, pitfalls | [library/cpp/oop/ORTHODOX_CANONICAL_FORM.md](../../../library/cpp/oop/ORTHODOX_CANONICAL_FORM.md) |
| References (used everywhere in ctor signatures) | [library/cpp/fundamentals/REFERENCE.md](../../../library/cpp/fundamentals/REFERENCE.md) |
| Per-exercise walkthrough (companion) | [library/cpp/modules/CPP03.md](../../../library/cpp/modules/CPP03.md) |

### The mental model — "is-a"

`ScavTrap` **is-a** `ClapTrap`. Every `ScavTrap` has, underneath it,
a full `ClapTrap` subobject. The class declaration
`class ScavTrap : public ClapTrap` says this explicitly. Consequences:

- A `ScavTrap` can be passed where a `ClapTrap&` is expected.
- A `ScavTrap*` can be implicitly converted to a `ClapTrap*`
  (upcasting, always safe).
- Constructing a `ScavTrap` **requires** constructing its `ClapTrap`
  subobject first — the base ctor runs before the derived ctor's
  body.
- Destruction is the mirror: derived dtor body first, then base
  dtor.

### Construction order — why

When `ScavTrap s("Steve")` runs, the compiler emits:

```
1. Allocate storage for a ScavTrap (which includes a ClapTrap subobject).
2. Call ClapTrap(std::string) on the embedded ClapTrap portion.
3. Initialise ScavTrap's own members.
4. Run ScavTrap's ctor body.
```

The *derived* class cannot run its body first because it may depend
on base-class state (e.g. `attack()` might call `takeDamage` which
modifies `ClapTrap::hp`). The base must be fully alive before the
derived ctor can safely touch anything.

### Destruction order — exact mirror

```
1. Run ~ScavTrap() body.
2. Destroy ScavTrap's own members.
3. Call ~ClapTrap() on the subobject.
4. Release the storage.
```

The subject makes you print messages in every ctor and dtor so you
can **see** this sequence in the terminal.

---

## Exercise 00 — Aaaaand… OPEN! (`ClapTrap`)

### Goal
The base class. No inheritance yet — you're building the
foundation.

- **Private:** `std::string name`, `unsigned int hitPoints`
  (start 10), `energyPoints` (start 10), `attackDamage` (start 0).
- **Members:** `attack(std::string const& target)`,
  `takeDamage(unsigned int)`, `beRepaired(unsigned int)`.
- Each action costs 1 energy point; if HP is 0 or energy is 0,
  nothing happens (print a "can't act" message).
- Full OCF (4 members) + **every ctor, dtor and action prints**.

### What the exercise drills

- **A class with non-default-initialisable-looking data** — all four
  private fields need to start at specific values. Use the
  **initialisation list**: `ClapTrap::ClapTrap(std::string name) :
  name(name), hitPoints(10), energyPoints(10), attackDamage(0) { ... }`.
- **Name-taking constructor is the primary ctor.** The default ctor
  either forbids construction or defaults the name to `"default"`.
  Subject's sample output typically uses named ctors, so the default
  ctor is rarely invoked — but OCF demands it exists.
- **Action guards.** Print a refusal message if HP or energy is 0;
  otherwise perform the action and print the success message.

### "These early ClapTraps don't interact"

The subject is explicit: `attack(target)` takes a `std::string` —
the name of something being attacked, not another `ClapTrap`. No
shared state, no actual damage dealt. `takeDamage` and `beRepaired`
are called directly in `main`, not wired through `attack`. Keep it
simple.

### Hardware-level notes

- `sizeof(ClapTrap)` is roughly `sizeof(std::string) + 3 *
  sizeof(unsigned int)` — 24-32 for the string on libstdc++, plus
  12 for the three counters, plus any padding for alignment
  (typically padded to 40 bytes).
- An `unsigned int` is 4 bytes on every platform you'll see at 42.
  It wraps on underflow — `takeDamage(100)` when HP is 10 sets HP
  to `4294967206`, not 0. Clamp or check.
- The `std::string` member means the object owns a heap buffer (if
  the name is longer than the SSO threshold, ~15 chars). The copy
  ctor / copy-assign must copy-construct the string, which
  allocates fresh storage. OCF's deep-copy guarantee comes for
  free because `std::string` handles it.

### Tips & gotchas

- **Unsigned underflow.** `takeDamage(1000)` on 10 HP wraps to a
  huge value unless you clamp: `hp = (damage >= hp) ? 0 : hp - damage;`.
- **Energy-0 refusal.** Both `attack` and `beRepaired` need to
  check *and* print the refusal. `takeDamage` does **not** cost
  energy (the subject is consistent about this — re-read).
- **OCF on a class owning only primitives + `std::string`.** The
  compiler-generated copy ctor and copy-assign already do the
  right thing. You still write them explicitly because the
  subject grades the traces.
- **Init-list order follows declaration order.** `name`, `hp`,
  `ep`, `atk` — write them in the same order in the init list or
  you get a `-Wreorder` warning (which is `-Werror`-ed into a
  fail).

### Search terms
`"c++ member initialization list order"`,
`"c++ unsigned int underflow"`,
`"orthodox canonical form example"`.

### Cross-reference
- [library/cpp/oop/ORTHODOX_CANONICAL_FORM.md](../../../library/cpp/oop/ORTHODOX_CANONICAL_FORM.md)
- [library/cpp/modules/CPP03.md §ex00](../../../library/cpp/modules/CPP03.md)

---

## Exercise 01 — Serena, my love! (`ScavTrap`)

### Goal
`ScavTrap : public ClapTrap`. Different stats (HP 100, energy 50,
damage 20). Different ctor/dtor/attack messages. New method
`guardGate()`.

### What the exercise drills

- **Public inheritance syntax.** `class ScavTrap : public ClapTrap`.
- **Explicitly calling the base ctor.** The init list can name the
  base: `ScavTrap(std::string name) : ClapTrap(name) { ... set
  ScavTrap stats ... }`. The `ClapTrap(name)` call constructs the
  base subobject first; then you override the stats in the body.
- **Ctor message chain.** When `ScavTrap s("Steve")` runs, you
  see: `"ClapTrap Steve constructed"` then `"ScavTrap Steve
  constructed"`. Dtor order is the opposite.
- **Overriding `attack`.** Non-virtual override in this module —
  `ScavTrap::attack` shadows `ClapTrap::attack`. When you call
  `s.attack("target")` on a `ScavTrap`, you get `ScavTrap::attack`.
  When you call `cp_ref.attack("target")` on a `ClapTrap&` pointing
  to a `ScavTrap`, you get `ClapTrap::attack` — **this is static
  dispatch**, and it's why Mod 04 introduces `virtual`.
- **Base's private members are not accessible.** You write `hp`,
  `ep`, `atk` from the `ScavTrap` ctor body? Compile error — the
  base declared them `private`. Options:
  - Expose setters (`setHitPoints(int)`) from `ClapTrap`.
  - Declare the fields **`protected`** in `ClapTrap` (subject
    accepts either).

### Why ctor-body reassignment is fine here

The ClapTrap ctor already set HP to 10. The ScavTrap ctor body
overwrites it to 100. A tiny inefficiency (two stores instead of
one) — but avoids having to add a protected ctor that takes the
stats. The subject hints toward this pattern.

Alternative: make `ClapTrap`'s fields `protected` and write a second
`ClapTrap(std::string name, int hp, int ep, int atk)` protected
constructor that the derived class's init list can call.

### Hardware-level notes

- A `ScavTrap` is laid out as:
  ```
  offset 0: [ ClapTrap subobject (std::string, hp, ep, atk) ]
  offset N: [ ScavTrap's own fields, if any ]
  ```
  `ScavTrap` has no new fields in this exercise, so `sizeof(ScavTrap)
  == sizeof(ClapTrap)`. The **vtable pointer** doesn't enter yet
  because nothing is virtual in Mod 03.
- Upcasting a `ScavTrap*` to a `ClapTrap*` is a **no-op at the
  machine level** — they point to the same address. The type
  system is the only thing that changes.
- `guardGate()` is a method on `ScavTrap` only. Calling it through
  a `ClapTrap*` is a compile error — static type controls method
  lookup.

### Tips & gotchas

- **Ctor body order inside the derived ctor.** The base ctor is
  already done. Override the stats, *then* print the derived
  message. Otherwise the message appears before the stats reflect
  the derived values — confusing output, and if moulinette checks
  state after the ctor, wrong.
- **`attack` override without `virtual`.** This is **shadowing**,
  not overriding. You lose polymorphism. The subject doesn't use
  virtuals here on purpose — Mod 04 makes the contrast.
- **Copy ctor forwards to base.** `ScavTrap(ScavTrap const& src)
  : ClapTrap(src) { ... }` — passing a `ScavTrap const&` where
  `ClapTrap const&` is expected slices the object at parameter
  binding, which is safe *because the copy ctor only needs the
  base-subobject view*.

### Search terms
`"c++ derived class constructor base initializer"`,
`"c++ protected member inheritance"`,
`"c++ override without virtual shadowing"`.

### Cross-reference
- [library/cpp/oop/INHERITANCE.md §5](../../../library/cpp/oop/INHERITANCE.md) — calling base ctor
- [library/cpp/oop/INHERITANCE.md §8](../../../library/cpp/oop/INHERITANCE.md) — name hiding

---

## Exercise 02 — Repetitive work (`FragTrap`)

### Goal
`FragTrap : public ClapTrap`. HP 100, energy 100, damage 30.
Distinct ctor/dtor messages. New method `highFivesGuys()`.

### What the exercise drills

This exercise is a **drill**: same shape as ex01 with different
numbers and a different signature method. The point is to make you
internalise the inheritance pattern by repetition.

### What's different from ex01

- Stats differ (100/100/30 vs 100/50/20).
- `highFivesGuys()` is the new signature method instead of
  `guardGate()`.
- Messages use "FragTrap" instead of "ScavTrap".

That's it. If ex01 felt natural, ex02 is 10 minutes of
copy-and-rename.

### The exam-scented repetition trap

It's tempting to copy-paste `ScavTrap.cpp` → `FragTrap.cpp` and
swap names. Fine — but **read what you've written**. The point
isn't the code, it's that the inheritance shape becomes muscle
memory. If you paste without reading, ex03 (the diamond) will hit
you with surprise because the pattern cracked under multiple
inheritance and you never noticed.

### Tips & gotchas

- **Copy ctor / copy assign messages.** Subject expects them
  printed too. Don't skip them because they feel redundant — the
  moulinette tests `FragTrap b(a);` and expects the trace.
- **HP, energy, damage are `unsigned int`.** Same clamping rules as
  ex00.

### Search terms
Same as ex01.

---

## Exercise 03 — Now it's weird! (`DiamondTrap`, bonus)

### Goal
Four-class diamond:

```
        ClapTrap
         /    \
   ScavTrap  FragTrap
         \    /
       DiamondTrap
```

- `DiamondTrap : public FragTrap, public ScavTrap` — multiple
  inheritance.
- Private `std::string name` **with the same identifier** as
  `ClapTrap::name`. The shadowing is intentional — it's what the
  subject wants you to practise.
- Constructor receives a name, sets `ClapTrap::name = name +
  "_clap_name"`.
- Attributes: HP from `FragTrap`, energy from `ScavTrap`, damage
  from `FragTrap`. Uses `ScavTrap::attack()`.
- `void whoAmI();` prints both its own name and its `ClapTrap` name.
- **The `ClapTrap` subobject must exist exactly once.**

### The problem this exercise exposes

Without `virtual` inheritance, the diamond has **two** `ClapTrap`
subobjects:

```
DiamondTrap
├── FragTrap
│   └── ClapTrap  ← copy 1
└── ScavTrap
    └── ClapTrap  ← copy 2
```

Two sets of `hitPoints`, two sets of `name`, two ctors running, two
dtors running. Ambiguity everywhere: which `hp` does
`DiamondTrap::takeDamage` touch?

### The fix — `virtual` inheritance

Change both intermediate classes:

```cpp
class ScavTrap : virtual public ClapTrap { ... };
class FragTrap : virtual public ClapTrap { ... };
class DiamondTrap : public ScavTrap, public FragTrap { ... };
```

Now the `ClapTrap` subobject exists **once**. Both `ScavTrap` and
`FragTrap` share the same base. But this introduces a new rule:

> The **most-derived class** must call the virtual base's constructor
> directly.

So `DiamondTrap`'s init list must name `ClapTrap`:

```cpp
DiamondTrap(std::string name)
    : ClapTrap(name + "_clap_name"),    // virtual base, called here
      ScavTrap(name),
      FragTrap(name),
      name(name) { ... }
```

### What virtual inheritance costs

- **Each virtually-inherited class carries a pointer** to the shared
  base subobject (sometimes called a *vbtbl pointer*). Extra 8
  bytes per intermediate class.
- **Construction goes through an extra indirection.** `ScavTrap`'s
  ctor can no longer construct the `ClapTrap` subobject directly
  because it doesn't know whether it's the most-derived class.
  `DiamondTrap` does.
- This is why the standard rule exists: the most-derived class
  "owns" the virtual base's construction. If `ScavTrap` is used
  standalone (not via `DiamondTrap`), it *is* the most-derived and
  constructs `ClapTrap` itself.

See [library/cpp/oop/INHERITANCE.md §10](../../../library/cpp/oop/INHERITANCE.md).

### The name-shadowing trick

`DiamondTrap` has its own private `name` member — same identifier
as `ClapTrap::name`. This is intentional:

- `DiamondTrap::name` — the human's actual name (e.g. "Stevie").
- `ClapTrap::name` — the robot designation
  (e.g. "Stevie_clap_name"), set by the `DiamondTrap` ctor.

`whoAmI()` prints both:

```cpp
void DiamondTrap::whoAmI() {
    std::cout << "I am " << this->name
              << " and my ClapTrap name is " << ClapTrap::name
              << std::endl;
}
```

The `ClapTrap::name` qualified name reaches through the hierarchy
past the shadowing. Without `virtual` inheritance, `ClapTrap::name`
would be ambiguous (two copies); with it, there's one.

### The `-Wshadow` hint

The subject hints at `-Wshadow` for a reason: the shadowing of
`name` is exactly the thing `-Wshadow` would complain about. You
have to live with the warning (or silence it selectively) because
the shadowing is the point.

### Hardware-level notes — what the diamond actually looks like

Without virtual inheritance:

```
offset 0:  [ FragTrap ]
  offset 0:  [ ClapTrap ]  ← copy 1
  offset A:  [ FragTrap's own members ]
offset B:  [ ScavTrap ]
  offset B:  [ ClapTrap ]  ← copy 2
  offset C:  [ ScavTrap's own members ]
offset D:  [ DiamondTrap's own members ]
```

With virtual inheritance:

```
offset 0:  [ vbtbl ptr -> points to shared ClapTrap ]
           [ FragTrap's own members ]
offset N:  [ vbtbl ptr -> points to same shared ClapTrap ]
           [ ScavTrap's own members ]
offset M:  [ DiamondTrap's own members ]
offset P:  [ ClapTrap ]  ← one shared copy
```

The pointers can be offsets or true pointers depending on the ABI.
What matters: **one `ClapTrap`, reached through a level of
indirection**.

### Tips & gotchas

- **Virtual-base rule.** Most-derived class initialises the virtual
  base. If you forget, intermediate ctors will *implicitly*
  default-construct `ClapTrap`, and your `name + "_clap_name"`
  trick is lost.
- **Init-list ordering is still declaration order.** Even with
  virtual bases, the compiler reorders if needed, but write them
  matching the class declaration to avoid warnings.
- **`ScavTrap::attack()` dispatch.** `using ScavTrap::attack;` in
  `DiamondTrap` brings the name into scope unambiguously. Or
  write `void attack(std::string const& t) { ScavTrap::attack(t); }`
  explicitly.
- **Ctor/dtor message count.** With virtual inheritance, `ClapTrap`'s
  ctor fires **once**. Without, twice. The subject's expected output
  is the one-ctor version — that's how you know the `virtual` is
  wired right.

### Search terms
`"c++ diamond problem virtual inheritance"`,
`"c++ virtual base class most derived constructor"`,
`"c++ multiple inheritance name ambiguity"`,
`"-Wshadow c++"`.

### Cross-reference
- [library/cpp/oop/INHERITANCE.md §9-10](../../../library/cpp/oop/INHERITANCE.md)
- [library/cpp/modules/CPP03.md §ex03](../../../library/cpp/modules/CPP03.md)

---

## Module-level sanity checks before moulinette

- Ctor/dtor trace matches subject output exactly — order and count.
- ex03: `ClapTrap` ctor fires **once** during `DiamondTrap`
  construction, not twice.
- `valgrind` clean — the string members and their deep copies must
  not leak.
- `-Wall -Wextra -Werror` — zero warnings. `-Wshadow` may be on or
  off depending on your Makefile; if on, ex03 needs a pragma or
  a rename.
- OCF on every class. Yes, even `DiamondTrap` — write its four
  explicitly.

You can pass the module without ex03.
