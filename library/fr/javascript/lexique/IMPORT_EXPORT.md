# `import`, `export`, `default`, `as`, `from` — les modules

> **TL;DR.** Chaque fichier est un **module** avec sa propre portée : rien n'en sort sans `export`, rien n'y entre sans `import`. Un module est **évalué une seule fois**, puis partagé. Les noms importés sont des **liaisons vivantes en lecture seule** : tu vois les changements faits par le module d'origine, mais tu ne peux pas réaffecter le nom. `import()` charge un module à la demande.

Liens : [`INDEX`](INDEX.md) · [`LEGACY`](LEGACY.md) (`require`) · fiche [`CLASSES_MODULES`](../CLASSES_MODULES.md) · fiche [`OUTILS`](../OUTILS.md)

---

## 1. Pont C/C++98

| C++98 | JavaScript | Note |
|---|---|---|
| `#include "math.h"` | `import { add } from "./math.js"` | Pas un copier-coller : on importe des **noms** |
| header guards `#ifndef` | — | Inutile : évalué une seule fois |
| fonction `static` (interne au `.cpp`) | ne pas l'exporter | Privé par défaut |
| `extern` dans le header | `export` | Public seulement si demandé |
| `namespace M` + `M::f()` | `import * as M` + `M.f()` | |
| édition de liens | le chargeur de modules / le bundler | |

---

## 2. Toutes les formes

```js
// math.mjs
export const PI = 3.14;
export let counter = 0;
export function inc() { counter++; }
export default function add(a, b) { return a + b; }
console.log("math.mjs évalué");
```

```js
// main.mjs
import add, { PI, counter, inc as increment } from "./math.mjs";  // default + nommés + renommage
import * as M from "./math.mjs";                                   // tout, dans un objet

console.log(add(1, 2), PI, M.PI, typeof M.default);   // → 3 3.14 3.14 function
increment();
console.log(counter);   // → 1      ⚠️ liaison VIVANTE : on voit la modification
PI = 3;                 // ❌ TypeError: Assignment to constant variable.

const lazy = await import("./math.mjs");   // dynamique, renvoie une Promise
```

`"math.mjs évalué"` n'est affiché **qu'une fois**, malgré trois imports du même fichier.

| Forme | Côté `export` | Côté `import` |
|---|---|---|
| Nommé | `export function f` / `export { f, g }` | `import { f } from …` (même nom, accolades) |
| Par défaut | `export default …` (un seul par fichier) | `import n_importe_quel_nom from …` |
| Renommé | `export { f as g }` | `import { f as g } from …` |
| Ré-export | `export { f } from "./a.js"` / `export * from "./a.js"` | — (fichier `index.js` « baril ») |
| Effet seul | — | `import "./styles.css"` |

---

## 3. Où ça s'exécute

- **Navigateur** : `<script type="module" src="main.js">`. Sans `type="module"` : `SyntaxError: Cannot use import statement outside a module`.
- **Node** : extension `.mjs`, ou `"type": "module"` dans `package.json`.
- **Vite / React / TypeScript** : les imports sont gérés par l'outil, qui tolère l'absence d'extension.

---

## 4. Pièges

- **L'extension `.js` est obligatoire** en ESM natif, même si le fichier source est un `.ts` compilé.
- **`import` avec accolades vs sans** : `import add from` prend le *default*, `import { add } from` cherche un export **nommé** `add`. Les confondre donne `undefined` ou une `SyntaxError`.
- **Imports circulaires** (A importe B qui importe A) : une des deux valeurs peut être encore non initialisée au moment de l'utilisation. Restructure.
- **Les modules sont en mode strict** : variable non déclarée → `ReferenceError`, `this` au niveau supérieur → `undefined`.
- `import` en haut de fichier est **statique** : pas de `if (…) import …`. Pour un import conditionnel, utilise `await import(...)`.
