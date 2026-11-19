# 7. Leva : régler la scène en direct

> **TL;DR.** `leva` affiche un panneau de réglages généré à partir d'un simple objet : `const { vitesse } = useControls({ vitesse: 1 })`. Le **type du contrôle est déduit de la valeur** : nombre → champ ou curseur, `"#rrggbb"` → couleur, booléen → case, `{ options }` → liste, `[x, y, z]` → vecteur. C'est un outil de **mise au point** : tu cherches les bonnes valeurs à l'œil, puis tu les recopies en dur. Le store de Leva est **global** : range tes contrôles dans des dossiers nommés, et cache le panneau en production avec `<Leva hidden />`.

Liens : [`INDEX`](INDEX.md) · [`LUMIERES_MATERIAUX`](LUMIERES_MATERIAUX.md) · [`ANIMATION`](ANIMATION.md) · [`PERFORMANCE_DEBUG`](PERFORMANCE_DEBUG.md)

🧪 Labo : `~/Labo/TestThreeFiber/src/lessons/L7_Leva.jsx`

---

## 1. Pourquoi un panneau de réglages

Trouver la bonne intensité de lumière, la bonne rugosité ou la bonne vitesse d'une animation en modifiant le code, c'est : modifier, sauvegarder, attendre le rechargement, regarder, recommencer. Avec Leva, tu fais glisser un curseur et la scène change **pendant que tu regardes**.

🔗 C'est l'équivalent d'un `printf` de débogage, mais dans l'autre sens : au lieu de lire une valeur, tu l'**écris** en direct.

---

## 2. Le minimum

```tsx
import { useRef } from "react";
import { Canvas, useFrame } from "@react-three/fiber";
import { useControls } from "leva";
import type { Mesh } from "three";

function Cube() {
    const ref = useRef<Mesh>(null);
    const { vitesse, couleur, filaire } = useControls({
        vitesse: { value: 1, min: 0, max: 5, step: 0.1 },
        couleur: "#4fc3f7",
        filaire: false,
    });

    useFrame((_, delta) => {
        if (ref.current) ref.current.rotation.y += delta * vitesse;
    });

    return (
        <mesh ref={ref}>
            <boxGeometry />
            <meshStandardMaterial color={couleur} wireframe={filaire} />
        </mesh>
    );
}

export default function Minimal() {
    return (
        <Canvas>
            <ambientLight intensity={0.4} />
            <directionalLight position={[3, 5, 2]} intensity={2} />
            <Cube />
        </Canvas>
    );
}
```

- `useControls` renvoie les **valeurs courantes**. Quand tu bouges un curseur, le composant est re-rendu avec la nouvelle valeur, exactement comme avec un `useState`.
- **TypeScript devine les types** depuis l'objet : `vitesse` est un `number`, `couleur` un `string`, `filaire` un `boolean`. Aucune annotation à écrire.
- Le panneau apparaît tout seul, en haut à droite de la page. Pas besoin de composant `<Leva />` pour commencer : il est ajouté automatiquement.
- `useControls` fonctionne **dedans comme dehors** du `<Canvas>` : c'est un hook React ordinaire, pas un hook R3F.

---

## 3. Quel contrôle pour quelle valeur

