# 📒 Synthèse : React Three Fiber

> **TL;DR.** La fiche de révision de [`react-three-fiber/`](INDEX.md). L'idée, la correspondance three.js ↔ JSX, les deux horloges (React et `useFrame`), puis chaque notion du dossier réduite à ce qu'il faut savoir par cœur, avec sa fiche et son paragraphe. Les deux blocs de code sont recopiés d'[`ANIMATION`](ANIMATION.md) §2 et §6 : la rotation continue et la transition douce, les deux motifs à savoir réécrire de tête.

Liens : [`INDEX`](INDEX.md) · [`ABORDER`](ABORDER.md) · [`../react/SYNTHESE`](../react/SYNTHESE.md) · [`../theatre-js/SYNTHESE`](../theatre-js/SYNTHESE.md) · [`../SYNTHESE`](../SYNTHESE.md)

Versions : `@react-three/fiber` 9.8, `@react-three/drei` 10.7, `three` 0.187, React 19.3, Leva 0.10, Zustand 5.1 (voir [`INDEX`](INDEX.md), « Ce qui a été vérifié »).

---

## 1. L'idée en cinq lignes

- **Le besoin** : de la 3D three.js dans une application React, sans deux mondes à synchroniser à la main.
- **La réponse** : la scène en JSX. Une balise = une classe three.js, une prop = un setter, un démontage = la mémoire GPU libérée.
- **React décide de la cible, `useFrame` s'en rapproche** à chaque image, en mutant l'objet par une ref.
- **Le serveur fait autorité** ; la scène lit le store et affiche.
- **drei avant de réinventer**, **mesurer avant d'optimiser**.

---

## 2. three.js ↔ R3F

| three.js | R3F | Fiche |
|---|---|---|
| renderer + scène + caméra + boucle | `<Canvas>` | [`ANATOMIE`](ANATOMIE.md) §1 |
| `new THREE.Mesh(geo, mat)` | `<mesh><boxGeometry /><meshStandardMaterial /></mesh>` | §3 |
| `new THREE.BoxGeometry(1, 2, 3)` | `<boxGeometry args={[1, 2, 3]} />` (ordre du constructeur) | §4 |
| `mesh.position.set(1, 0, 0)` | `position={[1, 0, 0]}` | §5 |
| `requestAnimationFrame` | `useFrame((state, delta) => …)` | [`ANIMATION`](ANIMATION.md) |
| `raycaster.intersectObjects` | `onClick`, `onPointerOver` sur un mesh | [`INTERACTION`](INTERACTION.md) |
| `scene.add(objetExistant)` | `<primitive object={…} />` | [`MODELES_GLTF`](MODELES_GLTF.md) §3 |
| `dispose()` à la main | automatique au démontage | [`PERFORMANCE_DEBUG`](PERFORMANCE_DEBUG.md) §6 |

Repère : **Y en haut**, X à droite, Z vers toi ; caméra par défaut en `z = 5`. Rotations en **radians** ([`ANATOMIE`](ANATOMIE.md) §2, §5).

---

## 3. Animer : les deux motifs

### Mouvement continu ([`ANIMATION`](ANIMATION.md) §2)
```tsx
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
```

### Transition vers une cible ([`ANIMATION`](ANIMATION.md) §6)
```tsx
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

| Règle | § |
|---|---|
| `useFrame` / `useThree` seulement **dans** `<Canvas>` | ANIMATION, TL;DR |
| Toujours `× delta` ; `elapsedTime` pour une valeur qui dépend de l'heure (oscillation) | 3 |
| **Jamais** `setState` dans `useFrame` : mute la ref | 4 |
| `damp` plutôt que `lerp` : indépendant de la fréquence d'écran | 6 |
| `useState` convient aux événements **rares** (clic, survol) | [`INTERACTION`](INTERACTION.md), TL;DR |

---

## 4. Le reste du dossier, par notion

| Notion | À savoir par cœur | Fiche |
|---|---|---|
| **Installation** | `three @react-three/fiber @react-three/drei` ; `height: 100%` sur `html > body > #root` ; R3F 9.8 exige `react >=19 <19.4`, monter les deux ensemble | [`INSTALLATION`](INSTALLATION.md) §2, §4 |
| **Matériaux** | `meshStandardMaterial` par défaut : `roughness` + `metalness`. `emissive` brille sans éclairer | [`LUMIERES_MATERIAUX`](LUMIERES_MATERIAUX.md) §2, §5 |
| **Lumières** | une `ambientLight` faible + une `directionalLight` principale | §3 |
| **Ombres** | trois réglages : `<Canvas shadows>`, `castShadow` sur la lumière, `castShadow` / `receiveShadow` sur les objets | §4 |
| **Interaction** | le rayon touche tout, du plus proche au plus loin : `e.stopPropagation()` ; `onPointerMissed` sur le `<Canvas>` pour le clic dans le vide | [`INTERACTION`](INTERACTION.md) §5, §7 |
| **Hiérarchie** | un enfant hérite des transformations du `<group>` parent ; tourner le groupe pour orbiter ; un groupe décalé pour choisir le pivot | [`HIERARCHIE`](HIERARCHIE.md) §2–4 |
| **drei** | contrôles, `Environment`, `Text`, `Html`… ; ce qui télécharge **suspend** : `<Suspense>` ; héberger ses fichiers dans `public/` (le CDN hors ligne = écran vide) | [`DREI`](DREI.md) §2–4 |
| **Leva** | `useControls({ vitesse: 1 })`, le type vient de la valeur ; store **global** → dossiers nommés ; `<Leva hidden />` en production | [`LEVA`](LEVA.md) §3–4, §6 |
| **glTF** | `.glb` dans `public/`, `useGLTF` (suspend, **met en cache**), `<primitive object={scene} />`, `<Clone>` pour plusieurs exemplaires, `gltfjsx` | [`MODELES_GLTF`](MODELES_GLTF.md) §3–4, §6 |
| **Squelettes** | une animation ne connaît que des **noms d'os** : même squelette = même bibliothèque mocap pour tous ; `useAnimations(clips, ref)`, `fadeOut` / `reset().fadeIn().play()` | [`SQUELETTE_ANIMATIONS`](SQUELETTE_ANIMATIONS.md) §2, §4–5 |
| **État et réseau** | store Zustand ; sélecteur dans les composants, `getState()` dans `useFrame` ; intentions vers le serveur, état en retour | [`ETAT_RESEAU`](ETAT_RESEAU.md) §3–6 |
| **Performance** | coût = appels de dessin + pixels (`dpr`) + ombres ; `<Stats />`, `gl.info.render` ; instancier ; `frameloop="demand"` pour les écrans **statiques** seulement | [`PERFORMANCE_DEBUG`](PERFORMANCE_DEBUG.md) §1–5 |

