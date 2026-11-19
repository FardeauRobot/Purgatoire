# 4. Interaction : clics, survol et état React

> **TL;DR.** R3F lance un **rayon** depuis la souris dans la scène (*raycasting*) et appelle `onClick`, `onPointerOver`, `onPointerOut`… sur les meshes touchés, avec la même API que le DOM. Un événement touche **tous** les objets sous la souris, du plus proche au plus lointain : `e.stopPropagation()` l'arrête au premier. Ici, `useState` est le bon outil, car un clic est **rare**. `onPointerMissed` sur le `<Canvas>` détecte un clic dans le vide.

Liens : [`INDEX`](INDEX.md) · [`ANIMATION`](ANIMATION.md) · [`HIERARCHIE`](HIERARCHIE.md) · [`ATELIER_CARTES`](ATELIER_CARTES.md)

🧪 Labo : `~/Labo/TestThreeFiber/src/lessons/L4_Interaction.jsx`

---

## 1. Comment la souris trouve un objet 3D

Dans le DOM, le navigateur sait quel `<div>` est sous la souris. Dans un `<canvas>`, il ne voit qu'une image : il faut **calculer** ce qui est dessous.

```text
  caméra ●──────────────────────────────►   rayon passant par le pixel de la souris
              │            │
          cube avant   cube arrière          → intersections triées par distance
          (distance 4)  (distance 7)           [cube avant, cube arrière]
```

À chaque mouvement ou clic, R3F :
1. convertit la position de la souris en coordonnées normalisées (-1 à 1) ;
2. lance un rayon depuis la caméra à travers ce point ;
3. teste les triangles des meshes **qui ont au moins un handler** (les autres sont ignorés, ce qui économise du calcul) ;
4. appelle les handlers du plus proche au plus lointain, jusqu'à un `stopPropagation()`.

---

## 2. Les événements disponibles

| Événement | Quand |
|---|---|
| `onClick` | Appui puis relâchement sur le même objet. |
| `onDoubleClick` | Double clic. |
| `onContextMenu` | Clic droit. |
| `onPointerDown` / `onPointerUp` | Appui / relâchement (souris, doigt, stylet). |
| `onPointerOver` / `onPointerOut` | La souris **entre sur** / **quitte** l'objet. |
| `onPointerEnter` / `onPointerLeave` | Déclenchés au même moment qu'`Over` / `Out` dans R3F. Prends `Over` / `Out`, la convention de la doc. |
| `onPointerMove` | Chaque mouvement au-dessus de l'objet. |
| `onWheel` | Molette au-dessus de l'objet. |
| `onPointerMissed` | Un clic qui ne touche **pas** cet objet. Sur `<Canvas>` : un clic qui ne touche rien du tout. |

« Pointer » couvre la souris, le tactile et le stylet : les mêmes handlers marchent sur téléphone.

---

## 3. L'objet événement

| Champ | Contenu |
|---|---|
| `e.object` | Le mesh réellement **touché** par le rayon. |
| `e.eventObject` | L'objet dont le **handler s'exécute**. Diffère de `object` quand le handler est sur un `<group>` parent. |
| `e.point` | Le point 3D exact de l'impact (`Vector3`, coordonnées du monde). |
| `e.distance` | Distance entre la caméra et l'impact. |
| `e.uv` | Coordonnées de texture à l'impact (0 à 1) : quelle zone de la carte a été cliquée. |
| `e.intersections` | Tous les objets traversés par le rayon, triés par distance. |
| `e.delta` | Distance **en pixels** entre l'appui et le relâchement. Sert à distinguer un clic d'un glissement. |
| `e.stopPropagation()` | Arrête l'événement : les objets plus loin ne le reçoivent pas. |
| `e.nativeEvent` | L'événement DOM d'origine (`shiftKey`, `button`…). |

---

## 4. Le cube interactif du labo

