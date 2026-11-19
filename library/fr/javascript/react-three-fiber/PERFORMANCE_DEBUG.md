# 12. Performance et débogage

> **TL;DR.** Ce qui coûte en 3D, c'est surtout le nombre d'**appels de dessin** (*draw calls*, en gros un par mesh), le nombre de **pixels** à calculer (taille de l'écran × `dpr`) et les **ombres**. Mesure avant d'optimiser : `<Stats />` pour les images par seconde, `gl.info.render` pour les appels et les triangles. Outils de R3F et drei : **instancier** les objets répétés, plafonner le `dpr`, `frameloop="demand"` pour les écrans **statiques** uniquement (un menu, une collection de cartes), jamais pour l'arène animée. R3F libère tout seul la mémoire GPU des objets qu'il crée, au démontage. En fin de fiche : les erreurs les plus fréquentes et leur cause.

Liens : [`INDEX`](INDEX.md) · [`ANIMATION`](ANIMATION.md) · [`MODELES_GLTF`](MODELES_GLTF.md) · [`DREI`](DREI.md) · [`LEVA`](LEVA.md)

🧪 Labo : aucune leçon. Exemples vérifiés au typecheck.

---

## 1. D'où vient le coût

| Coût | Ce qui le fait monter | Remède |
|---|---|---|
| **CPU : appels de dessin** | Un appel par mesh visible (et par matériau d'un mesh). 500 petits cailloux = 500 appels. | Instancier (section 4), fusionner les meshes dans Blender. |
| **GPU : pixels** | Taille du canvas × `dpr`². Un écran Retina en `dpr` 2 a **4 fois** plus de pixels qu'en `dpr` 1. | Plafonner `dpr` (section 3). |
| **GPU : ombres** | Chaque lumière qui projette des ombres **redessine la scène** depuis son point de vue. | Une seule lumière à ombres, `shadow-mapSize` raisonnable, `ContactShadows` pour le sol. |
| **GPU : matériaux** | `MeshPhysicalMaterial`, transmission, transparence superposée. | `MeshStandardMaterial` par défaut, le reste avec parcimonie. |
| **CPU : JavaScript** | Allocations dans `useFrame`, `setState` à chaque image, trop de `Html`. | Réutiliser les objets (voir [`HIERARCHIE`](HIERARCHIE.md), section 5), règles de [`ANIMATION`](ANIMATION.md). |
| **Mémoire** | Textures 4K, modèles non compressés, objets jamais libérés. | Textures en 1K/2K, compression (voir [`MODELES_GLTF`](MODELES_GLTF.md)), `dispose`. |

🔗 Comme en C : on ne devine pas où est la lenteur, on **mesure**. `Stats` et `gl.info` sont ton `time` et ton `valgrind`.

---

## 2. Mesurer

```tsx
import { Canvas, useFrame } from "@react-three/fiber";
import { Stats } from "@react-three/drei";

function InfoRendu() {
    useFrame(({ gl }) => {
        const { calls, triangles } = gl.info.render;
        if (Math.random() < 0.01) console.log(`appels : ${calls} — triangles : ${triangles}`);
    });
    return null;
}

export default function Mesure() {
    return (
        <Canvas>
            <Stats />
            <InfoRendu />
            <ambientLight />
            <mesh>
                <boxGeometry />
                <meshStandardMaterial />
            </mesh>
        </Canvas>
    );
}
```

- **`<Stats />`** affiche un petit panneau : images par seconde (FPS), temps par image (MS), mémoire. Clique dessus pour changer de vue. `StatsGl` mesure aussi le temps passé côté GPU.
- **`gl.info.render`** : `calls` (appels de dessin) et `triangles` de la **dernière** image. Le `Math.random() < 0.01` affiche environ une ligne toutes les 100 images, pour ne pas noyer la console.
- **Repères** : 60 FPS = 16,7 ms par image. Viser quelques centaines d'appels au plus pour une scène de jeu, et rester sous le million de triangles visibles sur une machine moyenne.
- Teste aussi sur une machine **modeste** : un portable de l'école, pas seulement ton Mac.

---

## 3. Le `dpr` et la qualité adaptative

**`dpr`** (*device pixel ratio*) vaut `[1, 2]` par défaut : le ratio de l'écran, plafonné à 2. Descendre à 1 sur un écran Retina divise le nombre de pixels par 4 : c'est souvent le plus gros levier. drei propose deux façons **alternatives** de l'ajuster ; n'en utilise qu'une à la fois, car les deux écrivent le même `dpr`.

### Option A : selon les FPS mesurés

```tsx
import { useState } from "react";
import { Canvas } from "@react-three/fiber";
import { PerformanceMonitor } from "@react-three/drei";

export default function SelonLesFps() {
    const [dpr, setDpr] = useState(1.5);

    return (
        <Canvas dpr={dpr}>
            <PerformanceMonitor
                onDecline={() => setDpr(1)}
                onIncline={() => setDpr(2)}
                flipflops={3}
                onFallback={() => setDpr(1)}
            />
            <ambientLight />
            <mesh>
                <torusKnotGeometry />
                <meshStandardMaterial />
            </mesh>
        </Canvas>
    );
}
```

**`PerformanceMonitor`** observe les FPS et appelle `onDecline` quand ils chutent, `onIncline` quand ils remontent. `flipflops={3}` : après 3 allers-retours, il abandonne et appelle `onFallback`, pour éviter que la qualité clignote sans fin.

### Option B : seulement pendant les mouvements de caméra

```tsx
import { Canvas } from "@react-three/fiber";
import { AdaptiveDpr, OrbitControls } from "@react-three/drei";

export default function PendantLesMouvements() {
    return (
        <Canvas>
            <AdaptiveDpr pixelated />
            <OrbitControls makeDefault regress />
            <ambientLight />
            <mesh>
                <torusKnotGeometry />
                <meshStandardMaterial />
            </mesh>
        </Canvas>
    );
}
```

**`AdaptiveDpr`** baisse la résolution **pendant** que la caméra bouge, puis la rétablit. Il ne réagit qu'à un signal de « régression » : c'est la prop **`regress`** d'`OrbitControls` qui l'envoie (lu dans le code source de drei). Sans `regress`, `AdaptiveDpr` ne fait rien. `pixelated` garde des bords nets pendant la baisse.

---

## 4. Instancier les objets répétés

Cent cristaux identiques autour de l'arène = cent meshes = cent appels de dessin. Avec l'**instanciation**, la géométrie et le matériau sont envoyés **une fois**, avec une liste de positions : **un seul** appel.

```tsx
import { Canvas } from "@react-three/fiber";
import { Instances, Instance } from "@react-three/drei";

const CRISTAUX = Array.from({ length: 200 }, (_, i) => {
    const a = (i / 200) * Math.PI * 2;
    const r = 6 + (i % 7) * 0.3;
    return { x: Math.cos(a) * r, z: Math.sin(a) * r, h: 0.3 + (i % 5) * 0.15 };
});

export default function Cristaux() {
    return (
        <Canvas camera={{ position: [0, 6, 12] }}>
            <ambientLight intensity={0.5} />
            <directionalLight position={[3, 5, 2]} />
            <Instances limit={CRISTAUX.length}>
                <octahedronGeometry args={[0.3]} />
                <meshStandardMaterial color="violet" />
                {CRISTAUX.map((c, i) => (
                    <Instance key={i} position={[c.x, c.h, c.z]} scale={[1, 2, 1]} />
                ))}
            </Instances>
        </Canvas>
    );
}
```

- **`Instances`** déclare la géométrie et le matériau **communs**. **`Instance`** est un exemplaire, avec sa propre position, rotation, échelle, et même `color`.
- Chaque `Instance` accepte les événements (`onClick`…) et peut être animée par ref, comme un mesh.
- `limit` : le nombre maximal d'instances, réservé d'avance sur le GPU.
- Pour des **milliers** d'objets animés, `<instancedMesh>` de three, piloté directement par `setMatrixAt` dans `useFrame`, est plus rapide encore, mais plus bas niveau.
- `key={i}` est acceptable ici : la liste est **fixe**, elle ne change jamais d'ordre (à l'inverse de la main de cartes, voir [`ATELIER_CARTES`](ATELIER_CARTES.md)).

---

## 5. `frameloop` : ne pas dessiner pour rien

Par défaut (`frameloop="always"`), R3F redessine **60 fois par seconde**, même si rien ne bouge. Pour un écran **statique**, c'est du gaspillage : la batterie d'un portable fond, le ventilateur souffle.

| Mode | Comportement | Pour quoi |
|---|---|---|
| `"always"` | Une image à chaque rafraîchissement de l'écran. | **L'arène** : personnages animés, particules, tout ce qui bouge en continu. |
| `"demand"` | Une image **seulement** quand quelque chose change : une prop modifiée, un mouvement d'`OrbitControls`, un appel à `invalidate()`. | Menus 3D, écran de collection, inspection d'une carte immobile. |
| `"never"` | Jamais d'image automatique : c'est toi qui appelles `advance()`. | Rendu manuel, captures d'écran. |

```tsx
import { useState } from "react";
import { Canvas, useThree } from "@react-three/fiber";
import { OrbitControls } from "@react-three/drei";

function BoutonCouleur() {
    const invalidate = useThree((s) => s.invalidate);
    const [rouge, setRouge] = useState(false);
    return (
        <mesh
            onClick={() => {
                setRouge(!rouge);
                invalidate();
            }}
        >
            <boxGeometry />
            <meshStandardMaterial color={rouge ? "crimson" : "white"} />
        </mesh>
    );
}

export default function Collection() {
    return (
        <Canvas frameloop="demand">
            <ambientLight />
            <BoutonCouleur />
            <OrbitControls makeDefault />
        </Canvas>
    );
}
```

> ⚠️ **Le piège de `"demand"`** : un `useFrame` qui anime quelque chose **ne demande pas** de nouvelle image. Un personnage qui joue `Idle` (le mixeur avance dans un `useFrame`) reste **figé** dès que la souris s'arrête. Une animation de `damp` s'arrête à mi-chemin. C'est pourquoi l'arène reste en `"always"`. Une prop React modifiée déclenche bien une image ; ici, `invalidate()` est une ceinture de sécurité, et il devient **indispensable** quand on modifie un objet three directement, sans passer par React.

---

## 6. Libérer la mémoire : `dispose`

Une géométrie, un matériau, une texture occupent de la mémoire **sur le GPU**. Le *garbage collector* de JavaScript ne la voit pas : il faut appeler `.dispose()`, comme un `free`.

- **Ce que R3F fait pour toi** : quand un élément JSX est démonté (`<boxGeometry />`, `<meshStandardMaterial />`, un `<mesh>`), R3F appelle `dispose()` sur l'objet et ses enfants. Tu n'as rien à faire pour ce que tu as créé en JSX.
- **`dispose={null}`** sur un élément désactive ce comportement pour lui et ses enfants. `gltfjsx` le met sur la racine de ses composants : les géométries viennent du **cache** de `useGLTF`, partagées par d'autres composants, et les libérer casserait les autres exemplaires.
- **Ce que tu dois faire toi-même** : les objets créés **à la main** (`new Texture()`, `new BufferGeometry()` dans un `useMemo`) et passés via `<primitive>` ou une prop. Libère-les dans le nettoyage d'un `useEffect`.

```tsx
import { useEffect, useMemo } from "react";
import { CanvasTexture } from "three";

export function EtiquetteDessinee({ texte }: { texte: string }) {
    const texture = useMemo(() => {
        const c = document.createElement("canvas");
        c.width = 256;
        c.height = 64;
        const ctx = c.getContext("2d");
        if (ctx) {
            ctx.fillStyle = "white";
            ctx.font = "32px sans-serif";
            ctx.fillText(texte, 10, 44);
        }
        return new CanvasTexture(c);
    }, [texte]);

    useEffect(() => () => texture.dispose(), [texture]);

    return (
        <mesh>
            <planeGeometry args={[2, 0.5]} />
            <meshBasicMaterial map={texture} transparent />
        </mesh>
    );
}
```

`useEffect(() => () => texture.dispose(), [texture])` : quand `texte` change, `useMemo` crée une nouvelle texture, et le nettoyage de l'effet libère l'**ancienne**. C'est le couple `malloc` / `free` dans le cycle de vie d'un composant.

Pour vérifier qu'il n'y a pas de fuite : `gl.info.memory` donne le nombre de `geometries` et de `textures` vivantes sur le GPU. Il doit rester stable quand on ouvre et ferme plusieurs fois le même écran.

---

## 7. Erreurs fréquentes

| Symptôme | Cause probable | Fiche |
|---|---|---|
| `R3F: Hooks can only be used within the Canvas component!` | `useFrame` / `useThree` appelé dans un composant qui n'est pas **à l'intérieur** du `<Canvas>`. | [`ANIMATION`](ANIMATION.md) |
| Écran blanc, canvas de 150 px de haut | La chaîne de hauteurs `html > body > #root` n'est pas à 100 %. | [`INSTALLATION`](INSTALLATION.md) |
| Tout est noir | Pas de lumière avec un matériau `Standard`. Ou `metalness` à 1 sans `Environment`. | [`LUMIERES_MATERIAUX`](LUMIERES_MATERIAUX.md) |
| Rien ne s'affiche, aucune erreur | Objet derrière la caméra, à l'échelle 0,01 (Mixamo), ou plus loin que `far` (1000). | [`ANATOMIE`](ANATOMIE.md) |
| Écran vide pendant un moment, puis tout apparaît | Pas de `<Suspense>` autour de ce qui charge : c'est le `<Canvas>` entier qui attend. | [`DREI`](DREI.md) |
| Écran vide hors ligne | `Environment preset`, police de `Text` ou décodeur Draco chargés depuis un CDN. | [`DREI`](DREI.md) |
| Pas d'ombres | Il manque l'un des trois réglages : `shadows` sur `<Canvas>`, `castShadow` (sur la lumière **et** sur les objets), `receiveShadow` sur le sol. | [`LUMIERES_MATERIAUX`](LUMIERES_MATERIAUX.md) |
| Ombre coupée net | Boîte de la caméra d'ombre trop petite. | [`LUMIERES_MATERIAUX`](LUMIERES_MATERIAUX.md) |
| Un seul modèle visible sur deux | Même objet du cache affiché deux fois : il faut cloner. | [`MODELES_GLTF`](MODELES_GLTF.md) |
| Deux personnages font les mêmes gestes | `scene.clone()` au lieu de `SkeletonUtils.clone`. | [`SQUELETTE_ANIMATIONS`](SQUELETTE_ANIMATIONS.md) |
| Le personnage ne bouge pas, aucune erreur | Noms d'os différents entre le modèle et les clips. | [`SQUELETTE_ANIMATIONS`](SQUELETTE_ANIMATIONS.md) |
| Le personnage figé dès que la souris s'arrête | `frameloop="demand"` sur une scène animée. | Section 5 |
| Deux objets réagissent au même clic | `stopPropagation` manquant. | [`INTERACTION`](INTERACTION.md) |
| Les animations accélèrent sur un écran 120 Hz | Incrément fixe au lieu de `× delta`. | [`ANIMATION`](ANIMATION.md) |
| Saccades régulières | Allocations dans `useFrame` (`new Vector3()` à chaque image), ou `setState` à chaque image. | [`ANIMATION`](ANIMATION.md) |
| Erreur de boucle de rendu avec zustand | Sélecteur qui renvoie un objet neuf, sans `useShallow`. | [`ETAT_RESEAU`](ETAT_RESEAU.md) |
| Avertissement `PCFSoftShadowMap has been removed` | `shadows` booléen avec three 0.187 : passe à `shadows="percentage"`. | [`LUMIERES_MATERIAUX`](LUMIERES_MATERIAUX.md) |
| Clignotement entre deux surfaces | *z-fighting* : deux faces au même endroit. Écarte-les légèrement. | [`ATELIER_CARTES`](ATELIER_CARTES.md) |

---

## 🧪 À essayer

1. Ajoute `<Stats />` à la leçon 6 (drei) et note les FPS. Retire `Environment`, puis `ContactShadows`, puis `MeshDistortMaterial` : lequel coûte le plus ?
2. Dans `Cristaux`, remplace `Instances` par 200 vrais `<mesh>` et compare `gl.info.render.calls`.
3. Mets `frameloop="demand"` sur la leçon 3 (animation) : les objets se figent, puis bougent quand tu fais tourner la caméra.
4. Force `dpr={0.5}` : la scène devient floue, mais très rapide. C'est ce que fait `AdaptiveDpr` pendant un mouvement.
5. Ouvre et ferme dix fois un écran qui charge un modèle, et surveille `gl.info.memory` : le nombre de géométries reste-t-il stable ?