### L'atelier en une phrase ([`ATELIER_CARTES`](ATELIER_CARTES.md))
Chaque carte calcule une **cible** (position, rotation) à partir de l'état (en main, survolée, retournée, jouée) et `damp` l'en rapproche : on ne programme jamais « de A vers B », **on change la cible**.

---

## 5. Les 14 pièges, en une ligne chacun

Recopiés de [`INDEX`](INDEX.md) : canvas écrasé · `args` positionnel · radians · tout est noir · pas d'ombre (un réglage sur trois) · `setState` dans `useFrame` · `useFrame` hors du `<Canvas>` · clic qui traverse · écran vide au chargement (`Suspense` ou CDN) · modèle affiché une seule fois (cache) · personnage figé en T (noms d'os) · la scène qui décide de la partie · `frameloop="demand"` dans l'arène · React monté sans R3F.

---

## 🧠 Questions de révision

> [!question]- Les trois réglages nécessaires pour qu'un cube projette une ombre sur le sol ?
> `<Canvas shadows>`, `castShadow` sur la lumière, `castShadow` sur le cube **et** `receiveShadow` sur le sol. ([`LUMIERES_MATERIAUX`](LUMIERES_MATERIAUX.md) §4)

> [!question]- Comment faire tourner une lune autour d'une planète sans cosinus ?
> Mets la lune décalée dans un `<group>` centré sur la planète, et fais tourner le **groupe**. ([`HIERARCHIE`](HIERARCHIE.md) §3)

> [!question]- Pourquoi `<Text>` de drei affiche-t-il un écran vide sans connexion internet ?
> Sa police par défaut vient d'un CDN. Sers la police depuis `public/`. ([`DREI`](DREI.md) §4)

> [!question]- Un personnage mocap reste en T-pose alors que l'animation « joue ». Cause la plus probable ?
> Les noms d'os du clip ne correspondent pas à ceux du squelette du modèle. ([`SQUELETTE_ANIMATIONS`](SQUELETTE_ANIMATIONS.md) §2)

> [!question]- Comment passer d'« Idle » à « Attaque » sans saut brusque ?
> `actions.Idle.fadeOut(d)` puis `actions.Attaque.reset().fadeIn(d).play()`. ([`SQUELETTE_ANIMATIONS`](SQUELETTE_ANIMATIONS.md) §5)

> [!question]- 200 cartes identiques font chuter les images par seconde. Premier outil ?
> Mesurer (`gl.info.render` : nombre d'appels de dessin), puis **instancier** : un seul appel pour tous les exemplaires. ([`PERFORMANCE_DEBUG`](PERFORMANCE_DEBUG.md) §2, §4)

> [!question]- Pourquoi pas `frameloop="demand"` dans l'arène ?
> La scène ne se redessine que sur demande : les personnages animés et les `useFrame` s'arrêtent. Réservé aux écrans statiques. ([`PERFORMANCE_DEBUG`](PERFORMANCE_DEBUG.md) §5)

> [!question]- Différence entre `e.object` et `e.eventObject` dans un handler posé sur un groupe ?
> `object` : le mesh réellement touché par le rayon. `eventObject` : l'objet qui porte le handler (le groupe). ([`INTERACTION`](INTERACTION.md) §6)
