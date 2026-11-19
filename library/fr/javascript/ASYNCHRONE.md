# ⏳ L'asynchrone : promesses, `async`/`await` et boucle d'événements

> **TL;DR.** Une opération lente (réseau, BDD, timer) ne bloque jamais : elle renvoie une **`Promise`**, un « bon de retrait » qui sera rempli plus tard. Dans une fonction **`async`**, **`await`** met la fonction en pause jusqu'à ce que la promesse soit remplie, **sans** bloquer le reste du programme. Les erreurs se rattrapent avec `try`/`catch`. Oublier `await` est le bug le plus fréquent.

Liens : [`INDEX`](INDEX.md) · [`PHILOSOPHIE`](PHILOSOPHIE.md) · [`FONCTIONS`](FONCTIONS.md) · [`VERS_TRANSCENDANCE`](VERS_TRANSCENDANCE.md)

---

## 1. Le problème

Un serveur de jeu reçoit les actions de 10 joueurs. L'un d'eux déclenche une requête en base de données qui prend 50 ms. Si le serveur **attendait** comme un `read()` bloquant, les 9 autres seraient gelés pendant ce temps — et JS n'a qu'un thread.

Solution : on **lance** l'opération, on s'occupe d'autre chose, et on reprend quand le résultat arrive.

---

## 2. La boucle d'événements

```
   ┌──────────────────────────────┐
   │  pile d'appels (ton code)    │  ← s'exécute jusqu'au bout, sans interruption
   └──────────────┬───────────────┘
                  │ vide ?
                  ▼
   ┌──────────────────────────────┐
   │  file des micro-tâches       │  ← suites de promesses (.then, après un await)
   └──────────────┬───────────────┘   vidée EN ENTIER à chaque fois
                  │ vide ?
                  ▼
   ┌──────────────────────────────┐
   │  file des tâches             │  ← timers, messages réseau, clics…
   └──────────────────────────────┘   une seule à la fois, puis on remonte
```

```js
console.log("A");
setTimeout(() => console.log("B"));            // tâche
Promise.resolve().then(() => console.log("C")); // micro-tâche
console.log("D");
// → A, D, C, B
```

1. `A` et `D` : le code synchrone s'exécute d'abord, en entier.
2. `C` : la pile est vide → on vide les micro-tâches.
3. `B` : ensuite seulement, la tâche du timer (même avec un délai de 0).

**Lien :** c'est la boucle `poll()` de webserv — attendre des événements, traiter chacun jusqu'au bout, recommencer — mais écrite pour toi par le moteur. Conséquence : **une boucle de calcul longue gèle tout**, parce que rien d'autre ne peut s'exécuter tant qu'elle tourne.

---

## 3. Les promesses

Une `Promise` est dans l'un de trois états : **en attente** → **tenue** (avec une valeur) ou **rompue** (avec une erreur).

```js
function attendre(ms) {
    return new Promise((resolve) => setTimeout(resolve, ms));
}

fetch("/api/cartes")                          // renvoie une Promise<Response>
    .then((reponse) => reponse.json())        // renvoie une Promise<données>
    .then((cartes) => console.log(cartes))
    .catch((err) => console.error("échec :", err));
```

Ça marche, mais les chaînes de `.then` deviennent vite illisibles. D'où…

---

## 4. `async` / `await`

```js
async function chargerCartes() {
    try {
        const reponse = await fetch("/api/cartes");
        if (!reponse.ok) throw new Error(`HTTP ${reponse.status}`);
        const cartes = await reponse.json();
        return cartes;
    } catch (err) {
        console.error("échec :", err);
        return [];
    }
}
```

- **`async`** devant la fonction : elle renvoie **toujours** une `Promise` (ici, `Promise<cartes>`).
- **`await`** : « mets cette fonction en pause jusqu'au résultat ». Pendant la pause, la boucle d'événements fait autre chose.
- **`try`/`catch`** : rattrape une promesse rompue, exactement comme une exception C++.
- `fetch` ne rompt **pas** la promesse sur une erreur 404/500 : il faut tester `reponse.ok` soi-même.

Ça se lit comme du code C séquentiel, mais ça ne bloque rien.

---

## 5. Les pièges

### Oublier `await`

```js
async function getPv() { return 30; }

const pv = getPv();          // ❌ pv est une Promise, pas 30
console.log(pv + 1);         // → "[object Promise]1"
const bon = await getPv();   // ✅ 30
```

Si tu vois `[object Promise]` ou `Promise { <pending> }` dans un log, c'est un `await` manquant.

### `await` en série alors que ce pourrait être en parallèle

```js
const joueur = await chargerJoueur(id);     // 50 ms
const deck   = await chargerDeck(id);       // + 50 ms = 100 ms

const [j, d] = await Promise.all([          // les deux en même temps ≈ 50 ms
    chargerJoueur(id),
    chargerDeck(id),
]);
```

`Promise.all` attend toutes les promesses ; si **une** échoue, l'ensemble échoue.

### Une promesse rompue jamais rattrapée

Sans `try`/`catch` (ou `.catch`), une erreur dans une fonction `async` devient une *unhandled rejection* — Node peut alors arrêter le processus. Toute frontière (handler de route, message WebSocket) doit rattraper ses erreurs.

### `forEach` n'attend pas

```js
for (const id of ids) await sauver(id);              // ✅ une après l'autre
await Promise.all(ids.map((id) => sauver(id)));      // ✅ toutes en parallèle
ids.forEach(async (id) => await sauver(id));         // ❌ n'attend rien du tout
```

---

## ⚠️ À retenir

- Lent = asynchrone = `Promise`.
- `async` + `await` + `try`/`catch` : la combinaison à utiliser 95 % du temps.
- `[object Promise]` dans un log → il manque un `await`.
- Indépendantes ? `Promise.all`.
- Jamais `forEach` avec `async`.
