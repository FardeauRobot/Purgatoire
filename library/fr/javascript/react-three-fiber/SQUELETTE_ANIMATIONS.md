# 9. Squelettes, skinning et animations de personnages

> **TL;DR.** Un personnage riggé, c'est un **mesh** (la peau) déformé par un **squelette** (une hiérarchie d'os). Une animation (`AnimationClip`) ne contient que des **courbes d'os, désignées par leur nom** : elle ne connaît pas le mesh. Conséquence directe pour Transcendance : si tous les personnages partagent **le même squelette, avec les mêmes noms d'os**, une seule bibliothèque d'animations mocap sert à **tous** les personnages. Dans R3F, `useAnimations(clips, ref)` de drei crée le mixeur, le fait avancer à chaque image et te donne les `actions` par nom. On passe de l'une à l'autre avec `fadeOut` / `reset().fadeIn().play()`.

Liens : [`INDEX`](INDEX.md) · [`MODELES_GLTF`](MODELES_GLTF.md) · [`HIERARCHIE`](HIERARCHIE.md) · [`ETAT_RESEAU`](ETAT_RESEAU.md) · [`ATELIER_CARTES`](ATELIER_CARTES.md)

🧪 Labo : aucune leçon. Exemples vérifiés au typecheck et dans le code source de drei, pas dans un navigateur.

---

## 1. Les quatre pièces

```text
SkinnedMesh "Corps"           ← la peau : des sommets
 │   chaque sommet a jusqu'à 4 os qui le tirent, avec un poids chacun
 └─ Skeleton
     └─ Bone "Hips"            ← l'os racine
         ├─ Bone "Spine"
         │   └─ Bone "Neck" ─ Bone "Head"
         ├─ Bone "LeftUpLeg" ─ Bone "LeftLeg" ─ …
         └─ …

AnimationClip "Attaque"        ← une liste de pistes, chacune sur UN os, par son NOM :
   "Hips.position"      [t0, t1, …] → [x, y, z, …]
   "Spine.quaternion"   [t0, t1, …] → [rotations…]
   …
```

| Pièce | Rôle |
|---|---|
| `Bone` | Un `Object3D` comme les autres : il a une position et une rotation, **relatives à son os parent**. C'est la hiérarchie de [`HIERARCHIE`](HIERARCHIE.md), appliquée à un corps. |
| `Skeleton` | La liste ordonnée des os qui déforment un mesh. |
| `SkinnedMesh` | Un mesh dont chaque sommet suit 1 à 4 os, selon des **poids** (*skin weights*). Un sommet du coude suit à moitié le bras, à moitié l'avant-bras : c'est ce qui donne une déformation douce. |
| `AnimationClip` | Des pistes (*tracks*) de valeurs dans le temps, **une par propriété d'os**, désignée par le nom de l'os. |

🔗 **Lien C.** Une piste est un tableau de couples (temps, valeur), et l'animation, c'est une **interpolation** entre ces points clés à l'instant courant : comme un `lerp` de [`ANIMATION`](ANIMATION.md), fait pour chaque os, à chaque image.

---

## 2. Pourquoi les noms d'os sont la clé

Quand on joue un clip sur un personnage, three **cherche chaque os par son nom** dans la hiérarchie du personnage. C'est tout. Le mesh, la taille du personnage, ses vêtements n'entrent pas en jeu.

Ce qui en découle pour ton jeu :

| Situation | Résultat |
|---|---|
| Personnage A et B ont le **même squelette** (mêmes noms, mêmes proportions de base), meshes différents | **Les mêmes clips marchent sur les deux.** C'est ton cas idéal : un squelette commun, N peaux. |
| Mêmes noms d'os, proportions différentes (un géant, un nain) | Les clips marchent, mais les pieds peuvent glisser ou flotter, les mains peuvent traverser le corps. |
| Noms d'os différents (`mixamorig:Hips` contre `Hips`) | **Rien ne bouge**, sans la moindre erreur : three ne trouve pas les os, et ignore les pistes. |

### Le piège Mixamo : `mixamorig:Hips` devient `mixamorigHips`

