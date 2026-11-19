# 🔤 Variables et types

> **TL;DR.** Déclare avec **`const`** par défaut, **`let`** si la valeur doit changer, **jamais `var`**. Il y a 7 types primitifs (`number`, `string`, `boolean`, `null`, `undefined`, `bigint`, `symbol`) ; tout le reste est un **objet**. Compare toujours avec **`===`**.

Liens : [`INDEX`](INDEX.md) · [`FONCTIONS`](FONCTIONS.md) · [`OBJETS_TABLEAUX`](OBJETS_TABLEAUX.md)

---

## 1. `const`, `let`, `var`

```js
const max = 30;          // ne peut pas être réaffectée
let tour = 1;            // peut changer
tour = tour + 1;

const deck = ["feu", "eau"];
deck.push("air");        // ✅ autorisé : on modifie le tableau, pas la variable
// deck = [];            // ❌ TypeError : réaffectation d'une const
```

- **`const`** ≈ un pointeur constant en C (`int *const p`) : tu ne changes pas *où* ça pointe, mais tu peux changer *ce qui est pointé*.
- **`let`** : variable normale, visible seulement dans son bloc `{ }` — comme en C++.
- **`var`** : ancienne syntaxe, portée à toute la fonction, comportements bizarres (voir [`FONCTIONS`](FONCTIONS.md) §4). Tu la croiseras sur internet, ne l'écris pas.

---

## 2. Les types primitifs

| Type | Exemples | Équivalent C++ |
|---|---|---|
| `number` | `42`, `3.14`, `-0`, `NaN`, `Infinity` | `double` (il n'y a **pas** d'entier séparé) |
| `string` | `"a"`, `'b'`, `` `c` `` | `std::string`, mais **immuable** |
| `boolean` | `true`, `false` | `bool` |
| `undefined` | variable non initialisée, champ absent | — (valeur indéterminée) |
| `null` | « vide » volontaire | `NULL` |
| `bigint` | `10n` | entier de taille arbitraire, rare |
| `symbol` | `Symbol("id")` | rare, ignore-le pour l'instant |

```js
console.log(typeof 42);         // → number
console.log(typeof "x");        // → string
console.log(typeof undefined);  // → undefined
console.log(typeof null);       // → object   ⚠️ bug historique
console.log(typeof []);         // → object   (un tableau EST un objet)
console.log(typeof NaN);        // → number   (« Not a Number » est un number…)
```

**`undefined` vs `null`** : `undefined` = « jamais rempli » (le langage le met tout seul), `null` = « vide exprès » (c'est toi qui le mets).

---

## 3. Les chaînes : template strings

```js
const nom = "Tibras";
const pv = 12;
console.log(`${nom} a ${pv} PV`);   // → Tibras a 12 PV
console.log(nom.length, nom.toUpperCase(), nom.includes("bra"));
```

Les backticks `` ` `` permettent d'insérer `${expression}` directement — fini les `<<` en chaîne ou les `ft_strjoin`.

---

## 4. `===` et pas `==`

```js
console.log(0 == "");             // → true    😱
console.log("1" == 1);            // → true
console.log(null == undefined);   // → true
console.log(null === undefined);  // → false
console.log("1" === 1);           // → false   ✅ ce que tu veux
```

`==` **convertit** les deux côtés avant de comparer, avec des règles que personne ne retient. `===` compare valeur **et** type, comme le ferait le C++. Règle simple : **toujours `===` et `!==`**.

---

## 5. Les conversions implicites

```js
console.log("5" + 3);           // → 53     (+ avec une string = concaténation)
console.log("5" - 3);           // → 2      (- force la conversion en number)
console.log(0.1 + 0.2);         // → 0.30000000000000004  (comme un double en C)
console.log(10 / 0);            // → Infinity   (pas de crash)
console.log(parseInt("42px"));  // → 42
console.log(Number("42px"));    // → NaN
console.log(NaN === NaN);       // → false  → utilise Number.isNaN(x)
```

Pour convertir une saisie utilisateur, utilise `Number(x)` puis vérifie `Number.isNaN(...)`. En pratique, en backend, une bibliothèque de validation le fait pour toi (voir [`TYPESCRIPT`](TYPESCRIPT.md) §6).

---

## 6. Truthy / falsy

Dans un `if`, ces valeurs valent `false` : **`false`, `0`, `""`, `null`, `undefined`, `NaN`**. Tout le reste vaut `true` — y compris `[]` et `{}` (contrairement à ce qu'on pourrait croire).

```js
const pseudo = "";
if (!pseudo) console.log("pseudo vide");

const pv = 0;
const affiche = pv || 100;   // → 100  ⚠️ 0 est falsy, donc remplacé !
const correct = pv ?? 100;   // → 0    ✅ ?? ne remplace que null/undefined
```

- **`||`** : « la valeur de gauche si elle est truthy, sinon celle de droite ».
- **`??`** (*nullish coalescing*) : « la valeur de gauche sauf si elle est `null` ou `undefined` ». C'est presque toujours ce que tu veux pour une valeur par défaut.

---

## ⚠️ À retenir

- `const` par défaut, `let` si besoin, jamais `var`.
- `===` toujours.
- `??` pour les valeurs par défaut, pas `||` (sinon `0` et `""` sautent).
- Un `number` est un `double` : attention aux comparaisons de flottants.
