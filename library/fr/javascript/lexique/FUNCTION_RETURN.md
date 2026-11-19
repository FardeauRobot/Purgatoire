# `function`, `return`, `=>` — les fonctions

> **TL;DR.** Trois façons d'écrire une fonction : la **déclaration** `function f() {}` (remontée), l'**expression** `const f = function () {}` et la **fléchée** `const f = () => {}` (pas de `this` propre). Sans `return`, une fonction renvoie `undefined`. Les fonctions sont des **valeurs** : on les range dans des variables et on les passe en argument.

Liens : [`INDEX`](INDEX.md) · [`THIS`](THIS.md) · [`ASYNC_AWAIT`](ASYNC_AWAIT.md) · [`YIELD_GENERATEURS`](YIELD_GENERATEURS.md) · fiche [`FONCTIONS`](../FONCTIONS.md)

---

## 1. Pont C/C++98

| C++98 | JavaScript | Note |
|---|---|---|
| `int add(int a, int b)` | `function add(a, b)` | Pas de types, pas de surcharge |
| prototype dans le `.h` | — | Inutile : la déclaration est remontée |
| `int (*fp)(int)` | `const fp = add;` | Une fonction est une valeur comme une autre |
| foncteur (`operator()`) | fonction fléchée / *closure* | Capture automatique des variables autour |
| `void f(int a, int b = 10)` | `function f(a, b = 10)` | Valeur par défaut évaluée **à chaque appel** |
| `va_list` / `...` | `function f(...rest)` | `rest` est un vrai tableau |

---

## 2. Les trois formes

```js
console.log(decl());            // → ok   (appelée AVANT sa ligne : remontée)
function decl() { return "ok"; }

notyet();                       // ❌ ReferenceError: Cannot access 'notyet' before initialization
const notyet = () => 1;         // une const suit les règles de const (TDZ)

const double = (x) => x * 2;            // corps-expression : return implicite
const bloc = (x) => { return x * 2; };  // corps-bloc : return obligatoire
const objet = () => ({ a: 1 });         // renvoyer un objet : parenthèses !
```

**Règle pratique** : `function` pour les fonctions de haut niveau d'un fichier, fléchée pour les callbacks (`map`, `then`, gestionnaires d'événements), parce qu'elle garde le `this` de l'extérieur ([`THIS`](THIS.md)).

---

## 3. Paramètres : souples par défaut

```js
function f(a, b = 10, ...rest) { return [a, b, rest]; }
console.log(f(1));          // → [ 1, 10, [] ]
console.log(f(1, 2, 3, 4)); // → [ 1, 2, [ 3, 4 ] ]
```

Il n'y a **aucune vérification du nombre d'arguments** : un argument manquant vaut `undefined`, un argument en trop est ignoré. TypeScript ajoute ce contrôle (lot TypeScript).

---

## 4. `return`

```js
function rien() {}
console.log(rien());        // → undefined

const oubli = (a) => { a * 2 };   // accolades = bloc, donc pas de return implicite
console.log(oubli(3));      // → undefined

function piege() {
    return              // ← un « ; » est inséré ICI automatiquement (ASI)
        42;
}
console.log(piege());       // → undefined
```

Ce dernier piège vient de l'**insertion automatique de point-virgule** (ASI) : un saut de ligne juste après `return` termine l'instruction. Garde toujours la valeur sur la même ligne que `return`.

---

## 5. Pièges

- **Fléchée + accolades sans `return`** → `undefined`. C'est l'erreur la plus fréquente dans un `.map(...)`.
- **`() => { a: 1 }`** est un bloc contenant une étiquette `a:`, pas un objet. Écris `() => ({ a: 1 })`.
- **Pas de surcharge** : deux `function f` dans le même fichier, la seconde écrase la première.
- `arguments` (le pseudo-tableau implicite des anciennes fonctions) : une fléchée n'a pas **le sien**, elle voit celui de la fonction qui l'entoure, comme pour `this`. Utilise `...rest`. Voir [`LEGACY`](LEGACY.md).
