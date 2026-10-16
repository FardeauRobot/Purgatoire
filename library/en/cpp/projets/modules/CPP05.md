# C++ Module 05 — Learning Guide

**Module theme:** *Repetition and Exceptions — throwing, catching,
and the `std::exception` hierarchy, wrapped around a growing
class chain (Bureaucrat → Form → Intern).*

Mod 04 taught you polymorphism. Mod 05 uses that foundation but
its *real* subject is **error handling the C++ way**: instead of
returning `-1` and hoping the caller checks it (the C habit),
you **throw** an exception and let it propagate up until someone
**catches** it. The four exercises are one continuous story — you
build a `Bureaucrat`, give it `Form`s to sign and execute, then
hire an `Intern` to fill the forms for you. Each step adds one new
idea on top of the last.

See also: [`lexique/`](../../lexique/INDEX.md) for `throw`/`try`/`catch`
keyword zoom-ins and [`oop/ORTHODOX_CANONICAL_FORM.md`](../../notions/oop/ORTHODOX_CANONICAL_FORM.md)
for the OCF rules that every non-exception class here must follow.

---

## The one sentence to memorise

> An **exception** is a value you `throw` at the point where you
> detect a problem; it unwinds the stack, running destructors on
> the way, until a matching `catch` handles it — decoupling *where
> the error happens* from *where it's dealt with*.

Real-life picture: a **fire alarm**. The person who smells smoke
(the low-level function) doesn't decide how to evacuate the
building — they just pull the alarm (`throw`). Everyone on the way
out closes their office door behind them (destructors run during
stack unwinding). The fire warden at the exit (`catch`) decides
what to actually do.

---

## What the whole module teaches you

- **`throw` / `try` / `catch`** — the three keywords of C++ error
  handling.
- **Custom exception classes** deriving from `std::exception`, with
  a `what()` method — and the C++98 `throw()` specifier quirk.
- **Exception safety**: because destructors run during unwinding,
  RAII (resources owned by objects) means no leaks even when things
  blow up. This is *why* the language leans on constructors/
  destructors so hard.
- **`const` members** and what they do to your Orthodox Canonical
  Form (the copy-assignment operator becomes a problem).
- A gentle **re-encounter with abstract classes** (ex02) and a
  **clean-dispatch pattern** to avoid `if/else` ladders (ex03).

Why it's useful: every non-trivial C++ program that touches files,
memory, network, or user input needs a disciplined way to signal
and recover from failure. `throw`/`catch` is that discipline, and
the STL itself throws (`std::out_of_range`, `std::bad_alloc`, …),
so you must be fluent in catching it.

---

## The mechanism, in one diagram

```
   deep_function()  ── detects bad grade ──►  throw GradeTooLow();
        │                                          │
        │  (stack unwinds: locals destructed)      │
        ▼                                          ▼
   middle_function()   ── no matching catch ──►  keeps propagating
        │                                          │
        ▼                                          ▼
   main() { try { ... } catch (std::exception& e) { e.what(); } }
                                     ▲
                          first matching handler wins
```

If **nobody** catches, `std::terminate()` is called and the
program aborts. A caught exception resumes execution right after
the `catch` block — not where the `throw` was.

---

## Exercise 00 — The Bureaucrat

### What you're learning
**How to declare, throw, and catch your own exception classes**,
using a class whose invariant (grade must stay in `[1, 150]`) is
enforced by throwing.

### New terms

- **Exception** — an object thrown to signal an error. Here, two
  of them: `GradeTooHighException`, `GradeTooLowException`.
- **`std::exception`** — the standard base class for all
  exceptions. Its only interface is `virtual const char* what()
  const throw();` — a human-readable message.
- **Nested class** — a class declared *inside* another
  (`Bureaucrat::GradeTooHighException`). It scopes the exception to
  its owner, which reads well and matches the subject.
- **Invariant** — a rule that must *always* hold for an object to
  be valid. Here: `1 <= grade <= 150`. Grade **1 is the highest**,
  **150 the lowest** — counter-intuitive, like chess/military
  ranks or "you're my number 1".

