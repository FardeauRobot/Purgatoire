# 2. Lumières et matériaux : pourquoi tout est noir, et comment faire des ombres

> **TL;DR.** Un matériau « éclairé » (`Lambert`, `Phong`, `Standard`) **sans lumière est noir**. `meshStandardMaterial` est le choix par défaut : `roughness` (rugosité) et `metalness` suffisent à décrire presque tout. Une scène lisible = une **lumière d'appoint** faible (`ambientLight`) + une **lumière principale** directionnelle. Les ombres demandent **trois** réglages : `<Canvas shadows>`, `castShadow` sur la lumière, `castShadow` / `receiveShadow` sur les objets.

Liens : [`INDEX`](INDEX.md) · [`ANATOMIE`](ANATOMIE.md) · [`ANIMATION`](ANIMATION.md) · [`DREI`](DREI.md) (Environment, ContactShadows)

🧪 Labo : `~/Labo/TestThreeFiber/src/lessons/L2_Lumieres.jsx`

---

## 1. Pourquoi c'est noir

Pour chaque pixel, le GPU calcule « combien de lumière arrive sur cette surface, et combien elle en renvoie vers la caméra ». S'il n'y a aucune lumière, la réponse est zéro : noir. `meshBasicMaterial` saute ce calcul et affiche sa couleur telle quelle. C'est pourquoi la leçon 1 marchait sans lumière, mais paraissait plate : le relief d'un objet, c'est **la différence d'éclairage** entre ses faces.

---

## 2. Les matériaux

| Matériau | Lumière ? | Coût | Usage |
|---|---|---|---|
| `meshBasicMaterial` | Ignore | Minimal | Interfaces, objets lumineux, debug. |
| `meshNormalMaterial` | Ignore | Minimal | Debug : couleur = orientation de la face. |
| `meshLambertMaterial` | Oui, mat | Faible | Surfaces mates, mobile. |
| `meshPhongMaterial` | Oui, reflets brillants (`shininess`) | Moyen | Plastique, look ancien. |
| `meshStandardMaterial` | Oui, **PBR** | Moyen | **Le choix par défaut.** |
| `meshPhysicalMaterial` | Oui, PBR étendu | Élevé | Verre, vernis (`clearcoat`, `transmission`). |
| `meshToonMaterial` | Oui, par paliers | Faible | Rendu « dessin animé ». Intéressant pour un jeu stylisé. |

**PBR** (*Physically Based Rendering*) : au lieu de régler « la brillance » à l'œil, on décrit la matière physique. Les mêmes valeurs rendent bien sous n'importe quel éclairage.

| Prop de `meshStandardMaterial` | Plage | Effet |
|---|---|---|
| `color` | Couleur | La couleur de base. |
| `roughness` | 0 → 1 | 0 = miroir net, 1 = complètement mat. Défaut 1. |
| `metalness` | 0 → 1 | 0 = plastique, bois, peau. 1 = métal. Rarement entre les deux. Défaut 0. |
| `emissive` + `emissiveIntensity` | Couleur, nombre | La surface **brille par elle-même**. Elle n'éclaire pas les autres objets. |
| `map` | Texture | Une image plaquée sur la surface : l'illustration d'une carte. |
| `normalMap` | Texture | Faux relief (rayures, gravures) sans ajouter de triangles. |
| `transparent` + `opacity` | Booléen, 0 → 1 | Transparence. Les deux sont nécessaires. |
| `side` | `FrontSide`, `BackSide`, `DoubleSide` | Quelles faces dessiner. Défaut : seulement l'avant. |

> ⚠️ Un métal (`metalness={1}`) **reflète son environnement**. Dans une scène vide, il n'a rien à refléter et paraît **noir ou gris terne**, même bien éclairé. C'est pour ça que les métaux de la leçon 6 sont beaux : `<Environment>` leur donne un décor à refléter (voir [`DREI`](DREI.md)).

---

## 3. Les lumières

| Lumière | Comportement | Ombres | Arguments clés |
|---|---|---|---|
| `ambientLight` | Partout, uniforme, sans direction | Non | `intensity` |
| `hemisphereLight` | Ciel en haut, sol en bas : un dégradé | Non | `args={[ciel, sol, intensité]}` |
| `directionalLight` | Rayons parallèles : le soleil | Oui | `position`, `intensity` |
| `pointLight` | Une ampoule, dans toutes les directions | Oui | `position`, `intensity`, `distance`, `decay` |
| `spotLight` | Un cône : projecteur de scène | Oui | `angle`, `penumbra` (bord flou), `distance`, `decay` |

