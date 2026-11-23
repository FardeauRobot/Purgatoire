# 📒 Synthèse : Theatre.js

> **TL;DR.** La fiche de révision de [`theatre-js/`](INDEX.md). La hiérarchie, le circuit studio → `state.json` → core, l'API à connaître (objets, `onValuesChange`, séquences), le passage en production et le relais avec `damp`, chacun avec sa fiche et son paragraphe. Les blocs de code sont recopiés de [`PRODUCTION`](PRODUCTION.md) §1, [`CORE_A_LA_MAIN`](CORE_A_LA_MAIN.md) §4 (raccourci par `// …`) et [`SEQUENCES`](SEQUENCES.md) §1.

Liens : [`INDEX`](INDEX.md) · [`ABORDER`](ABORDER.md) · [`../react-three-fiber/SYNTHESE`](../react-three-fiber/SYNTHESE.md) · [`../SYNTHESE`](../SYNTHESE.md)

Versions : `@theatre/core`, `@theatre/studio`, `@theatre/r3f` 0.7.2 (mai 2024, plus maintenus), avec R3F 9.8, React 19.3, three 0.187 (voir [`INDEX`](INDEX.md), « Ce qui a été vérifié »).

---

## 1. L'idée en cinq lignes

- **Le besoin** : des animations écrites d'avance (intro, victoire, coup spécial), réglées à l'œil, sans `if (t < …)` dans `useFrame`.
- **La réponse** : une timeline. Le **code** contient les noms, le **JSON** contient l'animation.
- **Le studio écrit** (dev), **le core lit** (production) à partir de `state.json`.
- **Les valeurs arrivent par un callback** qu'on branche sur une ref, jamais sur un état React.
- **Pour ce qui suit l'état, c'est `damp`** ; Theatre joue la mise en scène puis rend la main.

---

## 2. La hiérarchie ([`CONCEPTS`](CONCEPTS.md) §2)