### Real-life picture
Think **corporate or military hierarchy**. A `Bureaucrat` has a
name (fixed for life — `const`) and a rank. You can *promote*
(increment → grade number goes **down**, toward 1) or *demote*
(decrement → grade number goes **up**, toward 150). Try to promote
someone already at grade 1, or demote someone at 150, and the
system refuses — it **throws**.

### The core snippet — declaring an exception class

This is the pattern you'll reuse in every exercise. Note the
`throw()` after `what()` — in **C++98** the standard's `what()`
carries an empty exception-specification, and to override it
cleanly your signature must match:

```cpp
#include <exception>

class Bureaucrat {
public:
    class GradeTooHighException : public std::exception {
    public:
        virtual const char* what() const throw();
    };
    class GradeTooLowException : public std::exception {
    public:
        virtual const char* what() const throw();
    };
    // ... rest of Bureaucrat
};
```

```cpp
// in the .cpp
const char* Bureaucrat::GradeTooHighException::what() const throw() {
    return "grade is too high (must be >= 1)";
}
```

### Throwing and catching

```cpp
// throwing — anywhere the invariant would break:
if (grade < 1)
    throw Bureaucrat::GradeTooHighException();

// catching — in main (or any caller):
try {
    Bureaucrat boss("Alice", 0);   // 0 is out of range → throws
}
catch (std::exception& e) {        // catch by reference to the BASE
    std::cerr << e.what() << std::endl;
}
```

Catch by **`std::exception&`** (base, by reference): one handler
covers both your exceptions *and* STL ones, and the reference
avoids slicing. The subject explicitly requires that your
exceptions be catchable this way.

### Where the throws live
- **Constructor** — validate the grade; throw if out of `[1,150]`.
  (Throwing from a ctor is fine — the object is simply never
  considered constructed, and no destructor runs for it.)
- **`increment` / `decrement`** — throw *before* stepping past the
  boundary.

### Tips
- Overload `operator<<` (free function, `std::ostream&`) to print
  `<name>, bureaucrat grade <grade>.` — return the stream so calls
  chain.
- Give a class-level `what()` message that's actually useful; you
  print it, so make it readable.
- Test **both** the throwing path and the happy path in `main`,
  each wrapped in its own `try/catch`.

