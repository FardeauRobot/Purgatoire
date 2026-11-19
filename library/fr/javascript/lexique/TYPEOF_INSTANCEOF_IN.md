# `typeof`, `instanceof`, `in` — interroger une valeur à l'exécution

> **TL;DR.** `typeof x` renvoie une **chaîne** décrivant le type primitif (`"number"`, `"object"`, `"function"`…). `x instanceof C` vérifie si `C.prototype` est dans la chaîne de prototypes de `x` (≈ `dynamic_cast` réussi). `"cle" in obj` vérifie qu'une propriété existe, **y compris héritée**. Les trois servent au *narrowing* de TypeScript.

Liens : [`INDEX`](INDEX.md) · [`TYPES_PRIMITIFS`](TYPES_PRIMITIFS.md) · [`CLASS_EXTENDS_SUPER`](CLASS_EXTENDS_SUPER.md) · [`BOUCLES`](BOUCLES.md) (`for…in`)

---

## 1. Pont C/C++98

| C++98 | JavaScript | Note |
|---|---|---|
| `typeid(x).name()` | `typeof x` | Renvoie une chaîne, ne lève jamais d'erreur |
| `dynamic_cast<Dog*>(p) != NULL` | `p instanceof Dog` | Parcourt la chaîne de prototypes |
| `m.count("cle")` (`std::map`) | `"cle" in obj` | Inclut les propriétés héritées |
| — | `Object.hasOwn(obj, "cle")` | Propriétés **propres** seulement |

---

## 2. `typeof` : la table complète

```js
typeof 42             // "number"
typeof "x"            // "string"
typeof true           // "boolean"
typeof undefined      // "undefined"
typeof 10n            // "bigint"
typeof Symbol()       // "symbol"
typeof function(){}   // "function"
typeof class{}        // "function"    (une classe EST une fonction)
typeof {}             // "object"
typeof []             // "object"      ⚠️ → Array.isArray([]) donne true
typeof null           // "object"      ⚠️ bug historique
typeof variableJamaisDeclaree   // "undefined"   (pas de ReferenceError !)
```

Le dernier cas explique un idiome ancien : `if (typeof window !== "undefined")` teste si le code tourne dans un navigateur sans planter dans Node.

---

## 3. `instanceof`

```js
console.log([] instanceof Array);     // → true
console.log([] instanceof Object);    // → true   (Array hérite d'Object)
console.log("x" instanceof String);   // → false  (primitif, pas un objet String)
console.log(null instanceof Object);  // → false
```

```
  dog instanceof Animal ?
  dog ──► Dog.prototype ──► Animal.prototype ✔ trouvé → true
```

Usage typique : trier les erreurs dans un `catch` (`if (e instanceof TypeError)`). Voir [`TRY_CATCH_THROW`](TRY_CATCH_THROW.md).

---

## 4. `in`

```js
console.log("a" in { a: undefined });            // → true   (la clé existe, même vide)
console.log("toString" in {});                   // → true   (héritée d'Object.prototype)
console.log(Object.hasOwn({}, "toString"));      // → false
console.log(0 in ["z"], "z" in ["z"]);           // → true false   (indices, pas valeurs !)
```

Pour chercher une **valeur** dans un tableau : `arr.includes("z")`.

---

## 5. Pièges

- **`typeof` ne distingue pas** tableau, `null`, objet, `Date` : tout est `"object"`. Utilise `Array.isArray`, `x === null` ou `instanceof`.
- **`in` sur un tableau** teste les indices, pas les valeurs.
- **`obj.cle !== undefined`** ne vaut pas `"cle" in obj` : une clé peut exister avec la valeur `undefined`.
- `in` a un second sens dans `for (const k in obj)` : voir [`BOUCLES`](BOUCLES.md).
- `instanceof` échoue entre deux contextes (iframe, `vm` de Node) : deux `Array` différents. Rare, mais déroutant.
