# 📦 Objets et tableaux

> **TL;DR.** Un **objet** `{ cle: valeur }` est une `struct` dont on peut ajouter/retirer des champs à la volée. Un **tableau** `[a, b, c]` est un `std::vector` qui accepte n'importe quoi. Les deux sont manipulés **par référence**. On les transforme avec `map` / `filter` / `find` / `reduce` plutôt qu'avec des boucles, et on les « déballe » avec la **déstructuration**.

Liens : [`INDEX`](INDEX.md) · [`FONCTIONS`](FONCTIONS.md) · [`CLASSES_MODULES`](CLASSES_MODULES.md) · [`TYPESCRIPT`](TYPESCRIPT.md)

---

## 1. Les objets

```js
const carte = {
    nom: "Aura de feu",
    attaque: 7,
    effets: ["brûlure"],
};

console.log(carte.nom);          // accès par point
console.log(carte["attaque"]);   // accès par chaîne (utile si la clé est dans une variable)
carte.rarete = "rare";           // ajout d'un champ
delete carte.rarete;             // suppression d'un champ
console.log(carte.defense);      // → undefined (pas d'erreur !)
console.log(carte.stats?.pv);    // → undefined : ?. s'arrête si stats est absent
```

- Lire un champ qui n'existe pas renvoie `undefined` au lieu de planter. Pratique… et source de bugs silencieux (une faute de frappe ne fait pas d'erreur). TypeScript règle ça.
- **`?.`** (*optional chaining*) : `a?.b` vaut `undefined` si `a` est `null`/`undefined`, au lieu de lancer une erreur. Comme un `if (a != NULL)` intégré.

---

## 2. Références : le piège de la copie

```js
const a = { x: 1, n: { y: 2 } };
const alias = a;            // ⚠️ même objet : comme copier un pointeur
const copie = { ...a };     // nouvelle « boîte » extérieure…
copie.x = 9;
copie.n.y = 9;              // …mais n est toujours partagé !
console.log(a);             // → { x: 1, n: { y: 9 } }
```

**Lien C++ :** c'est exactement la *shallow copy* que la forme canonique t'apprenait à éviter. `{ ...a }` copie le premier niveau seulement. Pour une copie profonde : `structuredClone(a)`.

Et **`===` sur deux objets compare les adresses**, pas le contenu : `{ x: 1 } === { x: 1 }` vaut `false`.

---

## 3. Les tableaux

```js
const main = ["feu", "eau"];
main.push("air");              // ajoute à la fin
main.pop();                    // retire le dernier
main.length;                   // → 2
main.includes("eau");          // → true
main.indexOf("terre");         // → -1

for (const c of main) console.log(c);          // ✅ for…of : les valeurs
main.forEach((c, i) => console.log(i, c));     // avec l'index
```

⚠️ **`for…in`** parcourt les *clés* (des chaînes `"0"`, `"1"`…), pas les valeurs. Utilise **`for…of`** pour un tableau.

---

## 4. Transformer sans boucle : `map`, `filter`, `find`, `reduce`

```js
const nombres = [1, 2, 3, 4];
const resultat = nombres
    .filter((n) => n % 2 === 0)   // garde les pairs        → [2, 4]
    .map((n) => n * 10)           // transforme chacun       → [20, 40]
    .reduce((s, n) => s + n, 0);  // combine en une valeur   → 60
```

| Méthode | Fait | Renvoie |
|---|---|---|
| `map(f)` | applique `f` à chaque élément | un **nouveau** tableau, même taille |
| `filter(f)` | garde ceux où `f` renvoie truthy | un **nouveau** tableau, plus petit |
| `find(f)` | cherche le premier qui correspond | l'élément, ou `undefined` |
| `some(f)` / `every(f)` | au moins un / tous ? | `boolean` |
| `reduce(f, init)` | accumule | une valeur |

**Lien :** `ft_lstmap` et `ft_lstiter` de la libft, en une ligne. Ces méthodes **ne modifient pas** le tableau d'origine — très important en React, où l'on ne doit jamais modifier l'état directement.

### Le piège de `sort`

```js
[1, 10, 2].sort();                  // → [1, 10, 2]  (trié comme du texte !)
[1, 10, 2].sort((a, b) => a - b);   // → [1, 2, 10]  ✅
```

`sort` **modifie** le tableau en place (contrairement à `map`/`filter`) et compare en chaînes par défaut. Donne toujours un comparateur — comme la fonction de `qsort`.

---

## 5. Déstructuration et spread

```js
const carte = { nom: "Aura", attaque: 7, cout: 3 };
const { nom, attaque } = carte;            // crée deux variables d'un coup
const [premiere, ...reste] = ["a", "b", "c"];   // premiere = "a", reste = ["b", "c"]

const boostee = { ...carte, attaque: 10 }; // copie + écrase un champ
const deck = [...main, "terre"];           // copie + ajoute

function afficher({ nom, cout }) {         // déstructuration dans les paramètres
    return `${nom} (${cout})`;
}
```

Tu verras ces deux syntaxes **à chaque ligne** de React : `const [etat, setEtat] = useState(0)` et `function Carte({ nom, cout }) { … }`.

---

## 6. JSON : le format d'échange

```js
const texte = JSON.stringify({ nom: "Aura", attaque: 7 });   // objet → chaîne
// texte === '{"nom":"Aura","attaque":7}'
const obj = JSON.parse(texte);                               // chaîne → objet
```

C'est le format de **tout** ce qui transite entre le frontend, le backend et les WebSockets. `JSON.parse` lance une exception si la chaîne est invalide : entoure-le d'un `try`/`catch` quand le texte vient de l'extérieur.

---

## 7. `Map` et `Set`

Pour un dictionnaire dont les clés changent souvent (ex. joueurs connectés par id) :

```js
const joueurs = new Map();
joueurs.set("u42", { nom: "Ana" });
joueurs.get("u42");     // → { nom: "Ana" }
joueurs.has("u7");      // → false
joueurs.delete("u42");

const vus = new Set(["a", "b", "a"]);   // pas de doublons → taille 2
```

**Lien :** ce sont `std::map` et `std::set` de CPP08/CPP09 (mais sans tri : l'ordre est celui d'insertion).

---

## ⚠️ À retenir

- Objets et tableaux = **références**. `=` ne copie pas.
- `{...obj}` / `[...arr]` = copie superficielle.
- `for…of` pour les valeurs, pas `for…in`.
- `map`/`filter` renvoient un nouveau tableau ; `sort`/`push` modifient l'original.
- `JSON.parse` sur des données externes → `try`/`catch`.
