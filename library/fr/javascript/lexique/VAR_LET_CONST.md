# `let`, `const`, `var` — déclarer une variable

> **TL;DR.** **`const` par défaut**, **`let`** si la valeur doit être réaffectée, **jamais `var`**. `const` bloque la *réaffectation* du nom, pas la *modification* de l'objet. `let` et `const` ont une portée de **bloc** `{ }`, comme en C++ ; `var` a une portée de **fonction** et traîne des comportements hérités.

Liens : [`INDEX`](INDEX.md) · [`FUNCTION_RETURN`](FUNCTION_RETURN.md) · [`GLOSSAIRE`](GLOSSAIRE.md) (hoisting, TDZ) · fiche [`VARIABLES_TYPES`](../VARIABLES_TYPES.md)

---

## 1. Pont C/C++98

| C++98 | JavaScript | Note |
|---|---|---|
| `int n = 0;` | `let n = 0;` | Pas de type écrit : la *valeur* porte le type |
| `int *const p = &x;` | `const p = obj;` | Le **nom** est figé, l'objet pointé reste modifiable |
| `const int N = 10;` | `const N = 10;` | Pour un primitif, ça revient au même |
| `const T& ref` (lecture seule) | `Object.freeze(obj)` | Gel **superficiel**, à l'exécution |
| `int x;` (valeur indéterminée) | `let x;` → `undefined` | Jamais de « garbage » en JS |

---

## 2. `const` fige le nom, pas l'objet

```js
const o = { a: 1 };
o.a = 2;                 // ✅ on modifie l'objet
console.log(o.a);        // → 2
o = {};                  // ❌ TypeError: Assignment to constant variable.

const fr = Object.freeze({ a: 1 });
fr.a = 9;                // ignoré en silence (TypeError en mode strict)
console.log(fr.a);       // → 1
```

```
 const o ──────► { a: 2 }        le nom « o » est soudé à CETTE case mémoire
     ✗                            mais le contenu de la case reste libre
     └──► { } (interdit)
```

---

## 3. Portée de bloc et TDZ

```js
console.log(x);   // ❌ ReferenceError: Cannot access 'x' before initialization
let x = 1;
```

Entre le début du bloc et la ligne `let x`, la variable existe mais est **inaccessible** : c'est la *Temporal Dead Zone* (TDZ). C'est le comportement que tu attends d'un langage compilé : on n'utilise pas une variable avant sa déclaration.

---

## 4. Pourquoi pas `var`

```js
console.log(typeof v);   // → undefined   (pas d'erreur : v est « remontée »)
var v = 2;
```

`var` est **remontée** (*hoisting*) en haut de la fonction et initialisée à `undefined`. Pire, une seule variable est partagée par toute la boucle :

```js
for (var i = 0; i < 3; i++) setTimeout(() => console.log(i), 0);  // → 3 3 3
for (let j = 0; j < 3; j++) setTimeout(() => console.log(j), 0);  // → 0 1 2
```

Avec `let`, **chaque tour de boucle a sa propre `j`**, que la fonction fléchée capture (voir *closure* dans le [`GLOSSAIRE`](GLOSSAIRE.md)). Avec `var`, les trois callbacks lisent la même `i`, qui vaut 3 quand ils s'exécutent.

---

## 5. Pièges

- **Oublier la déclaration** : `total = 5` sans `let` crée une **variable globale** en mode non strict, et une `ReferenceError` en mode strict (modules, classes). Voir [`LEGACY`](LEGACY.md).
- **`const` ≠ immuable** : pour un tableau ou un objet, `push`, `sort`, `obj.x = …` restent permis.
- **`Object.freeze` est superficiel** : les sous-objets restent modifiables.
- `let` n'est pas un mot réservé en mode non strict : `var let = 1` est valide. N'y touche pas.
