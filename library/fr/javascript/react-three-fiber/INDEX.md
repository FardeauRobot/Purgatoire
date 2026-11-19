# 🧊 React Three Fiber : la 3D en composants React

> Rédigé directement en français, comme le reste de [`javascript/`](../INDEX.md).

**three.js** dessine de la 3D dans un `<canvas>` avec WebGL. **React Three Fiber** (R3F) te laisse écrire cette scène en **JSX** : `<mesh>` crée un `THREE.Mesh`, chaque prop appelle un setter, et le démontage d'un composant libère ses ressources GPU. **drei** ajoute des composants tout faits (caméra à la souris, textes, modèles), **Leva** un panneau de réglages en direct.

Ce dossier suit ton labo **`~/Labo/TestThreeFiber`** leçon par leçon, puis va plus loin vers ce dont le jeu de cartes de Transcendance a besoin : modèles glTF, personnages animés par squelette, état synchronisé par WebSocket.

Liens : [`../INDEX`](../INDEX.md) · [`../VERS_TRANSCENDANCE`](../VERS_TRANSCENDANCE.md) · [sujet ft_transcendence](../../projects/LEVEL6/ft_transcendence-subject.md) · [doc R3F](https://r3f.docs.pmnd.rs) · [doc drei](https://drei.docs.pmnd.rs) · [doc three.js](https://threejs.org/docs/)

---

## 🧭 Ordre de lecture

| Étape | Fiche | Labo | Pourquoi maintenant |
|---|---|---|---|
| 0 | [`INSTALLATION`](INSTALLATION.md) | `package.json`, `vite.config.js`, `styles.css` | Créer le projet, comprendre chaque fichier, éviter le canvas écrasé. |
| 1 | [`ANATOMIE`](ANATOMIE.md) | `L1_Anatomie.jsx` | `<Canvas>`, mesh = géométrie + matériau, `args`, coordonnées et radians. |
| 2 | [`LUMIERES_MATERIAUX`](LUMIERES_MATERIAUX.md) | `L2_Lumieres.jsx` | Pourquoi tout est noir, quel matériau choisir, les ombres en trois étapes. |
| 3 | [`ANIMATION`](ANIMATION.md) | `L3_Animation.jsx` | `useFrame` + `useRef`, `delta`, et la règle « jamais de `setState` par image ». |
| 4 | [`INTERACTION`](INTERACTION.md) | `L4_Interaction.jsx` | Clics et survol par *raycasting*, `stopPropagation`, état React. |
| 5 | [`HIERARCHIE`](HIERARCHIE.md) | `L5_Hierarchie.jsx` | `<group>`, repères locaux, orbites sans trigonométrie. |
| 6 | [`DREI`](DREI.md) | `L6_Drei.jsx` | La boîte à outils : contrôles, environnement, texte, HTML, `Suspense`. |
| 7 | [`LEVA`](LEVA.md) | `L7_Leva.jsx` | Régler les valeurs en direct avant de les écrire en dur. |
| 8 | [`MODELES_GLTF`](MODELES_GLTF.md) | — | Charger un `.glb`, `gltfjsx`, compression, ombres sur un modèle. |
| 9 | [`SQUELETTE_ANIMATIONS`](SQUELETTE_ANIMATIONS.md) | — | Os, *skinning*, clips, fondus, un squelette partagé, le clonage. |
| 10 | [`ETAT_RESEAU`](ETAT_RESEAU.md) | — | Store zustand, WebSocket, « le serveur décide, la scène affiche ». |
| 11 | [`ATELIER_CARTES`](ATELIER_CARTES.md) | — | Tout assembler : une main de cartes en éventail, survol, retournement. |
| 12 | [`PERFORMANCE_DEBUG`](PERFORMANCE_DEBUG.md) | — | Mesurer, `frameloop`, instanciation, les erreurs fréquentes. |

> 🌟 Si tu n'as le temps que pour trois fiches : **ANATOMIE**, **ANIMATION**, **SQUELETTE_ANIMATIONS**.

---

## 🧪 Le labo et ces fiches

- Les fichiers du labo sont en **JavaScript** (`.jsx`). Les extraits des fiches sont en **TypeScript** (`.tsx`), comme Transcendance. C'est le même code, avec les annotations de type en plus : retire `: Mesh`, `<Group>` et `as const`, et tu retombes sur le labo.
- Chaque fiche se termine par **« 🧪 À essayer »**. Ces exercices reprennent les sections *À ESSAYER* du labo et disent ce que tu dois observer.
- Le labo n'est pas dans le vault : ses chemins sont donnés en texte (`~/Labo/TestThreeFiber/src/lessons/…`), pas en liens.

---

## ✅ Ce qui a été vérifié

Versions vérifiées sur npm le **18/11/2026** : `@react-three/fiber` 9.8.2, `@react-three/drei` 10.7.9, `three` 0.187.0, `react` 19.3.1, `leva` 0.10.1, `zustand` 5.1.0.

- Chaque bloc ` ```tsx ` est un module complet. Tous ont été **vérifiés au typecheck** avec ces versions : `tsc --strict`, plus les options du gabarit Vite `react-ts` (`noUnusedLocals`, `noUnusedParameters`, `verbatimModuleSyntax`, `erasableSyntaxOnly`).
- Les valeurs par défaut citées (caméra, `dpr`, ombres, tone mapping, Draco) ont été **lues dans le code source** des paquets installés.
- Aucun extrait n'a été lancé dans un navigateur depuis ces fiches. Les fiches 1 à 7 suivent les leçons du labo : lance-les pour voir le résultat. Les fiches 8 à 12 n'ont pas de leçon dans le labo ; l'atelier de cartes (fiche 11) est prévu pour en devenir une.
- Non vérifié : le nom exact du fichier produit par `gltfjsx --transform` (la fiche te dit de lire la sortie de la commande).
- Si la doc officielle contredit une fiche, c'est la doc qui a raison. Corrige la fiche.

---

## ⚖️ three.js ↔ R3F en un coup d'œil

| three.js (impératif) | R3F (déclaratif) |
|---|---|
| `new THREE.WebGLRenderer()`, `new THREE.Scene()`, caméra, boucle | `<Canvas>` |
| `new THREE.Mesh(geo, mat); scene.add(mesh)` | `<mesh><boxGeometry /><meshStandardMaterial /></mesh>` |
| `new THREE.BoxGeometry(1, 2, 3)` | `<boxGeometry args={[1, 2, 3]} />` |
| `mesh.position.set(1, 0, 0)` | `<mesh position={[1, 0, 0]} />` |
| `requestAnimationFrame(loop)` | `useFrame((state, delta) => …)` |
| `raycaster.intersectObjects(…)` | `<mesh onClick={…} />` |
| `geometry.dispose()` à la main | Automatique au démontage |
| `scene.add(objetDejaConstruit)` | `<primitive object={objetDejaConstruit} />` |

🔗 **Lien C++.** La libération au démontage, c'est le **RAII** : la ressource vit aussi longtemps que l'objet qui la possède, ici le composant.

---

## 🚨 Les pièges, toutes fiches confondues

1. **Canvas écrasé** : un parent sans hauteur. Il faut `height: 100%` sur toute la chaîne `html > body > #root`. → [`INSTALLATION`](INSTALLATION.md)
2. **`args` est positionnel** : `[0.4, 0.15, 100, 16]` sur une sphère ne veut pas dire ce que tu crois. → [`ANATOMIE`](ANATOMIE.md)
3. **Rotations en radians** : `Math.PI` = 180°, pas `180`. → [`ANATOMIE`](ANATOMIE.md)
4. **Tout est noir** : un matériau éclairé sans lumière. → [`LUMIERES_MATERIAUX`](LUMIERES_MATERIAUX.md)
5. **Pas d'ombre** : il manque un des trois réglages (`shadows`, `castShadow`, `receiveShadow`). → [`LUMIERES_MATERIAUX`](LUMIERES_MATERIAUX.md)
6. **`setState` dans `useFrame`** : 60 re-rendus par seconde. Mute une ref. → [`ANIMATION`](ANIMATION.md)
7. **`useFrame` / `useThree` hors de `<Canvas>`** : erreur immédiate. → [`ANIMATION`](ANIMATION.md)
8. **Clic qui traverse tout** : oubli de `e.stopPropagation()`. → [`INTERACTION`](INTERACTION.md)
9. **Écran vide pendant un chargement** : pas de `<Suspense>`, ou un CDN injoignable hors ligne. → [`DREI`](DREI.md)
10. **Même modèle affiché deux fois, un seul visible** : le cache de `useGLTF`. Clone. → [`MODELES_GLTF`](MODELES_GLTF.md)
11. **Personnage figé en T** : les noms d'os ne correspondent pas. → [`SQUELETTE_ANIMATIONS`](SQUELETTE_ANIMATIONS.md)
12. **La scène décide de la partie** : triche possible. Le serveur décide. → [`ETAT_RESEAU`](ETAT_RESEAU.md)
13. **`frameloop="demand"` dans l'arène** : personnages figés. → [`PERFORMANCE_DEBUG`](PERFORMANCE_DEBUG.md)
14. **Monter React sans R3F, ou l'inverse** : R3F 9.8 exige `react >=19 <19.4`. → [`INSTALLATION`](INSTALLATION.md)
