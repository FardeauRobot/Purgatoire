# 5. Groupes et hiérarchie : penser en repères locaux

> **TL;DR.** `<group>` est un conteneur invisible (`THREE.Group`). Ses enfants héritent de ses transformations : leurs `position`, `rotation` et `scale` sont **relatives au parent**. Pour faire tourner un objet **autour d'un point**, on ne calcule pas de cosinus : on le place décalé dans un groupe centré sur ce point, et on fait tourner **le groupe**. Le même truc sert à choisir le **pivot** d'une carte et à disposer une main en éventail.

Liens : [`INDEX`](INDEX.md) · [`ANIMATION`](ANIMATION.md) · [`INTERACTION`](INTERACTION.md) · [`ATELIER_CARTES`](ATELIER_CARTES.md)

🧪 Labo : `~/Labo/TestThreeFiber/src/lessons/L5_Hierarchie.jsx`

---

## 1. Le graphe de scène

Une scène three.js est un **arbre**, exactement comme le DOM ou comme ton arbre de répertoires :

```text
Scene
├─ ambientLight
├─ mesh Soleil
├─ pointLight
└─ group orbiteTerre          ← tourne sur Y
   └─ group Terre (x = 4)     ← décalé de 4 par rapport à son parent
      ├─ mesh Terre           ← tourne sur elle-même
      └─ group orbiteLune     ← tourne sur Y
         └─ mesh Lune (x = 1) ← décalé de 1 par rapport à la Terre
```

L'arbre JSX **est** cet arbre : chaque balise imbriquée devient un enfant three.js. Chaque nœud a sa propre `position`, `rotation` et `scale`, exprimées **dans le repère de son parent**.

🔗 **Lien C++.** Un chemin relatif (`../images/a.png`) ne veut rien dire tout seul : il dépend du répertoire courant. Une `position` d'enfant est pareille : `x = 1` veut dire « 1 unité à droite **de mon parent**, dans le sens où il est tourné ».

---

## 2. Ce qu'hérite un enfant

La transformation d'un objet dans le monde, c'est **celle de son parent, puis la sienne** :

| Le parent… | Effet sur l'enfant |
|---|---|
| se déplace | L'enfant se déplace avec lui. |
| tourne | L'enfant **tourne autour du parent** (son décalage tourne), et son orientation tourne aussi. |
| grandit (`scale={2}`) | L'enfant grandit **et** son décalage double : il s'éloigne. |
| devient invisible (`visible={false}`) | Tous ses descendants disparaissent. |

C'est pour ça qu'une Lune en `x = 1` à l'intérieur de la Terre suit la Terre partout, sans le moindre calcul : sa position est **définie par rapport à** la Terre.

---

## 3. Le système solaire du labo

```tsx
import { useRef } from "react";
import { Canvas, useFrame } from "@react-three/fiber";
import type { Object3D, Group, Mesh } from "three";

function useSpin<T extends Object3D>(speed: number) {
    const ref = useRef<T>(null);
    useFrame((_, delta) => {
        if (ref.current) ref.current.rotation.y += delta * speed;
    });
    return ref;
}

function SolarSystem() {
    const sun = useSpin<Mesh>(0.1);
    const earthOrbit = useSpin<Group>(0.5);
    const earth = useSpin<Mesh>(2);
    const moonOrbit = useSpin<Group>(2.5);
    const marsOrbit = useSpin<Group>(0.3);

    return (
        <>
            <mesh ref={sun}>
                <sphereGeometry args={[1.2, 32, 16]} />
                <meshStandardMaterial color="gold" emissive="orange" emissiveIntensity={1.5} />
            </mesh>
            <pointLight intensity={60} decay={1.5} />

            <group ref={earthOrbit}>
                <group position={[4, 0, 0]}>
                    <mesh ref={earth}>
                        <sphereGeometry args={[0.4, 32, 16]} />
                        <meshStandardMaterial color="dodgerblue" />
                    </mesh>
                    <group ref={moonOrbit}>
                        <mesh position={[1, 0, 0]}>
                            <sphereGeometry args={[0.12, 16, 8]} />
                            <meshStandardMaterial color="lightgray" />
                        </mesh>
                    </group>
                </group>
            </group>

            <group rotation={[0.3, 0, 0]}>
                <group ref={marsOrbit}>
                    <mesh position={[6.5, 0, 0]}>
                        <sphereGeometry args={[0.3, 32, 16]} />
                        <meshStandardMaterial color="indianred" />
                    </mesh>
                </group>
            </group>
        </>
    );
}

export default function Lecon5() {
    return (
        <Canvas camera={{ position: [0, 6, 12] }}>
            <ambientLight intensity={0.08} />
            <SolarSystem />
        </Canvas>
    );
}
```

Démontage pièce par pièce :
- **`earthOrbit`** est un groupe **vide, au centre**, qui tourne. Il ne se voit pas : son seul rôle est d'emporter ses enfants dans sa rotation.
- **Le groupe `position={[4, 0, 0]}`** décale la Terre de 4. Comme son parent tourne, ce décalage tourne : la Terre décrit un cercle de rayon 4. Aucun `Math.cos` nécessaire.
- **`earth`** tourne sur lui-même (la journée), **indépendamment** de l'orbite (l'année). Deux rotations, deux nœuds.
- **`moonOrbit`** est placé **dans** la Terre décalée : la Lune orbite autour de la Terre, et suit la Terre dans son orbite autour du Soleil.
- **Mars** : `rotation={[0.3, 0, 0]}` sur un groupe **parent** de l'orbite incline tout le plan de l'orbite. Le groupe enfant tourne ensuite « à plat » dans ce plan incliné.

