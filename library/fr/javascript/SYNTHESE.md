# 📒 Synthèse : JavaScript, TypeScript et la stack de Transcendance

> **TL;DR.** La fiche de révision du dossier. La partie 1 condense JavaScript, la partie 2 TypeScript, la partie 3 met toute la stack sur une page et renvoie à la synthèse de chaque technologie. Chaque ligne cite la fiche qui l'explique (`FICHE §n`) : si une ligne ne te parle plus, va relire la fiche, pas cette page. Les questions de fin se replient : réponds de tête avant d'ouvrir.

Liens : [`INDEX`](INDEX.md) · [`ABORDER`](ABORDER.md) · [`react/SYNTHESE`](react/SYNTHESE.md) · [`adonis/SYNTHESE`](adonis/SYNTHESE.md) · [`react-three-fiber/SYNTHESE`](react-three-fiber/SYNTHESE.md) · [`theatre-js/SYNTHESE`](theatre-js/SYNTHESE.md)

> ℹ️ Pas de code nouveau ici : les lignes de code sont des rappels d'une ligne, tirés des fiches citées.

---

## 1. JavaScript en une page

### Les trois idées de fond ([`PHILOSOPHIE`](PHILOSOPHIE.md))
- **Typage dynamique** : la valeur porte le type, pas la variable (§2).
- **Garbage collector** : pas de `delete`, mais on peut encore fuir (écouteur jamais retiré, `setInterval` jamais arrêté) (§3).
- **Un thread + une boucle d'événements** : ce qui est lent est asynchrone, on ne bloque jamais (§4).
- **Navigateur ≠ Node** : même langage, API différentes (§1).

### Variables et types ([`VARIABLES_TYPES`](VARIABLES_TYPES.md))
| Règle | Rappel |
|---|---|
| `const` par défaut, `let` si ça change, jamais `var` | §1 |
| 7 primitifs : `number`, `string`, `boolean`, `null`, `undefined`, `bigint`, `symbol`. Le reste est objet. | §2 |
| `` `PV : ${pv}` `` : les *template strings* | §3 |
| Toujours `===`, jamais `==` | §4 |
| Falsy : `false`, `0`, `""`, `null`, `undefined`, `NaN` (et `0n`) | §6 |
| `pv ?? 100` ne remplace que `null`/`undefined` ; `pv \|\| 100` remplace aussi `0` | §6 |

### Fonctions ([`FONCTIONS`](FONCTIONS.md))
- Une fonction est une **valeur** : on la stocke, on la passe, on la renvoie (§1, §3).
- Fléchée : `(a) => a * 2`. Paramètres par défaut, `...rest`, `...spread` (§2).
- **Closure** : une fonction garde l'accès aux variables qui l'entouraient à sa création (§4).
- `this` dépend de l'**appel** ; une fléchée n'a pas son propre `this` → fléchées dans les callbacks (§5).

### Objets et tableaux ([`OBJETS_TABLEAUX`](OBJETS_TABLEAUX.md))
- Objet `{ cle: valeur }` ≈ `struct` extensible ; tableau ≈ `std::vector` hétérogène (§1, §3).
- **Par référence** : `b = a` partage ; `{ ...a }` copie le premier niveau seulement (§2).
- `a?.b` : `undefined` au lieu d'une erreur si `a` est absent (§1).
- `map` (transformer), `filter` (garder), `find` (le premier), `reduce` (accumuler) (§4).
- Déstructuration : `const { nom, pv } = carte;`, `const [a, b] = paire;` (§5).
- `JSON.stringify` / `JSON.parse` : le format d'échange (§6). `Map` et `Set` quand les clés ne sont pas des chaînes (§7).

### Classes et modules ([`CLASSES_MODULES`](CLASSES_MODULES.md))
- `class`, `constructor`, `extends`, `super`, champs privés `#`. Pas de forme canonique (§1–2).
- Souvent, un objet simple suffit (§3).
- Un fichier = un module : `export` ce qui sort, `import` ce qui entre (§4).

