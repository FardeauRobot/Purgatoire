# `===`, `??`, `?.`, `...`, `void` — les opérateurs propres à JS

> **TL;DR.** Compare avec **`===`** (pas de conversion), jamais `==`. **`??`** donne une valeur par défaut si `null`/`undefined` seulement (contrairement à `||`). **`?.`** s'arrête au lieu de planter sur `null`/`undefined`. **`...`** étale ou rassemble les éléments d'un tableau ou d'un objet. `void x` vaut toujours `undefined`.

Liens : [`INDEX`](INDEX.md) · [`TYPES_PRIMITIFS`](TYPES_PRIMITIFS.md) · [`IF_ELSE_SWITCH`](IF_ELSE_SWITCH.md) · [`TYPEOF_INSTANCEOF_IN`](TYPEOF_INSTANCEOF_IN.md)

---

## 1. Pont C/C++98

| C++98 | JavaScript | Note |
|---|---|---|
| `a == b` | `a === b` | `==` en JS convertit les types : à bannir |
| `p ? p->name : NULL` | `p?.name` | *Optional chaining* |
| `p ? p : def` | `p ?? def` | Seulement si `null` / `undefined` |
| `std::copy` / concaténer | `[...a, ...b]` | *Spread* |
| `memcpy` d'une struct | `{ ...obj }` | Copie **superficielle** |
| `std::pow(2, 10)` | `2 ** 10` | |
| `sprintf("%d", n)` | `` `n = ${n}` `` | *Template literal* |

---

## 2. `===` contre `==`

```js
"5" + 3      // → "53"   (+ avec une chaîne = concaténation)
"5" - 3      // → 2      (- force la conversion en nombre)
true + 1     // → 2
[] + {}      // → "[object Object]"
null == 0    // → false   null == undefined → true   (règles spéciales)
```

`===` compare type **et** valeur, sans conversion. La seule exception tolérée : `x == null`, qui teste `null` **et** `undefined` en une fois.

---

## 3. `??`, `||`, `?.`

```js
0 || "def"     // → "def"   ⚠️ 0 est falsy
0 ?? "def"     // → 0       ✅ 0 est une vraie valeur
"" ?? "def"    // → ""
null ?? "def"  // → "def"

const user = { profile: null };
user.profile?.name    // → undefined   (au lieu de TypeError)
user.getX?.()         // → undefined   (appelle seulement si la méthode existe)
user.list?.[0]        // → undefined

let cfg = {};
cfg.port ??= 3000;    // affecte seulement si null/undefined
cfg.port ??= 9;       // déjà défini : inchangé → 3000
```

---

## 4. `...` : *spread* et *rest*

```js
[..."abc"]                      // → [ 'a', 'b', 'c' ]
Math.max(...[3, 9, 1])          // → 9
({ ...{ a: 1, b: 2 }, b: 3 })   // → { a: 1, b: 3 }   (le dernier gagne)

const { a, ...others } = { a: 1, b: 2, c: 3 };   // a = 1, others = { b: 2, c: 3 }
const [first, , third = "def"] = [10, 20];       // first = 10, third = "def"
```

*Spread* (à droite du `=`, dans un appel) **étale** ; *rest* (à gauche, dans des paramètres) **rassemble**. La syntaxe `{ a, b } = obj` est la *déstructuration* : voir le [`GLOSSAIRE`](GLOSSAIRE.md).

```js
const src = { inner: { v: 1 } };
const cp = { ...src };
cp.inner.v = 99;
console.log(src.inner.v);   // → 99   ⚠️ copie superficielle : inner est partagé
structuredClone(src);       // copie profonde (« deep copy »)
```

---

## 5. Divers

| Opérateur | Rôle |
|---|---|
| `void expr` | évalue `expr` et renvoie `undefined` (`void 0` = vieux synonyme de `undefined`) |
| `,` | évalue à gauche puis à droite, garde la droite (comme en C) |
| `&`, `|`, `^`, `~`, `<<`, `>>` | bit à bit **sur 32 bits signés** : `2 ** 32 | 0` → `0` |
| `>>>` | décalage à droite non signé (pas d'équivalent direct en C, où ça dépend du type) |

---

## 6. Pièges

- **`||` pour une valeur par défaut** écrase `0`, `""` et `false`. Utilise `??`.
- **`[1, 10, 2].sort()`** → `[1, 10, 2]` : tri **en texte**. Écris `.sort((a, b) => a - b)`.
- **`"10" < "9"`** → `true` : comparaison de chaînes, caractère par caractère.
- **Mélanger `??` et `||`** sans parenthèses est une `SyntaxError` : `a ?? (b || c)`.
