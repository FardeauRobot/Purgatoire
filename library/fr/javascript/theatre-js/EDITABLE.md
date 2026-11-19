# 3. `@theatre/r3f` : `<SheetProvider>` et `e.mesh`

> **TL;DR.** `import { editable as e, SheetProvider } from "@theatre/r3f"`. Entoure ta scène de `<SheetProvider sheet={sheet}>`, puis remplace `<mesh>` par `<e.mesh theatreKey="Carte">`. L'objet apparaît dans le studio avec `position`, `rotation` et `scale`, et ses props JSX deviennent ses **valeurs initiales**. Les lumières et les caméras exposent en plus leurs réglages (`intensity`, `color`, `fov`…). Pour une caméra, utilise le `PerspectiveCamera` **de `@theatre/r3f`** avec `makeDefault`, pas `e.perspectiveCamera`. Pour animer autre chose (un matériau), ajoute des props avec `additionalProps` et récupère l'objet Theatre par `objRef`.

Liens : [`INDEX`](INDEX.md) · [`CORE_A_LA_MAIN`](CORE_A_LA_MAIN.md) · [`STUDIO`](STUDIO.md) · [`../react-three-fiber/HIERARCHIE`](../react-three-fiber/HIERARCHIE.md) · [`../react-three-fiber/DREI`](../react-three-fiber/DREI.md)

🧪 Labo : aucun pour l'instant.

---

## 1. Ce que fait `e.mesh` à ta place

`<e.mesh theatreKey="Carte">` fait exactement ce que tu as écrit à la main en fiche [`CORE_A_LA_MAIN`](CORE_A_LA_MAIN.md), §4 :

```text
<e.mesh theatreKey="Carte" position={[0, 0.5, 0]}>
   │
   ├─ sheet = la feuille du <SheetProvider> le plus proche
   ├─ obj = sheet.object("Carte", { position, rotation, scale })
   ├─ obj.initialValue = { position: { x: 0, y: 0.5, z: 0 }, … }   ← tes props JSX
   ├─ obj.onValuesChange(v → mesh.position.set(…), mesh.rotation…, invalidate())
   └─ au démontage : désabonnement + sheet.detachObject("Carte")
```

Il appelle aussi `invalidate()` à chaque changement, donc il marche avec `frameloop="demand"` (voir [`../react-three-fiber/PERFORMANCE_DEBUG`](../react-three-fiber/PERFORMANCE_DEBUG.md)).

🔗 **Lien C++.** C'est un **décorateur** : `e.mesh` enveloppe `<mesh>` sans le modifier, et ajoute le branchement à Theatre autour. Le `<mesh>` d'origine ne sait pas qu'il est animé.

---

## 2. Une scène complète

```tsx
import { useRef } from "react";
import { Canvas } from "@react-three/fiber";
import { editable as e, PerspectiveCamera, SheetProvider } from "@theatre/r3f";
import type { Mesh } from "three";
import { project } from "./theatre";

const sheet = project.sheet("Intro");

export default function Intro() {
    const plateau = useRef<Mesh>(null);

    return (
        <Canvas>
            <SheetProvider sheet={sheet}>
                <PerspectiveCamera
                    theatreKey="Caméra"
                    makeDefault
                    position={[0, 3, 8]}
                    fov={50}
                    lookAt={plateau}
                />
                <ambientLight intensity={0.3} />
                <e.directionalLight theatreKey="Soleil" position={[3, 5, 2]} intensity={2} />

                <e.mesh theatreKey="Plateau" ref={plateau} rotation={[-Math.PI / 2, 0, 0]}>
                    <planeGeometry args={[6, 4]} />
                    <meshStandardMaterial color="#2d5a3d" />
                </e.mesh>

                <e.group theatreKey="Carte" position={[0, 0.5, 0]}>
                    <mesh>
                        <boxGeometry args={[1.4, 2, 0.02]} />
                        <meshStandardMaterial color="ivory" />
                    </mesh>
                </e.group>
            </SheetProvider>
        </Canvas>
    );
}
```

