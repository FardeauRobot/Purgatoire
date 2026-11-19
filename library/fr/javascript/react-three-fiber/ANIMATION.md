# 3. Animer : `useFrame`, `useRef` et la règle d'or

> **TL;DR.** `useFrame((state, delta) => …)` s'exécute **à chaque image**. On y modifie **directement** l'objet three.js obtenu par une **ref** (`ref.current.rotation.y += …`), et **jamais** l'état React. Multiplie toujours par `delta` (secondes depuis l'image précédente) pour une vitesse indépendante de l'écran. Pour une transition douce vers une valeur choisie par React, utilise `MathUtils.damp`. `useFrame` ne marche **qu'à l'intérieur de `<Canvas>`**.

Liens : [`INDEX`](INDEX.md) · [`ANATOMIE`](ANATOMIE.md) · [`INTERACTION`](INTERACTION.md) · [`../FONCTIONS`](../FONCTIONS.md) (closures)

🧪 Labo : `~/Labo/TestThreeFiber/src/lessons/L3_Animation.jsx` (et `useSpin` dans `L5_Hierarchie.jsx`)

---

## 1. Le modèle mental : deux horloges

```text
 React (rare)                                   three.js (60 à 144 fois par seconde)
 ─────────────                                  ─────────────────────────────────────
 props / useState changent                      useFrame : chaque image
   → React recalcule le JSX (re-rendu)            → tu modifies ref.current.position…
   → R3F applique les différences                 → three redessine
   ⚠️ coûteux : comparaisons, allocations         ✅ quasi gratuit : on écrit dans un objet existant
```

React est fait pour des changements **rares** : un clic, un message réseau, un changement de tour. Une animation, c'est un changement **à chaque image**. On la fait donc **à côté** de React : on récupère l'objet three.js réel par une ref, et on le modifie sur place. React ne voit rien passer, et n'a rien à recalculer.

🔗 **Lien C.** C'est la boucle de jeu de `so_long` : `mlx_loop_hook` appelle ta fonction à chaque image, et tu modifies directement la structure du joueur. `useFrame` est ce hook. La différence : ici, une autre couche (React) gère aussi l'objet, et il ne faut pas la réveiller pour rien.

---

## 2. Le schéma de base

```tsx
import { useRef } from "react";
import { Canvas, useFrame } from "@react-three/fiber";
import type { Mesh } from "three";

function SpinningCube({ position, color, speed }: {
    position: [number, number, number];
    color: string;
    speed: number;
}) {
    const ref = useRef<Mesh>(null);

    useFrame((_state, delta) => {
        if (!ref.current) return;
        ref.current.rotation.x += delta * speed;
        ref.current.rotation.y += delta * speed * 0.7;
    });

    return (
        <mesh ref={ref} position={position}>
            <boxGeometry />
            <meshStandardMaterial color={color} />
        </mesh>
    );
}

export default function Lecon3() {
    return (
        <Canvas camera={{ position: [0, 2, 6] }}>
            <ambientLight intensity={0.3} />
            <directionalLight position={[3, 5, 2]} intensity={2} />
            <SpinningCube position={[-2.5, 0.5, 0]} color="tomato" speed={1} />
            <SpinningCube position={[2.5, 0.5, 0]} color="mediumseagreen" speed={3} />
        </Canvas>
    );
}
```

Ligne par ligne :
- `useRef<Mesh>(null)` : une boîte qui contiendra le `THREE.Mesh` réel. Elle vaut `null` jusqu'au montage, d'où le `if (!ref.current) return;` exigé par TypeScript. Le labo, en JS, s'en passe : au moment où `useFrame` tourne, le mesh est déjà monté.
- `ref={ref}` sur `<mesh>` : R3F range l'objet créé dans `ref.current`.
- `useFrame(callback)` enregistre `callback` dans la boucle de rendu. Il est **désinscrit automatiquement** au démontage du composant.
- Chaque `<SpinningCube>` a **sa propre** ref et son propre `useFrame` : deux instances, deux cubes indépendants.

> 🚨 **`useFrame` hors de `<Canvas>` = erreur** (`R3F: Hooks can only be used within the Canvas component!`). C'est pour ça que le cube est un composant séparé : `Lecon3` contient le `<Canvas>`, mais n'est pas **dedans**. Même règle pour `useThree`, `useLoader` et les hooks de drei qui en dépendent.

---

## 3. `delta` et `elapsedTime` : deux façons de compter le temps

| Variable | Valeur | Usage |
|---|---|---|
| `delta` | Secondes depuis l'**image précédente** (≈ 0.016 à 60 Hz, ≈ 0.007 à 144 Hz) | Mouvement **incrémental** : `rotation += vitesse * delta` |
| `state.clock.elapsedTime` | Secondes depuis le **début** | Mouvement **périodique** : `y = Math.sin(t)` |