three **nettoie** les noms de nœuds au chargement (`PropertyBinding.sanitizeNodeName`) : les espaces deviennent `_`, et les caractères `[ ] . : /` sont **supprimés**. Le `:` des squelettes Mixamo disparaît donc : l'os s'appelle `mixamorigHips` dans three.

C'est sans conséquence tant que le personnage **et** les animations viennent du même pipeline. Mais si tu cherches un os à la main, cherche `mixamorigHips`, pas `mixamorig:Hips`.

---

## 3. Lister les os d'un modèle (à garder sous le coude)

Le premier réflexe quand une animation ne bouge pas : **comparer les noms**.

```tsx
import { useEffect } from "react";
import { useGLTF } from "@react-three/drei";
import { Bone, SkinnedMesh } from "three";

export function InspecteurDeSquelette({ url }: { url: string }) {
    const { scene, animations } = useGLTF(url);

    useEffect(() => {
        const os: string[] = [];
        scene.traverse((obj) => {
            if (obj instanceof Bone) os.push(obj.name);
            if (obj instanceof SkinnedMesh) console.log("peau :", obj.name, "—", obj.skeleton.bones.length, "os");
        });
        console.log("os :", os);
        for (const clip of animations) {
            const cibles = new Set(clip.tracks.map((t) => t.name.split(".")[0]));
            const orphelines = [...cibles].filter((n) => !os.includes(n));
            console.log(`clip "${clip.name}" (${clip.duration.toFixed(2)} s) — pistes sans os :`, orphelines);
        }
    }, [scene, animations]);

    return <primitive object={scene} />;
}
```

- Une piste s'appelle `nomDeLOs.propriété` : `split(".")[0]` en extrait le nom de l'os.
- **« Pistes sans os »** non vide = ce clip vise des os qui n'existent pas dans ce modèle. Ces parties du corps ne bougeront pas.
- Pour **voir** le squelette, ajoute dans la scène `<primitive object={new SkeletonHelper(scene)} />` (import `SkeletonHelper` depuis `three`) : des traits colorés relient les os.

---

## 4. Jouer une animation : `useAnimations`

```tsx
import { useEffect, useRef } from "react";
import { useGLTF, useAnimations } from "@react-three/drei";
import type { Group } from "three";

export function Combattant() {
    const groupe = useRef<Group>(null);
    const { scene, animations } = useGLTF("/models/combattant.glb");
    const { actions, names } = useAnimations(animations, groupe);

    useEffect(() => {
        console.log("clips disponibles :", names);
        actions["Idle"]?.play();
    }, [actions, names]);

    return (
        <group ref={groupe}>
            <primitive object={scene} />
        </group>
    );
}
```

Ce que fait `useAnimations` (lu dans son code source) :
- il crée **un** `AnimationMixer` par composant : le moteur qui mélange les clips et écrit les rotations dans les os ;
- il appelle `mixer.update(delta)` dans un `useFrame` : **tu n'as rien à faire** pour que le temps avance ;
- `actions` est un objet **paresseux** : `actions["Idle"]` crée l'`AnimationAction` au premier accès. Ce n'est possible qu'une fois la ref attachée, donc dans un `useEffect`, jamais pendant le rendu ;
- au démontage (ou si les clips changent), il arrête toutes les actions et vide son cache.

Pourquoi `actions["Idle"]?.play()` avec un `?.` : le type d'une action est `AnimationAction | null`, et un nom de clip mal orthographié renvoie `undefined`. Le `?.` évite un plantage, mais **cache l'erreur** : d'où le `console.log(names)` pour vérifier les noms exacts.

### Le vocabulaire d'une `AnimationAction`

| Méthode / champ | Effet |
|---|---|
| `play()` | Démarre (ou reprend) l'action. |
| `stop()` | Arrête net. |
| `reset()` | Revient au début, poids et vitesse remis à 1. À appeler avant de rejouer un coup. |
| `fadeIn(s)` / `fadeOut(s)` | Fait monter / descendre le **poids** de l'action en `s` secondes. |
| `crossFadeTo(autre, s)` | Fondu direct de cette action vers une autre. |
| `setLoop(LoopOnce, 1)` | Joue une seule fois (par défaut : `LoopRepeat`, en boucle). |
| `clampWhenFinished = true` | Avec `LoopOnce` : reste figé sur la dernière pose au lieu de revenir à la pose de repos. |
| `timeScale` | Vitesse : `2` = deux fois plus vite, `-1` = à l'envers. |
| `weight` | Influence de 0 à 1. Plusieurs actions de poids non nul se **mélangent**. |