Ligne par ligne :
- `<SheetProvider sheet={sheet}>` : un **contexte React** (comme le `<Canvas>` pour `useFrame`). Tous les `e.*` en dessous s'enregistrent dans cette feuille. Il doit être **dans** le `<Canvas>`, car `e.*` utilise `useThree`.
- `theatreKey="Plateau"` : la **clé** de l'objet dans la feuille. Obligatoire, et unique dans la feuille. C'est elle qu'on retrouve dans `state.json` : ne la renomme pas après avoir animé (voir [`CONCEPTS`](CONCEPTS.md), §2).
- `rotation={[-Math.PI / 2, 0, 0]}` : la valeur **initiale**. Le studio part de là ; une fois que tu l'as modifiée dans le studio, c'est la valeur du studio qui gagne.
- `ref={plateau}` : la ref arrive bien sur le `THREE.Mesh`. `e.mesh` la transmet.
- `<e.group>` autour d'un `<mesh>` simple : pour animer un **ensemble** (une carte = face + dos + bordure), anime le groupe, pas chaque morceau (voir [`../react-three-fiber/HIERARCHIE`](../react-three-fiber/HIERARCHIE.md)).
- `PerspectiveCamera` vient de **`@theatre/r3f`**. C'est une copie de la caméra de drei, réécrite dans `@theatre/r3f` et rendue éditable : `makeDefault` en fait la caméra de rendu, et `lookAt` (une ref ou `[x, y, z]`) la garde braquée sur une cible pendant que tu animes sa position.

### Ce que chaque balise expose

Lu dans le code de `@theatre/r3f` 0.7.2 :

| Balise | Props dans le studio |
|---|---|
| `e.mesh`, `e.group`, `e.points`, `e.line`, `e.lineLoop`, `e.lineSegments` | `position`, `rotation`, `scale` |
| `e.directionalLight` | + `intensity`, `color` |
| `e.pointLight`, `e.spotLight` | + `intensity`, `distance`, `decay`, `color` (spot : + `angle`, `penumbra`) |
| `e.ambientLight` | `intensity`, `color` |
| `e.hemisphereLight` | `intensity`, `color`, `groundColor` |
| `PerspectiveCamera` (de `@theatre/r3f`) | + `near`, `far`, `fov`, `zoom` |
| `e.fog` | `color`, `near`, `far` |

> 🚨 **Pas de matériau.** `e.mesh` n'expose ni la couleur, ni l'opacité, ni l'émission. Pour ça : §3.

> 🚨 **`e.perspectiveCamera` ne fait pas ce que tu crois.** La balise existe, mais seule, R3F ne s'en sert pas pour dessiner. Le studio affiche alors un avertissement (`Possibly incorrect use of <e.perspectiveCamera />`). Prends `PerspectiveCamera` de `@theatre/r3f`, avec `makeDefault`.

---

## 3. Animer un matériau : `additionalProps` + `objRef`

```tsx
import { useEffect, useRef } from "react";
import { types, type ISheetObject } from "@theatre/core";
import { editable as e } from "@theatre/r3f";
import type { MeshStandardMaterial } from "three";

const propsEclat = {
    eclat: types.number(0, { range: [0, 4] }),
    opacite: types.number(1, { range: [0, 1] }),
};

export function CarteBrillante() {
    const materiau = useRef<MeshStandardMaterial>(null);
    const objet = useRef<ISheetObject<typeof propsEclat> | null>(null);

    useEffect(() => {
        if (!objet.current) return;
        return objet.current.onValuesChange((v) => {
            if (!materiau.current) return;
            materiau.current.emissiveIntensity = v.eclat;
            materiau.current.opacity = v.opacite;
        });
    }, []);

    return (
        <e.mesh theatreKey="Carte brillante" additionalProps={propsEclat} objRef={objet}>
            <boxGeometry args={[1.4, 2, 0.02]} />
            <meshStandardMaterial ref={materiau} color="ivory" emissive="gold" transparent />
        </e.mesh>
    );
}
```

