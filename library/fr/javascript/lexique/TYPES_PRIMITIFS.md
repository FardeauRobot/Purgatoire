# `null`, `undefined`, `true` / `false`, `NaN` — les valeurs primitives

> **TL;DR.** 7 types primitifs : `number` (toujours un `double`), `string` (immuable), `boolean`, `undefined` (« jamais rempli »), `null` (« vide exprès »), `bigint`, `symbol`. Tout le reste est un **objet**. `null`, `true` et `false` sont des mots-clés ; `undefined`, `NaN` et `Infinity` sont des **variables globales** en lecture seule.

Liens : [`INDEX`](INDEX.md) · [`TYPEOF_INSTANCEOF_IN`](TYPEOF_INSTANCEOF_IN.md) · [`OPERATEURS`](OPERATEURS.md) · fiche [`VARIABLES_TYPES`](../VARIABLES_TYPES.md)

---

## 1. Pont C/C++98

| C++98 | JavaScript | Note |
|---|---|---|
| `int`, `long`, `float`, `double` | `number` | **Un seul** type : double IEEE 754 |
| `7 / 2` → `3` | `7 / 2` → `3.5` | Pour la division entière : `Math.trunc(7 / 2)` |
| `long long` géant | `bigint` : `10n` | Taille arbitraire, ne se mélange pas avec `number` |
| `std::string` modifiable | `string` **immuable** | Toute méthode renvoie une nouvelle chaîne |
| `bool` | `boolean` | `true` / `false` |
| `NULL` | `null` | |
| valeur indéterminée | `undefined` | Mise par le langage, jamais du « garbage » |
| `1.0 / 0` → `inf` | `1 / 0` → `Infinity` | Pas de crash, pas de SIGFPE |

---

## 2. `number` : un `double` partout

```js
console.log(7 / 2, Math.trunc(7 / 2));   // → 3.5 3
console.log(0.1 + 0.2);                  // → 0.30000000000000004  (comme en C)
console.log(Number.MAX_SAFE_INTEGER);    // → 9007199254740991   (2^53 - 1)
console.log(2 ** 53 + 1);                // → 9007199254740992   ⚠️ précision perdue
console.log(-7 % 3);                     // → -1   (% suit le signe, comme en C)
console.log(10n ** 20n);                 // → 100000000000000000000n
1n + 1;   // ❌ TypeError: Cannot mix BigInt and other types, use explicit conversions
```

---

## 3. `NaN` : le nombre qui n'est pas égal à lui-même

```js
console.log(typeof NaN);                 // → number
console.log(NaN === NaN);                // → false   (règle IEEE 754, comme en C)
console.log(Number.isNaN(NaN));          // → true    ← le seul bon test
console.log(Object.is(NaN, NaN));        // → true
console.log(0 === -0, Object.is(0, -0)); // → true false
```

`NaN` apparaît dès qu'un calcul n'a pas de sens : `"abc" * 2`, `Math.sqrt(-1)`, `parseInt("x")`. Il se **propage** silencieusement dans tous les calculs suivants.

---

## 4. `undefined` vs `null`

```js
let u;
console.log(u, ({}).absent, [1][5]);   // → undefined undefined undefined
console.log(null == undefined);        // → true    (le seul cas utile de ==)
console.log(null === undefined);       // → false
```

| | `undefined` | `null` |
|---|---|---|
| Qui la met ? | le langage | toi |
| Sens | « pas encore de valeur » | « volontairement vide » |
| `typeof` | `"undefined"` | `"object"` ⚠️ bug historique |
| En JSON | la clé **disparaît** | `null` conservé |

---

## 5. Pièges

- **`typeof null === "object"`** : teste `x === null` directement.
- **Chaînes immuables** : `s[0] = "z"` ne fait rien en mode non strict et lève une `TypeError` en module ou en classe.
- **Les entiers au-delà de 2^53 sont faux** : un identifiant de base de données sur 64 bits doit circuler en `string` ou en `bigint`.
- **Valeurs *falsy*** : `false`, `0`, `-0`, `0n`, `""`, `null`, `undefined`, `NaN`. Tout le reste est *truthy*, y compris `"0"`, `[]` et `{}`. Voir [`IF_ELSE_SWITCH`](IF_ELSE_SWITCH.md).
- `symbol` : `Symbol("id") === Symbol("id")` → `false`, chaque symbole est unique. Sert surtout de clé spéciale, comme `Symbol.iterator` ([`YIELD_GENERATEURS`](YIELD_GENERATEURS.md)).