---

## 5. Changer d'animation avec un fondu

Passer d'`Idle` à `Attaque` d'un coup fait « sauter » le personnage d'une pose à l'autre. Un **fondu** de 0,2 à 0,4 s mélange les deux poses pendant la transition :

```tsx
import { useEffect, useRef } from "react";
import { useGLTF, useAnimations } from "@react-three/drei";
import type { Group } from "three";

type Etat = "Idle" | "Attaque" | "Aura" | "Coup" | "Victoire";

export function CombattantAnime({ etat, position }: { etat: Etat; position: [number, number, number] }) {
    const groupe = useRef<Group>(null);
    const { scene, animations } = useGLTF("/models/combattant.glb");
    const { actions } = useAnimations(animations, groupe);

    useEffect(() => {
        const action = actions[etat];
        if (!action) return;
        action.reset().fadeIn(0.3).play();
        return () => {
            action.fadeOut(0.3);
        };
    }, [etat, actions]);

    return (
        <group ref={groupe} position={position}>
            <primitive object={scene} />
        </group>
    );
}
```

Le mécanisme est élégant, et c'est la **fonction de nettoyage** de `useEffect` qui fait tout :
1. `etat` passe de `"Idle"` à `"Attaque"`.
2. React exécute d'abord le nettoyage de l'effet **précédent** : `Idle.fadeOut(0.3)`.
3. Puis le nouvel effet : `Attaque.reset().fadeIn(0.3).play()`.
4. Pendant 0,3 s, les deux actions ont un poids intermédiaire : le mixeur les mélange.

