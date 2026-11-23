# 🎬 Theatre.js : une timeline pour ta scène R3F

> Rédigé directement en français, comme le reste de [`javascript/`](../INDEX.md).

**Theatre.js** ajoute à ta scène une **timeline**, comme celle d'After Effects ou de Blender. Le **studio** (en dev) te laisse poser des keyframes à la souris, choisir les courbes et voir le résultat en direct. Le **core** (en production) rejoue ces keyframes depuis un fichier `state.json`. **`@theatre/r3f`** branche le tout sur React Three Fiber : `<e.mesh>` à la place de `<mesh>`, et l'objet apparaît dans le studio.

Ce dossier fait suite à [`../react-three-fiber/`](../react-three-fiber/INDEX.md) : il suppose `useFrame`, les refs et `damp` connus. Il vise ce dont Transcendance a besoin : des cinématiques (intro, victoire) et des effets mis en scène (une carte jouée).

Liens : [`../INDEX`](../INDEX.md) · [`../react-three-fiber/INDEX`](../react-three-fiber/INDEX.md) · [`../VERS_TRANSCENDANCE`](../VERS_TRANSCENDANCE.md) · [doc Theatre.js](https://www.theatrejs.com/docs/latest) · [dépôt](https://github.com/theatre-js/theatre)

---

## 🧭 Ordre de lecture

| Étape | Fiche | Labo | Pourquoi maintenant |
|---|---|---|---|
| 0 | [`INSTALLATION`](INSTALLATION.md) | — | Les trois paquets, le conflit avec R3F 9 (`overrides`), le studio en dev seulement. |
| 1 | [`CONCEPTS`](CONCEPTS.md) | — | Projet, feuille, objet, séquence. Les noms sont l'identité. Theatre ou `damp` ? |
| 2 | [`CORE_A_LA_MAIN`](CORE_A_LA_MAIN.md) | — | `sheet.object`, les types de props, `onValuesChange` + ref : le mécanisme sans magie. |
| 3 | [`EDITABLE`](EDITABLE.md) | — | `<SheetProvider>`, `e.mesh`, les caméras, animer un matériau. |
| 4 | [`STUDIO`](STUDIO.md) | — | Séquencer une prop, poser des keyframes, les courbes, où le travail est sauvé. |
| 5 | [`SEQUENCES`](SEQUENCES.md) | — | `play()` et sa promesse, la tête de lecture, le scroll, le son. |
| 6 | [`PRODUCTION`](PRODUCTION.md) | — | `state.json`, le conflit navigateur ↔ disque, vérifier un build. |
| 7 | [`INSTANCES_CARTES`](INSTANCES_CARTES.md) | — | Atelier : un effet joué par chaque carte, puis le relais à `damp`. |
| 8 | [`LIMITES_DEBUG`](LIMITES_DEBUG.md) | — | Un projet plus maintenu, les pannes classiques, les alternatives. |

> 🌟 Si tu n'as le temps que pour trois fiches : **CONCEPTS**, **EDITABLE**, **PRODUCTION**.

> 🪜 **Avant l'étape 0** : [`ABORDER`](ABORDER.md), les prérequis, le modèle mental et l'ordre d'étude. 📒 **Pour réviser** : [`SYNTHESE`](SYNTHESE.md), tout le dossier en une fiche, avec quiz.

---

## 🧪 Le labo et ces fiches

- Il n'y a **pas encore de labo** Theatre. Pour suivre les fiches, pars d'une copie de `~/Labo/TestThreeFiber` : ajoute les trois paquets avec l'override ([`INSTALLATION`](INSTALLATION.md)), crée `src/theatre.ts` et `src/theatreStudio.ts`, puis colle les modules des fiches un par un.
- Comme pour R3F, les extraits sont en **TypeScript** (`.tsx`). Retire les annotations de type pour un labo en `.jsx`.
- Chaque fiche se termine par **« 🧪 À essayer »** : des manipulations dans le studio et des pannes à provoquer volontairement.

---

## ✅ Ce qui a été vérifié

Versions vérifiées sur npm le **19/11/2026** : `@theatre/core`, `@theatre/studio` et `@theatre/r3f` 0.7.2 (publiés le 19/05/2024), avec `@react-three/fiber` 9.8.2, `@react-three/drei` 10.7.9, `three` 0.187.0, `react` 19.3.1, Vite 8.3.8.

- **Installation** : sans override, `npm install` échoue (`ERESOLVE`) ; avec le bloc `overrides` de [`INSTALLATION`](INSTALLATION.md), il passe, et `npm ls` montre une seule copie de R3F 9.8.2.
- **Typecheck** : chaque bloc ` ```ts ` / ` ```tsx ` est un module complet, vérifié avec `tsc --strict` et les options du gabarit Vite `react-ts`. Le module `./theatre` qu'ils importent est celui de [`PRODUCTION`](PRODUCTION.md), §1.
- **Lancé dans Chrome** (sans écran, rendu logiciel) :
  - le core seul, branché à la main : valeur interpolée correcte (1.5 à mi-course), `play()` qui va au bout ;
  - `@theatre/r3f` sans studio : un `e.mesh` suit `state.json` ;
  - `@theatre/r3f` + studio + extension sur R3F 9 : outline, panneau de propriétés, éditeur de snapshot et timeline s'affichent ;
  - le chargement du studio par `import()` derrière `import.meta.env.DEV` : présent sous `npm run dev`, absent du build (1,1 Mo au lieu de 2,4 Mo, `grep` à 0) ;
  - les pannes citées dans les fiches (studio non initialisé, `.default` en import dynamique, pointer concaténé, `state: {}`, `play()` avant `ready`), avec leur message exact ;
  - les instances de feuille (mêmes keyframes, têtes de lecture indépendantes), `pause()` puis reprise, un second `play()` qui interrompt le premier.
- **Lu dans le code source** des paquets : les props de chaque balise `e.*`, les libellés du studio (menus, courbes, boîte de dialogue), la règle navigateur ↔ disque, l'ordre de `revisionHistory`.
- **Non vérifié** : le déplacement d'objets à la souris dans l'éditeur de snapshot, l'enchaînement exact des clics du studio (les libellés, eux, sont exacts), la façon dont l'outline liste les instances, l'espace colorimétrique de `types.rgba` appliqué par `setRGB`, `attachAudio` avec un vrai fichier son, et les alternatives de [`LIMITES_DEBUG`](LIMITES_DEBUG.md).
- **`ABORDER` et `SYNTHESE`** ne contiennent pas de code nouveau : leurs extraits sont recopiés des fiches du dossier, qui indiquent leur source. Ce sont des **extraits**, parfois raccourcis par `// …` ou privés de leurs imports : ils ne sont pas des modules complets et ne comptent pas dans la vérification ci-dessus.
- Si la doc officielle contredit une fiche, c'est la doc qui a raison. Corrige la fiche.

---

## ⚖️ R3F seul ↔ R3F + Theatre en un coup d'œil

| R3F seul | Avec Theatre |
|---|---|
| `<mesh position={[0, 1, 0]}>` | `<e.mesh theatreKey="Carte" position={[0, 1, 0]}>` : la position devient la valeur **initiale**, réglable dans le studio |
| Une valeur à régler : Leva | Une prop **statique** dans le studio |
| `if (t < 0.4) … else …` dans `useFrame` | Des keyframes dans la timeline, exportées en `state.json` |
| `damp(actuel, cible, 8, delta)` | Une courbe (`In Out`, `Back Out`…) entre deux keyframes |
| Relancer une animation : remettre un compteur à 0 | `sheet.sequence.position = 0; sheet.sequence.play()` |
| Le code contient l'animation | Le code contient les **noms**, le JSON contient l'animation |

🔗 **Lien C.** C'est le passage d'une map écrite en dur dans le code à une map lue depuis un `.ber`, comme dans `so_long`.

---

## 🚨 Les pièges, toutes fiches confondues

1. **`npm install` refuse** (`ERESOLVE`) : `@theatre/r3f` attend R3F 8. Un `overrides`, pas `--legacy-peer-deps`. → [`INSTALLATION`](INSTALLATION.md)
2. **Studio importé, jamais initialisé** : `project.ready` ne se résout jamais, tout est figé, souvent **seulement en production**. → [`INSTALLATION`](INSTALLATION.md)
3. **Le studio importé tel quel** : `studio.initialize is not a function` en dev (Vite 8), ou `e.initialize…` avec `(await import(…)).default`. La ligne `const studio` de `theatreStudio.ts`. → [`INSTALLATION`](INSTALLATION.md)
4. **Renommer une feuille ou un objet** : ses keyframes deviennent orphelines. → [`CONCEPTS`](CONCEPTS.md)
5. **`setState` dans `onValuesChange`** : un re-rendu par image pendant la lecture. Mute une ref. → [`CORE_A_LA_MAIN`](CORE_A_LA_MAIN.md)
6. **`additionalProps` écrit en ligne** : un objet neuf par rendu, une reconfiguration par rendu. → [`EDITABLE`](EDITABLE.md)
7. **`e.perspectiveCamera`** : ne filme pas. `PerspectiveCamera` de `@theatre/r3f` + `makeDefault`. → [`EDITABLE`](EDITABLE.md)
8. **Ne pas exporter** : le travail ne vit que dans le `localStorage` d'une origine. → [`STUDIO`](STUDIO.md)
9. **`play()` avant `ready`** : ne joue rien, et renvoie quand même `true`. → [`SEQUENCES`](SEQUENCES.md)
10. **Pointer utilisé comme une valeur** : `object is not a function`. Passe par `val()`. → [`SEQUENCES`](SEQUENCES.md)
11. **`state.json` pas branché ou périmé** : rien n'anime, ou pas la bonne version. → [`PRODUCTION`](PRODUCTION.md)
12. **`state: {}` sans studio** : erreur au démarrage. → [`PRODUCTION`](PRODUCTION.md)
13. **Deux `play()` sur la même feuille** : le second coupe le premier. Des instances. → [`INSTANCES_CARTES`](INSTANCES_CARTES.md)
14. **Projet non maintenu** : chaque mise à jour de React, R3F ou three.js est à retester. → [`LIMITES_DEBUG`](LIMITES_DEBUG.md)
