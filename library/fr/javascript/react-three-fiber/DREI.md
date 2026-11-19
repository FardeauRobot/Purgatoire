# 6. drei : la boîte à outils

> **TL;DR.** `@react-three/drei` est une grosse collection de composants et de hooks prêts à l'emploi pour R3F : contrôles de caméra, éclairage HDR, textes, HTML dans la 3D, chargeurs, effets. **Avant d'écrire quelque chose de compliqué, cherche si drei l'a déjà.** Ce qui télécharge un fichier (HDR, police, modèle) **suspend** le composant : il faut un `<Suspense>`. Les `preset` d'`Environment` et la police par défaut de `Text` viennent d'un **CDN** : hors ligne, l'écran reste vide. Pour un jeu, sers ces fichiers toi-même depuis `public/`.

Liens : [`INDEX`](INDEX.md) · [`LUMIERES_MATERIAUX`](LUMIERES_MATERIAUX.md) · [`MODELES_GLTF`](MODELES_GLTF.md) · [`../ASYNCHRONE`](../ASYNCHRONE.md) · [catalogue drei](https://drei.docs.pmnd.rs)

🧪 Labo : `~/Labo/TestThreeFiber/src/lessons/L6_Drei.jsx`

---

## 1. La leçon 6, en TypeScript

```tsx
import { Suspense } from "react";
import { Canvas } from "@react-three/fiber";
import {
    OrbitControls,
    Environment,
    ContactShadows,
    Float,
    Text,
    RoundedBox,
    MeshDistortMaterial,
    Html,
} from "@react-three/drei";

export default function Lecon6() {
    return (
        <Canvas camera={{ position: [0, 1.5, 6], fov: 45 }}>
            <Suspense fallback={null}>
                <Environment preset="city" />

                <Float speed={2} rotationIntensity={1} floatIntensity={1}>
                    <RoundedBox args={[1, 1, 1]} radius={0.15} position={[-1.8, 0.5, 0]}>
                        <meshStandardMaterial color="#ff6b6b" metalness={1} roughness={0.15} />
                    </RoundedBox>
                </Float>

                <mesh position={[0, 0.6, 0]}>
                    <sphereGeometry args={[0.7, 64, 64]} />
                    <MeshDistortMaterial color="#6b8bff" distort={0.4} speed={3} roughness={0.1} />
                </mesh>

                <Text position={[0, 2.2, 0]} fontSize={0.45} color="white">
                    Bonjour drei
                </Text>

                <Html position={[0, -0.6, 1]} center>
                    <div style={{ background: "#000a", padding: "4px 8px", borderRadius: 6 }}>
                        Je suis du HTML dans la 3D
                    </div>
                </Html>

                <ContactShadows position={[0, -0.8, 0]} opacity={0.6} blur={2.5} scale={10} />
            </Suspense>

            <OrbitControls makeDefault />
        </Canvas>
    );
}
```

Remarque l'absence de toute `ambientLight` ou `directionalLight` : **`Environment` éclaire tout seul**, et c'est lui qui rend les métaux beaux (voir section 3).

---

## 2. `<Suspense>` : attendre un téléchargement

`Environment`, `Text`, `useGLTF`, `useTexture` ont besoin d'un fichier avant de pouvoir s'afficher. Au lieu de renvoyer « rien pour l'instant » et de te laisser gérer un état `chargement`, ils **suspendent** : ils interrompent le rendu du composant, et React remonte jusqu'au `<Suspense>` le plus proche, qui affiche son `fallback`. Quand le fichier arrive, React réessaie.

```text
<Suspense fallback={<Sablier />}>      ← affiche <Sablier /> tant qu'un enfant attend
    <Environment preset="city" />      ← suspend : télécharge le .hdr
    <Text>…</Text>                     ← suspend : télécharge la police
</Suspense>
<OrbitControls />                      ← hors du Suspense : disponible tout de suite
```

- **Sans `<Suspense>`**, la suspension remonte jusqu'au `<Canvas>`, qui en contient un par défaut : toute la scène reste vide pendant le chargement.
- Le `fallback` doit être **un élément 3D** s'il est dans le `<Canvas>` (un `<mesh>`, ou `null`). Un `<div>` y est interdit : la scène n'est pas du HTML.
- Plusieurs `<Suspense>` = plusieurs zones qui apparaissent indépendamment. Par exemple : le plateau tout de suite, les personnages quand leurs modèles arrivent.

🔗 C'est le même principe que `await` (voir [`../ASYNCHRONE`](../ASYNCHRONE.md)), mais à l'échelle d'un composant : « je ne peux pas continuer sans cette donnée, reviens me voir quand elle est là ».

### Afficher une progression

```tsx
import { Suspense } from "react";
import { Canvas } from "@react-three/fiber";
import { Environment, Html, useProgress, Loader } from "@react-three/drei";

function Pourcentage() {
    const { progress } = useProgress();
    return <Html center>{progress.toFixed(0)} %</Html>;
}

export default function AvecChargement() {
    return (
        <>
            <Canvas>
                <Suspense fallback={<Pourcentage />}>
                    <Environment preset="sunset" background />
                </Suspense>
            </Canvas>
            <Loader />
        </>
    );
}
```

- `useProgress` suit **tous** les téléchargements three en cours, et pas seulement ceux du `Suspense` le plus proche.
- `<Loader />` est un écran de chargement tout fait, à placer **hors** du `<Canvas>`, car il est en HTML.

---

## 3. Le catalogue utile, par famille

### Caméra et contrôles
| Composant | Rôle | Props utiles |
|---|---|---|
| `OrbitControls` | Tourner autour d'un point, zoomer, se déplacer latéralement. | `makeDefault`, `autoRotate`, `enableZoom={false}`, `enablePan={false}`, `minPolarAngle` / `maxPolarAngle` (bloquer sous le sol), `target` |
| `PresentationControls` | Faire tourner **un objet** à la souris, avec retour élastique. Idéal pour inspecter une carte. | `polar`, `azimuth`, `snap` |
| `CameraControls` | Contrôles plus riches, transitions animées (`setLookAt`). | — |
| `PerspectiveCamera` | Une caméra déclarée en JSX. `makeDefault` en fait la caméra active. | `makeDefault`, `position`, `fov` |

`makeDefault` sur un contrôle l'enregistre dans le state R3F : les autres composants drei (`Bounds`, `Html`…) savent alors quels contrôles sont actifs.

> 💡 Pour un plateau de jeu, **bride** la caméra : `enablePan={false}`, `maxPolarAngle={Math.PI / 2.2}` (on ne passe pas sous la table), `minDistance` / `maxDistance` (pas de zoom extrême). Un joueur ne doit pas pouvoir se perdre dans la scène.

### Éclairage et décor
| Composant | Rôle |
|---|---|
| `Environment` | Éclairage et reflets à partir d'une image HDR à 360°. `preset` : `apartment`, `city`, `dawn`, `forest`, `lobby`, `night`, `park`, `studio`, `sunset`, `warehouse`. `background` l'affiche aussi en fond. |
| `ContactShadows` | Ombre douce au sol, calculée par une caméra d'en dessous. Aucune *shadow map* à régler. |
| `AccumulativeShadows` | Ombres douces et réalistes, accumulées sur plusieurs images. Pour des scènes statiques. |
| `Stage` | Studio tout fait : éclairage, ombre, centrage de l'objet. Pratique pour une vitrine de personnage. |
| `Sky`, `Stars`, `Cloud` | Ciel physique, champ d'étoiles, nuages. |
| `Sparkles` | Particules scintillantes : parfait pour une **aura**. |

**Pourquoi `Environment` change tout.** Un métal ne fait que refléter son environnement (voir [`LUMIERES_MATERIAUX`](LUMIERES_MATERIAUX.md)). Une image HDR donne à chaque matériau un vrai décor à refléter, et un éclairage venant de toutes les directions, avec les bonnes couleurs. C'est le plus gros gain visuel pour une ligne de code.

### Formes et matériaux
| Composant | Rôle |
|---|---|
| `RoundedBox` | Boîte aux coins arrondis. Une carte épaisse, en une ligne. |
| `Float` | Fait flotter et tanguer ses enfants. |
| `MeshDistortMaterial`, `MeshWobbleMaterial` | Matériaux qui ondulent. |
| `MeshTransmissionMaterial` | Verre réaliste, avec réfraction. Coûteux. |
| `Outlines`, `Edges` | Contour autour d'un mesh : mettre en surbrillance la carte sélectionnée. |
| `Billboard` | Ses enfants font **toujours face à la caméra** (un nom au-dessus d'un personnage). |
| `Center` | Centre ses enfants sur l'origine (un modèle exporté décalé). |

### Texte et HTML
| Composant | Rôle |
|---|---|
| `Text` | Texte plat net à toute distance (technique SDF). `fontSize`, `color`, `anchorX`, `anchorY`, `maxWidth` (retour à la ligne), `font="/fonts/x.woff"`. |
| `Text3D` | Texte **en relief** (extrudé), à partir d'une police au format JSON. |
| `Html` | Du vrai DOM accroché à un point 3D. `center`, `transform` (suit la perspective), `occlude` (caché derrière les objets), `distanceFactor` (taille selon la distance). |

> ⚠️ `Html` est du DOM **par-dessus** le canvas : il ne reçoit ni lumière ni ombre, et beaucoup d'éléments `Html` coûtent cher. Pour une barre de vie ou une bulle, c'est parfait. Pour le texte d'une carte, préfère `Text`.

### Chargement et ressources
| Hook | Rôle |
|---|---|
| `useGLTF(url)` | Charger un modèle `.glb` / `.gltf` (voir [`MODELES_GLTF`](MODELES_GLTF.md)). |
| `useAnimations(clips, ref)` | Jouer ses animations (voir [`SQUELETTE_ANIMATIONS`](SQUELETTE_ANIMATIONS.md)). |
| `useTexture(url)` | Charger une image comme texture. Accepte un tableau ou un objet `{ map, normalMap }`. |
| `useProgress` / `Loader` | Suivre et afficher le chargement. |

### Outils de développement
| Outil | Rôle |
|---|---|
| `Stats` | Compteur d'images par seconde dans un coin. |
| `useHelper(ref, Helper)` | Dessine un helper three sur un objet (le cône d'une `spotLight`, la boîte d'ombre d'une lumière). |
| `useCursor(survol)` | Passe le curseur en « main » pendant le survol. Remplace le `document.body.style.cursor` du labo. |

---

## 4. Travailler hors ligne : héberger ses fichiers

Dans le labo, `Environment preset="city"` télécharge `potsdamer_platz_1k.hdr` depuis un CDN (`raw.githack.com`), et `Text` sans `font` télécharge une police par défaut. Pour un jeu en production :
- un CDN tiers peut tomber, ou être bloqué par un pare-feu (à l'école, par exemple) ;
- le sujet de Transcendance exige que tout tourne depuis ton serveur, avec un seul `docker compose up`.

Mets les fichiers dans `public/`, et pointe dessus :

```tsx
import { Suspense } from "react";
import { Canvas } from "@react-three/fiber";
import { Environment, Text } from "@react-three/drei";

export default function SansCDN() {
    return (
        <Canvas>
            <Suspense fallback={null}>
                <Environment files="/hdr/potsdamer_platz_1k.hdr" />
                <Text font="/fonts/Inter-Bold.woff" fontSize={0.4} color="white">
                    Aura
                </Text>
            </Suspense>
        </Canvas>
    );
}
```

- Les HDR des `preset` viennent de [Poly Haven](https://polyhaven.com/hdris) (licence CC0, libre d'usage). Télécharge-les en **1k** : au-delà, le fichier pèse lourd pour un gain invisible.
- `Text` accepte les polices `.ttf`, `.otf` et `.woff`. Vérifie la licence de la police.

---

## 5. Inspecter une carte : `PresentationControls`

```tsx
import { Canvas } from "@react-three/fiber";
import { PresentationControls, RoundedBox, Environment } from "@react-three/drei";
import { Suspense } from "react";

export default function InspecterCarte() {
    return (
        <Canvas camera={{ position: [0, 0, 4], fov: 40 }}>
            <Suspense fallback={null}>
                <Environment preset="studio" />
            </Suspense>
            <PresentationControls
                global
                snap
                polar={[-0.4, 0.4]}
                azimuth={[-0.8, 0.8]}
            >
                <RoundedBox args={[1.4, 2, 0.03]} radius={0.02}>
                    <meshStandardMaterial color="#e8d8b0" roughness={0.4} />
                </RoundedBox>
            </PresentationControls>
        </Canvas>
    );
}
```

- `global` : on peut cliquer-glisser n'importe où dans le canvas, pas seulement sur la carte.
- `snap` : au relâchement, la carte **revient** en position initiale, avec un ressort.
- `polar` / `azimuth` : limites de rotation verticale et horizontale, en radians.

---

## 🧪 À essayer

1. Change le `preset` de l'`Environment` et observe les reflets du cube métallique changer complètement.
2. Ajoute `background` sur `<Environment>` pour voir l'image HDR en fond.
3. `<OrbitControls autoRotate />`, puis `enableZoom={false}`, puis `maxPolarAngle={Math.PI / 2}` : essaie de passer sous le sol.
4. Remplace la sphère qui ondule par `<Sparkles count={100} scale={4} size={3} />` (à importer de drei).
5. Coupe le Wi-Fi et recharge la leçon 6 : elle reste vide. Puis télécharge un HDR de Poly Haven dans `public/hdr/`, passe à `files="/hdr/…"`, et recommence.
6. Ajoute `useCursor` au cube de la leçon 4 à la place du `document.body.style.cursor` manuel.