```tsx
import { useRef, useState } from "react";
import { Canvas, useFrame, type ThreeEvent } from "@react-three/fiber";
import type { Mesh } from "three";

function Box({ position }: { position: [number, number, number] }) {
    const ref = useRef<Mesh>(null);
    const [hovered, setHovered] = useState(false);
    const [active, setActive] = useState(false);

    useFrame((_, delta) => {
        if (ref.current) ref.current.rotation.y += delta * (active ? 2 : 0.3);
    });

    const clic = (e: ThreeEvent<MouseEvent>) => {
        e.stopPropagation();
        setActive(!active);
    };

    return (
        <mesh
            ref={ref}
            position={position}
            scale={active ? 1.5 : 1}
            onClick={clic}
            onPointerOver={(e) => {
                e.stopPropagation();
                setHovered(true);
                document.body.style.cursor = "pointer";
            }}
            onPointerOut={() => {
                setHovered(false);
                document.body.style.cursor = "auto";
            }}
        >
            <boxGeometry />
            <meshStandardMaterial color={hovered ? "hotpink" : "orange"} />
        </mesh>
    );
}

export default function Lecon4() {
    return (
        <Canvas camera={{ position: [0, 0, 6] }}>
            <ambientLight intensity={0.4} />
            <directionalLight position={[3, 5, 2]} intensity={2} />
            <Box position={[-1.5, 0, 0]} />
            <Box position={[1.5, 0, 0]} />
            <Box position={[0, 0, -3]} />
        </Canvas>
    );
}
```

Ce qu'il faut remarquer :
- **Deux mondes mélangés, chacun à sa place.** L'état React (`hovered`, `active`) pilote l'**apparence** : couleur, taille. `useFrame` pilote l'animation **continue** : la rotation, dont la vitesse dépend de `active`. C'est la règle de [`ANIMATION`](ANIMATION.md), appliquée.
- `ThreeEvent<MouseEvent>` : le type TypeScript d'un événement R3F. Pour `onPointerOver`, ce serait `ThreeEvent<PointerEvent>`. Dans un handler écrit directement en prop, TypeScript le devine tout seul.
- `scale={active ? 1.5 : 1}` saute d'un coup. Pour une transition douce, combine avec `MathUtils.damp` (voir [`ANIMATION`](ANIMATION.md), section 6).
- Le curseur est géré à la main via `document.body.style.cursor`. drei fournit `useCursor(hovered)` qui fait la même chose proprement (voir [`DREI`](DREI.md)).

---

## 5. La propagation : pourquoi `stopPropagation`

Le troisième cube est en `z = -3`, derrière les deux autres. Quand la souris est sur une zone où le cube avant cache le cube arrière, **le rayon traverse les deux** :

| Sans `stopPropagation` | Avec `stopPropagation` |
|---|---|
| Les deux cubes reçoivent `onPointerOver` : les deux deviennent roses. | Seul le cube avant le reçoit. |
| Un clic active les deux. | Seul le plus proche est activé. |

C'est différent du DOM, où un élément devant en cache un autre par défaut. En 3D, R3F ne peut pas savoir si tu veux qu'un objet soit « transparent aux clics » (une vitre, un effet de particules) ou non. Il te laisse décider.

> 💡 Un objet qu'on veut **ignorer** au clic (un effet visuel posé devant une carte) peut recevoir `raycast={() => null}` : le rayon le traverse comme s'il n'existait pas.

Il n'y a pas besoin de `stopPropagation` dans `onPointerOut`. R3F appelle même `onPointerOut` sur le cube arrière quand le cube avant arrête la propagation au-dessus de lui : pour la souris, il vient d'être caché.

---

## 6. `object` contre `eventObject` : un handler sur un groupe