### La directionnelle vise une cible
Une `directionalLight` éclaire **de sa `position` vers sa `target`**, par défaut l'origine `[0, 0, 0]`. Seule la direction compte : `position={[5, 5, 2]}` et `position={[50, 50, 20]}` donnent le même éclairage, car elles pointent dans la même direction vers l'origine.

### Pourquoi `intensity={10}` pour une ampoule ?
Depuis three r155, les lumières ponctuelles suivent la physique : `decay = 2` par défaut, c'est-à-dire que l'intensité diminue avec **le carré de la distance**. À 3 unités, il ne reste qu'un neuvième de la lumière. D'où les `intensity={10}` (L2) ou `intensity={60}` (L5) du labo, qui paraissent énormes à côté du `intensity={2}` de la directionnelle, qui, elle, ne faiblit pas avec la distance.

- `distance={0}` (défaut) : portée infinie, la décroissance seule fait le travail.
- `decay={1.5}` (soleil de L5) : décroissance plus douce, pour éclairer des planètes lointaines sans aveugler les proches.

### La recette d'un éclairage lisible
1. Une **lumière principale** (*key light*) : directionnelle, intensité 1.5 à 3, venant d'en haut et de côté. Elle crée le relief.
2. Une **lumière d'appoint** faible : `ambientLight` de 0.2 à 0.4, pour que les faces à l'ombre ne soient pas totalement noires.
3. Optionnel : une lumière de **contour** (*rim light*) derrière l'objet, qui détache sa silhouette du fond.

Une ambiante trop forte **aplatit tout** : quand toutes les faces reçoivent la même lumière, il n'y a plus de relief.

---

## 4. Les ombres, en trois réglages

```tsx
import { Canvas } from "@react-three/fiber";

export default function Ombres() {
    return (
        <Canvas shadows="percentage" camera={{ position: [6, 5, 6] }}>
            <ambientLight intensity={0.2} />
            <directionalLight
                position={[5, 5, 2]}
                intensity={2}
                castShadow
                shadow-mapSize={[1024, 1024]}
                shadow-camera-left={-6}
                shadow-camera-right={6}
                shadow-camera-top={6}
                shadow-camera-bottom={-6}
                shadow-bias={-0.0005}
            />

            <mesh position={[-2, 0.6, 0]} castShadow>
                <sphereGeometry args={[0.6, 64, 32]} />
                <meshLambertMaterial color="white" />
            </mesh>
            <mesh position={[0, 0.6, 0]} castShadow>
                <sphereGeometry args={[0.6, 64, 32]} />
                <meshPhongMaterial color="white" shininess={100} />
            </mesh>
            <mesh position={[2, 0.6, 0]} castShadow>
                <sphereGeometry args={[0.6, 64, 32]} />
                <meshStandardMaterial color="white" roughness={0.2} metalness={0.5} />
            </mesh>

            <mesh rotation={[-Math.PI / 2, 0, 0]} receiveShadow>
                <planeGeometry args={[10, 10]} />
                <meshStandardMaterial color="#556" />
            </mesh>
        </Canvas>
    );
}
```

| Réglage | Rôle |
|---|---|
| `<Canvas shadows>` | Active le calcul des ombres dans le renderer. Sans ça, rien. |
| `castShadow` sur la lumière | Cette lumière calcule une ombre. Chaque lumière qui projette coûte un rendu supplémentaire de la scène. |
| `castShadow` sur un objet | Il **projette** une ombre. |
| `receiveShadow` sur un objet | Il **reçoit** les ombres des autres (le sol). |

Un objet peut faire les deux : une carte posée sur une autre reçoit l'ombre de la main et projette la sienne sur la table.

Le sol est un `planeGeometry`, **vertical par défaut** (dans le plan XY, face à la caméra). `rotation={[-Math.PI / 2, 0, 0]}` le couche de -90° autour de X. Avec `+Math.PI / 2`, il serait couché face vers le bas, donc invisible d'en haut.

