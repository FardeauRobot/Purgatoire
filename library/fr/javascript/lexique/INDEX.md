# 🔑 LEXIQUE — Les mots-clés JavaScript / TypeScript, en un clic

Une page par mot-clé (ou groupe de mots-clés liés) : ce qu'il fait, son équivalent en C/C++98, des exemples vérifiés avec Node v26 (`// →`) et les pièges. Trouve ton mot dans le tableau A→Z, clique, lis, repars.

> Tu cherches un **concept** plutôt qu'un mot-clé (closure, hoisting, prototype, event loop…) ? C'est le [`GLOSSAIRE`](GLOSSAIRE.md).
> Les fiches thématiques, à lire dans l'ordre, sont dans [`../INDEX.md`](../INDEX.md). Le lexique sert à **retrouver** un mot, les fiches servent à **apprendre**.

---

## 🔤 A→Z — JavaScript

| Mot-clé | Page | | Mot-clé | Page |
|---|---|---|---|---|
| `arguments` | [`LEGACY`](LEGACY.md) | | `import` | [`IMPORT_EXPORT`](IMPORT_EXPORT.md) |
| `as` | [`IMPORT_EXPORT`](IMPORT_EXPORT.md) | | `in` | [`TYPEOF_INSTANCEOF_IN`](TYPEOF_INSTANCEOF_IN.md) · [`BOUCLES`](BOUCLES.md) |
| `async` | [`ASYNC_AWAIT`](ASYNC_AWAIT.md) | | `instanceof` | [`TYPEOF_INSTANCEOF_IN`](TYPEOF_INSTANCEOF_IN.md) |
| `await` | [`ASYNC_AWAIT`](ASYNC_AWAIT.md) | | `let` | [`VAR_LET_CONST`](VAR_LET_CONST.md) |
| `break` | [`BOUCLES`](BOUCLES.md) | | `NaN` / `Infinity` | [`TYPES_PRIMITIFS`](TYPES_PRIMITIFS.md) |
| `case` | [`IF_ELSE_SWITCH`](IF_ELSE_SWITCH.md) | | `new` | [`NEW_DELETE`](NEW_DELETE.md) |
| `catch` | [`TRY_CATCH_THROW`](TRY_CATCH_THROW.md) | | `null` | [`TYPES_PRIMITIFS`](TYPES_PRIMITIFS.md) |
| `class` | [`CLASS_EXTENDS_SUPER`](CLASS_EXTENDS_SUPER.md) | | `of` | [`BOUCLES`](BOUCLES.md) |
| `const` | [`VAR_LET_CONST`](VAR_LET_CONST.md) | | `return` | [`FUNCTION_RETURN`](FUNCTION_RETURN.md) |
| `constructor` | [`CLASS_EXTENDS_SUPER`](CLASS_EXTENDS_SUPER.md) | | `set` | [`STATIC_GET_SET`](STATIC_GET_SET.md) |
| `continue` | [`BOUCLES`](BOUCLES.md) | | `static` | [`STATIC_GET_SET`](STATIC_GET_SET.md) |
| `debugger` | [`LEGACY`](LEGACY.md) | | `super` | [`CLASS_EXTENDS_SUPER`](CLASS_EXTENDS_SUPER.md) |
| `default` | [`IF_ELSE_SWITCH`](IF_ELSE_SWITCH.md) · [`IMPORT_EXPORT`](IMPORT_EXPORT.md) | | `switch` | [`IF_ELSE_SWITCH`](IF_ELSE_SWITCH.md) |
| `delete` ⚠️ | [`NEW_DELETE`](NEW_DELETE.md) | | `this` | [`THIS`](THIS.md) |
| `do` | [`BOUCLES`](BOUCLES.md) | | `throw` | [`TRY_CATCH_THROW`](TRY_CATCH_THROW.md) |
| `else` | [`IF_ELSE_SWITCH`](IF_ELSE_SWITCH.md) | | `true` / `false` | [`TYPES_PRIMITIFS`](TYPES_PRIMITIFS.md) |
| `enum` | [`LEGACY`](LEGACY.md) (réservé, inutilisé en JS) | | `try` | [`TRY_CATCH_THROW`](TRY_CATCH_THROW.md) |
| `eval` | [`LEGACY`](LEGACY.md) | | `typeof` | [`TYPEOF_INSTANCEOF_IN`](TYPEOF_INSTANCEOF_IN.md) |
| `export` | [`IMPORT_EXPORT`](IMPORT_EXPORT.md) | | `undefined` | [`TYPES_PRIMITIFS`](TYPES_PRIMITIFS.md) |
| `extends` | [`CLASS_EXTENDS_SUPER`](CLASS_EXTENDS_SUPER.md) | | `var` | [`VAR_LET_CONST`](VAR_LET_CONST.md) · [`LEGACY`](LEGACY.md) |
| `finally` | [`TRY_CATCH_THROW`](TRY_CATCH_THROW.md) | | `void` | [`OPERATEURS`](OPERATEURS.md) |
| `for` | [`BOUCLES`](BOUCLES.md) | | `while` | [`BOUCLES`](BOUCLES.md) |
| `from` | [`IMPORT_EXPORT`](IMPORT_EXPORT.md) | | `with` | [`LEGACY`](LEGACY.md) |
| `function` / `=>` | [`FUNCTION_RETURN`](FUNCTION_RETURN.md) | | `yield` / `function*` | [`YIELD_GENERATEURS`](YIELD_GENERATEURS.md) |
| `get` | [`STATIC_GET_SET`](STATIC_GET_SET.md) | | `#privé` | [`STATIC_GET_SET`](STATIC_GET_SET.md) |
| `if` | [`IF_ELSE_SWITCH`](IF_ELSE_SWITCH.md) | | `===` `??` `?.` `...` | [`OPERATEURS`](OPERATEURS.md) |

