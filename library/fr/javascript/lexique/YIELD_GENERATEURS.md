# `function*`, `yield`, `yield*` — les générateurs

> **TL;DR.** Une fonction `function*` peut **se mettre en pause** à chaque `yield` et reprendre plus tard, en gardant ses variables locales. L'appeler ne l'exécute pas : on obtient un **itérateur**, qu'on fait avancer avec `.next()` ou qu'on parcourt avec `for…of`. Usage courant : rendre une classe itérable, produire une suite infinie, ou parcourir un flux asynchrone avec `for await`.

Liens : [`INDEX`](INDEX.md) · [`BOUCLES`](BOUCLES.md) · [`ASYNC_AWAIT`](ASYNC_AWAIT.md) · [`GLOSSAIRE`](GLOSSAIRE.md) (itérable)

---

## 1. Pont C/C++98

| C++98 | JavaScript | Note |
|---|---|---|
| classe itérateur (`operator++`, `operator*`, `begin`/`end`) | `*[Symbol.iterator]() { … yield … }` | Quelques lignes au lieu d'une classe entière |
| `static` local qui garde l'état entre appels | variables locales du générateur | Chaque générateur a son propre état |
| `it != end()` | `{ done: true }` | |

---

## 2. Mécanique

```js
function* compteur() {
    yield 1;
    yield 2;
    return "fin";
}

const g = compteur();       // rien n'est exécuté ici
g.next();   // → { value: 1, done: false }
g.next();   // → { value: 2, done: false }
g.next();   // → { value: 'fin', done: true }
g.next();   // → { value: undefined, done: true }

console.log([...compteur()]);   // → [ 1, 2 ]   (for…of et ... ignorent la valeur du return)
```

```
 appel        next()          next()          next()
   │    ┌────────────┐  ┌────────────┐  ┌──────────────┐
   └──► │ → yield 1 ⏸│─►│ → yield 2 ⏸│─►│ → return fin │
        └────────────┘  └────────────┘  └──────────────┘
```

---

## 3. Suites infinies et dialogue

```js
function* ids() { let i = 0; while (true) yield i++; }
const it = ids();
console.log(it.next().value, it.next().value, it.next().value);   // → 0 1 2
```

Pas de boucle infinie : le générateur ne calcule que ce qu'on lui demande (évaluation **paresseuse**).

```js
function* dialogue() {
    const n = yield "nom ?";          // la valeur de next(...) arrive ici
    yield `salut ${n}`;
}
const d = dialogue();
console.log(d.next().value, d.next("Rex").value);   // → nom ? salut Rex
```

`yield*` délègue à un autre itérable :

```js
function* inner() { yield "a"; yield "b"; }
function* outer() { yield* inner(); yield "c"; }
console.log([...outer()]);   // → [ 'a', 'b', 'c' ]
```

---

## 4. Rendre une classe itérable

```js
class Range {
    constructor(n) { this.n = n; }
    *[Symbol.iterator]() { for (let i = 0; i < this.n; i++) yield i; }
}
console.log([...new Range(3)]);   // → [ 0, 1, 2 ]
```

---

## 5. Version asynchrone : `for await`

```js
async function* ticks() {
    for (let i = 0; i < 3; i++) { await wait(1); yield i; }
}
for await (const x of ticks()) console.log(x);   // → 0 1 2
```

Sert à consommer un flux : lignes d'un fichier, pages d'une API, messages d'une socket.

---

## 6. Pièges

- **Un générateur ne se rembobine pas** : une fois `done`, il faut en créer un nouveau.
- **`[...ids()]` sur une suite infinie** ne s'arrête jamais.
- **Pas de fléchée génératrice** : `*() => …` n'existe pas.
- `yield` n'est réservé que dans un générateur et en mode strict.