### Le message `PCFSoftShadowMap has been removed`
Avec `<Canvas shadows>` (booléen, comme dans le labo), R3F 9.8 choisit le type `PCFSoftShadowMap`. Or three 0.187 l'a supprimé. Il affiche donc dans la console :

```text
THREE.WebGLShadowMap: PCFSoftShadowMap has been removed. Using PCFShadowMap instead.
```

Ce n'est pas une erreur : three bascule tout seul sur `PCFShadowMap`. Pour un console propre, demande ce type directement avec `shadows="percentage"`, comme ci-dessus. Les autres valeurs : `"basic"` (dur, pixelisé), `"variance"` (VSM, très doux, mais avec parfois des fuites de lumière).

### Comment une ombre est calculée, et pourquoi elle rate
La lumière fait un **rendu de la scène depuis son propre point de vue**, dans une texture (la *shadow map*). Chaque pixel de cette texture retient la distance du premier objet touché. Au rendu final, un point plus loin que cette distance est dans l'ombre.

| Symptôme | Cause | Remède |
|---|---|---|
| Ombre coupée net, ou absente au bord de la scène | L'ombre d'une directionnelle n'est calculée que dans une boîte de **-5 à 5** unités de côté, centrée sur l'axe lumière → cible (par défaut). | Agrandir avec `shadow-camera-left/right/top/bottom`. |
| Ombre floue et pixelisée | La *shadow map* fait **512 × 512** par défaut, étalée sur toute la boîte. | `shadow-mapSize={[1024, 1024]}` ou plus, ou une boîte plus petite. |
| Rayures ou moirés sur les surfaces (*shadow acne*) | L'objet se fait de l'ombre à lui-même par erreur d'arrondi. | `shadow-bias={-0.0005}` (petite valeur négative). |
| L'ombre se détache de l'objet (*peter panning*) | `bias` trop fort. | Réduire `bias`. |

Les props à tiret (`shadow-mapSize`, `shadow-camera-left`) atteignent les sous-objets : `light.shadow.mapSize`, `light.shadow.camera.left`. C'est la règle des tirets vue dans [`ANATOMIE`](ANATOMIE.md).

> 💡 Pour une ombre douce au sol sous des cartes ou un personnage, **`<ContactShadows>`** de drei est souvent plus simple et plus joli qu'une vraie *shadow map* (voir [`DREI`](DREI.md)).

---

## 5. `emissive` : briller sans éclairer

Le soleil de la leçon 5 combine deux choses :

```text
<meshStandardMaterial color="gold" emissive="orange" emissiveIntensity={1.5} />   ← il a l'air lumineux
<pointLight intensity={60} decay={1.5} />                                          ← il éclaire les planètes
```

`emissive` rend **le soleil lui-même** lumineux, mais n'envoie aucune lumière aux planètes. La `pointLight` placée au même endroit s'en charge. Pour une carte rare qui « rayonne » son aura, c'est la même recette : `emissive` sur la carte, et éventuellement une `pointLight` colorée juste devant.

---

## 🧪 À essayer

1. Supprime toutes les lumières : tout devient noir, sauf ce qui est en `meshBasicMaterial`.
2. Passe `ambientLight` à `intensity={1}` : le relief et les ombres s'aplatissent.
3. Sur la sphère Standard, fais varier `roughness` de 0 à 1, puis `metalness` de 0 à 1. Avec `metalness={1}` et `roughness={0}`, la sphère devient sombre : sans `Environment`, elle n'a rien à refléter.
4. Remplace la directionnelle par `<spotLight position={[0, 4, 0]} angle={0.4} penumbra={0.5} intensity={30} castShadow />`. Joue avec `penumbra`.
5. Ouvre la console : repère l'avertissement `PCFSoftShadowMap`, puis écris `shadows="percentage"` et vérifie qu'il disparaît.
6. Réduis la boîte d'ombre : `shadow-camera-left={-1}`, `right={1}`, `top={1}`, `bottom={-1}`. Seule l'ombre de la sphère centrale survit, coupée net : c'est le symptôme d'une boîte trop petite. Puis remets ±6 et baisse `shadow-mapSize` à `[256, 256]` pour voir l'ombre se pixeliser.
7. Remplace un matériau par `meshToonMaterial` : rendu à paliers, façon dessin animé.
