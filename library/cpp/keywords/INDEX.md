# 🔑 C++98 Keywords — Per-Word Reference

One file per keyword (or tightly-related group), each focused on **what it does, how it's implemented, how it behaves on the hardware, tips & tricks, and ASCII visualizations**. Written for a 42 student going through the CPP modules — designed so you actually understand what the compiler is doing.

> Every page cross-links back to the relevant topic file in `cpp/fundamentals`, `cpp/oop`, `cpp/advanced`, etc. The keyword files are the *zoom-in*; the topic files are the *zoom-out*.

---

## 🧱 Storage & qualifiers

| Keyword | What it controls |
|---|---|
| [`CONST.md`](CONST.md) | "Won't change after init." Compiler enforcement + `.rodata` placement. |
| [`STATIC.md`](STATIC.md) | Three jobs: internal linkage, persistent locals, class-wide members. |
| [`VOLATILE.md`](VOLATILE.md) | Don't optimize reads/writes — for memory-mapped I/O and signals. |
| [`MUTABLE.md`](MUTABLE.md) | The legitimate `const`-escape for caches and mutexes. |
| [`EXPLICIT.md`](EXPLICIT.md) | "Don't convert behind my back." Single-argument constructors. |
| [`INLINE.md`](INLINE.md) | A hint, a linker promise, and a header-file lifehack. |
| [`EXTERN.md`](EXTERN.md) | "Defined elsewhere" + `extern "C"` for C interop. |

## 🧬 Object orientation

| Keyword | What it controls |
|---|---|
| [`CLASS.md`](CLASS.md) | A `struct` that hides its members by default. Layout, sizeof, lifecycle. |
| [`STRUCT.md`](STRUCT.md) | Same as `class`, public by default. C-compat and POD rules. |
| [`PUBLIC_PRIVATE_PROTECTED.md`](PUBLIC_PRIVATE_PROTECTED.md) | Access control — compile-time only, no runtime cost. |
| [`THIS.md`](THIS.md) | The hidden first argument every member function gets. |
| [`FRIEND.md`](FRIEND.md) | Trapdoor through access control. Operators, tightly-coupled types. |
| [`VIRTUAL.md`](VIRTUAL.md) | Dynamic dispatch — vtable + vptr layout. |
| [`OPERATOR.md`](OPERATOR.md) | Overload built-in operators with normal functions. |

## 🧠 Memory, templates, scope

| Keyword | What it controls |
|---|---|
| [`NEW.md`](NEW.md) | Allocate + construct. Five forms; cookies; exception safety. |
| [`DELETE.md`](DELETE.md) | Destruct + deallocate. Form-matching is mandatory. |
| [`TEMPLATE.md`](TEMPLATE.md) | Code that writes code. Monomorphization. |
| [`TYPENAME.md`](TYPENAME.md) | Two jobs: type parameter + dependent-name disambiguator. |
| [`NAMESPACE.md`](NAMESPACE.md) | Logical boxes. ADL, anonymous namespaces, `using`. |
| [`TYPEDEF.md`](TYPEDEF.md) | Alias for an existing type. |
| [`USING.md`](USING.md) | Three jobs: bring-name-in, bring-namespace-in, re-expose-inherited. |
| [`ENUM.md`](ENUM.md) | Named integer constants. C++98 vs `enum class` (C++11+). |

## 🔀 Control flow

| Keyword | What it controls |
|---|---|
| [`SWITCH.md`](SWITCH.md) | Multi-way branching, fall-through, jump tables, `-Wswitch` on enums. |

## 🛡️ Exceptions & types

| Keyword | What it controls |
|---|---|
| [`TRY_CATCH_THROW.md`](TRY_CATCH_THROW.md) | Stack unwinding, catch order, exception safety. |
| [`SIZEOF.md`](SIZEOF.md) | Compile-time byte count of a type. |
| [`TYPEID.md`](TYPEID.md) | Runtime type info via vtable. |
| [`VOID.md`](VOID.md) | "No value." Functions, generic pointers, the `void*` escape. |
| [`BOOL.md`](BOOL.md) | Real boolean type. `true`/`false`/`std::boolalpha`. |

---

## 🗺️ Topic crossroads — when keywords meet topics

| Reading… | Then dive into… |
|---|---|
| [`fundamentals/BASICS.md`](../fundamentals/BASICS.md) | [`CLASS`](CLASS.md) · [`PUBLIC_PRIVATE_PROTECTED`](PUBLIC_PRIVATE_PROTECTED.md) · [`CONST`](CONST.md) · [`STATIC`](STATIC.md) · [`NAMESPACE`](NAMESPACE.md) |
| [`fundamentals/REFERENCE.md`](../fundamentals/REFERENCE.md) | [`CONST`](CONST.md) · [`THIS`](THIS.md) |
| [`fundamentals/MEMORY.md`](../fundamentals/MEMORY.md) | [`NEW`](NEW.md) · [`DELETE`](DELETE.md) · [`STATIC`](STATIC.md) |
| [`oop/ORTHODOX_CANONICAL_FORM.md`](../oop/ORTHODOX_CANONICAL_FORM.md) | [`OPERATOR`](OPERATOR.md) · [`THIS`](THIS.md) · [`EXPLICIT`](EXPLICIT.md) |
| [`oop/INHERITANCE.md`](../oop/INHERITANCE.md) | [`PUBLIC_PRIVATE_PROTECTED`](PUBLIC_PRIVATE_PROTECTED.md) · [`USING`](USING.md) · [`VIRTUAL`](VIRTUAL.md) |
| [`oop/POLYMORPHISM.md`](../oop/POLYMORPHISM.md) | [`VIRTUAL`](VIRTUAL.md) · [`TYPEID`](TYPEID.md) |
| [`oop/OPERATOR_OVERLOADING.md`](../oop/OPERATOR_OVERLOADING.md) | [`OPERATOR`](OPERATOR.md) · [`FRIEND`](FRIEND.md) |
| [`advanced/TEMPLATES.md`](../advanced/TEMPLATES.md) | [`TEMPLATE`](TEMPLATE.md) · [`TYPENAME`](TYPENAME.md) |
| [`advanced/CASTS.md`](../advanced/CASTS.md) | [`TYPEID`](TYPEID.md) · [`CONST`](CONST.md) · [`EXPLICIT`](EXPLICIT.md) |
| [`io-errors/ERROR_MANAGEMENT.md`](../io-errors/ERROR_MANAGEMENT.md) | [`TRY_CATCH_THROW`](TRY_CATCH_THROW.md) |
| [`tooling/MAKEFILE_CPP.md`](../tooling/MAKEFILE_CPP.md) | [`INLINE`](INLINE.md) · [`EXTERN`](EXTERN.md) |
| [`tooling/LIBRARIES.md`](../tooling/LIBRARIES.md) | [`EXTERN`](EXTERN.md) · [`STATIC`](STATIC.md) · [`INLINE`](INLINE.md) |

---

## 📐 What each page covers

Every keyword file follows the same skeleton so you know where to look:

1. **TL;DR** — one sentence to anchor the rest.
2. **What it does** — the language-level meaning.
3. **Implementation / hardware view** — what the compiler emits, what the linker sees, where the data lives in memory.
4. **Worked examples** — copy-pasteable C++98 snippets with explanations.
5. **Tips & tricks** — idioms, the 42-norm view, common-but-useful patterns.
6. **Common errors** — compiler / linker / runtime, with the quickest fix.
7. **Visual summary** — an ASCII diagram pulling the page together.
8. **Practice questions** — self-check at the end.

If a page doesn't follow this layout, treat it as a bug and fix it.
