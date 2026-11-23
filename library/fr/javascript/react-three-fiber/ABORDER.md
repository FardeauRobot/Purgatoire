# 🪜 Aborder React Three Fiber

> **TL;DR.** R3F n'est pas un moteur 3D : c'est **React qui pilote three.js**. Il faut donc deux bagages : **React** (composants, état, refs, effets, closures) et **les bases de la 3D** (scène, caméra, mesh = forme + matériau, lumières, repère x/y/z, radians). Le piège principal n'est ni l'un ni l'autre, c'est leur **frontière** : React décide **quoi** afficher et **où aller** (rare), `useFrame` fait bouger **chaque image** en mutant l'objet three.js par une ref (60 fois par seconde). Suis le labo `~/Labo/TestThreeFiber` leçon par leçon, en changeant **une** valeur à la fois.

Liens : [`INDEX`](INDEX.md) · [`SYNTHESE`](SYNTHESE.md) · [`../react/ABORDER`](../react/ABORDER.md) · [`../react/PHILOSOPHIE`](../react/PHILOSOPHIE.md) · [`../theatre-js/ABORDER`](../theatre-js/ABORDER.md) · [doc R3F](https://r3f.docs.pmnd.rs) · [doc three.js](https://threejs.org/docs/)

---

## 1. À quel besoin il répond

three.js dessine de la 3D dans le navigateur avec WebGL, mais en **impératif** : tu crées un renderer, une scène, une caméra, des objets, tu les ajoutes, tu écris la boucle, et tu libères la mémoire GPU à la main. Dans une application React, ça fait deux mondes qui se parlent mal : l'état du jeu est dans React, la scène est dans three.js, et il faut recopier l'un dans l'autre.

R3F supprime la frontière : la scène s'écrit en **JSX**, chaque balise crée l'objet three.js correspondant, chaque prop appelle son setter, et le démontage libère les ressources. Ta main de cartes 3D devient un composant qui lit **le même store** que le HUD en HTML ([`ETAT_RESEAU`](ETAT_RESEAU.md)).

🔗 **Lien C++.** La libération au démontage, c'est le **RAII** : la ressource GPU vit aussi longtemps que le composant qui la possède ([`INDEX`](INDEX.md), tableau three.js ↔ R3F).

---

## 2. Ce que tu dois savoir avant

### Indispensable : React

| Prérequis | Où R3F s'en sert | Où le revoir |
|---|---|---|
| Composants, props, JSX, listes et `key` | toute la scène est en JSX | [`../react/COMPOSANTS_JSX`](../react/COMPOSANTS_JSX.md) |
| `useState` et le cycle de rendu | survol, sélection, carte retournée | [`../react/ETAT`](../react/ETAT.md) |
| **`useRef`** | **la** façon d'atteindre l'objet three.js pour l'animer | [`../react/REFS_FORMULAIRES`](../react/REFS_FORMULAIRES.md) §1 |
| `useEffect` et son nettoyage | s'abonner à une socket, à un store, à Theatre | [`../react/EFFETS`](../react/EFFETS.md) |
| Closures | `useFrame` lit l'état du rendu courant | [`../FONCTIONS`](../FONCTIONS.md) §4 |
| `<Suspense>` (au moins l'idée) | tout ce qui télécharge un fichier (modèle, police, HDR) | [`DREI`](DREI.md) §2 |
| Un store (Zustand) | l'état de partie partagé entre HUD et scène | [`ETAT_RESEAU`](ETAT_RESEAU.md) §2 |

Si tu n'as pas encore fait [`../react/`](../react/INDEX.md) jusqu'à [`EFFETS`](../react/EFFETS.md), fais-le d'abord : sinon, tu ne sauras pas si un bug vient de React ou de la 3D.

### Indispensable : les bases de la 3D

| Notion | En une ligne | Fiche |
|---|---|---|
| Scène, caméra, renderer | ce qu'il y a, d'où on regarde, qui dessine | [`ANATOMIE`](ANATOMIE.md) §1 |
| Repère x / y / z | x à droite, **y en haut**, z vers toi | [`ANATOMIE`](ANATOMIE.md) §2 |
| Mesh = géométrie + matériau | la forme et l'apparence | [`ANATOMIE`](ANATOMIE.md) §3 |
| Position, rotation, échelle | rotations en **radians** (`Math.PI` = 180°) | [`ANATOMIE`](ANATOMIE.md) §5 |
| Lumières | sans lumière, un matériau éclairé est noir | [`LUMIERES_MATERIAUX`](LUMIERES_MATERIAUX.md) §1 |
| Repère local et repère monde | un enfant bouge **avec** son parent | [`HIERARCHIE`](HIERARCHIE.md) §2, §5 |

### Utile, ça s'apprend en route

- **Vecteurs** (addition, longueur, normaliser) et un peu de **trigonométrie** (cos/sin pour un cercle). La hiérarchie de groupes t'évite souvent les cosinus ([`HIERARCHIE`](HIERARCHIE.md) §3).
- **Interpolation** : aller d'une valeur à une autre en douceur (`lerp`, `damp`) ([`ANIMATION`](ANIMATION.md) §6).
- **La boucle de jeu** : ton `so_long` en avait une (`mlx_loop_hook`). `useFrame` est la même idée.
- **Blender**, pour exporter un `.glb` : seulement à partir de [`MODELES_GLTF`](MODELES_GLTF.md).
- **WebSocket** côté client : à partir de [`ETAT_RESEAU`](ETAT_RESEAU.md) ([`../react/TEMPS_REEL`](../react/TEMPS_REEL.md)).

### Pas nécessaire

Les shaders GLSL, les matrices écrites à la main, WebGL brut. three.js et drei les cachent ; tu n'en auras besoin que pour des effets très spéciaux.

---

## 3. Le modèle mental à installer : deux horloges

```text
  HORLOGE REACT (rare : un clic, un message du serveur)        HORLOGE GPU (60+ fois par seconde)
  ───────────────────────────────────────────────────          ─────────────────────────────────────
  useState / store  → « la carte 3 est survolée »       ──►     useFrame : ref.current.position.y =
  re-rendu du composant, nouvelle CIBLE                         damp(actuel, cible, 8, delta)
```

- **L'état React donne la cible**, `useFrame` s'en rapproche à chaque image ([`ANIMATION`](ANIMATION.md) §1, §6).
- **Jamais de `setState` dans `useFrame`** : ce serait 60 rendus React par seconde ([`ANIMATION`](ANIMATION.md) §4).
- **Toujours multiplier par `delta`** : sinon la vitesse dépend de l'écran ([`ANIMATION`](ANIMATION.md) §3).
- **Le serveur décide de la partie, la scène l'affiche** ([`ETAT_RESEAU`](ETAT_RESEAU.md) §5).

---

## 4. Comment s'y prendre

1. **Monte le projet** avec [`INSTALLATION`](INSTALLATION.md), en respectant la contrainte de version React ↔ R3F. Vérifie d'abord que le canvas a une hauteur.
2. **Une leçon du labo = une fiche** ([`INDEX`](INDEX.md), colonne « Labo »). Lance la leçon, lis la fiche, puis fais le « 🧪 À essayer » en changeant **une seule** valeur à la fois. En 3D, deux changements simultanés rendent l'effet illisible.
3. **Allume les *helpers*** (axes, grille, lumières) tant que tu ne sais pas où sont les choses ([`ANATOMIE`](ANATOMIE.md) §7).
4. **Règle à l'œil avec Leva**, puis recopie les valeurs en dur ([`LEVA`](LEVA.md) §7).
5. **Cherche dans drei avant d'écrire** quelque chose de compliqué ([`DREI`](DREI.md) §3).
6. **Passe aux modèles et aux squelettes** ([`MODELES_GLTF`](MODELES_GLTF.md), [`SQUELETTE_ANIMATIONS`](SQUELETTE_ANIMATIONS.md)) seulement quand les primitives n'ont plus de secret.
7. **Branche le réseau, puis assemble** ([`ETAT_RESEAU`](ETAT_RESEAU.md), [`ATELIER_CARTES`](ATELIER_CARTES.md)). **Mesure** avant d'optimiser ([`PERFORMANCE_DEBUG`](PERFORMANCE_DEBUG.md) §2).

---

## 5. Les erreurs de débutant à éviter

- **Écrire du three.js impératif dans un composant** (`new THREE.Mesh`, `scene.add`) : la balise JSX le fait, et libère la mémoire.
- **Animer avec l'état React.** L'état choisit la cible ; la ref bouge l'objet.
- **Penser en degrés.** `rotation-y={90}` fait plus de 14 tours.
- **Calculer des cosinus pour une orbite** : un `<group>` qui tourne fait le travail ([`HIERARCHIE`](HIERARCHIE.md) §3).
- **Laisser la scène décider** d'une règle du jeu : c'est le serveur ([`ETAT_RESEAU`](ETAT_RESEAU.md) §5).
- **Optimiser à l'aveugle** : `<Stats />` et `gl.info.render` d'abord ([`PERFORMANCE_DEBUG`](PERFORMANCE_DEBUG.md) §2).

---

## 6. Tu es prêt pour l'atelier et Theatre.js si…

> [!question]- Que crée `<Canvas>` à ta place ?
> Le renderer, la scène, une caméra perspective et la boucle de rendu. ([`ANATOMIE`](ANATOMIE.md) §1)

> [!question]- Que veut dire `<boxGeometry args={[1, 2, 3]} />` ?
> `new THREE.BoxGeometry(1, 2, 3)` : `args` est la liste des arguments du **constructeur**, dans l'ordre. ([`ANATOMIE`](ANATOMIE.md) §4)

> [!question]- Ta scène est entièrement noire. Première hypothèse ?
> Un matériau éclairé (`meshStandardMaterial`) sans aucune lumière. ([`LUMIERES_MATERIAUX`](LUMIERES_MATERIAUX.md) §1)

> [!question]- Pourquoi `ref.current.rotation.y += 0.01` dans `useFrame` est-il faux sur un écran à 144 Hz ?
> L'objet tourne 2,4 fois plus vite qu'à 60 Hz. Il faut `+= vitesse * delta`. ([`ANIMATION`](ANIMATION.md) §3)

> [!question]- Un clic sur une carte déclenche aussi le clic de la table derrière. Pourquoi, et comment l'éviter ?
> Le rayon touche **tous** les objets sous la souris, du plus proche au plus lointain. `e.stopPropagation()` l'arrête au premier. ([`INTERACTION`](INTERACTION.md) §5)

> [!question]- Dans `useFrame`, comment lire le store Zustand sans provoquer de re-rendu ?
> `useJeu.getState()`, qui lit sans s'abonner. ([`ETAT_RESEAU`](ETAT_RESEAU.md) §4)

> [!question]- Tu affiches deux fois le même `.glb` chargé par `useGLTF` ; un seul apparaît. Pourquoi ?
> `useGLTF` met en cache et renvoie **le même** objet, qui ne peut être qu'à un endroit de la scène. Il faut cloner (`<Clone>`). ([`MODELES_GLTF`](MODELES_GLTF.md) §4)