| Valeur donnée | Contrôle affiché | Type renvoyé |
|---|---|---|
| `1` | Champ numérique | `number` |
| `{ value: 1, min: 0, max: 5, step: 0.1 }` | **Curseur** (dès qu'il y a `min` et `max`) | `number` |
| `"#ff0000"` | Sélecteur de couleur | `string` |
| `"bonjour"` | Champ texte | `string` |
| `true` | Case à cocher | `boolean` |
| `{ value: "box", options: ["box", "sphere"] }` | Liste déroulante | `string` |
| `[0, 0, 0]` | Trois champs x, y, z | tableau de 3 nombres |
| `{ x: 0, y: 0 }` | Pavé 2D à glisser | `{ x, y }` |
| `button(() => …)` | Bouton | — (rien n'est renvoyé) |

Options utiles sur n'importe quel contrôle : `label` (texte affiché à la place de la clé), `hint` (info-bulle), `render: (get) => get("Objet.filaire")` (n'affiche le contrôle que si une condition est vraie).

---

## 4. La leçon 7 : dossiers et store global

```tsx
import { useRef } from "react";
import { Canvas, useFrame } from "@react-three/fiber";
import { OrbitControls } from "@react-three/drei";
import { useControls, folder } from "leva";
import type { Mesh } from "three";

function Playground() {
    const ref = useRef<Mesh>(null);

    const { geometrie, couleur, wireframe, roughness, metalness, vitesse, echelle, position } =
        useControls("Leçon 7", {
            Objet: folder({
                geometrie: { value: "torusKnot", options: ["box", "sphere", "torus", "torusKnot"] },
                echelle: { value: 1, min: 0.2, max: 3, step: 0.1 },
                position: [0, 0, 0],
                vitesse: { value: 0.5, min: -5, max: 5, step: 0.1 },
            }),
            Matériau: folder({
                couleur: "#4fc3f7",
                roughness: { value: 0.3, min: 0, max: 1 },
                metalness: { value: 0.6, min: 0, max: 1 },
                wireframe: false,
            }),
        });

    const { ambiante, directionnelle, lumierePos } = useControls("Leçon 7 — Lumières", {
        ambiante: { value: 0.3, min: 0, max: 2 },
        directionnelle: { value: 2, min: 0, max: 10 },
        lumierePos: [3, 5, 2],
    });

    useFrame((_, delta) => {
        if (!ref.current) return;
        ref.current.rotation.x += delta * vitesse * 0.5;
        ref.current.rotation.y += delta * vitesse;
    });

    return (
        <>
            <ambientLight intensity={ambiante} />
            <directionalLight position={lumierePos} intensity={directionnelle} />
            <mesh ref={ref} scale={echelle} position={position}>
                {geometrie === "box" && <boxGeometry args={[1.5, 1.5, 1.5]} />}
                {geometrie === "sphere" && <sphereGeometry args={[1, 64, 32]} />}
                {geometrie === "torus" && <torusGeometry args={[0.8, 0.3, 32, 100]} />}
                {geometrie === "torusKnot" && <torusKnotGeometry args={[0.7, 0.25, 200, 32]} />}
                <meshStandardMaterial
                    color={couleur}
                    roughness={roughness}
                    metalness={metalness}
                    wireframe={wireframe}
                />
            </mesh>
            <mesh position={lumierePos}>
                <sphereGeometry args={[0.1]} />
                <meshBasicMaterial color="yellow" />
            </mesh>
        </>
    );
}

export default function Lecon7() {
    return (
        <Canvas camera={{ position: [0, 1, 5] }}>
            <Playground />
            <OrbitControls makeDefault />
        </Canvas>
    );
}
```

Ce qu'il faut remarquer :
- **Le premier argument `"Leçon 7"`** crée un dossier dans le panneau. Le store de Leva est **unique pour toute la page** : deux composants qui déclarent chacun un contrôle `vitesse` à la racine **partageraient** la même valeur. Un dossier par composant évite cette collision silencieuse.
- **`folder({...})`** crée des sous-dossiers repliables. Les valeurs restent **à plat** dans le résultat : on écrit `vitesse`, pas `Objet.vitesse`.
- **Changer de géométrie** : chaque `{geometrie === "box" && …}` monte ou démonte un enfant. Quand la liste change, React démonte l'ancienne géométrie (R3F appelle son `dispose()`) et monte la nouvelle.
- **La petite sphère jaune** placée sur `lumierePos` rend visible une lumière qui, sinon, ne se voit pas. C'est un helper fait main.

---

## 5. Agir sur la caméra : `useThree` + `updateProjectionMatrix`

Certaines propriétés three ne prennent effet qu'après un **recalcul** explicite. Le champ de vision (`fov`) de la caméra en est l'exemple typique :

```tsx
import { useEffect } from "react";
import { useThree } from "@react-three/fiber";
import { useControls } from "leva";
import { PerspectiveCamera } from "three";

export function ReglageCamera() {
    const camera = useThree((s) => s.camera);
    const { fov } = useControls("Caméra", { fov: { value: 50, min: 15, max: 100 } });

    useEffect(() => {
        if (!(camera instanceof PerspectiveCamera)) return;
        camera.fov = fov;
        camera.updateProjectionMatrix();
    }, [camera, fov]);

    return null;
}
```

- `useThree((s) => s.camera)` récupère la caméra créée par `<Canvas>`.
- `camera.fov = fov` seul **ne change rien à l'écran** : three garde une matrice de projection en cache, calculée une fois. `updateProjectionMatrix()` la recalcule. Même règle pour `near`, `far`, `zoom` et `aspect`.
- `instanceof PerspectiveCamera` : le type de `s.camera` est une union (perspective **ou** orthographique), et seule la perspective a un `fov`. Le test sert de garde à TypeScript.
- `useEffect` et pas `useFrame` : la valeur ne change que quand on bouge le curseur. Inutile de la réécrire 60 fois par seconde.
- Le composant renvoie `null` : il n'affiche rien, il **agit** sur la scène. Ce motif est courant en R3F.

---

## 6. Boutons, écriture depuis le code, et production

```tsx
import { useControls, button, Leva } from "leva";

function Outils({ onReset }: { onReset: () => void }) {
    const [{ score }, set] = useControls("Debug", () => ({
        score: 0,
        "Remettre à zéro": button(() => {
            set({ score: 0 });
            onReset();
        }),
    }));

    return <p>Score : {score}</p>;
}

export default function App() {
    return (
        <>
            <Leva hidden={import.meta.env.PROD} collapsed />
            <Outils onReset={() => console.log("reset")} />
        </>
    );
}
```

- **Passer une fonction** à `useControls` (au lieu d'un objet) renvoie un tableau `[valeurs, set]`. `set({ score: 0 })` écrit dans le panneau depuis le code.
- **`button(() => …)`** ajoute un bouton : idéal pour « rejouer l'animation d'attaque », « distribuer une main », « déclencher une aura » pendant la mise au point.
- **`<Leva hidden={import.meta.env.PROD} />`** : en production (`npm run build`), Vite remplace `import.meta.env.PROD` par `true` et le panneau disparaît. Les `useControls` continuent de renvoyer leurs valeurs par défaut. `collapsed` le démarre replié.

> ⚠️ Pour que `import.meta.env` soit typé dans un projet Vite, `tsconfig` doit inclure `"types": ["vite/client"]`. C'est le cas dans le gabarit `react-ts` de Vite.

---

## 7. La bonne façon de s'en servir

1. Ajoute des contrôles sur les valeurs que tu hésites à choisir.
2. Règle à l'œil jusqu'à ce que ce soit beau.
3. **Recopie les valeurs en dur** dans le code, et retire le contrôle. Leva n'est pas un écran d'options pour le joueur : son style et sa position ne sont pas faits pour ça. Un menu d'options de jeu se fait en React normal, en dehors du `<Canvas>`.

Pour Transcendance, Leva est parfait pour régler l'éclairage de l'arène, la vitesse de fondu entre deux animations de personnage, ou la courbure de l'éventail de cartes.

---

## 🧪 À essayer

1. Dans la leçon 7, passe `metalness` à 1 et `roughness` à 0 : le nœud devient presque noir. Il n'a rien à refléter (voir [`LUMIERES_MATERIAUX`](LUMIERES_MATERIAUX.md)). Ajoute `<Environment preset="city" />` et recommence.
2. Ajoute `"icosahedron"` aux options, avec `<icosahedronGeometry args={[1, 0]} />`, puis passe le deuxième argument (`detail`) à 3.
3. Retire le nom de dossier `"Leçon 7"` dans **deux** composants qui ont un contrôle `vitesse` : ils se mettent à partager la même valeur.
4. Ajoute le composant `ReglageCamera` de la section 5 dans le `<Canvas>`, puis retire `updateProjectionMatrix()` : le curseur ne fait plus rien.
5. Ajoute un `button` qui remet la rotation du nœud à zéro via la ref.
