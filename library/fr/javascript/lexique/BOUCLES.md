# `for`, `of`, `in`, `while`, `do`, `break`, `continue` — les boucles

> **TL;DR.** `for (;;)`, `while` et `do…while` : identiques au C. Les deux nouveautés : **`for…of` parcourt les valeurs** de tout itérable (tableau, chaîne, `Map`, `Set`) et **`for…in` parcourt les clés** d'un objet. Sur un tableau, utilise **`for…of`**. `break` et `continue` acceptent une **étiquette** pour sortir de boucles imbriquées.

Liens : [`INDEX`](INDEX.md) · [`IF_ELSE_SWITCH`](IF_ELSE_SWITCH.md) · [`YIELD_GENERATEURS`](YIELD_GENERATEURS.md) (itérables) · fiche [`OBJETS_TABLEAUX`](../OBJETS_TABLEAUX.md)

---

## 1. Pont C/C++98

| C++98 | JavaScript | Note |
|---|---|---|
| `for (int i = 0; i < n; i++)` | `for (let i = 0; i < n; i++)` | `let`, pas `var` ([`VAR_LET_CONST`](VAR_LET_CONST.md)) |
| `for (it = v.begin(); it != v.end(); ++it)` | `for (const x of arr)` | Valeurs, en lecture |
| parcourir les clés d'une `std::map` | `for (const k in obj)` ou `Object.keys(obj)` | |
| `std::for_each` | `arr.forEach(x => …)` | Pas de `break` possible |
| `goto fin;` pour sortir de 2 boucles | `break etiquette;` | Propre et lisible |

---

## 2. `for…of` contre `for…in`

```js
const arr = ["x", "y"];
arr.extra = "!";

for (const i in arr) console.log(i);   // → 0 1 extra   ⚠️ clés (en string !) + propriété ajoutée
for (const v of arr) console.log(v);   // → x y         ✅ les valeurs

for (const [k, v] of Object.entries({ a: 1, b: 2 })) console.log(`${k}=${v}`);  // → a=1 b=2
for (const ch of "hé") console.log(ch);                                          // → h é
for (const [cle, val] of new Map([["k", 1]])) console.log(cle, val);              // → k 1
```

**Moyen mnémotechnique** : `in` → **i**ndices / clés ; `of` → **o**bjets contenus / valeurs.

---

## 3. Étiquettes

```js
outer: for (let i = 0; i < 3; i++) {
    for (let j = 0; j < 3; j++) {
        if (j === 1) continue outer;   // passe au i suivant
        if (i === 2) break outer;      // sort des DEUX boucles
        console.log(`${i}${j}`);
    }
}
// → 00 10
```

---

## 4. `forEach`, `map` et compagnie

```js
[1, 2, 3].forEach(x => {
    if (x === 2) return;     // ⚠️ ne sort PAS de forEach, saute juste ce tour
    console.log(x);
});
// → 1 3
```

`return` dans un callback sort **du callback**, pas de la boucle. Pour arrêter tôt : `for…of` + `break`, ou `arr.find(...)` / `arr.some(...)`.

---

## 5. Pièges

- **`for…in` sur un tableau** : clés en `string` (`"0"`, `"1"`), propriétés ajoutées et héritées incluses. À éviter.
- **`for…of` sur un objet simple** : `TypeError: obj is not iterable`. Passe par `Object.entries(obj)`.
- **`await` dans `forEach`** : ne fonctionne pas comme tu crois. Voir [`ASYNC_AWAIT`](ASYNC_AWAIT.md).
- **Modifier un tableau pendant qu'on le parcourt** : comportement surprenant, comme l'invalidation d'itérateurs en C++.
