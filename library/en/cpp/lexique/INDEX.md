# 🔑 LEXIQUE — Every C++98 Keyword, One Click Away

One page per keyword (or tightly-related group): what it does, how it's implemented, hardware behavior, tips, ASCII diagrams. **All 63 C++98 keywords are covered** — find yours in the A→Z table, click, understand, leave.

> Looking for a *concept* rather than a keyword (RAII, lvalue, deep copy, vtable…)? That's the [`GLOSSAIRE.md`](GLOSSAIRE.md).
> Zoom-out topic files live in [`../notions/`](../notions/INDEX.md); project walkthroughs in [`../projets/`](../projets/INDEX.md).

---

## 🔤 A→Z — all 63 keywords

| Keyword | Page | | Keyword | Page |
|---|---|---|---|---|
| `asm` | [`LEGACY_KEYWORDS.md`](LEGACY_KEYWORDS.md) | | `namespace` | [`NAMESPACE.md`](NAMESPACE.md) |
| `auto` ⚠️ | [`LEGACY_KEYWORDS.md`](LEGACY_KEYWORDS.md) | | `new` | [`NEW.md`](NEW.md) |
| `bool` | [`BOOL.md`](BOOL.md) | | `operator` | [`OPERATOR.md`](OPERATOR.md) |
| `break` | [`LOOPS.md`](LOOPS.md) | | `private` | [`PUBLIC_PRIVATE_PROTECTED.md`](PUBLIC_PRIVATE_PROTECTED.md) |
| `case` | [`SWITCH.md`](SWITCH.md) | | `protected` | [`PUBLIC_PRIVATE_PROTECTED.md`](PUBLIC_PRIVATE_PROTECTED.md) |
| `catch` | [`TRY_CATCH_THROW.md`](TRY_CATCH_THROW.md) | | `public` | [`PUBLIC_PRIVATE_PROTECTED.md`](PUBLIC_PRIVATE_PROTECTED.md) |
| `char` | [`TYPES.md`](TYPES.md) | | `register` | [`LEGACY_KEYWORDS.md`](LEGACY_KEYWORDS.md) |
| `class` | [`CLASS.md`](CLASS.md) | | `reinterpret_cast` | [`CASTS.md`](CASTS.md) |
| `const` | [`CONST.md`](CONST.md) | | `return` | [`RETURN_GOTO.md`](RETURN_GOTO.md) |
| `const_cast` | [`CASTS.md`](CASTS.md) | | `short` | [`TYPES.md`](TYPES.md) |
| `continue` | [`LOOPS.md`](LOOPS.md) | | `signed` | [`TYPES.md`](TYPES.md) |
| `default` | [`SWITCH.md`](SWITCH.md) | | `sizeof` | [`SIZEOF.md`](SIZEOF.md) |
| `delete` | [`DELETE.md`](DELETE.md) | | `static` | [`STATIC.md`](STATIC.md) |
| `do` | [`LOOPS.md`](LOOPS.md) | | `static_cast` | [`CASTS.md`](CASTS.md) |
| `double` | [`TYPES.md`](TYPES.md) | | `struct` | [`STRUCT.md`](STRUCT.md) |
| `dynamic_cast` | [`CASTS.md`](CASTS.md) | | `switch` | [`SWITCH.md`](SWITCH.md) |
| `else` | [`IF_ELSE.md`](IF_ELSE.md) | | `template` | [`TEMPLATE.md`](TEMPLATE.md) |
| `enum` | [`ENUM.md`](ENUM.md) | | `this` | [`THIS.md`](THIS.md) |
| `explicit` | [`EXPLICIT.md`](EXPLICIT.md) | | `throw` | [`TRY_CATCH_THROW.md`](TRY_CATCH_THROW.md) |
| `export` | [`LEGACY_KEYWORDS.md`](LEGACY_KEYWORDS.md) | | `true` / `false` | [`BOOL.md`](BOOL.md) |
| `extern` | [`EXTERN.md`](EXTERN.md) | | `try` | [`TRY_CATCH_THROW.md`](TRY_CATCH_THROW.md) |
| `float` | [`TYPES.md`](TYPES.md) | | `typedef` | [`TYPEDEF.md`](TYPEDEF.md) |
| `for` | [`LOOPS.md`](LOOPS.md) | | `typeid` | [`TYPEID.md`](TYPEID.md) |
| `friend` | [`FRIEND.md`](FRIEND.md) | | `typename` | [`TYPENAME.md`](TYPENAME.md) |
| `goto` | [`RETURN_GOTO.md`](RETURN_GOTO.md) | | `union` | [`UNION.md`](UNION.md) |
| `if` | [`IF_ELSE.md`](IF_ELSE.md) | | `unsigned` | [`TYPES.md`](TYPES.md) |
| `inline` | [`INLINE.md`](INLINE.md) | | `using` | [`USING.md`](USING.md) |
| `int` | [`TYPES.md`](TYPES.md) | | `virtual` | [`VIRTUAL.md`](VIRTUAL.md) |
| `long` | [`TYPES.md`](TYPES.md) | | `void` | [`VOID.md`](VOID.md) |
| `mutable` | [`MUTABLE.md`](MUTABLE.md) | | `volatile` | [`VOLATILE.md`](VOLATILE.md) |
| | | | `wchar_t` | [`TYPES.md`](TYPES.md) · `while` → [`LOOPS.md`](LOOPS.md) |