```tsx
import { Canvas, type ThreeEvent } from "@react-three/fiber";

export default function GroupeCliquable() {
    const clic = (e: ThreeEvent<MouseEvent>) => {
        e.stopPropagation();
        console.log("touché :", e.object.name, "— handler de :", e.eventObject.name);
    };

    return (
        <Canvas>
            <ambientLight intensity={1} />
            <group name="carte" onClick={clic}>
                <mesh name="illustration">
                    <planeGeometry args={[1.4, 1.2]} />
                    <meshStandardMaterial color="skyblue" />
                </mesh>
                <mesh name="cartouche" position={[0, -0.85, 0]}>
                    <planeGeometry args={[1.4, 0.4]} />
                    <meshStandardMaterial color="beige" />
                </mesh>
            </group>
        </Canvas>
    );
}
```

Un clic sur le bas de la carte affiche `touché : cartouche — handler de : carte`. L'événement **remonte** du mesh touché vers ses parents, comme dans le DOM. C'est utile pour une carte faite de plusieurs morceaux : un seul handler sur le groupe, et `e.object` dit quelle partie a été touchée.

---

## 7. Clic dans le vide, clic contre glissement

```tsx
import { useState } from "react";
import { Canvas, type ThreeEvent } from "@react-three/fiber";
import { OrbitControls } from "@react-three/drei";

export default function Selection() {
    const [choisie, setChoisie] = useState<number | null>(null);

    const choisir = (i: number) => (e: ThreeEvent<MouseEvent>) => {
        if (e.delta > 4) return;
        e.stopPropagation();
        setChoisie(i);
    };

    return (
        <>
            <p style={{ position: "absolute", zIndex: 1, color: "white" }}>
                Carte choisie : {choisie ?? "aucune"}
            </p>
            <Canvas onPointerMissed={() => setChoisie(null)}>
                <ambientLight intensity={1} />
                {[0, 1, 2].map((i) => (
                    <mesh key={i} position={[(i - 1) * 1.6, 0, 0]} onClick={choisir(i)}>
                        <planeGeometry args={[1.4, 2]} />
                        <meshStandardMaterial color={choisie === i ? "gold" : "white"} />
                    </mesh>
                ))}
                <OrbitControls />
            </Canvas>
        </>
    );
}
```

- **`onPointerMissed`** sur `<Canvas>` : un clic qui ne touche aucun objet désélectionne la carte.
- **`e.delta > 4`** : avec `OrbitControls`, faire tourner la caméra commence par un appui sur une carte. Sans ce test, relâcher après un glissement **sélectionnerait** la carte. Au-delà de 4 pixels de déplacement, on considère que c'était un glissement.
- **L'état vit au-dessus du `<Canvas>`** : `choisie` est déclaré dans `Selection`, qui affiche à la fois le `<p>` HTML et la scène 3D. Le `<Canvas>` est un composant React comme un autre : on lui passe des props et des callbacks, rien de spécial. Pour un état partagé entre beaucoup de composants, on passera à un store (voir [`ETAT_RESEAU`](ETAT_RESEAU.md)).
- `choisir(i)` renvoie **une fonction** : c'est une fabrique de handlers, une closure qui retient `i` (voir [`../FONCTIONS`](../FONCTIONS.md)).

---

## 🧪 À essayer

1. Survole et clique les cubes. Clique le cube arrière là où il dépasse.
2. Retire `e.stopPropagation()` de `onPointerOver`, puis place la souris là où le cube avant cache le cube arrière : les deux s'allument.
3. Ajoute `onPointerMove={(e) => console.log(e.point)}` sur un cube et regarde la console : les coordonnées du point d'impact changent en glissant sur la face.
4. Ajoute un compteur de clics affiché **en HTML, hors du Canvas**. Il faut remonter l'état dans `Lesson4` et le passer en props aux `Box` : c'est la section 7.
5. Ajoute `onPointerMissed={() => console.log("raté")}` sur le `<Canvas>`, puis clique dans le vide.
6. Affiche `e.uv` au clic sur un plan : (0, 0) en bas à gauche, (1, 1) en haut à droite.
