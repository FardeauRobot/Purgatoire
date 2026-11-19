# 1. Les concepts : projet, feuille, objet, séquence

> **TL;DR.** Theatre.js est une **timeline**, comme celle d'After Effects ou de Blender, pilotable depuis le code. Un **projet** (`getProject`) contient des **feuilles** (`project.sheet`), une par scène animée. Une feuille contient des **objets** (`sheet.object`) : chacun déclare des **props** typées (nombres, couleurs, booléens…). Chaque feuille a **une séquence** : la timeline où tu poses des **keyframes** sur ces props. Le studio sert à **écrire** ces keyframes à la souris, puis à les exporter en **`state.json`**. Le core les **relit** et calcule les valeurs à chaque instant.

Liens : [`INDEX`](INDEX.md) · [`INSTALLATION`](INSTALLATION.md) · [`CORE_A_LA_MAIN`](CORE_A_LA_MAIN.md) · [`EDITABLE`](EDITABLE.md) · [`../react-three-fiber/ANIMATION`](../react-three-fiber/ANIMATION.md)

---

## 1. Le problème que Theatre résout

Dans [`../react-three-fiber/ANIMATION`](../react-three-fiber/ANIMATION.md), toutes les animations sont **calculées** : `rotation += vitesse * delta`, `damp(actuel, cible, 8, delta)`. C'est parfait pour ce qui **réagit** (survol, suivi de souris, carte qui suit une cible).

C'est pénible pour ce qui est **mis en scène** : la caméra qui plonge sur le plateau au début de la partie, la carte légendaire qui tourne deux fois sur elle-même, s'illumine puis retombe avec un petit rebond. Écrire ça en code, c'est enchaîner des `if (t < 0.4) … else if (t < 1.1) …` et relancer le navigateur cinquante fois pour régler un timing.

Theatre inverse le travail :

```text
 Sans Theatre                                Avec Theatre
 ────────────                                ────────────
 tu écris des formules                       tu poses des keyframes à la souris
 tu recharges, tu regardes                   tu vois le résultat en direct
 tu changes 0.4 en 0.35, tu recharges…       tu tires sur un point de la courbe
                                             tu exportes state.json
 le code contient l'animation                le code contient les NOMS, le JSON contient l'animation
```

🔗 **Lien C.** C'est la différence entre écrire une map de `so_long` en dur dans le code et la lire depuis un fichier `.ber`. Le code connaît la **structure** (un mur, un joueur, des collectibles), le fichier contient les **données**. Ici, le code déclare « il y a un objet `Caméra` avec une `position` », et `state.json` dit où elle est à chaque seconde.

---

## 2. La hiérarchie

```text
Project "Aura"                         getProject("Aura", { state })
├── Sheet "Intro"                      project.sheet("Intro")
│   ├── Object "Caméra"                sheet.object("Caméra", { position: … })
│   │     props: position {x,y,z}, fov
│   ├── Object "Logo"
│   │     props: opacité, échelle
│   └── Sequence                       sheet.sequence
│         keyframes de Caméra.position.y à 0 s, 1.2 s, 3 s…
└── Sheet "Carte jouée"
    ├── Object "Carte"
    └── Sequence
```

| Concept | Ce que c'est | Créé par | Identifié par |
|---|---|---|---|
| **Project** | L'ensemble de tes animations, et l'unité d'export (un `state.json` par projet). | `getProject(id, config)` | son `id` (chaîne) |
| **Sheet** (feuille) | Une scène animée, avec sa propre timeline. | `project.sheet(id, instanceId?)` | son `id`, plus une instance optionnelle |
| **Object** | Un ensemble de props qu'on peut animer. Pas forcément un objet 3D : ça peut être « le volume de la musique ». | `sheet.object(clé, props)` | sa **clé** |
| **Prop** | Une valeur typée de l'objet : nombre, couleur, booléen, texte, choix dans une liste, ou un groupe de props (`compound`). | dans `props` | son **chemin** : `["position", "x"]` |
| **Sequence** | La timeline de la feuille : longueur, keyframes, tête de lecture. | automatique, `sheet.sequence` | (une par feuille) |

> 🚨 **Les noms sont l'identité.** Dans `state.json`, les keyframes sont rangées sous l'id de la feuille, puis la clé de l'objet, puis le chemin de la prop. Renomme `"Caméra"` en `"Camera"` dans le code, et l'animation existe toujours dans le JSON, mais plus personne ne la lit : ton objet repart de ses valeurs par défaut. Choisis des noms stables dès le début.