### Gotcha
**The name is `const`** (`const std::string _name`). The moment a
member is `const`, the compiler-generated **copy-assignment
operator is deleted-in-spirit** — you cannot reassign a `const`
member, so `a = b;` won't compile if you try to assign `_name`.
Options: write `operator=` to copy only the non-const members (and
accept the name won't change), or leave assignment out if `main`
never uses it. Decide deliberately — don't let it surprise you.
*(Heads-up: your current `ex00/Bureaucrat.hpp` still has a
non-`const` `_name` and no `_grade` yet — bring it in line with the
subject.)*

---

## Exercise 01 — The Form

### What you're learning
**Objects that interact through exceptions and permissions** — one
class (`Form`) throws, another (`Bureaucrat`) catches and reports.

### New terms

- **`beSigned(const Bureaucrat&)`** — a method on `Form` that
  checks the bureaucrat's grade against the form's required
  *signing* grade. High enough → sets `signed = true`; too low →
  throws `Form::GradeTooLowException`.
- **Required grade** — each form carries two `const` grades: one to
  **sign** it, one to **execute** it (used in ex02). Both fixed at
  construction, both validated (throw if outside `[1,150]`).

### Real-life picture
A **purchase-order form**. It sits unsigned. A manager whose
authority (grade) is high enough can sign it; a junior employee
can't — the form "refuses their signature". The form itself owns
the rule about *who is allowed*; the employee just tries and
handles the rejection.

### Class shape
- `Form` — `const std::string _name`, `bool _signed` (false at
  construction), `const int _gradeToSign`, `const int _gradeToExec`.
  All **private** (the subject is explicit: private, not
  protected — that matters in ex02).
- Same two nested exception classes as `Bureaucrat`, but scoped to
  `Form`.
- `Bureaucrat::signForm(Form&)` — calls `beSigned`, and prints
  either `<bureaucrat> signed <form>` or
  `<bureaucrat> couldn't sign <form> because <reason>.`

### The interaction snippet

```cpp
void Bureaucrat::signForm(Form& f) {
    try {
        f.beSigned(*this);
        std::cout << _name << " signed " << f.getName() << std::endl;
    }
    catch (std::exception& e) {
        std::cout << _name << " couldn't sign " << f.getName()
                  << " because " << e.what() << "." << std::endl;
    }
}
```

Notice the division of labour: `Form::beSigned` **throws**;
`Bureaucrat::signForm` **decides what to do about it** (print a
friendly line, don't crash). That separation is the whole point of
exceptions.

### Grade comparison — the mental trap
"High enough" means the **grade number is small enough**. A
bureaucrat can sign if `bureaucrat.grade <= form.gradeToSign`
(lower number = more authority). Write it once, carefully, and
comment the `<=` if it helps — this is the #1 logic bug in the
module.

### Tips
- `operator<<` for `Form` should dump all its state (name, signed
  status, both grades) — handy for debugging.
- `_signed` starts `false`; only `beSigned` flips it.
- Getters are `const`; the grade members being `const` means (like
  ex00) your `operator=` can only copy `_signed`.

### Gotcha
Forward declaration / include order: `Form` needs to know about
`Bureaucrat` and vice-versa. Use a **forward declaration**
(`class Bureaucrat;`) in one header and include the full definition
in the `.cpp` to break the circular `#include`.

---

## Exercise 02 — Abstract Form & concrete forms

### What you're learning
**Combining exceptions with polymorphism**: `Form` becomes
**abstract** (`AForm`), and each concrete form does its own thing
when executed — but the *permission checks* live once, in the base.

### New terms

- **`AForm`** — the abstract base (rename of `Form`). Add a **pure
  virtual** action so the class can't be instantiated directly:
  ```cpp
  virtual void execute(Bureaucrat const& executor) const = 0;
  ```
  (Or keep `execute` concrete in the base for the checks and make a
  protected pure-virtual `action()` — see the elegant pattern
  below.)
- **Concrete forms** — three subclasses, each with its own sign/
  exec grades and its own action:

  | Form | sign | exec | Action |
  |---|---|---|---|
  | `ShrubberyCreationForm` | 145 | 137 | writes `<target>_shrubbery` with ASCII trees |
  | `RobotomyRequestForm` | 72 | 45 | drilling noises; robotomizes target **50%** of the time |
  | `PresidentialPardonForm` | 25 | 5 | announces target pardoned by Zaphod Beeblebrox |

- **New exceptions** you'll want: something like
  `AForm::FormNotSignedException` and an execution grade check that
  reuses `GradeTooLowException`.

### Real-life picture
A **government office with different paperwork**. Every form shares
the same front-desk rule: *"is it signed? is your clearance high
enough to execute it?"* — that check is identical for all forms, so
it lives at the front desk (the base class). But *what the form
actually does* — plant a shrubbery, lobotomize someone, grant a
pardon — is specific to each form (the override).

### The elegant pattern (the subject hints at it)
Do the **checks once** in the base, then delegate the unique work
to a virtual hook. This is the Template-Method pattern:

```cpp
// AForm.cpp — concrete, in the base:
void AForm::execute(Bureaucrat const& executor) const {
    if (!_signed)
        throw AForm::FormNotSignedException();
    if (executor.getGrade() > _gradeToExec)
        throw AForm::GradeTooLowException();
    this->action();        // ← pure virtual, each form overrides
}
```

```cpp
// each concrete form:
protected:
    virtual void action() const;   // does the shrubbery/robotomy/pardon
```

You write the grade/signed check **once**; every new form only
supplies its `action()`. Compare that to copy-pasting the same two
`if`s into three `execute()`s — the base-class version is why the
subject calls it "more elegant".

### Reaching private base members
`AForm`'s attributes are **private**, but the concrete forms need
their names/grades. Set them by passing values **up to the base
constructor** in the initializer list; read them via **protected
getters**. Don't make the members `protected` — that's exactly
what the subject forbids.

```cpp
ShrubberyCreationForm::ShrubberyCreationForm(std::string const& target)
    : AForm("ShrubberyCreationForm", 145, 137), _target(target) {}
```

### Tips
- **Virtual destructor on `AForm`** (reflex from Mod 04) — you
  delete concrete forms through `AForm*`.
- `ShrubberyCreationForm` uses `<fstream>` (`std::ofstream`) to
  write the file — RAII means the file closes even if something
  throws.
- `RobotomyRequestForm`'s 50% — `std::rand()` seeded once; don't
  over-engineer randomness for a piscine exercise.
- Add `Bureaucrat::executeForm(AForm const&)` mirroring `signForm`:
  try `execute`, print `<bureaucrat> executed <form>` or the error.

### Gotcha
An `AForm` reference can point to any concrete form —
`execute()` dispatches virtually to the right `action()`. But if
you forgot the virtual destructor, deleting through `AForm*` leaks
the concrete part. Valgrind will tell you.

---

## Exercise 03 — The Intern

### What you're learning
**Clean dispatch without `if/else if/else` ladders** — building an
`AForm*` of the right type from a *string name*.

### New terms

- **`Intern`** — a class with no name and no grade. One method:
  `AForm* makeForm(std::string const& formName, std::string const&
  target);`. Returns a heap-allocated form of the matching type, or
  handles the "unknown form name" case (print an error / return
  `0`). Prints `Intern creates <form>` on success.

### Real-life picture
You walk up to the **intern** and say *"get me a robotomy request
for Bender."* You don't care *how* they find the right form in the
cabinet — they hand you the correct one, filled with your target.
`makeForm("robotomy request", "Bender")` → a `RobotomyRequestForm*`.

### The forbidden approach
The subject **explicitly bans** this:

```cpp
// ❌ do NOT do this — the subject penalises it:
if (name == "shrubbery creation") return new ShrubberyCreationForm(target);
else if (name == "robotomy request") return new RobotomyRequestForm(target);
else if (name == "presidential pardon") return new PresidentialPardonForm(target);
```

### The clean pattern — a lookup table of function pointers
Pair each name with a **maker function**, loop once, call through a
pointer. This is the technique the exercise is testing; the exact
wiring is yours to write:

```cpp
// a private static helper per form type — same signature:
//   static AForm* makeShrubbery(std::string const& target)
//       { return new ShrubberyCreationForm(target); }

AForm* Intern::makeForm(std::string const& name, std::string const& target) {
    std::string       names[]  = { "shrubbery creation",
                                   "robotomy request",
                                   "presidential pardon" };
    AForm* (*makers[])(std::string const&) = { /* &makeShrubbery, ... */ };

    for (int i = 0; i < 3; ++i)
        if (names[i] == name)
            return makers[i](target);

    // unknown name: print error, return 0
    return 0;
}
```

Why it's better: adding a fourth form = add one row to each array,
touch nothing else. No branching logic to re-read.

### Tips
- **Who deletes the form?** `makeForm` `new`s it; the caller owns
  it and must `delete`. Test that in `main` and check with
  valgrind.
- The reader of the returned `AForm*` typically signs it and
  executes it — that's the full end-to-end test of the module.
- `AForm* (*makers[])(std::string const&)` is a scary type; a
  `typedef` makes it readable:
  ```cpp
  typedef AForm* (*FormMaker)(std::string const&);
  ```

### Gotcha
On an **unknown form name**, don't `new` anything and don't leak —
just report and return `0`. And guard the caller: if `makeForm`
can return `0`, the caller must check before `delete`/`execute`.

---

## What this module leaves you with

- You can declare, throw, and catch custom exceptions, and you know
  the C++98 `what() const throw()` signature by heart.
- You catch by `std::exception&` (base, by reference) and know why
  (one handler, no slicing).
- You understand *why* RAII and exceptions are the same story:
  destructors run during unwinding, so resources are safe.
- You've felt what a `const` member does to your OCF, and made a
  deliberate choice about `operator=`.
- You can replace an `if/else` type-dispatch ladder with a
  function-pointer table without thinking.

After Mod 05, "handle the error where it makes sense, not where it
happens" is a reflex — and you're ready for templates (Mod 06/07)
where the STL will throw at you constantly.
