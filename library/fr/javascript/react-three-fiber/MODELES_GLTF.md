# 8. Charger des modèles glTF

> **TL;DR.** Les modèles 3D d'un jeu web se livrent en **glTF**, de préférence en `.glb` (un seul fichier binaire). On les place dans `public/`, on les charge avec `useGLTF("/models/x.glb")` (qui **suspend** : il faut un `<Suspense>`), et on les affiche avec `<primitive object={scene} />`. `useGLTF` met le résultat **en cache** : charger deux fois le même fichier renvoie **le même objet**, qui ne peut être affiché qu'à un seul endroit. Pour plusieurs exemplaires, il faut **cloner** (`<Clone>` de drei, qui gère aussi les personnages riggés). `gltfjsx` transforme un modèle en composant JSX éditable.

Liens : [`INDEX`](INDEX.md) · [`DREI`](DREI.md) · [`SQUELETTE_ANIMATIONS`](SQUELETTE_ANIMATIONS.md) · [`PERFORMANCE_DEBUG`](PERFORMANCE_DEBUG.md)

🧪 Labo : aucune leçon. Le labo n'a pas encore de modèle (pas de dossier `public/`). Les exemples de cette fiche sont vérifiés au typecheck, pas dans un navigateur.

---

## 1. Le format glTF

**glTF** (*GL Transmission Format*) est le « JPEG de la 3D » : un format ouvert, pensé pour être chargé vite par un moteur temps réel. Un fichier contient tout ce qu'il faut :

| Contenu | Ce que ça devient dans three |
|---|---|
| Meshes (géométries) | `Mesh` / `SkinnedMesh` |
| Matériaux PBR | `MeshStandardMaterial` (ou `MeshPhysicalMaterial`) |
| Textures | `Texture` intégrées |
| Hiérarchie de nœuds | `Group` / `Object3D` imbriqués |
| Squelette et poids de skinning | `Skeleton`, `Bone` |
| Animations | `AnimationClip` |
| Caméras, lumières | Optionnelles, souvent ignorées |

| Extension | Forme |
|---|---|
| `.gltf` | Un JSON lisible + un `.bin` + des images à côté. Pratique pour inspecter. |
| `.glb` | **Tout dans un seul fichier binaire.** C'est celui qu'on livre. |

Blender exporte en glTF nativement (*Fichier ▸ Exporter ▸ glTF 2.0*). Mixamo exporte en FBX : il faudra passer par Blender pour convertir (voir [`SQUELETTE_ANIMATIONS`](SQUELETTE_ANIMATIONS.md)).

