# 2. Le core à la main : `sheet.object` et `onValuesChange`

> **TL;DR.** `sheet.object("Cube", { position: { x: 0, y: 0, z: 0 } })` déclare un objet animable. Ses props sont **typées** par leur valeur par défaut (`0` → nombre, `true` → booléen, `{…}` → groupe) ou par `types.number`, `types.rgba`, `types.stringLiteral`… `obj.onValuesChange(cb)` appelle `cb` avec les valeurs courantes **une fois tout de suite**, puis à chaque changement. Il renvoie la fonction de désabonnement : renvoie-la depuis `useEffect`. Dans le callback, **mute l'objet three.js par une ref**, jamais `setState`. C'est la règle d'or de R3F, inchangée.

Liens : [`INDEX`](INDEX.md) · [`CONCEPTS`](CONCEPTS.md) · [`EDITABLE`](EDITABLE.md) · [`../react-three-fiber/ANIMATION`](../react-three-fiber/ANIMATION.md) · [`../react-three-fiber/LEVA`](../react-three-fiber/LEVA.md)

🧪 Labo : aucun pour l'instant. Les modules ci-dessous sont autonomes, sauf l'import de `./theatre`.

---

## 1. Pourquoi commencer sans `@theatre/r3f`

`@theatre/r3f` (fiche suivante) cache ce mécanisme derrière `<e.mesh>`. Mais :
- `e.mesh` n'expose que `position`, `rotation` et `scale`. Pour animer **l'opacité d'un matériau**, l'intensité d'un halo ou une valeur qui n'a rien de 3D (un compteur, le volume), il faut le core à la main.
- Le core seul n'a **aucune** dépendance à R3F. Si un jour R3F 10 casse `@theatre/r3f`, ce code-là continue de marcher.
- C'est le même motif que `useFrame` + ref : une fois compris ici, `e.mesh` n'a plus rien de magique.

---

## 2. Le projet, une seule fois

`src/theatre.ts` :

```ts
import { getProject } from "@theatre/core";

export const project = getProject("Aura");
```

- `getProject("Aura")` crée le projet au premier appel, puis renvoie **le même** aux appels suivants. Mais on le crée à **un seul** endroit, et on l'importe partout : c'est l'endroit où l'on branchera `state.json` en fiche [`PRODUCTION`](PRODUCTION.md).
- 🔗 **Lien C++.** C'est un singleton : une instance globale, accessible depuis tous les fichiers. Ici, c'est le système de modules qui garantit l'unicité : un module ES ne s'exécute qu'une fois, quel que soit le nombre de fichiers qui l'importent.

---

## 3. Déclarer un objet et ses props

```ts
import { types } from "@theatre/core";
import { project } from "./theatre";

const sheet = project.sheet("Démo");

export const reglages = sheet.object("Réglages", {
    vitesse: types.number(1, { range: [0, 10] }),
    position: { x: 0, y: 0, z: 0 },
    couleur: types.rgba({ r: 1, g: 0.4, b: 0.2, a: 1 }),
    visible: true,
    titre: "Partie 1",
    ambiance: types.stringLiteral("calme", { calme: "Calme", tendue: "Tendue" }),
});

export function lireVitesse(): number {
    return reglages.value.vitesse;
}
```

| Écriture | Type de prop | Dans le studio | S'interpole ? |
|---|---|---|---|
| `0` ou `types.number(0, opts)` | nombre | champ glissable | ✅ |
| `{ x: 0, y: 0 }` ou `types.compound({…})` | groupe | sous-props dépliables | chaque sous-prop |
| `types.rgba({ r, g, b, a })` | couleur (composantes 0 → 1) | sélecteur de couleur | ✅ |
| `true` ou `types.boolean(true)` | booléen | case à cocher | ❌ saute d'une valeur à l'autre |
| `"texte"` ou `types.string("texte")` | texte | champ texte | ❌ |
| `types.stringLiteral(défaut, { clé: "Libellé" })` | choix dans une liste | menu (`as: "switch"` pour des boutons) | ❌ |

Options utiles de `types.number` :
- `range: [min, max]` : borne le curseur du studio.
- `nudgeMultiplier` : de combien la valeur bouge quand tu glisses la souris. `0.01` pour une rotation en radians, `1` pour des pixels.

TypeScript **déduit** le type de `reglages.value` : `reglages.value.vitesse` est un `number`, `reglages.value.ambiance` est `"calme" | "tendue"`. Une faute de frappe dans un nom de prop est une erreur de compilation.

🔗 **Lien C.** `sheet.object(clé, props)` ressemble à la déclaration d'une `struct` avec des valeurs par défaut. La différence : chaque champ est aussi décrit pour un éditeur (bornes, libellés), comme si ta `struct` portait son propre formulaire de saisie.

---

## 4. Brancher l'objet sur un mesh R3F

