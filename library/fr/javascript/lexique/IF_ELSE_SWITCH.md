# `if`, `else`, `switch`, `case`, `default`, `? :` — les conditions

> **TL;DR.** Même syntaxe qu'en C. Deux différences : la condition accepte **n'importe quelle valeur**, convertie en booléen selon les règles *truthy* / *falsy* ; et `switch` compare avec **`===`** et accepte des chaînes. Le *fall-through* entre `case` existe toujours : n'oublie pas `break`.

Liens : [`INDEX`](INDEX.md) · [`BOUCLES`](BOUCLES.md) · [`OPERATEURS`](OPERATEURS.md) · [`TYPES_PRIMITIFS`](TYPES_PRIMITIFS.md)

---

## 1. Pont C/C++98

| C++98 | JavaScript | Note |
|---|---|---|
| `if (ptr)` | `if (valeur)` | Conversion *truthy* / *falsy*, plus large qu'en C |
| `switch` sur `int` / `enum` | `switch` sur **n'importe quoi** | Chaînes acceptées, comparaison stricte `===` |
| `cond ? a : b` | `cond ? a : b` | Identique |
| `case 1: case 2:` | idem | Regroupement par *fall-through* |

---

## 2. *Truthy* / *falsy*

```js
// Les 8 valeurs falsy — tout le reste est truthy :
false, 0, -0, 0n, "", null, undefined, NaN

Boolean("0");   // → true   ⚠️ une chaîne non vide
Boolean([]);    // → true   ⚠️ un tableau vide est un objet
Boolean({});    // → true
```

```js
if (users.length)      { /* au moins un */ }      // idiome courant
if (name)              { /* ni "", ni null, ni undefined */ }
if (count)             { /* ⚠️ faux aussi pour count === 0 ! */ }
if (count !== undefined) { /* ce qu'on voulait vraiment */ }
```

---

## 3. `switch`

```js
function kind(x) {
    switch (x) {
        case 1:
        case 2:   return "petit";
        case "1": return "chaîne";      // "1" !== 1 : cas séparé
        default:  return "autre";
    }
}
console.log(kind(1), kind("1"), kind(3));   // → petit chaîne autre

function fall(x) {
    let r = "";
    switch (x) {
        case "a": r += "a";             // pas de break → on tombe dans "b"
        case "b": r += "b"; break;
        default:  r += "d";
    }
    return r;
}
console.log(fall("a"));                      // → ab
```

Les `case` acceptent des **expressions** (`case LIMIT * 2:`), évaluées dans l'ordre. Une seule portée de bloc pour tout le `switch` : deux `let x` dans deux `case` différents entrent en collision. Ajoute des accolades : `case 1: { let x = …; break; }`.

---

## 4. Alternatives idiomatiques

```js
// Table de correspondance au lieu d'un long switch
const LABELS = { win: "Victoire", lose: "Défaite", draw: "Égalité" };
const label = LABELS[result] ?? "Inconnu";

// Garde en tête de fonction au lieu d'un else imbriqué
function play(card) {
    if (!card) return;
    // ...
}
```

---

## 5. Pièges

- **`if (x = 5)`** affecte au lieu de comparer, comme en C. Un linter le signale.
- **`if (count)`** rate le cas `0`. Pense *falsy* à chaque condition sur un nombre.
- **`switch` + `let`** sans accolades : `SyntaxError` à cause de la double déclaration.
- `5 > 3 > 1` → `false` : `(5 > 3)` donne `true`, puis `true > 1` donne `1 > 1`. Même piège qu'en C.
