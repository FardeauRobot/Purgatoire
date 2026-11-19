# 🟨 JavaScript / TypeScript — l'essentiel pour Transcendance

> Rédigé directement en français (pas de version anglaise pour l'instant).

Une petite bibliothèque écrite pour quelqu'un qui connaît **le C et le C++98** et qui découvre le web. Chaque fiche répond à la même question : *« Je sais comment ça marche en C++. Qu'est-ce que JavaScript fait à la place, et pourquoi ? »*

Le but n'est pas de tout couvrir : c'est de te donner **assez de bases pour lire et écrire le code de Transcendance** (React côté frontend, AdonisJS côté backend, tous les deux en TypeScript) sans tomber dans les pièges classiques.

Voir aussi : [sujet ft_transcendence](../projects/LEVEL6/ft_transcendence-subject.md)

---

## 🧭 Ordre de lecture conseillé

| Étape | Fiche | Pourquoi maintenant |
|---|---|---|
| 1 | [`PHILOSOPHIE`](PHILOSOPHIE.md) | Le changement d'état d'esprit : pas de compilation native, pas de `delete`, un seul thread. |
| 2 | [`OUTILS`](OUTILS.md) | Node, npm, `package.json` : pour pouvoir lancer chaque exemple toi-même. |
| 3 | [`VARIABLES_TYPES`](VARIABLES_TYPES.md) | `let`/`const`, les types primitifs, `===`, les pièges de conversion. |
| 4 | [`FONCTIONS`](FONCTIONS.md) | Fonctions fléchées, closures, callbacks : le cœur du langage. |
| 5 | [`OBJETS_TABLEAUX`](OBJETS_TABLEAUX.md) | Les deux structures que tu manipuleras partout, et `map`/`filter`. |
| 6 | [`CLASSES_MODULES`](CLASSES_MODULES.md) | Les classes (proches du C++) et `import`/`export`. |
| 7 | [`ASYNCHRONE`](ASYNCHRONE.md) | **La fiche la plus importante.** Promesses, `async`/`await`, boucle d'événements. |
| 8 | [`TYPESCRIPT`](TYPESCRIPT.md) | Ajouter des types à tout ça, et comprendre ce qu'ils ne protègent **pas**. |
| 9 | [`VERS_TRANSCENDANCE`](VERS_TRANSCENDANCE.md) | Où chaque notion réapparaît dans React, AdonisJS et les WebSockets. |

> 🌟 Si tu n'as le temps que pour trois fiches : **VARIABLES_TYPES**, **ASYNCHRONE**, **TYPESCRIPT**.

### Pour aller plus loin

| Fiche | Contenu |
|---|---|
| [`lexique/`](lexique/INDEX.md) | **Un mot-clé par page**, de `async` à `yield`, chacun relié à son équivalent C/C++98, plus un [`GLOSSAIRE`](lexique/GLOSSAIRE.md) des concepts (closure, prototype, event loop…). Pour **retrouver** un mot vite, pas pour apprendre dans l'ordre. |
| [`react-three-fiber/`](react-three-fiber/INDEX.md) | **Dossier de 13 fiches** sur la 3D en composants React, calé sur le labo `TestThreeFiber` : scène, lumières, `useFrame`, clics, hiérarchie, drei, Leva, modèles glTF, personnages animés par squelette, état et réseau, atelier « main de cartes », performance. Pour le module Major « Advanced 3D graphics ». À lire après **VERS_TRANSCENDANCE**. |

---

## ⚖️ C++98 ↔ JavaScript en un coup d'œil

| C++98 | JavaScript / TypeScript |
|---|---|
| Compilé en binaire natif | Interprété / compilé à la volée par un moteur (V8) |
| `int`, `double`, `char`… | Un seul type numérique : `number` (un `double`) |
| `std::string` | `string` (immuable) |
| Tableau C / `std::vector` | `Array` (taille dynamique, types mélangeables) |
| `std::map<std::string, T>` | Objet `{}` ou `Map` |
| `new` / `delete` | `new` seulement — le *garbage collector* libère |
| Pointeurs, références | Tout objet est manipulé **par référence** ; les primitifs **par valeur** |
| `#include` | `import` / `export` |
| Exceptions `try`/`catch` | Pareil, plus les promesses rejetées |
| Threads (`pthread`) | Un seul thread + une boucle d'événements |
| Types vérifiés à la compilation | JS : aucun. TS : à la compilation **seulement**, effacés à l'exécution |
| `Makefile` | `package.json` + `npm run …` |

---

## 🧪 Tester un exemple

```sh
node fichier.js            # JavaScript
npx tsx fichier.ts         # TypeScript, sans config
```

Tous les extraits marqués d'un résultat en commentaire (`// → …`) ont été exécutés avec Node v26.

---

## 🚨 Les 10 pièges qui piquent le plus

1. `==` convertit les types → utilise **toujours `===`**.
2. `const` empêche la **réaffectation**, pas la **modification** d'un objet.
3. Copier un objet avec `=` copie la **référence**, pas le contenu.
4. `{...obj}` est une copie **superficielle** : les sous-objets restent partagés.
5. Oublier `await` → tu manipules une `Promise`, pas la valeur.
6. `typeof null === "object"` (un bug historique, jamais corrigé).
7. `[1, 10, 2].sort()` trie **en texte** → `[1, 10, 2]`. Passe un comparateur.
8. `0.1 + 0.2 !== 0.3` (comme en C avec les `double`).
9. Les types TypeScript **disparaissent** à l'exécution → le backend doit **valider** toute entrée.
10. `this` dépend de **la façon d'appeler** une fonction → préfère les fonctions fléchées dans les callbacks.