> ⚠️ `delete` est **le** faux ami du C++ : il retire une propriété d'un objet, il ne libère jamais de mémoire. Voir [`NEW_DELETE`](NEW_DELETE.md).
> `implements`, `interface`, `package`, `private`, `protected`, `public` sont réservés mais inutilisés en JS : voir [`LEGACY`](LEGACY.md). TypeScript leur donne un sens.

---

## 🔷 TypeScript — ⏳ à venir (lot 2)

| Mots-clés | Page prévue |
|---|---|
| `type`, `interface` | ⏳ `TS_TYPE_INTERFACE` |
| `enum` | ⏳ `TS_ENUM` |
| `public`, `private`, `protected`, `readonly`, `abstract`, `implements` | ⏳ `TS_MODIFICATEURS` |
| `<T>`, `extends` (contrainte), `keyof`, `infer` | ⏳ `TS_GENERIQUES` |
| `as`, `satisfies`, `is`, `asserts`, `unknown`, `any`, `never` | ⏳ `TS_NARROWING` |
| `declare`, `namespace` | ⏳ `TS_DECLARE_NAMESPACE` |

En attendant : la fiche [`TYPESCRIPT`](../TYPESCRIPT.md).

---

## 🗂️ Par thème

| Thème | Pages |
|---|---|
| **Déclarations** | [`VAR_LET_CONST`](VAR_LET_CONST.md) · [`FUNCTION_RETURN`](FUNCTION_RETURN.md) |
| **Valeurs & types** | [`TYPES_PRIMITIFS`](TYPES_PRIMITIFS.md) · [`TYPEOF_INSTANCEOF_IN`](TYPEOF_INSTANCEOF_IN.md) · [`OPERATEURS`](OPERATEURS.md) |
| **Objets & classes** | [`CLASS_EXTENDS_SUPER`](CLASS_EXTENDS_SUPER.md) · [`STATIC_GET_SET`](STATIC_GET_SET.md) · [`THIS`](THIS.md) · [`NEW_DELETE`](NEW_DELETE.md) |
| **Contrôle** | [`IF_ELSE_SWITCH`](IF_ELSE_SWITCH.md) · [`BOUCLES`](BOUCLES.md) · [`TRY_CATCH_THROW`](TRY_CATCH_THROW.md) |
| **Asynchrone & itération** | [`ASYNC_AWAIT`](ASYNC_AWAIT.md) · [`YIELD_GENERATEURS`](YIELD_GENERATEURS.md) |
| **Modules** | [`IMPORT_EXPORT`](IMPORT_EXPORT.md) |
| **Fossiles** | [`LEGACY`](LEGACY.md) — `var`, `with`, `arguments`, `eval`, `require`, mots réservés |

---

## 🗺️ Carrefour fiches ↔ mots-clés

| En lisant la fiche… | Zoome sur ces mots-clés |
|---|---|
| [`PHILOSOPHIE`](../PHILOSOPHIE.md) | [`NEW_DELETE`](NEW_DELETE.md) · [`ASYNC_AWAIT`](ASYNC_AWAIT.md) · [`LEGACY`](LEGACY.md) (mode strict) |
| [`VARIABLES_TYPES`](../VARIABLES_TYPES.md) | [`VAR_LET_CONST`](VAR_LET_CONST.md) · [`TYPES_PRIMITIFS`](TYPES_PRIMITIFS.md) · [`TYPEOF_INSTANCEOF_IN`](TYPEOF_INSTANCEOF_IN.md) · [`OPERATEURS`](OPERATEURS.md) |
| [`FONCTIONS`](../FONCTIONS.md) | [`FUNCTION_RETURN`](FUNCTION_RETURN.md) · [`THIS`](THIS.md) |
| [`OBJETS_TABLEAUX`](../OBJETS_TABLEAUX.md) | [`BOUCLES`](BOUCLES.md) · [`OPERATEURS`](OPERATEURS.md) (`...`) · [`NEW_DELETE`](NEW_DELETE.md) (`delete`) |
| [`CLASSES_MODULES`](../CLASSES_MODULES.md) | [`CLASS_EXTENDS_SUPER`](CLASS_EXTENDS_SUPER.md) · [`STATIC_GET_SET`](STATIC_GET_SET.md) · [`IMPORT_EXPORT`](IMPORT_EXPORT.md) |
| [`ASYNCHRONE`](../ASYNCHRONE.md) | [`ASYNC_AWAIT`](ASYNC_AWAIT.md) · [`TRY_CATCH_THROW`](TRY_CATCH_THROW.md) · [`YIELD_GENERATEURS`](YIELD_GENERATEURS.md) |
| [`TYPESCRIPT`](../TYPESCRIPT.md) | [`TYPEOF_INSTANCEOF_IN`](TYPEOF_INSTANCEOF_IN.md) (narrowing) · lot TypeScript ⏳ |

---

## 🧪 Vérifier un exemple

```sh
node exemple.mjs     # .mjs = module ESM, donc mode strict (comme ton code réel)
node exemple.cjs     # .cjs = vieux script, utile pour voir les comportements « legacy »
```

Le résultat de certains exemples **dépend du mode** (strict ou non). Les pages précisent lequel à chaque fois.