**Pourquoi multiplier par `delta`.** Sans `delta`, `rotation.y += 0.01` ajoute 0.01 **par image** : deux fois plus vite sur un écran 120 Hz que sur un 60 Hz, et plus lent quand l'ordinateur rame. Avec `delta`, `rotation.y += 1 * delta` ajoute 1 radian **par seconde**, quel que soit l'écran. En multijoueur, c'est indispensable : deux joueurs doivent voir la même animation à la même vitesse.

La sphère qui rebondit du labo utilise le temps absolu :

```tsx
import { useRef } from "react";
import { useFrame } from "@react-three/fiber";
import type { BufferGeometry, Mesh, MeshStandardMaterial } from "three";

export function BouncingSphere() {
    const ref = useRef<Mesh<BufferGeometry, MeshStandardMaterial>>(null);

    useFrame((state) => {
        if (!ref.current) return;
        const t = state.clock.elapsedTime;
        ref.current.position.y = Math.abs(Math.sin(t * 3)) * 1.5;
        ref.current.material.color.setHSL((t * 0.1) % 1, 0.7, 0.5);
    });

    return (
        <mesh ref={ref}>
            <sphereGeometry args={[0.5, 32, 16]} />
            <meshStandardMaterial />
        </mesh>
    );
}
```

- `Math.sin(t * 3)` oscille entre -1 et 1. `Math.abs` replie la partie négative : la sphère rebondit sur le sol au lieu de passer dessous. Le `* 3` règle la fréquence, le `* 1.5` la hauteur.
- `setHSL(teinte, saturation, luminosité)` : la teinte fait le tour du cercle chromatique en 10 secondes (`t * 0.1`, modulo 1).
- `Mesh<BufferGeometry, MeshStandardMaterial>` : en TypeScript, un `Mesh` a par défaut un matériau de type générique, sans `color`. On précise le type du matériau pour avoir droit à `.color`.

---

## 4. La règle d'or : jamais de `setState` dans `useFrame`

Le contre-exemple, à ne **pas** recopier :

```text
const [angle, setAngle] = useState(0);
useFrame((_, delta) => setAngle((a) => a + delta));   // ❌
return <mesh rotation-y={angle}> … </mesh>;
```

À chaque image : `setAngle` programme un re-rendu, React rappelle la fonction composant, reconstruit le JSX, le compare à l'ancien, puis R3F applique `rotation-y`. 60 fois par seconde, **pour chaque objet animé**. Avec 30 cartes à l'écran, ça fait 1 800 re-rendus par seconde, pour obtenir le même résultat qu'une affectation `ref.current.rotation.y += delta`.

| Le changement est… | Outil |
|---|---|
| Continu, à chaque image (rotation, flottement, suivi de souris) | **ref + `useFrame`** |
| Rare et discret (sélection, survol, tour de jeu, donnée réseau) | **`useState`** (ou un store, voir [`ETAT_RESEAU`](ETAT_RESEAU.md)) |
| Rare, mais doit **s'animer** pour y arriver | **Les deux** : l'état donne la **cible**, `useFrame` s'en **rapproche** (section 6) |

---

## 5. Un hook à soi : `useSpin`

La leçon 5 factorise le motif « une ref qui tourne » dans un **hook personnalisé** :

```tsx
import { useRef } from "react";
import { useFrame } from "@react-three/fiber";
import type { Object3D } from "three";

export function useSpin<T extends Object3D>(speed: number) {
    const ref = useRef<T>(null);
    useFrame((_, delta) => {
        if (ref.current) ref.current.rotation.y += delta * speed;
    });
    return ref;
}
```

Usage : `const terre = useSpin<Mesh>(2);` puis `<mesh ref={terre}>`.

- Un hook, c'est une simple fonction dont le nom commence par `use` et qui appelle d'autres hooks. Rien de magique.
- **Règle des hooks** : appelle-les toujours dans le même ordre, au premier niveau du composant. Jamais dans un `if` ni dans une boucle. React les identifie par leur **position** dans la liste des appels, comme `args` identifie les paramètres par leur position.
- `<T extends Object3D>` : un générique TypeScript. Le hook marche pour un `Mesh` comme pour un `Group`. C'est l'équivalent d'un `template <typename T>` en C++.

---

## 6. Transitions douces : l'état donne la cible, `useFrame` s'en rapproche

Le motif le plus utile pour un jeu de cartes. Une carte survolée doit **monter progressivement**, pas sauter :

