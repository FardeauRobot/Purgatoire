# `async`, `await` — l'asynchrone lisible

> **TL;DR.** Une fonction `async` **renvoie toujours une `Promise`**. `await p` met la fonction en pause jusqu'à ce que `p` soit résolue, **sans bloquer le thread** : la boucle d'événements continue de servir le reste. Une promesse rejetée devient une exception au point du `await`. Oublier `await` est le bug n°1 : tu manipules la promesse au lieu de la valeur.

Liens : [`INDEX`](INDEX.md) · [`TRY_CATCH_THROW`](TRY_CATCH_THROW.md) · [`YIELD_GENERATEURS`](YIELD_GENERATEURS.md) · [`GLOSSAIRE`](GLOSSAIRE.md) (event loop, promesse) · fiche [`ASYNCHRONE`](../ASYNCHRONE.md)

---

## 1. Pont C/C++98

| C / C++98 | JavaScript | Note |
|---|---|---|
| `read()` bloquant | `await readFile()` | Pause **de la fonction**, pas du programme |
| `poll()` / `select()` + boucle (webserv) | la boucle d'événements | Fournie par le moteur, invisible |
| `pthread_create` + `pthread_join` | `Promise.all([...])` | Concurrence sur **un seul** thread |
| callback C `void (*cb)(int)` | `.then(cb)` | `await` remplace les `.then` imbriqués |

---

## 2. Les bases

```js
const wait = (ms, v) => new Promise(r => setTimeout(() => r(v), ms));

async function f() { return 42; }
console.log(f());                  // → Promise { 42 }   ⚠️ pas 42
console.log(await f());            // → 42

async function oubli() {
    const v = wait(1, "val");      // ⚠️ pas de await
    return typeof v;               // → "object" : c'est la Promise
}
```

`await` au niveau supérieur fonctionne dans un **module** (`.mjs`, `"type": "module"`). Ailleurs, seulement dans une fonction `async`.

---

## 3. L'ordre d'exécution

```js
console.log("A");
setTimeout(() => console.log("D timeout"), 0);
Promise.resolve().then(() => console.log("C microtâche"));
console.log("B");
// → A  B  C microtâche  D timeout
```

```
  pile d'appels : A, B (code synchrone, jusqu'au bout)
        │
        ▼ pile vide
  file des microtâches  (promesses)      → C   ← toujours vidée en premier
        │
        ▼
  file des tâches       (setTimeout, I/O) → D
```

---

## 4. Séquentiel ou parallèle

```js
await wait(50); await wait(50);                 // ~100 ms : l'un après l'autre
await Promise.all([wait(50), wait(50)]);        // ~50 ms  : en même temps

await Promise.allSettled([ok, ko]);             // → statuts 'fulfilled' / 'rejected', ne rejette jamais
await Promise.race([wait(30, "lent"), wait(1, "rapide")]);   // → rapide
```

`Promise.all` **rejette dès le premier échec**. `allSettled` attend tout le monde et te donne le bilan.

Dans une boucle, `forEach(async …)` lance tout et **n'attend rien**. À la place :

```js
for (const id of ids) await save(id);                       // séquentiel
await Promise.all(ids.map(id => save(id)));                 // parallèle
```

---

## 5. Pièges

- **Oublier `await`** : `if (await isAdmin())` et non `if (isAdmin())`. Une `Promise` est toujours *truthy*.
- **`await` dans `forEach`** : la boucle se termine avant les traitements. Voir §4.
- **Promesse rejetée sans `catch`** : Node affiche `UnhandledPromiseRejection` et, par défaut, **arrête le processus**.
- **`await` en série par réflexe** : deux requêtes indépendantes → `Promise.all`.
- `async` n'est pas un mot réservé (`const async = 1` est valide). `await` l'est dans un module.