---

## 3. Core et studio : qui fait quoi

```text
            DEV (npm run dev)                               PROD (npm run build)
 ┌─────────────────────────────────────┐        ┌─────────────────────────────────┐
 │ studio : tu édites à la souris      │        │                                 │
 │   ↓ sauvegarde auto                 │        │                                 │
 │ localStorage du navigateur          │        │                                 │
 │   ↓ bouton « Export … to JSON »     │        │                                 │
 │ Aura.theatre-project-state.json ────┼───────►│ src/state.json (commité)        │
 │                                     │        │   ↓                             │
 │ core : calcule les valeurs          │        │ core : getProject("Aura",       │
 │                                     │        │          { state }) → valeurs   │
 └─────────────────────────────────────┘        └─────────────────────────────────┘
```

- Tant que tu n'as pas **exporté**, ton travail ne vit que dans le `localStorage` **de ce navigateur**. Un autre navigateur, un collègue, ou la production : rien. Voir [`PRODUCTION`](PRODUCTION.md).
- Le core sait jouer un `state.json` sans studio. C'est tout ce qui part en production.

---

## 4. Ce que « calculer les valeurs » veut dire

Une prop séquencée, c'est une liste de keyframes `(temps, valeur)` et, entre deux keyframes, une **courbe** d'interpolation :

```text
 valeur
   3 ┤                         ●  keyframe (2 s, 3)
     │                    ╭────╯
     │               ╭────╯
 1.5 ┤          ╭────╯             ← à 1 s : 1.5 (courbe symétrique)
     │     ╭────╯
   0 ┤●────╯                         ← keyframe (0 s, 0)
     └──────────────┬──────────────┬── temps
                   1 s            2 s
```

Quand la tête de lecture est à `t`, le core cherche les deux keyframes qui encadrent `t` et évalue la courbe entre elles. La courbe par défaut **accélère puis ralentit** (*ease in-out*) : elle n'est pas linéaire. Dans les tests de ces fiches, une prop qui va de 0 à 3 entre 0 s et 2 s vaut **1.5 à 1 s** (le milieu, par symétrie), mais **0.318 à 0.5 s**, et non 0.75. Si tu veux du linéaire, change la courbe dans le studio (voir [`STUDIO`](STUDIO.md)).

🔗 **Lien C.** C'est un tableau trié de points plus une recherche de l'intervalle, puis une interpolation. Tu as écrit plus dur dans `push_swap`. La valeur ajoutée de Theatre, c'est l'éditeur, pas l'algorithme.

---

## 5. Où Theatre s'arrête

| Besoin | Outil |
|---|---|
| Animation **écrite d'avance**, réglée à l'œil : cinématique, intro, effet spectaculaire | **Theatre** |
| Animation **qui suit l'état** et peut être interrompue : survol, main qui se réorganise, carte qui suit le curseur | **`damp` dans `useFrame`** ([`../react-three-fiber/ATELIER_CARTES`](../react-three-fiber/ATELIER_CARTES.md)) |
| Animation **d'un modèle** faite dans Blender (marche, attaque) | **Clips glTF** ([`../react-three-fiber/SQUELETTE_ANIMATIONS`](../react-three-fiber/SQUELETTE_ANIMATIONS.md)) |
| Régler une **valeur fixe** en direct (une couleur, une intensité) | **Leva** ([`../react-three-fiber/LEVA`](../react-three-fiber/LEVA.md)), ou Theatre sans keyframes |

Les deux premiers se combinent très bien : Theatre joue la mise en scène, puis rend la main à `damp`. C'est le sujet de [`INSTANCES_CARTES`](INSTANCES_CARTES.md).

---

## 🧪 À essayer

1. Prends une animation que tu voudrais dans Transcendance (début de partie, victoire, carte jouée). Dessine sa hiérarchie sur papier, comme à la section 2 : quelles feuilles, quels objets, quelles props ?
2. Pour chaque prop, demande-toi : est-ce **écrit d'avance** (Theatre) ou **piloté par l'état** (`damp`) ? Si la réponse est « les deux », note où se fait le passage de relais.
3. Après la fiche [`STUDIO`](STUDIO.md), exporte un `state.json` et ouvre-le. Retrouve tes noms de feuille, d'objet et de prop dans la structure.