L'animation est pilotée **par une prop** : le composant parent décide de l'état (souvent à partir d'un message du serveur, voir [`ETAT_RESEAU`](ETAT_RESEAU.md)), et le personnage se contente de l'afficher. C'est exactement le rôle d'un composant React.

---

## 6. Un coup joué une fois, puis retour au repos

Une attaque ne boucle pas : on la joue une fois, puis on revient à `Idle`. Le mixeur émet un événement `finished` quand une action `LoopOnce` se termine :

```tsx
import { useEffect, useRef } from "react";
import { useGLTF, useAnimations } from "@react-three/drei";
import { LoopOnce, type Group, type AnimationMixerEventMap } from "three";

export function CombattantQuiFrappe({ frappe, onFini }: { frappe: number; onFini: () => void }) {
    const groupe = useRef<Group>(null);
    const { scene, animations } = useGLTF("/models/combattant.glb");
    const { actions, mixer } = useAnimations(animations, groupe);

    useEffect(() => {
        actions["Idle"]?.play();
    }, [actions]);

    useEffect(() => {
        const attaque = actions["Attaque"];
        const repos = actions["Idle"];
        if (frappe === 0 || !attaque || !repos) return;

        attaque.reset();
        attaque.setLoop(LoopOnce, 1);
        attaque.clampWhenFinished = true;
        repos.crossFadeTo(attaque, 0.2, false);
        attaque.play();

        const fin = (e: AnimationMixerEventMap["finished"]) => {
            if (e.action !== attaque) return;
            repos.reset().play();
            attaque.crossFadeTo(repos, 0.3, false);
            onFini();
        };
        mixer.addEventListener("finished", fin);
        return () => mixer.removeEventListener("finished", fin);
    }, [frappe, actions, mixer, onFini]);

    return (
        <group ref={groupe}>
            <primitive object={scene} />
        </group>
    );
}
```

- **`frappe` est un compteur**, pas un booléen : chaque coup l'incrémente, ce qui relance l'effet même si on frappe deux fois de suite. Avec un booléen `true` → `true`, React ne verrait aucun changement.
- `e.action !== attaque` : le mixeur prévient pour **toutes** ses actions. On ne réagit qu'à la nôtre.
- `removeEventListener` dans le nettoyage : sans ça, chaque coup **ajouterait** un écouteur de plus, et le 5ᵉ coup appellerait `onFini` 5 fois. C'est la fuite classique, l'équivalent d'un `malloc` sans `free`.
- `onFini` en dépendance : le parent doit la stabiliser avec `useCallback`, sinon l'effet repart à chaque rendu du parent.

---

## 7. Les animations dans un fichier séparé

Avec un squelette commun, la bonne organisation est : **un fichier par personnage (sans animations)**, et **un fichier d'animations partagé** (squelette seul, ou un personnage de référence).

```text
public/models/
├─ anims.glb          ← Idle, Attaque, Aura, Coup, Victoire… (mocap)
├─ guerrier.glb       ← mesh + squelette commun, pas d'animation
└─ mage.glb           ← autre mesh, même squelette
```

```tsx
import { useEffect, useMemo, useRef } from "react";
import { useGLTF, useAnimations } from "@react-three/drei";
import { clone } from "three/examples/jsm/utils/SkeletonUtils.js";
import type { Group } from "three";

const ANIMS = "/models/anims.glb";

export function Personnage({ modele, clip }: { modele: string; clip: string }) {
    const groupe = useRef<Group>(null);
    const { scene } = useGLTF(modele);
    const { animations } = useGLTF(ANIMS);

    const copie = useMemo(() => clone(scene), [scene]);
    const { actions } = useAnimations(animations, groupe);

    useEffect(() => {
        const action = actions[clip];
        action?.reset().fadeIn(0.25).play();
        return () => {
            action?.fadeOut(0.25);
        };
    }, [clip, actions]);

    return (
        <group ref={groupe}>
            <primitive object={copie} />
        </group>
    );
}

useGLTF.preload(ANIMS);
```

- **Les clips viennent de `anims.glb`, le squelette de `modele`** : ça marche parce que les noms d'os correspondent (section 2).
- **`clone(scene)` de `SkeletonUtils`** : indispensable dès qu'un même modèle apparaît deux fois (deux joueurs qui choisissent le même personnage). Un `scene.clone()` ordinaire copie les meshes mais les laisse attachés au squelette **d'origine** : les deux copies feraient les mêmes gestes. `SkeletonUtils.clone` recâble chaque copie sur ses propres os. C'est aussi ce que fait `<Clone>` de drei (voir [`MODELES_GLTF`](MODELES_GLTF.md)).
- **`useMemo`** : le clonage ne se fait qu'une fois par modèle, pas à chaque rendu.
- **Un mixeur par personnage** : chaque `Personnage` appelle `useAnimations`, donc deux personnages jouent des clips différents en même temps sans se gêner. Les **clips**, eux, sont partagés : c'est de la donnée en lecture seule.

---

## 8. Le *root motion* : le personnage qui s'en va

Une animation capturée en mocap contient souvent une **translation de l'os racine** (`Hips.position`) : le comédien a avancé de deux pas en frappant. Sur ton plateau, le personnage **quitte sa case**, puis se téléporte en arrière quand la boucle recommence.

Deux solutions :
1. **Dans Blender** (propre) : supprimer ou figer les clés de position X/Z de l'os racine avant l'export.
2. **Au chargement** (rapide) : retirer la piste de position horizontale.

```tsx
import { useMemo } from "react";
import { useGLTF } from "@react-three/drei";
import type { AnimationClip } from "three";

function sansDeplacement(clip: AnimationClip): AnimationClip {
    const copie = clip.clone();
    copie.tracks = copie.tracks.filter((t) => !t.name.endsWith("Hips.position"));
    return copie;
}

export function useClipsSurPlace(url: string) {
    const { animations } = useGLTF(url);
    return useMemo(() => animations.map(sansDeplacement), [animations]);
}
```

- `endsWith("Hips.position")` attrape `Hips.position` comme `mixamorigHips.position`. Adapte au nom de **ton** os racine (affiché par la section 3).
- Retirer la piste entière retire aussi le mouvement **vertical** du bassin (le personnage qui s'accroupit ou saute). Si ça gêne, il faut plutôt réécrire les valeurs X et Z de la piste à zéro, en gardant Y : c'est plus simple dans Blender.
- `clip.clone()` : on ne modifie **jamais** les clips du cache de `useGLTF`, partagés par tous les composants.
- Le `useMemo` est **indispensable** : `useAnimations` refait tout son travail quand le tableau de clips change. Sans `useMemo`, on lui donnerait un nouveau tableau à chaque rendu.

---

## 9. Accrocher un objet à un os

Une épée dans la main, un effet d'aura qui suit la poitrine : l'objet doit être **enfant de l'os**, pour hériter de ses mouvements (voir [`HIERARCHIE`](HIERARCHIE.md)). `createPortal` de R3F affiche du JSX **à l'intérieur d'un objet three existant** :

```tsx
import { useMemo } from "react";
import { createPortal } from "@react-three/fiber";
import { Sparkles } from "@react-three/drei";
import type { Object3D } from "three";

export function AuraSurOs({ racine, nomOs }: { racine: Object3D; nomOs: string }) {
    const os = useMemo(() => racine.getObjectByName(nomOs), [racine, nomOs]);
    if (!os) return null;

    return createPortal(
        <Sparkles count={40} scale={1.2} size={4} color="violet" />,
        os,
    );
}
```

- `getObjectByName("mixamorigSpine2")` cherche l'os dans la hiérarchie du **personnage cloné** (`copie` de la section 7), pas dans l'original en cache.
- Les `Sparkles` deviennent des enfants de l'os : ils suivent chaque mouvement de la poitrine, sans aucun calcul.
- Attention à l'**échelle** : les modèles Mixamo sont souvent exportés avec une échelle de 0,01 sur un nœud parent. Un objet accroché à un os hérite de cette échelle, et peut paraître 100 fois trop petit.

---

## 10. Le pipeline mocap vidéo → jeu

```text
 vidéo d'un comédien
      │  outil de mocap vidéo (service en ligne ou logiciel)
      ▼
 fichier BVH ou FBX : un squelette « source » + ses courbes
      │  Blender : *retargeting* vers TON squelette commun
      │  (faire correspondre os source → os cible, corriger les pieds)
      ▼
 actions Blender, une par mouvement, bien nommées : Idle, Attaque…
      │  nettoyage : root motion (section 8), boucle Idle propre,
      │  pieds qui ne glissent pas, jitter lissé
      ▼
 export glTF 2.0 : squelette + toutes les actions  →  anims.glb
```

Points d'attention :
- **Le squelette commun doit être figé tôt.** Renommer un os après coup casse toutes les animations déjà exportées.
- **Nomme les actions dans Blender** comme tu veux les appeler en code : le nom de l'action devient `clip.name`, donc la clé de `actions[...]`.
- **La mocap vidéo est bruitée** : de petits tremblements, des pieds qui flottent. Prévois une passe de lissage (filtres de courbes dans Blender) avant l'export.
- **Pose de repos identique** (T-pose ou A-pose) entre le squelette des personnages et celui des animations : sinon le retargeting tord les membres.
- three sait aussi recibler au chargement (`retargetClip` dans `SkeletonUtils`), mais c'est plus fragile à régler qu'un retargeting fait une fois pour toutes dans Blender.

Pour le module « Advanced 3D graphics » du sujet, un personnage riggé qui enchaîne des animations avec fondus, cloné pour plusieurs joueurs et piloté par l'état du jeu, est une démonstration solide.

---

## 🧪 À essayer

1. Télécharge un personnage riggé avec animations (Mixamo, en FBX, puis converti en `.glb` dans Blender ; ou un modèle CC0 animé déjà en `.glb`). Passe-le à `InspecteurDeSquelette` et lis la console.
2. Joue `Idle` avec la section 4, puis ajoute un `useControls` (Leva) avec une liste des `names` pour changer d'état à la main. Branche-le sur `CombattantAnime` et regarde les fondus.
3. Change la durée de fondu : 0 (saut brutal), 0,3, 1,5 (personnage « mou »).
4. Affiche le même personnage deux fois, d'abord avec `scene.clone()` puis avec `clone(scene)` de `SkeletonUtils`, avec deux clips différents.
5. Ajoute `<primitive object={new SkeletonHelper(copie)} />` pour voir les os bouger.
6. Accroche une petite sphère à l'os de la main droite avec `createPortal`.