### L'asynchrone ([`ASYNCHRONE`](ASYNCHRONE.md)) — la partie la plus importante
```text
  appel lent ──► Promise (en attente) ──► remplie (valeur)  ou  rejetée (erreur)
                         │
          await dans une fonction async : pause de CETTE fonction, le reste du programme continue
```
- `await` ne s'écrit que dans une fonction `async`, qui renvoie elle-même une `Promise` (§4).
- Erreurs : `try` / `catch` autour de l'`await` (§4).
- Indépendantes ? `Promise.all([...])` ; une seule échoue → tout échoue (§5).
- `await` dans un `forEach` n'attend rien : `for…of` (en série) ou `Promise.all` (en parallèle) (§5).

### Outils ([`OUTILS`](OUTILS.md))
`node f.js`, `npx tsx f.ts` · `package.json` = le Makefile · `npm install` / `npm run dev` · `node_modules/` ne se commite pas · `npm ci` dans Docker (installation stricte depuis le lock).

---

## 2. TypeScript en une page ([`TYPESCRIPT`](TYPESCRIPT.md))

| Notion | Rappel | § |
|---|---|---|
| Annoter | `let pv: number`, `function f(a: string): boolean` | 1 |
| Décrire | `type Carte = { nom: string; pv?: number }` ou `interface` | 2 |
| Union | `type Element = "feu" \| "eau"` ; le *narrowing* (`if (typeof x === "string")`) affine le type | 3 |
| Génériques | `Array<T>`, `function premier<T>(t: T[]): T` : les templates de TS | 4 |
| `any` / `unknown` / `!` | `any` éteint la vérification ; `unknown` oblige à prouver le type ; `!` est une promesse que tu fais au compilateur | 5 |
| **Effacement** | À l'exécution, plus aucun type. Toute entrée externe se **valide** | 6 |
| `tsconfig.json` | `"strict": true`, toujours | 7 |

🔗 **Lien C++.** TypeScript, c'est le compilateur sans le binaire : il vérifie comme `c++ -Wall -Werror`, puis jette les types. `static_cast` n'existe pas : un `as Carte` ne convertit **rien**, il fait taire le compilateur.

---

## 3. La stack en une page ([`VERS_TRANSCENDANCE`](VERS_TRANSCENDANCE.md))

```text
  NAVIGATEUR                                               SERVEUR                    BDD
 ┌───────────────────────────────────────┐   HTTPS + JSON   ┌──────────────────────┐  ┌──────────┐
 │ React 19 (composants, état, effets)   │ ───────────────► │ AdonisJS 7           │─►│ Postgres │
 │  ├ React Router (pages)               │ ◄─────────────── │  routes → contrôleurs│◄─│ (Lucid)  │
 │  ├ Zustand (état partagé)             │   Tuyau (typé)   │  Vine (validation)   │  └──────────┘
 │  ├ Tailwind (style)                   │                  │  auth (jetons)       │
 │  └ R3F (3D) ── Theatre.js (timeline)  │ ◄──── Socket.IO ──► start/ws.ts         │
 └───────────────────────────────────────┘    (temps réel)   └──────────────────────┘
          Caddy devant les deux · Docker Compose pour tout lancer
```

| Technologie | Elle répond à… | Commence par | Révise avec |
|---|---|---|---|
| **React** | une interface qui doit toujours refléter les données | [`react/PHILOSOPHIE`](react/PHILOSOPHIE.md), [`react/ABORDER`](react/ABORDER.md) | [`react/SYNTHESE`](react/SYNTHESE.md) |
| **AdonisJS** | un backend complet (routes, validation, BDD, auth, tests) sans tout assembler soi-même | [`adonis/ABORDER`](adonis/ABORDER.md) | [`adonis/SYNTHESE`](adonis/SYNTHESE.md) |
| **React Three Fiber** | de la 3D three.js écrite comme des composants React | [`react-three-fiber/ABORDER`](react-three-fiber/ABORDER.md) | [`react-three-fiber/SYNTHESE`](react-three-fiber/SYNTHESE.md) |
| **Theatre.js** | des animations réglées à la souris sur une timeline, rejouées en production | [`theatre-js/ABORDER`](theatre-js/ABORDER.md) | [`theatre-js/SYNTHESE`](theatre-js/SYNTHESE.md) |