- `additionalProps` : des props Theatre **en plus** de `position` / `rotation` / `scale`, écrites comme pour `sheet.object`. Elles apparaissent sous le même objet dans le studio.
- `objRef` : `e.mesh` y range l'objet Theatre qu'il a créé. On s'y abonne comme en fiche [`CORE_A_LA_MAIN`](CORE_A_LA_MAIN.md).
- Pourquoi l'objet est disponible dans le `useEffect` : `e.mesh` crée l'objet dans un `useLayoutEffect`, et React exécute les *layout effects* des enfants **avant** les `useEffect` du parent.
- 🚨 **`propsEclat` est déclaré hors du composant.** `e.mesh` reconfigure l'objet chaque fois que `additionalProps` change **d'identité**. Écrit en ligne (`additionalProps={{ eclat: … }}`), c'est un objet neuf à chaque rendu, donc une reconfiguration à chaque rendu. C'est le même piège que l'objet neuf dans un sélecteur zustand (voir [`../react-three-fiber/ETAT_RESEAU`](../react-three-fiber/ETAT_RESEAU.md)).

À utiliser dans un `<SheetProvider>`, comme toute balise `e.*`.

---

## 4. `visible="editor"` : des repères qui ne s'affichent que dans le studio

```text
<e.mesh theatreKey="Repère caméra" visible="editor"> … </e.mesh>
```

L'objet est **caché** dans ta scène, mais **visible** dans l'éditeur de snapshot. Pratique pour une cible de caméra, un point de passage ou la zone de dépôt d'une carte : tu les vois en animant, les joueurs ne les voient jamais.

---

## 5. Rendre éditable un composant qui n'est pas une balise

`editable(Composant, "type")` fabrique une balise éditable à partir d'un composant qui **rend** un objet three.js de ce type :

```tsx
import { PerspectiveCamera as DreiCamera } from "@react-three/drei";
import { editable } from "@theatre/r3f";

export const CameraEditable = editable(DreiCamera, "perspectiveCamera");
```

Le second argument dit à Theatre **quelles props** proposer (ici celles d'une caméra). Le `PerspectiveCamera` du §2 est construit de la même façon : `editable(sa caméra à lui, "perspectiveCamera")`.

---

## 6. Les modèles chargés et le snapshot

L'éditeur de snapshot (fiche [`STUDIO`](STUDIO.md)) travaille sur une **copie** de ta scène, prise quand tu cliques sur l'appareil photo. Un modèle glTF qui finit de charger **après** n'y apparaît pas. Place `<RefreshSnapshot />` dans le même `<Suspense>` que le modèle :

```text
<Suspense fallback={null}>
    <RefreshSnapshot />
    <Plateau />          ← useGLTF à l'intérieur
</Suspense>
```

Quand le `Suspense` se résout, `RefreshSnapshot` se monte et rafraîchit le snapshot. `refreshSnapshot()` fait la même chose depuis du code.

---

## 🧪 À essayer

1. Monte la scène du §2 avec le studio. Ouvre l'outline : `Aura › Intro` doit contenir `Caméra`, `Soleil`, `Plateau`, `Carte`.
2. Clique sur l'appareil photo, sélectionne `Carte` dans la vue 3D et déplace-la avec la flèche verte (axe Y). La carte bouge aussi dans ta vraie scène.
3. Change `position={[0, 0.5, 0]}` en `[1, 0.5, 0]` dans le code, **avant** puis **après** avoir déplacé la carte dans le studio. Dans quel cas le code gagne-t-il ? (Indice : §2, « valeur initiale ».)
4. Remplace `PerspectiveCamera` par `<e.perspectiveCamera theatreKey="Caméra" …>` et lis la notification du studio (la cloche, en haut à droite).
5. Écris `additionalProps={{ eclat: types.number(0) }}` en ligne, ajoute un `console.log` dans un composant parent qui se re-rend souvent (un compteur), et observe le comportement du panneau de propriétés.
