# 🪜 Aborder Theatre.js

> **TL;DR.** Theatre.js est une **timeline** pilotable depuis le code : tu règles une animation à la souris dans le **studio** (en dev), tu l'exportes en `state.json`, et le **core** la rejoue en production. C'est la **dernière marche** de la stack 3D : il suppose R3F déjà acquis (`useFrame`, refs, `damp`), parce que toute sa mécanique se branche dessus. Prérequis en plus : la notion de **keyframe** et de **courbe d'interpolation**, et l'asynchrone (`project.ready`, la promesse de `play()`). Aborde-le **à la main d'abord** (`sheet.object` + `onValuesChange`), puis avec `@theatre/r3f`, et garde en tête qu'il **n'est plus maintenu** depuis mai 2024.

Liens : [`INDEX`](INDEX.md) · [`SYNTHESE`](SYNTHESE.md) · [`../react-three-fiber/ABORDER`](../react-three-fiber/ABORDER.md) · [`../react-three-fiber/SYNTHESE`](../react-three-fiber/SYNTHESE.md) · [doc Theatre.js](https://www.theatrejs.com/docs/latest)

---

## 1. À quel besoin il répond

Une intro de partie, un écran de victoire, une carte jouée qui saute et tourne avant de se poser : plusieurs props bougent **ensemble**, selon un timing qu'on règle **à l'œil**. Écrit en code, ça donne des `if (t < 0.4) … else …` dans `useFrame`, et chaque retouche impose de modifier, recharger, regarder, recommencer.

Theatre sort l'animation du code ([`CONCEPTS`](CONCEPTS.md) §1) :
- **le code contient les noms** (un projet, des feuilles, des objets, leurs props) ;
- **le JSON contient l'animation** (les keyframes et les courbes), produit à la souris dans le studio.

🔗 **Lien C.** C'est le passage d'une map écrite en dur dans le code à une map lue depuis un `.ber`, comme dans `so_long` ([`INDEX`](INDEX.md)).

Et ce à quoi il **ne** répond **pas** ([`CONCEPTS`](CONCEPTS.md) §5) : une animation qui suit l'état et peut être interrompue (survol, main qui se réorganise) reste du `damp` dans `useFrame` ; la marche d'un personnage reste un clip glTF ; un simple réglage reste Leva.

---

## 2. Ce que tu dois savoir avant

### Indispensable : le dossier R3F

| Prérequis | Où Theatre s'en sert | Où le revoir |
|---|---|---|
| `<Canvas>`, meshes, matériaux, caméra | `e.mesh`, `e.directionalLight`, la caméra de Theatre | [`../react-three-fiber/ANATOMIE`](../react-three-fiber/ANATOMIE.md) |
| `useRef` sur un objet three.js | `onValuesChange` **mute la ref** | [`../react-three-fiber/ANIMATION`](../react-three-fiber/ANIMATION.md) §2 |
| La règle « jamais `setState` dans la boucle » | la même, dans `onValuesChange` | [`../react-three-fiber/ANIMATION`](../react-three-fiber/ANIMATION.md) §4 |
| `damp` et « l'état donne la cible » | le relais Theatre → `damp` des cartes | [`../react-three-fiber/ANIMATION`](../react-three-fiber/ANIMATION.md) §6, [`../react-three-fiber/ATELIER_CARTES`](../react-three-fiber/ATELIER_CARTES.md) |
| `useEffect` et sa fonction de nettoyage | renvoyer le désabonnement de `onValuesChange` | [`../react/EFFETS`](../react/EFFETS.md) §1 |
| Promesses, `async` / `await` | `await project.ready`, `await sequence.play()` | [`../ASYNCHRONE`](../ASYNCHRONE.md) |
| `import()` dynamique, `import.meta.env.DEV` | charger le studio en dev seulement | [`../CLASSES_MODULES`](../CLASSES_MODULES.md), [`INSTALLATION`](INSTALLATION.md) §3 |

Si [`../react-three-fiber/ANIMATION`](../react-three-fiber/ANIMATION.md) n'est pas clair pour toi, reviens-y avant : Theatre ne fait que fournir **d'autres valeurs** au même mécanisme.

### Utile : le vocabulaire de l'animation

- **Keyframe** : une valeur fixée à un instant. Entre deux keyframes, le moteur **interpole** ([`CONCEPTS`](CONCEPTS.md) §4).
- **Courbe** (*easing*) : la façon de passer d'une keyframe à l'autre : linéaire, qui accélère puis ralentit, qui dépasse et revient, ou qui saute d'un coup (`Hold`) ([`STUDIO`](STUDIO.md) §4).
- **Tête de lecture** : l'instant courant de la timeline, en secondes ([`SEQUENCES`](SEQUENCES.md) §3).
- Si tu as déjà touché à After Effects, Blender ou un montage vidéo, tu as tout ce vocabulaire.

### Utile : `npm` un peu plus loin

Les **peer dependencies** et le bloc `overrides` de `package.json` : Theatre 0.7.2 réclame R3F 8, il faut forcer R3F 9 ([`INSTALLATION`](INSTALLATION.md) §2). Et `git` pour commiter `state.json`.

### Pas nécessaire

GSAP, Blender ou une autre bibliothèque d'animation. Ni le code source de Theatre.

---

## 3. Le modèle mental à installer

```text
  DEV                                               PRODUCTION
  ───                                               ──────────
  studio (souris) ─► keyframes ─► localStorage       state.json (commité)
                                     │                     │
                         Export … to JSON ──► src/state.json
                                                           ▼
                                      getProject("Aura", { state })  ─►  core calcule les valeurs
                                                                               │
                                       onValuesChange(v) / e.mesh  ◄───────────┘
                                                   │
                                       ref.current.position.set(…)   (jamais setState)
```

Quatre phrases :
1. **Projet → feuilles → objets → props** ; chaque feuille a **une** séquence ([`CONCEPTS`](CONCEPTS.md) §2).
2. **Les noms sont l'identité** : renommer une feuille ou un objet rend ses keyframes orphelines ([`CONCEPTS`](CONCEPTS.md) §2).
3. **Le studio écrit, le core lit** ; sans `state.json`, la production joue les valeurs par défaut ([`CONCEPTS`](CONCEPTS.md) §3, [`PRODUCTION`](PRODUCTION.md) §1).
4. **Les valeurs Theatre arrivent par un callback** que tu branches sur une ref, exactement comme `useFrame` ([`CORE_A_LA_MAIN`](CORE_A_LA_MAIN.md) §4).

---

## 4. Comment s'y prendre

Il n'y a pas encore de labo Theatre : pars d'une **copie** de `~/Labo/TestThreeFiber` ([`INDEX`](INDEX.md), « Le labo et ces fiches »).

1. **Installe** avec l'`overrides`, studio en `import()` derrière `DEV` ([`INSTALLATION`](INSTALLATION.md)). Vérifie que le studio s'affiche **et** qu'un `npm run build` ne le contient pas.
2. **Comprends la hiérarchie** sur papier ([`CONCEPTS`](CONCEPTS.md)) avant d'ouvrir le studio.
3. **Branche un objet à la main** ([`CORE_A_LA_MAIN`](CORE_A_LA_MAIN.md)) : `sheet.object` + `onValuesChange` + ref. C'est le mécanisme sans magie ; après ça, `@theatre/r3f` n'a plus de mystère.
4. **Passe à `e.mesh`** ([`EDITABLE`](EDITABLE.md)), puis **anime dans le studio** ([`STUDIO`](STUDIO.md)). Exporte **dès la première** animation qui te plaît.
5. **Pilote depuis le code** ([`SEQUENCES`](SEQUENCES.md)) : `await project.ready`, puis `play()`.
6. **Branche `state.json` et teste un build** ([`PRODUCTION`](PRODUCTION.md) §1, §6), avant de croire que ça marche.
7. **L'atelier** ([`INSTANCES_CARTES`](INSTANCES_CARTES.md)) quand l'atelier R3F des cartes tourne. Garde [`LIMITES_DEBUG`](LIMITES_DEBUG.md) §2 sous la main pour les pannes.

---

## 5. Les erreurs de débutant à éviter

- **`--legacy-peer-deps`** au lieu d'un `overrides` ([`INSTALLATION`](INSTALLATION.md) §2).
- **Importer le studio sans l'initialiser** : tout est figé, souvent seulement en production ([`INSTALLATION`](INSTALLATION.md) §3).
- **Oublier d'exporter** : le travail ne vit que dans le `localStorage` de ce navigateur ([`STUDIO`](STUDIO.md) §5).
- **`play()` avant `project.ready`** : rien ne joue, et la promesse dit quand même `true` ([`SEQUENCES`](SEQUENCES.md) §1).
- **Utiliser Theatre pour un survol** : il ne sait pas viser une cible qui bouge ; c'est `damp` ([`LIMITES_DEBUG`](LIMITES_DEBUG.md) §3).
- **Le laisser se répandre** dans tout le code : garde-le isolé dans quelques fichiers, pour pouvoir le remplacer ([`LIMITES_DEBUG`](LIMITES_DEBUG.md) §1, §4).

---

## 6. Tu es prêt pour l'atelier si…

> [!question]- Quelle différence entre `@theatre/core` et `@theatre/studio`, et lequel part en production ?
> Le core calcule les valeurs à partir des keyframes : il est livré. Le studio est l'éditeur visuel : en dev seulement. ([`INSTALLATION`](INSTALLATION.md) §1)

> [!question]- Pourquoi `sheet.object(…)` s'écrit-il au niveau du module et pas dans le composant ?
> Le composant est rappelé à chaque rendu ; l'objet Theatre doit exister une seule fois, comme le projet. ([`CORE_A_LA_MAIN`](CORE_A_LA_MAIN.md) §4)

> [!question]- Pourquoi `return cubeObj.onValuesChange(…)` dans le `useEffect` ?
> `onValuesChange` renvoie la fonction de désabonnement ; `useEffect` l'appelle au démontage. Sinon, le callback viserait un mesh détruit. ([`CORE_A_LA_MAIN`](CORE_A_LA_MAIN.md) §4)

> [!question]- Tu renommes l'objet `"Cube"` en `"Carte"`. Que deviennent ses keyframes ?
> Elles restent dans `state.json` sous l'ancien nom, orphelines : le nouvel objet n'a aucune animation. ([`CONCEPTS`](CONCEPTS.md) §2)

> [!question]- Une animation marche en dev, et rien ne bouge en production. Deux causes possibles ?
> `state.json` pas passé à `getProject` (la production joue les valeurs par défaut), ou studio importé sans être initialisé (`project.ready` ne se résout jamais). ([`PRODUCTION`](PRODUCTION.md) §1, [`LIMITES_DEBUG`](LIMITES_DEBUG.md) §2)

> [!question]- Theatre ou `damp` : une carte qui se soulève au survol ?
> `damp` : l'animation suit l'état et peut être interrompue à tout moment. ([`CONCEPTS`](CONCEPTS.md) §5)