### Les règles qui traversent toute la stack
1. **Le serveur fait autorité.** Le client envoie des **intentions**, le serveur renvoie l'**état** ([`react-three-fiber/ETAT_RESEAU`](react-three-fiber/ETAT_RESEAU.md) §5).
2. **Les types ne protègent pas à l'exécution** : Vine valide tout ce qui entre dans Adonis, HTTP comme socket ([`adonis/VINE`](adonis/VINE.md), [`adonis/SOCKET_IO`](adonis/SOCKET_IO.md) §5).
3. **Ce qu'on ouvre, on le ferme** : effet + nettoyage, `on` + `off`, abonnement + désabonnement ([`react/EFFETS`](react/EFFETS.md) §1, [`react/TEMPS_REEL`](react/TEMPS_REEL.md) §2).
4. **Rien de secret côté client** : ni dans une variable `VITE_`, ni dans le bundle ([`react/INSTALLATION`](react/INSTALLATION.md) §5).
5. **Ne jamais bloquer le thread** : côté serveur, tous les joueurs attendent ; côté client, l'écran gèle ([`ASYNCHRONE`](ASYNCHRONE.md) §1).

---

## 🧠 Questions de révision

> [!question]- Pourquoi `typeof null` vaut-il `"object"` ?
> Un bug historique de JavaScript, jamais corrigé pour ne pas casser le web. Teste `x === null` directement. ([`VARIABLES_TYPES`](VARIABLES_TYPES.md) §2)

> [!question]- Que vaut `0.1 + 0.2 === 0.3` ?
> `false`. `number` est un `double` IEEE 754, comme en C. ([`INDEX`](INDEX.md), piège 8)

> [!question]- Qu'affiche ce code, et dans quel ordre ? `console.log(1); setTimeout(() => console.log(2), 0); console.log(3);`
> `1`, `3`, `2`. Le rappel du minuteur passe par la boucle d'événements, après la fin du code en cours. ([`ASYNCHRONE`](ASYNCHRONE.md) §2)

> [!question]- Deux requêtes indépendantes de 50 ms chacune, avec deux `await` successifs : combien de temps ? Et avec `Promise.all` ?
> Environ 100 ms, puis environ 50 ms. ([`ASYNCHRONE`](ASYNCHRONE.md) §5)

> [!question]- Qu'est-ce qu'une closure, en une phrase ?
> Une fonction qui garde l'accès aux variables de l'endroit où elle a été créée, même après que cet endroit a fini de s'exécuter. ([`FONCTIONS`](FONCTIONS.md) §4)

> [!question]- `const copie = { ...partie }` puis `copie.joueurs.push(j)` : `partie.joueurs` change-t-il ?
> Oui. Le spread est superficiel : `joueurs` est le même tableau dans les deux objets. ([`OBJETS_TABLEAUX`](OBJETS_TABLEAUX.md) §2)

> [!question]- Que fait `x as Carte` à l'exécution ?
> Rien. C'est une annotation pour le compilateur, effacée au build. ([`TYPESCRIPT`](TYPESCRIPT.md) §6)

> [!question]- Comment TypeScript sait-il, dans `if (typeof v === "string") { … }`, que `v` est une chaîne dans le bloc ?
> Par le *narrowing* : il suit les tests de type et réduit l'union branche par branche. ([`TYPESCRIPT`](TYPESCRIPT.md) §3)

> [!question]- Quelle est la différence entre une variable `VITE_X` et une variable du `.env` d'Adonis ?
> `VITE_X` est **recopiée dans le bundle** et lisible par tout le monde. Le `.env` d'Adonis reste sur le serveur. ([`react/INSTALLATION`](react/INSTALLATION.md) §5)
