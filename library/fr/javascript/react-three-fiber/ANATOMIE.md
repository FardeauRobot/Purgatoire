# 1. Anatomie d'une scène : `<Canvas>`, mesh, géométrie, matériau

> **TL;DR.** `<Canvas>` crée le renderer, la scène, une caméra perspective et la boucle de rendu. Chaque balise en minuscules est une **classe three.js** : `<boxGeometry>` donne `new THREE.BoxGeometry()`. Un **mesh**, c'est une **géométrie** (la forme) plus un **matériau** (l'apparence). `args` est la liste des arguments **du constructeur, dans l'ordre**. Les rotations sont en **radians**.

Liens : [`INDEX`](INDEX.md) · [`INSTALLATION`](INSTALLATION.md) · [`LUMIERES_MATERIAUX`](LUMIERES_MATERIAUX.md) · [doc three : BufferGeometry](https://threejs.org/docs/#api/en/core/BufferGeometry)

🧪 Labo : `~/Labo/TestThreeFiber/src/lessons/L1_Anatomie.jsx`

---

## 1. Ce que `<Canvas>` fait à ta place

En three.js « nu », une scène minimale demande une vingtaine de lignes : créer un `WebGLRenderer`, l'accrocher au DOM, créer une `Scene`, une `PerspectiveCamera`, écouter le redimensionnement de la fenêtre, et écrire une boucle `requestAnimationFrame`. `<Canvas>` fait tout ça, avec ces valeurs par défaut (lues dans le code source de R3F 9.8) :

| Élément | Valeur par défaut | Prop pour la changer |
|---|---|---|
| Caméra | `PerspectiveCamera`, `fov` 75°, `near` 0.1, `far` 1000 | `camera={{ fov: 50, position: [0, 2, 5] }}` |
| Position caméra | `[0, 0, 5]`, tournée vers l'origine `[0, 0, 0]` | `camera={{ position: […] }}` |
| Densité de pixels | `dpr={[1, 2]}` : suit l'écran, plafonné à 2 | `dpr={1}` pour économiser le GPU |
| Couleurs | Sortie en sRGB, *tone mapping* ACESFilmic | `flat` (pas de tone mapping), `linear` (pas de sRGB) |
| Ombres | Désactivées | `shadows` (voir [`LUMIERES_MATERIAUX`](LUMIERES_MATERIAUX.md)) |
| Boucle | `frameloop="always"` : on redessine en continu | `"demand"` (voir [`PERFORMANCE_DEBUG`](PERFORMANCE_DEBUG.md)) |
| Caméra orthographique | Non | `orthographic` (pas de perspective : vue « plan ») |

Autres props utiles : `onCreated={(state) => …}` (une fois le renderer prêt), `onPointerMissed` (clic dans le vide, voir [`INTERACTION`](INTERACTION.md)), `gl={{ antialias: false }}` (options du renderer).

> 💡 Le *tone mapping* ACESFilmic adoucit les couleurs très lumineuses comme le ferait une caméra de cinéma. C'est pourquoi un `color="red"` pur ne sort pas exactement `#ff0000` à l'écran. Pour une interface plate où la couleur exacte compte (une carte au design précis), essaie `<Canvas flat>`.

---

## 2. Le repère : où est quoi

three.js utilise un repère **main droite**, **Y vers le haut** :

```text
        Y (vert, haut)
        │
        │
        └──────── X (rouge, droite)
       ╱
      Z (bleu, vers toi)
```

- Le `<axesHelper args={[3]} />` du labo dessine ces trois axes, longs de 3 unités, avec ces couleurs.
- Les **unités n'ont pas de sens physique**. Choisis une convention et tiens-la dans toute l'équipe : « 1 unité = 1 mètre » est la plus courante, et c'est celle de Blender et Mixamo. Un personnage mesure alors environ 1.8.
- La caméra par défaut est en `z = 5` et regarde vers les `z` négatifs. Un objet en `z = -3` est donc **plus loin**, un objet en `z = 6` est **derrière** la caméra, donc invisible.

---

## 3. Un mesh = une géométrie + un matériau

```tsx
import { Canvas } from "@react-three/fiber";

export default function Anatomie() {
    return (
        <Canvas camera={{ position: [3, 2, 5] }}>
            <mesh>
                <boxGeometry args={[1, 0.5, 1]} />
                <meshBasicMaterial color="orange" />
            </mesh>

            <mesh position={[-2, 0, 0]} rotation={[0, Math.PI / 4, 0]}>
                <sphereGeometry args={[0.6, 32, 16]} />
                <meshBasicMaterial color="hotpink" wireframe />
            </mesh>

            <mesh position={[2, 0, 0]} scale={[1, 2, 1]}>
                <torusKnotGeometry args={[0.4, 0.15, 100, 16]} />
                <meshBasicMaterial color="royalblue" />
            </mesh>

            <axesHelper args={[3]} />
            <gridHelper args={[10, 10, "#444", "#222"]} />
        </Canvas>
    );
}
```

- La **géométrie** est une liste de sommets et de triangles. Elle ne sait rien des couleurs.
- Le **matériau** dit comment colorier ces triangles. `meshBasicMaterial` est une couleur plate qui **ignore la lumière** : pratique pour débuter, mais tout paraît plat (une sphère ressemble à un disque).
- `wireframe` n'affiche que les arêtes des triangles. C'est le meilleur moyen de **voir** ce que produit une géométrie.
- Les enfants `<boxGeometry>` et `<meshBasicMaterial>` sont rattachés automatiquement à `mesh.geometry` et `mesh.material`. On verra la règle exacte (`attach`) en section 6.

🔗 **Lien C.** Une géométrie, c'est un tableau de sommets (`float[]`) et un tableau d'indices de triangles (`uint16_t[]`), envoyés une fois à la carte graphique. Exactement ce que tu ferais avec la MiniLibX, mais en 3D et calculé par le GPU.

---

## 4. `args` : les arguments du constructeur, **dans l'ordre**

`args` est passé tel quel au constructeur : `<sphereGeometry args={[a, b, c]} />` exécute `new THREE.SphereGeometry(a, b, c)`. Les noms n'apparaissent nulle part. Seule la **position** dans le tableau compte, comme les paramètres d'une fonction C.

| Géométrie | `args` (dans l'ordre) |
|---|---|
| `boxGeometry` | largeur, hauteur, profondeur, segments… |
| `sphereGeometry` | rayon, `widthSegments` (autour), `heightSegments` (de haut en bas), `phiStart`, `phiLength`, `thetaStart`, `thetaLength` |
| `planeGeometry` | largeur, hauteur, segments… |
| `circleGeometry` | rayon, segments |
| `cylinderGeometry` | rayon du haut, rayon du bas, hauteur, segments radiaux |
| `coneGeometry` | rayon, hauteur, segments radiaux |
| `capsuleGeometry` | rayon, longueur, segments des calottes, segments radiaux |
| `torusGeometry` | rayon de l'anneau, rayon du tube, segments radiaux, segments tubulaires |
| `torusKnotGeometry` | rayon, rayon du tube, segments tubulaires, segments radiaux, `p`, `q` |
| `ringGeometry` | rayon intérieur, rayon extérieur, segments |
| `icosahedronGeometry` | rayon, niveau de subdivision (`0` = 20 faces, plus = plus rond) |

### Cas pratique : la sphère ajoutée dans ton labo

Dans `L1_Anatomie.jsx`, tu as ajouté une sphère avec les arguments d'un nœud torique :

```jsx
<sphereGeometry args={[0.4, 0.15, 100, 16]} />
```

three.js lit ça comme `new SphereGeometry(rayon = 0.4, widthSegments = 0.15, heightSegments = 100, phiStart = 16)`. Le constructeur corrige les valeurs impossibles : `widthSegments = Math.max(3, Math.floor(0.15))`, donc **3**. Résultat :
- **3 segments autour** : la « sphère » a une section triangulaire, comme un fuseau à trois faces ;
- **100 segments de haut en bas** : très lisse dans un sens, anguleuse dans l'autre ;
- `phiStart = 16` **radians** : le point de départ du tour est décalé. Comme le tour reste complet, la forme est seulement tournée sur elle-même.

Rien ne plante et aucun avertissement ne s'affiche : JavaScript accepte n'importe quel nombre. **C'est le piège de `args`.** Quand une forme a l'air bizarre, ouvre la doc three.js de la géométrie et relis l'ordre des paramètres. Pour une vraie sphère lisse : `args={[0.4, 32, 16]}`.

> ⚠️ Changer `args` **reconstruit** l'objet (on ne peut pas rappeler un constructeur). Changer une autre prop appelle seulement un setter, ce qui coûte bien moins cher. N'écris donc pas un `args` qui change à chaque rendu, par exemple `args={[Math.random(), 1, 1]}`.

---

## 5. Position, rotation, échelle

| Prop | Valeur | Équivalent three.js |
|---|---|---|
| `position={[x, y, z]}` | Unités | `mesh.position.set(x, y, z)` |
| `rotation={[x, y, z]}` | **Radians**, appliqués dans l'ordre X puis Y puis Z | `mesh.rotation.set(x, y, z)` |
| `scale={[x, y, z]}` ou `scale={2}` | Facteur (1 = taille normale) | `mesh.scale.set(…)` |
| `position-x={1}` | Un seul axe | `mesh.position.x = 1` |
| `rotation-y={Math.PI}` | Un seul axe | `mesh.rotation.y = Math.PI` |
| `visible={false}` | Booléen | `mesh.visible = false` |

Conversion degrés ↔ radians : `radians = degrés * Math.PI / 180`. three.js fournit `MathUtils.degToRad(45)`.

| Degrés | Radians |
|---|---|
| 45° | `Math.PI / 4` |
| 90° | `Math.PI / 2` |
| 180° | `Math.PI` |
| 360° | `Math.PI * 2` |

> ⚠️ `rotation={[0, 45, 0]}` ne fait pas 45° : ça fait 45 **radians**, soit plus de sept tours. Le résultat a l'air aléatoire.

L'échelle `scale={[1, 2, 1]}` du nœud torique du labo l'étire **deux fois en hauteur**, sans toucher à la géométrie elle-même : c'est l'objet qui est étiré, pas sa forme d'origine.

---

## 6. La correspondance JSX ↔ three.js, en détail

R3F applique trois règles à chaque balise :

1. **Le nom** : `<truc>` donne `new THREE.Truc(...args)`. Ça marche pour toute classe de `three` : `<pointLight>`, `<group>`, `<fog>`, `<color>`.
2. **Les props** : si la propriété a une méthode `set` (vecteurs, couleurs), R3F l'appelle avec ta valeur. Sinon, il assigne directement.
3. **`attach`** : où rattacher l'objet dans son parent. Par défaut, une géométrie va dans `parent.geometry`, un matériau dans `parent.material`, et tout le reste est ajouté comme enfant (`parent.add(…)`).

Avec `attach`, on peut rattacher n'importe quoi à n'importe quelle propriété. Par exemple, la couleur de fond et le brouillard de la scène :

```tsx
import { Canvas } from "@react-three/fiber";

export default function FondEtBrouillard() {
    return (
        <Canvas>
            <color attach="background" args={["#101018"]} />
            <fog attach="fog" args={["#101018", 4, 12]} />
            <mesh position={[0, 0, -6]}>
                <boxGeometry />
                <meshNormalMaterial />
            </mesh>
        </Canvas>
    );
}
```

- `<color attach="background" args={["#101018"]} />` exécute `scene.background = new THREE.Color("#101018")`.
- `<fog … args={[couleur, début, fin]} />` : les objets s'estompent dans la couleur entre 4 et 12 unités de la caméra. Mets la même couleur que le fond, sinon le brouillard se voit comme un voile.

### Les classes hors catalogue : `extend`

R3F connaît d'office tout ce qui est exporté par `three`. Les **addons** (`three/examples/jsm/…`) ne le sont pas : il faut les déclarer avec `extend`, et en TypeScript, ajouter leur type.

```tsx
import { Canvas, extend, type ThreeElement } from "@react-three/fiber";
import { RoundedBoxGeometry } from "three/examples/jsm/geometries/RoundedBoxGeometry.js";

extend({ RoundedBoxGeometry });

declare module "@react-three/fiber" {
    interface ThreeElements {
        roundedBoxGeometry: ThreeElement<typeof RoundedBoxGeometry>;
    }
}

export default function BoiteArrondie() {
    return (
        <Canvas>
            <mesh>
                <roundedBoxGeometry args={[1, 1.4, 0.05, 4, 0.08]} />
                <meshNormalMaterial />
            </mesh>
        </Canvas>
    );
}
```

En pratique, drei fournit déjà la plupart de ces addons sous forme de composants (`<RoundedBox>` dans la leçon 6). Vérifie drei avant d'écrire un `extend`.

---

## 7. Les *helpers* : voir l'invisible

| Helper | `args` | Ce qu'il montre |
|---|---|---|
| `axesHelper` | longueur | Les axes X, Y, Z en rouge, vert, bleu. |
| `gridHelper` | taille, divisions, couleur des axes, couleur de la grille | Une grille au sol (plan XZ). |
| `<mesh>` + `wireframe` | — | Les triangles d'une géométrie. |

drei ajoute `useHelper`, qui dessine par exemple le cône d'une lumière (voir [`DREI`](DREI.md)). Garde les helpers pendant le développement et retire-les ensuite.

---

## 🧪 À essayer

1. Passe `args` de la boîte à `[2, 0.5, 1]` : elle s'allonge sur X uniquement.
2. Remplace la sphère « fuseau » par `args={[0.4, 32, 16]}`, puis active `wireframe` sur les deux versions pour comparer les triangles.
3. Retire `wireframe` de la sphère rose : en `meshBasicMaterial`, elle ressemble à un disque plat. C'est l'absence de lumière, sujet de la fiche suivante.
4. Mets la caméra en `position: [0, 0, 0.5]` : tu es presque dans la boîte. Puis `[0, 10, 0]` : vue de dessus.
5. Ajoute `<color attach="background" args={["#202030"]} />` en premier enfant du `<Canvas>`.
6. Écris `rotation={[0, 45, 0]}` sur une boîte, puis corrige avec `Math.PI / 4` et compare.