```tsx
import { useEffect, useRef } from "react";
import { Canvas } from "@react-three/fiber";
import { types } from "@theatre/core";
import type { BufferGeometry, Mesh, MeshStandardMaterial } from "three";
import { project } from "./theatre";

const sheet = project.sheet("Démo");
const cubeObj = sheet.object("Cube", {
    position: { x: 0, y: 0, z: 0 },
    rotationY: types.number(0, { nudgeMultiplier: 0.01 }),
    couleur: types.rgba({ r: 1, g: 0.4, b: 0.2, a: 1 }),
    opacite: types.number(1, { range: [0, 1] }),
});

function Cube() {
    const ref = useRef<Mesh<BufferGeometry, MeshStandardMaterial>>(null);

    useEffect(() => {
        return cubeObj.onValuesChange((v) => {
            const mesh = ref.current;
            if (!mesh) return;
            mesh.position.set(v.position.x, v.position.y, v.position.z);
            mesh.rotation.y = v.rotationY;
            mesh.material.color.setRGB(v.couleur.r, v.couleur.g, v.couleur.b);
            mesh.material.opacity = v.opacite;
        });
    }, []);

    return (
        <mesh ref={ref}>
            <boxGeometry />
            <meshStandardMaterial transparent />
        </mesh>
    );
}

export default function Demo() {
    return (
        <Canvas camera={{ position: [0, 2, 6] }}>
            <ambientLight intensity={0.4} />
            <directionalLight position={[3, 5, 2]} intensity={2} />
            <Cube />
        </Canvas>
    );
}
```

Ligne par ligne :
- `sheet.object(…)` est appelé **au niveau du module**, pas dans le composant. Le composant est une fonction que React rappelle à chaque rendu, alors que l'objet Theatre doit exister une seule fois, comme le projet.
- `cubeObj.onValuesChange(cb)` : `cb` reçoit **toutes** les valeurs de l'objet, déjà interpolées pour l'instant courant de la séquence. Il est appelé une première fois **tout de suite** (vérifié), donc le mesh prend ses valeurs dès le montage.
- `return cubeObj.onValuesChange(…)` : `onValuesChange` renvoie la fonction de désabonnement, et `useEffect` appelle la fonction qu'on lui renvoie au démontage. Sans ça, le callback continuerait de viser un mesh détruit. 🔗 C'est le `free` qui va avec le `malloc`, rangé au même endroit que lui.
- On écrit dans `ref.current`, **pas** dans un `useState`. Pendant la lecture d'une séquence, le callback tourne à chaque image. Un `setState` ici, c'est 60 re-rendus par seconde (voir [`../react-three-fiber/ANIMATION`](../react-three-fiber/ANIMATION.md), section 4).
- `transparent` sur le matériau : sans lui, `opacity` est ignorée.
- `setRGB(r, g, b)` : c'est aussi ce que fait `@theatre/r3f` avec ses props de couleur.

> 💡 **Et le studio ?** Ouvre l'outline : `Aura › Démo › Cube` apparaît, avec ses quatre props. Glisse `rotationY`, et le cube tourne. Tu n'as écrit aucune ligne d'interface : c'est la déclaration du §3 qui la produit.

---

## 5. L'autre façon : lire `obj.value` dans `useFrame`

```tsx
import { useRef } from "react";
import { useFrame } from "@react-three/fiber";
import type { Mesh } from "three";
import { project } from "./theatre";

const halo = project.sheet("Démo").object("Halo", { echelle: 1 });

export function Halo() {
    const ref = useRef<Mesh>(null);

    useFrame(() => {
        ref.current?.scale.setScalar(halo.value.echelle);
    });

    return (
        <mesh ref={ref}>
            <ringGeometry args={[0.8, 1, 64]} />
            <meshBasicMaterial color="gold" />
        </mesh>
    );
}
```

| | `onValuesChange` | `obj.value` dans `useFrame` |
|---|---|---|
| Appelé quand | une valeur change | à chaque image de R3F, qu'elle change ou non |
| Rythme | la boucle de Theatre | la boucle de R3F |
| Avec `frameloop="demand"` | il faut appeler `invalidate()` dans le callback, sinon rien ne se redessine | ne tourne que quand R3F dessine : une séquence qui joue ne réveille pas R3F |
| Idéal pour | la plupart des cas | mélanger avec un calcul de `useFrame` (une valeur Theatre **plus** un `damp`) |

---

## 6. Écrire une valeur depuis le code

`obj.initialValue = { … }` remplace les valeurs par défaut, sans écraser ce que tu as réglé dans le studio :

```ts
import { project } from "./theatre";

const lumiere = project.sheet("Démo").object("Lumière", {
    intensite: 1,
    position: { x: 0, y: 3, z: 0 },
});

lumiere.initialValue = { position: { y: 5 } };
```

L'ordre de priorité, du plus faible au plus fort : valeur par défaut de la déclaration → `initialValue` → valeur réglée dans le studio → keyframes de la séquence.

Pour **forcer** une valeur comme si tu l'avais tapée dans le studio, il existe `studio.transaction(({ set }) => set(obj.props.x, 10))`. Ça ne marche qu'avec le studio chargé, donc jamais en production.

---

## 🧪 À essayer

1. Monte la démo du §4. Dans le studio, sélectionne `Cube`, glisse chaque prop, et vérifie que le cube suit.
2. Ajoute un `console.log("cb")` au début du callback, puis retire le `return` devant `cubeObj.onValuesChange`. Affiche et masque le cube plusieurs fois (un `useState` + un bouton), puis glisse une valeur : combien de `cb` par mouvement ? Chaque montage a laissé un abonnement derrière lui, comme une fuite mémoire. Remets le `return` et recompte.
3. Remplace la mutation de la ref par un `useState` qui reçoit `v`. Pose deux keyframes et lance la lecture (fiche [`STUDIO`](STUDIO.md)) avec le *Profiler* des React DevTools ouvert : compte les rendus.
4. Ajoute `types.stringLiteral` pour choisir entre trois géométries, et affiche la bonne selon la valeur. Ici, un `useState` est **justifié** : la valeur change rarement et change la structure du JSX.
5. Mets `range: [0, 1]` sur `rotationY` et essaie d'aller au-delà dans le studio.