### Pourquoi deux groupes pour la Terre et pas un ?
Si on mettait `position={[4, 0, 0]}` et la rotation d'orbite **sur le même groupe**, il tournerait **sur lui-même, en restant en x = 4**. La rotation d'un nœud se fait toujours autour de **sa propre origine**. Pour tourner autour d'un autre point, il faut un parent placé sur ce point.

---

## 4. Choisir le pivot : la charnière

Une `planeGeometry` est centrée sur son origine : une rotation la fait tourner **autour de son centre**. Pour la faire basculer autour de **son bord bas** (une carte qu'on relève, une porte), on décale le mesh dans un groupe placé sur le bord :

```tsx
import { useRef } from "react";
import { useFrame } from "@react-three/fiber";
import type { Group } from "three";

export function CarteSurCharniere() {
    const charniere = useRef<Group>(null);

    useFrame((state) => {
        if (!charniere.current) return;
        charniere.current.rotation.x = -Math.abs(Math.sin(state.clock.elapsedTime)) * (Math.PI / 2);
    });

    return (
        <group ref={charniere} position={[0, -1, 0]}>
            <mesh position={[0, 1, 0]}>
                <planeGeometry args={[1.4, 2]} />
                <meshStandardMaterial color="white" side={2} />
            </mesh>
        </group>
    );
}
```

- Le **groupe** est placé là où on veut le pivot : le bord bas de la carte, en `y = -1`.
- Le **mesh** est décalé de `+1` (la demi-hauteur) dans le groupe : son bord bas tombe pile sur l'origine du groupe.
- On fait tourner **le groupe** : la carte bascule autour de son bord bas, comme sur une charnière.

Retiens la formule : **groupe = pivot, mesh = décalé de l'opposé**.

---

## 5. Repère local contre repère monde

Parfois il faut la position **réelle** d'un objet profond dans l'arbre, par exemple pour faire partir un effet de la main d'un personnage :

| Méthode | Ce qu'elle fait |
|---|---|
| `objet.position` | Position **locale** : relative au parent. |
| `objet.getWorldPosition(vecteur)` | Écrit la position **dans le monde** dans `vecteur`. |
| `objet.localToWorld(vecteur)` | Convertit un point du repère de l'objet vers le monde. |
| `objet.worldToLocal(vecteur)` | L'inverse. |
| `objet.lookAt(x, y, z)` | Tourne l'objet pour qu'il regarde un point du monde. |

```tsx
import { useRef, type RefObject } from "react";
import { useFrame } from "@react-three/fiber";
import { Vector3, type Mesh } from "three";

const tmp = new Vector3();

export function SuiveurDeLune({ cible }: { cible: RefObject<Mesh | null> }) {
    const ref = useRef<Mesh>(null);

    useFrame(() => {
        if (!ref.current || !cible.current) return;
        cible.current.getWorldPosition(tmp);
        ref.current.lookAt(tmp);
    });

    return (
        <mesh ref={ref} position={[0, 3, 0]}>
            <coneGeometry args={[0.2, 0.6, 16]} />
            <meshNormalMaterial />
        </mesh>
    );
}
```

- `const tmp = new Vector3()` est créé **une seule fois**, hors du composant. Dans `useFrame`, on le **réutilise** au lieu d'écrire `new Vector3()` à chaque image. Allouer 60 objets par seconde, c'est donner du travail au *garbage collector*, et ça finit par provoquer des saccades.
- `lookAt` oriente l'axe **+Z** de l'objet vers la cible. Pour un cône (pointe sur +Y), il faudra un groupe parent tourné de 90° pour aligner la pointe : encore la hiérarchie.
- `RefObject<Mesh | null>` : on passe la ref d'un **autre** composant en prop. C'est comme passer un pointeur vers un objet qu'on ne possède pas.

---

## 6. Tableau récapitulatif des astuces de hiérarchie

| Besoin | Montage |
|---|---|
| Orbiter autour d'un point | Groupe sur le point qui tourne ▸ enfant décalé |
| Tourner autour d'un bord | Groupe sur le bord qui tourne ▸ enfant décalé de la demi-taille |
| Incliner une orbite | Groupe incliné ▸ groupe qui tourne ▸ enfant décalé |
| Déplacer un ensemble (carte + texte + effets) | Un seul groupe parent, on ne bouge que lui |
| Éventail de cartes | Groupe par carte, tourné d'un angle ▸ carte décalée vers le haut (voir [`ATELIER_CARTES`](ATELIER_CARTES.md)) |

---

## 🧪 À essayer

1. Mets `scale={2}` sur le groupe de la Terre (celui en `x = 4`) : la Terre **et** la Lune grossissent, et la Lune s'éloigne, car son `x = 1` est multiplié par 2.
2. Ajoute une deuxième lune à la Terre, avec son propre groupe d'orbite et une vitesse différente.
3. Ajoute un anneau à Mars : `<mesh rotation={[Math.PI / 2, 0, 0]}><ringGeometry args={[0.45, 0.6, 32]} /><meshStandardMaterial side={2} /></mesh>`, **à l'intérieur** du mesh de Mars pour qu'il le suive. Sans `side={2}` (`DoubleSide`), l'anneau n'est visible que d'un côté.
4. Fusionne `earthOrbit` et le groupe `position={[4, 0, 0]}` en un seul groupe : la Terre tourne sur place au lieu d'orbiter. Comprends pourquoi (section 3), puis annule.
5. Dans `CarteSurCharniere`, retire le décalage `position={[0, 1, 0]}` du mesh : la carte tourne autour de son centre, et plus de son bord.