> ⚠️ `auto` is the classic eval trap — in C++98 it does **not** mean type deduction. See [`LEGACY_KEYWORDS.md`](LEGACY_KEYWORDS.md).

---

## 🗂️ By theme

| Theme | Pages |
|---|---|
| **Types & values** | [`TYPES`](TYPES.md) · [`BOOL`](BOOL.md) · [`VOID`](VOID.md) · [`ENUM`](ENUM.md) · [`UNION`](UNION.md) · [`SIZEOF`](SIZEOF.md) · [`TYPEDEF`](TYPEDEF.md) |
| **Control flow** | [`IF_ELSE`](IF_ELSE.md) · [`LOOPS`](LOOPS.md) · [`SWITCH`](SWITCH.md) · [`RETURN_GOTO`](RETURN_GOTO.md) |
| **Storage & qualifiers** | [`CONST`](CONST.md) · [`STATIC`](STATIC.md) · [`VOLATILE`](VOLATILE.md) · [`MUTABLE`](MUTABLE.md) · [`EXPLICIT`](EXPLICIT.md) · [`INLINE`](INLINE.md) · [`EXTERN`](EXTERN.md) |
| **OOP** | [`CLASS`](CLASS.md) · [`STRUCT`](STRUCT.md) · [`PUBLIC_PRIVATE_PROTECTED`](PUBLIC_PRIVATE_PROTECTED.md) · [`THIS`](THIS.md) · [`FRIEND`](FRIEND.md) · [`VIRTUAL`](VIRTUAL.md) · [`OPERATOR`](OPERATOR.md) |
| **Memory** | [`NEW`](NEW.md) · [`DELETE`](DELETE.md) |
| **Templates & scope** | [`TEMPLATE`](TEMPLATE.md) · [`TYPENAME`](TYPENAME.md) · [`NAMESPACE`](NAMESPACE.md) · [`USING`](USING.md) |
| **Casts & RTTI** | [`CASTS`](CASTS.md) · [`TYPEID`](TYPEID.md) |
| **Exceptions** | [`TRY_CATCH_THROW`](TRY_CATCH_THROW.md) |
| **Fossils** | [`LEGACY_KEYWORDS`](LEGACY_KEYWORDS.md) — `auto`, `register`, `asm`, `export` |

---

## 🗺️ Keyword ↔ notion crossroads

| Reading the notion… | Zoom into these keywords |
|---|---|
| [`fundamentals/BASICS.md`](../notions/fundamentals/BASICS.md) | [`CLASS`](CLASS.md) · [`PUBLIC_PRIVATE_PROTECTED`](PUBLIC_PRIVATE_PROTECTED.md) · [`CONST`](CONST.md) · [`NAMESPACE`](NAMESPACE.md) |
| [`fundamentals/MEMORY.md`](../notions/fundamentals/MEMORY.md) | [`NEW`](NEW.md) · [`DELETE`](DELETE.md) · [`STATIC`](STATIC.md) |
| [`oop/ORTHODOX_CANONICAL_FORM.md`](../notions/oop/ORTHODOX_CANONICAL_FORM.md) | [`OPERATOR`](OPERATOR.md) · [`THIS`](THIS.md) · [`EXPLICIT`](EXPLICIT.md) |
| [`oop/INHERITANCE.md`](../notions/oop/INHERITANCE.md) | [`PUBLIC_PRIVATE_PROTECTED`](PUBLIC_PRIVATE_PROTECTED.md) · [`USING`](USING.md) · [`VIRTUAL`](VIRTUAL.md) |
| [`oop/POLYMORPHISM.md`](../notions/oop/POLYMORPHISM.md) | [`VIRTUAL`](VIRTUAL.md) · [`TYPEID`](TYPEID.md) · [`CASTS`](CASTS.md) |
| [`advanced/TEMPLATES.md`](../notions/advanced/TEMPLATES.md) | [`TEMPLATE`](TEMPLATE.md) · [`TYPENAME`](TYPENAME.md) |
| [`io-errors/ERROR_MANAGEMENT.md`](../notions/io-errors/ERROR_MANAGEMENT.md) | [`TRY_CATCH_THROW`](TRY_CATCH_THROW.md) · [`RETURN_GOTO`](RETURN_GOTO.md) |
| [`tooling/LIBRARIES.md`](../notions/tooling/LIBRARIES.md) | [`EXTERN`](EXTERN.md) · [`STATIC`](STATIC.md) · [`INLINE`](INLINE.md) |

---

## 📐 What each page covers

Every keyword file follows the same skeleton so you know where to look:

1. **TL;DR** — one sentence to anchor the rest.
2. **What it does** — the language-level meaning.
3. **Implementation / hardware view** — what the compiler emits, where the data lives.
4. **Worked examples** — copy-pasteable C++98 snippets.
5. **Tips, common errors, visual summary.**

The newer grouped pages (`TYPES`, `LOOPS`, `IF_ELSE`, `RETURN_GOTO`, `UNION`, `LEGACY_KEYWORDS`) are deliberately shorter — TL;DR, shape, traps, done.