| Niveau | Créé par | Identifié par |
|---|---|---|
| **Projet** (unité d'export) | `getProject(id, config)` | son `id` |
| **Feuille** (une scène animée, **une** séquence) | `project.sheet(nom, instance?)` | son nom (+ instance) |
| **Objet** | `sheet.object(nom, props)` | son nom |
| **Props** | la valeur par défaut, ou `types.number`, `types.rgba`, `types.stringLiteral`… | leur chemin |

Renommer = perdre ses keyframes ([`INDEX`](INDEX.md), piège 4).

---

## 3. Le circuit, et le brancher en production ([`PRODUCTION`](PRODUCTION.md))

```text
studio ── Export Aura to JSON ──► ~/Téléchargements/Aura.theatre-project-state.json
                                         │  mv … src/state.json
                                         ▼
                                  git add src/state.json
                                         │
                                         ▼
               npm run build ──► getProject("Aura", { state }) ──► les joueurs voient l'animation
```
```ts
import { getProject } from "@theatre/core";
import state from "./state.json";

export const project = getProject("Aura", { state });
```
| À savoir | Fiche § |
|---|---|
| Studio : `import()` derrière `import.meta.env.DEV`, dans un fichier à part ; absent du build | [`INSTALLATION`](INSTALLATION.md) §3 |
| Conflit `ERESOLVE` (R3F 8 attendu) : bloc `overrides`, pas `--legacy-peer-deps` | [`INSTALLATION`](INSTALLATION.md) §2 |
| Sans `state`, la production joue les **valeurs par défaut** | [`PRODUCTION`](PRODUCTION.md) §1 |
| En dev, deux sources : `localStorage` (brouillon) et `state.json` (disque) ; conflit → « Browser state is not based on disk state » | §3 |
| Jamais `state: {}` : sans studio, erreur au démarrage | §5 |
| Toujours vérifier un **build** | §6 |

---

## 4. Brancher un objet à la main ([`CORE_A_LA_MAIN`](CORE_A_LA_MAIN.md) §4)

```tsx
const sheet = project.sheet("Démo");
const cubeObj = sheet.object("Cube", {
    position: { x: 0, y: 0, z: 0 },
    rotationY: types.number(0, { nudgeMultiplier: 0.01 }),
    // …
});

function Cube() {
    const ref = useRef<Mesh<BufferGeometry, MeshStandardMaterial>>(null);

    useEffect(() => {
        return cubeObj.onValuesChange((v) => {
            const mesh = ref.current;
            if (!mesh) return;
            mesh.position.set(v.position.x, v.position.y, v.position.z);
            mesh.rotation.y = v.rotationY;
            // …
        });
    }, []);
    // …
}
```
- `sheet.object` au niveau du **module** : une seule fois.
- `onValuesChange` : appelé **tout de suite**, puis à chaque changement, avec toutes les valeurs déjà interpolées ; renvoie le désabonnement, rendu par `useEffect`.
- On **mute la ref** : pendant la lecture, le callback tourne à chaque image.
- Autre voie : lire `obj.value` dans `useFrame` (§5). Écrire depuis le code : §6.

### La même chose avec `@theatre/r3f` ([`EDITABLE`](EDITABLE.md))
| À savoir | § |
|---|---|
| `<SheetProvider sheet={sheet}>` autour de la scène, puis `<e.mesh theatreKey="Carte">` : position, rotation, échelle dans le studio ; les props JSX deviennent les valeurs **initiales** | TL;DR, 1–2 |
| Caméra : `PerspectiveCamera` **de `@theatre/r3f`** + `makeDefault`, pas `e.perspectiveCamera` | 2 |
| Animer un matériau : `additionalProps` (défini **hors** du rendu) + `objRef` | 3 |
| `visible="editor"` : un repère visible seulement dans le studio | 4 |

---

## 5. Le studio ([`STUDIO`](STUDIO.md))

Outline → objet → clic droit sur une prop → **Sequence** (§2) · placer la tête, changer la valeur = une keyframe (§3) · clic sur un segment = la courbe : `linear`, `In Out`, `Back Out`…, `Hold` pour un saut (§4) · **Espace** lit / arrête · **Export … to JSON** depuis le projet sélectionné, sinon tout reste dans le `localStorage` (§5).

---

## 6. Piloter les séquences ([`SEQUENCES`](SEQUENCES.md))

```ts
import { project } from "./theatre";

const intro = project.sheet("Intro");

export async function lancerIntro(): Promise<void> {
    await project.ready;
    const finie = await intro.sequence.play();
    if (finie) console.log("intro terminée, la partie commence");
}
```
| À savoir | § |
|---|---|
| `await project.ready` **avant** le premier `play()`, sinon rien ne joue et la promesse dit `true` | 1 |
| `play()` → `Promise<boolean>` : `true` au bout, `false` si interrompue (`pause()`, autre `play()`) | 1 |
| Options : `iterationCount` (`Infinity`), `range: [début, fin]`, `rate`, `direction` | 2 |
| `sequence.position` se **lit et s'écrit** (secondes) : piloter par le scroll, une barre, l'état du jeu | 3–4 |
| Un *pointer* n'est pas une valeur : `val()` | [`INDEX`](INDEX.md), piège 10 |

---

## 7. L'atelier : une carte jouée ([`INSTANCES_CARTES`](INSTANCES_CARTES.md))

- **Une feuille = une tête de lecture.** `project.sheet("Carte jouée", id)` crée une **instance** : mêmes keyframes, lecture indépendante (§1).
- **Partage des rôles** : `damp` calcule la position **de base**, Theatre ajoute un **décalage** (saut, tour, éclat) (§2).
- **La séquence commence et finit à zéro** sur toutes ses pistes : le relais avec `damp` est invisible (§2).

```text
                position affichée = base (damp)   +   décalage (Theatre)
```

---

## 8. Quand l'utiliser, et l'état du projet ([`LIMITES_DEBUG`](LIMITES_DEBUG.md), [`CONCEPTS`](CONCEPTS.md) §5)

| Besoin | Outil |
|---|---|
| Plusieurs props ensemble, timing réglé à l'œil (intro, victoire, coup spécial, caméra) | **Theatre** |
| Suit l'état, interruptible (survol, main) | **`damp`** |
| Animation d'un modèle (marche, attaque) | **clips glTF** |
| Une valeur à régler | **Leva** |

- **Plus maintenu** depuis mai 2024 : fonctionne avec R3F 9 et React 19 (vérifié), mais chaque mise à jour est à retester. Garde-le **isolé** dans quelques fichiers (§1).
- Quatre causes pour la plupart des pannes : `ready` pas attendu, studio non initialisé, `state.json` absent ou périmé, nom changé (TL;DR, §2).
- Remplaçants possibles, non testés : keyframes à la main, Blender + glTF, GSAP (§4).

---

## 🧠 Questions de révision

> [!question]- Dans quel fichier vit l'animation, et dans quel fichier vivent les noms ?
> L'animation (keyframes, courbes) dans `state.json` ; les noms (projet, feuilles, objets, props) dans le code. ([`INDEX`](INDEX.md), tableau R3F ↔ Theatre)

> [!question]- `onValuesChange` est-il appelé seulement quand une valeur change ?
> Non : une première fois **tout de suite**, puis à chaque changement. Le mesh prend ses valeurs dès le montage. ([`CORE_A_LA_MAIN`](CORE_A_LA_MAIN.md) §4)

> [!question]- Pourquoi `e.perspectiveCamera` ne filme-t-il pas ?
> Ce n'est pas la caméra active. Il faut la `PerspectiveCamera` de `@theatre/r3f` avec `makeDefault`. ([`EDITABLE`](EDITABLE.md) §2)

> [!question]- Que renvoie `await intro.sequence.play()` si le joueur passe l'intro avec `pause()` ?
> `false` : la lecture a été interrompue. ([`SEQUENCES`](SEQUENCES.md) §1)

> [!question]- Comment faire avancer une animation avec le scroll au lieu du temps ?
> Écrire `sheet.sequence.position` (en secondes) à partir de la position de scroll. ([`SEQUENCES`](SEQUENCES.md) §3–4)

> [!question]- Deux cartes jouées en même temps : pourquoi la première s'arrête-t-elle net, et quel remède ?
> Le second `play()` sur la même feuille coupe le premier. Une **instance** de feuille par carte : `project.sheet("Carte jouée", id)`. ([`INSTANCES_CARTES`](INSTANCES_CARTES.md) §1)

> [!question]- Pourquoi la séquence de la carte jouée doit-elle finir à zéro ?
> Theatre ajoute un décalage à la position de `damp` ; s'il vaut 0 au début et à la fin, le relais ne fait aucun saut. ([`INSTANCES_CARTES`](INSTANCES_CARTES.md) §2)

> [!question]- Ton brouillon du studio n'apparaît pas chez ton coéquipier. Pourquoi ?
> Il vit dans le `localStorage` de **ton** navigateur. Il faut exporter `state.json` et le commiter. ([`STUDIO`](STUDIO.md) §5, [`PRODUCTION`](PRODUCTION.md) §1)
