# `this` — dépend de l'appel, pas de la définition

> **TL;DR.** En C++, `this` est fixé : c'est l'objet dont la méthode est membre. En JS, `this` est décidé **au moment de l'appel** : `obj.f()` → `this` vaut `obj` ; `f()` tout seul → `undefined` (mode strict). Les **fonctions fléchées n'ont pas de `this`** : elles prennent celui du code qui les entoure. `bind` / `call` / `apply` le forcent.

Liens : [`INDEX`](INDEX.md) · [`FUNCTION_RETURN`](FUNCTION_RETURN.md) · [`CLASS_EXTENDS_SUPER`](CLASS_EXTENDS_SUPER.md) · fiche [`FONCTIONS`](../FONCTIONS.md)

---

## 1. Pont C/C++98

| C++98 | JavaScript |
|---|---|
| `this` = pointeur caché, toujours l'objet de la méthode | `this` = l'objet **à gauche du point** lors de l'appel |
| pointeur sur méthode `(obj.*pm)()` | `f.call(obj)` |
| `std::bind1st` / foncteur lié | `f.bind(obj)` |
| — | fléchée : pas de `this` propre, capture celui de l'extérieur |

---

## 2. Les quatre règles

```js
const obj = { n: "obj", normal() { return this.n; } };

obj.normal();                    // 1. appel de méthode      → "obj"
const lost = obj.normal;
lost();                          // 2. appel nu (module)     ❌ TypeError: Cannot read
                                 //    properties of undefined (reading 'n')
obj.normal.call({ n: "call" });  // 3. explicite             → "call"
obj.normal.bind({ n: "lié" })(); //    lié pour toujours     → "lié"
new Dog("Rex");                  // 4. new                   → this = l'objet neuf
```

```
   obj.normal()          lost()
   ───┬───               ──┬──
      └► this = obj        └► rien à gauche du point → this = undefined
```

Hors module et hors classe (vieux script non strict), la règle 2 donne l'objet global à la place de `undefined` : pas d'erreur, juste une valeur fausse. C'est pire.

---

## 3. Le piège classique : la méthode passée en callback

```js
class Timer {
    constructor() { this.n = 0; }
    bad()  { return [1, 2].map(function () { return this; }); }
    good() { return [1, 2].map(() => this.n); }
}
console.log(new Timer().bad());   // → [ undefined, undefined ]
console.log(new Timer().good());  // → [ 0, 0 ]
```

`map` appelle ta `function` « nue », donc `this` est perdu. La fléchée, elle, voit le `this` de `good()`. **C'est la raison principale pour laquelle on écrit des fléchées dans les callbacks.**

Même problème avec `setTimeout(this.tick, 1000)` ou `button.addEventListener("click", this.onClick)`. Correctifs :

```js
setTimeout(() => this.tick(), 1000);   // fléchée : le plus lisible
setTimeout(this.tick.bind(this), 1000); // bind : équivalent
```

---

## 4. Pièges

- **Fléchée comme méthode d'objet** : `{ f: () => this.n }` n'a pas accès à l'objet, puisqu'elle prend le `this` de l'extérieur.
- **Méthode extraite par déstructuration** : `const { normal } = obj; normal();` perd `this`, exactement comme `lost`.
- **En React moderne** (fonctions + hooks), tu n'écris presque jamais `this`. Le problème revient surtout dans les classes et les vieux exemples.
- `this` dans une méthode `static` vaut la classe elle-même ([`STATIC_GET_SET`](STATIC_GET_SET.md)).