> 💡 Pour inspecter un fichier sans écrire de code : glisse-le sur [gltf-viewer.donmccurdy.com](https://gltf-viewer.donmccurdy.com). Tu y vois les animations, le nombre de triangles et les éventuelles erreurs.

---

## 2. Où ranger les fichiers : `public/`

```text
mon-jeu/
├─ public/
│  └─ models/
│     ├─ arene.glb
│     └─ personnage.glb
└─ src/
```

Vite sert le contenu de `public/` **tel quel**, à la racine du site : `public/models/arene.glb` est accessible à l'URL `/models/arene.glb`. On écrit donc `useGLTF("/models/arene.glb")`, **sans** `public`.

🔗 C'est comme le dossier racine de ton serveur web (Webserv) : ce qui est dedans est servi directement, sans passer par le *bundler*.

---

## 3. Le chargement de base

```tsx
import { Suspense } from "react";
import { Canvas } from "@react-three/fiber";
import { useGLTF, OrbitControls, Environment } from "@react-three/drei";

function Arene() {
    const { scene } = useGLTF("/models/arene.glb");
    return <primitive object={scene} />;
}

useGLTF.preload("/models/arene.glb");

export default function Scene() {
    return (
        <Canvas camera={{ position: [0, 3, 8] }}>
            <Suspense fallback={null}>
                <Environment files="/hdr/studio_small_03_1k.hdr" />
                <Arene />
            </Suspense>
            <OrbitControls makeDefault />
        </Canvas>
    );
}
```

- **`useGLTF`** télécharge, décode, puis renvoie un objet `{ scene, nodes, materials, animations, … }`. Pendant le téléchargement, le composant **suspend** (voir [`DREI`](DREI.md), section 2).
- **`<primitive object={scene} />`** : un élément R3F spécial qui insère dans la scène **un objet three déjà construit**, au lieu d'en créer un. Toutes les props habituelles fonctionnent : `position`, `scale`, `rotation`.
- **`useGLTF.preload(url)`**, appelé au niveau du module, lance le téléchargement **dès que le fichier JS est importé**, avant même que le composant ne s'affiche. Le modèle est souvent prêt quand on en a besoin.

### Ce que renvoie `useGLTF`

| Champ | Contenu |
|---|---|
| `scene` | La racine du modèle, prête à être affichée. |
| `nodes` | Tous les objets **par nom** : `nodes.Epee`, `nodes.Corps`… |
| `materials` | Tous les matériaux par nom. |
| `animations` | Le tableau des `AnimationClip` (vide si le fichier n'en a pas). |

`nodes` et `materials` permettent d'aller chercher une pièce précise, par exemple pour changer la couleur d'un matériau selon l'équipe du joueur.

---

## 4. Le piège du cache : un objet, un seul endroit

`useGLTF` garde chaque fichier chargé en **cache**, indexé par son URL. Deux appels à `useGLTF("/models/carte.glb")` renvoient **exactement le même objet** `scene`, pas deux copies.

Or un objet three ne peut avoir **qu'un seul parent** (c'est un arbre, voir [`HIERARCHIE`](HIERARCHIE.md)). Ajouter le même objet à un deuxième endroit le **retire** du premier :

```text
<Carte position={[-2, 0, 0]} />   ← affiche scene
<Carte position={[ 2, 0, 0]} />   ← affiche… le même scene : il déménage ici
                                    → une seule carte visible, à droite
```

🔗 C'est un **pointeur partagé**, pas une copie : deux variables C qui pointent la même `struct`. Modifier l'une modifie « l'autre ».

### La solution : cloner

```tsx
import { useGLTF, Clone } from "@react-three/drei";

export function Carte({ position }: { position: [number, number, number] }) {
    const { scene } = useGLTF("/models/carte.glb");
    return <Clone object={scene} position={position} castShadow receiveShadow />;
}
```

- **`<Clone>`** (drei) fait une copie de la hiérarchie. Les **géométries et matériaux restent partagés** par défaut : c'est voulu, la mémoire du GPU n'est pas dupliquée. Passe `deep="materialsOnly"` si chaque exemplaire doit avoir sa propre couleur.
- **Personnages riggés** : `<Clone>` détecte les `SkinnedMesh` et utilise alors `SkeletonUtils.clone`, qui recâble chaque copie sur **son propre squelette**. Un `scene.clone()` simple ferait pointer toutes les copies vers le même squelette : elles bougeraient toutes ensemble.
- **`castShadow` / `receiveShadow`** sur `<Clone>` les applique à **tous** les meshes du modèle.

---

## 5. Parcourir le modèle : `traverse`

Sans `<Clone>`, pour activer les ombres sur tous les meshes d'un modèle, il faut les visiter un par un :

```tsx
import { useEffect } from "react";
import { useGLTF } from "@react-three/drei";
import { Mesh } from "three";

export function ArenePleineOmbre() {
    const { scene } = useGLTF("/models/arene.glb");

    useEffect(() => {
        scene.traverse((obj) => {
            if (obj instanceof Mesh) {
                obj.castShadow = true;
                obj.receiveShadow = true;
            }
        });
    }, [scene]);

    return <primitive object={scene} />;
}
```

- `traverse(fn)` appelle `fn` sur l'objet **et tous ses descendants**, en profondeur : un parcours d'arbre récursif, comme un `ft_lstiter` sur une hiérarchie.
- `obj instanceof Mesh` : les nœuds d'un glTF sont de types variés (`Group`, `Bone`, `Mesh`, `SkinnedMesh`). `SkinnedMesh` hérite de `Mesh`, donc le test les attrape aussi.
- Ça modifie **l'objet en cache** : tous les endroits qui l'utilisent sont touchés. Ici c'est voulu.

---

## 6. `gltfjsx` : le modèle devient un composant

[`gltfjsx`](https://github.com/pmndrs/gltfjsx) lit un `.glb` et **génère un composant JSX** qui reconstruit sa hiérarchie, mesh par mesh :

```sh
npx gltfjsx public/models/personnage.glb --types
```

Il produit un fichier qui ressemble à ceci (forme simplifiée) :

```text
export function Model(props) {
  const { nodes, materials } = useGLTF('/personnage.glb')
  return (
    <group {...props} dispose={null}>
      <mesh geometry={nodes.Epee.geometry} material={materials.Metal} />
      <mesh geometry={nodes.Bouclier.geometry} material={materials.Bois} />
    </group>
  )
}
```

Pourquoi c'est utile :
- **Chaque pièce devient du JSX** : tu peux cacher l'épée (`visible={false}`), ajouter un `onClick` sur le bouclier, remplacer un matériau, comme sur n'importe quel mesh R3F.
- **Pas de problème de cache** : chaque `<Model />` crée ses propres meshes à partir des géométries partagées, donc on peut en afficher plusieurs sans cloner.
- **`--types`** génère les types TypeScript des `nodes` et `materials` : `nodes.Epe` (faute de frappe) devient une erreur de compilation.
- **`--transform`** compresse le modèle (textures redimensionnées, géométrie compressée) et produit un **nouveau** `.glb`, souvent beaucoup plus léger. Lis la sortie de la commande pour connaître le nom exact du fichier généré, et pointe `useGLTF` dessus.
- Le chemin dans le `useGLTF` généré est à vérifier : corrige-le pour qu'il corresponde à ton `public/`.

> ⚠️ Le fichier généré est un **point de départ** : si tu ré-exportes le modèle depuis Blender, tu régénères le composant, et tes modifications sont écrasées. Garde tes ajouts dans un composant qui **enveloppe** le composant généré.

---

## 7. Compression : Draco et Meshopt

Un personnage détaillé pèse vite plusieurs Mo. Deux compressions de géométrie existent :

| Compression | Gain | Coût |
|---|---|---|
| **Draco** | Géométrie très compressée. | Décodage par un module WebAssembly à télécharger. |
| **Meshopt** | Un peu moins compressée, décodage très rapide. | Décodeur plus léger. |

`useGLTF` gère les deux **automatiquement** : si le fichier n'est pas compressé, rien ne se passe. Si le fichier est compressé en Draco, drei télécharge le décodeur depuis un **CDN de Google** (`www.gstatic.com/draco/…`).

Comme pour les HDR (voir [`DREI`](DREI.md), section 4), un jeu qui doit tourner sans dépendre d'Internet héberge le décodeur lui-même :

```tsx
import { useGLTF } from "@react-three/drei";

useGLTF.setDecoderPath("/draco/");

export function Personnage() {
    const { scene } = useGLTF("/models/personnage-draco.glb");
    return <primitive object={scene} />;
}
```

Les fichiers du décodeur sont fournis par three, dans `node_modules/three/examples/jsm/libs/draco/` : copie ce dossier dans `public/draco/`.

---

## 8. Vider le cache

`useGLTF.clear(url)` retire un modèle du cache. C'est rarement utile : on le fait quand un modèle ne servira plus du tout (on quitte un niveau). Il faut alors aussi libérer la mémoire GPU (voir [`PERFORMANCE_DEBUG`](PERFORMANCE_DEBUG.md)).

---

## 🧪 À essayer

1. Télécharge un modèle CC0 (par exemple sur [Poly Pizza](https://poly.pizza) ou [Kenney](https://kenney.nl/assets)), en `.glb`. Mets-le dans `public/models/` du labo et affiche-le avec la section 3.
2. Ouvre-le dans le visualiseur glTF (section 1) : combien a-t-il de triangles ? d'animations ?
3. Affiche-le **deux fois** avec `<primitive>` à deux positions : un seul apparaît. Passe à `<Clone>`.
4. Dans un `useEffect`, fais `console.log(Object.keys(nodes))` : ce sont les noms donnés dans Blender.
5. Lance `npx gltfjsx public/models/x.glb --types` et compare le fichier généré à ce que tu as vu dans `nodes`.