```tsx
import { useRef, useState } from "react";
import { useFrame } from "@react-three/fiber";
import { MathUtils, type Mesh } from "three";

export function CarteQuiMonte() {
    const ref = useRef<Mesh>(null);
    const [survol, setSurvol] = useState(false);

    useFrame((_, delta) => {
        if (!ref.current) return;
        const cible = survol ? 0.4 : 0;
        ref.current.position.y = MathUtils.damp(ref.current.position.y, cible, 8, delta);
    });

    return (
        <mesh
            ref={ref}
            onPointerOver={() => setSurvol(true)}
            onPointerOut={() => setSurvol(false)}
        >
            <planeGeometry args={[1.4, 2]} />
            <meshStandardMaterial color="white" />
        </mesh>
    );
}
```

- `useState` change **deux fois** (entrée et sortie de la souris). Ça, c'est rare.
- `useFrame` lit `survol` (grâce à la *closure*, voir [`../FONCTIONS`](../FONCTIONS.md)) et rapproche la position de la cible à chaque image.
- `MathUtils.damp(actuel, cible, lambda, delta)` parcourt une **fraction** de la distance restante, calculée à partir de `delta`. Plus `lambda` est grand, plus c'est rapide (8 ≈ vif, 2 ≈ lent). Le mouvement ralentit en arrivant : c'est ce qui le rend naturel.

### `lerp` contre `damp`
On voit souvent `x = MathUtils.lerp(x, cible, 0.1)` : « avance de 10 % de la distance restante ». Mais **par image** : à 144 Hz, la carte arrive deux fois plus vite qu'à 60 Hz. `damp` corrige ça en tenant compte de `delta`. Préfère `damp`.

> 💡 Bibliothèques possibles quand les transitions se multiplient : `maath` (fonctions `easing.damp3`, `dampE` pour les vecteurs et les rotations, de pmndrs) ou `@react-spring/three` (animations à ressort pilotées par l'état). Commence sans : `damp` couvre beaucoup de besoins.

---

## 7. `useThree` : accéder au reste de la scène

```tsx
import { useFrame, useThree } from "@react-three/fiber";

export function CameraQuiSuitLaSouris() {
    const camera = useThree((s) => s.camera);

    useFrame((state, delta) => {
        camera.position.x += (state.pointer.x * 2 - camera.position.x) * Math.min(1, delta * 3);
        camera.lookAt(0, 0, 0);
    });

    return null;
}
```

| Champ de `state` | Contenu |
|---|---|
| `camera`, `scene`, `gl` | La caméra, la scène, le renderer WebGL. |
| `pointer` | La souris, normalisée de -1 à 1 (0 au centre). |
| `size` | Taille du canvas en pixels. |
| `viewport` | Taille visible **en unités 3D** à la distance de la cible. Pratique pour placer une carte au bord de l'écran. |
| `clock` | L'horloge : `elapsedTime`, `getDelta()`. |
| `invalidate()` | Demande une image en mode `frameloop="demand"`. |

- Écris `useThree((s) => s.camera)` (avec un **sélecteur**) plutôt que `const { camera } = useThree()`. Sans sélecteur, le composant se re-rend à **chaque** changement du contexte, y compris un redimensionnement.
- Dans `useFrame`, `state` contient déjà tout : pas besoin de `useThree` pour lire `pointer` ou `clock`.

### Ordre d'exécution : le second argument de `useFrame`
`useFrame(cb, priorité)` : les callbacks s'exécutent par priorité croissante. Mais **une priorité positive** dit à R3F « je me charge du rendu moi-même » : R3F arrête alors de dessiner la scène. Ne mets jamais de priorité positive sans appeler `gl.render` toi-même, sinon l'écran reste figé. Ça ne sert qu'au *post-processing* écrit à la main.

---

## 🧪 À essayer

1. Ajoute un troisième `<SpinningCube>` avec `speed={-1}` : il tourne dans l'autre sens.
2. Fais orbiter la sphère autour de l'origine avec `position.x = Math.cos(t) * 2` et `position.z = Math.sin(t) * 2`. Compare ensuite avec la solution « groupe qui tourne » de [`HIERARCHIE`](HIERARCHIE.md).
3. Remplace `delta * speed` par `0.01 * speed`. Si tu as un écran 120 Hz, branche un écran 60 Hz (ou active le mode économie d'énergie) et compare la vitesse.
4. Le contre-exemple : passe le cube en `useState` + `setState` dans `useFrame`. Ouvre l'onglet *Profiler* des React DevTools et enregistre deux secondes : compte les rendus. Puis remets la ref.
5. Copie `CarteQuiMonte` dans le labo. Fais varier `lambda` (2, 8, 30). Remplace `damp` par `lerp(…, 0.1)` et compare.
