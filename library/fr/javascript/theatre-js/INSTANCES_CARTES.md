# 7. Atelier : l'effet « carte jouée », une instance par carte

> **TL;DR.** On veut un effet spectaculaire, réglé à l'œil dans le studio, joué par **n'importe quelle** carte, éventuellement par plusieurs en même temps. Une feuille Theatre n'a qu'une tête de lecture, mais `project.sheet("Carte jouée", id)` crée une **instance** : même animation, tête de lecture à part. Chaque carte prend son instance, lance `sequence.play()` quand l'état dit qu'elle est jouée, et **ajoute** les valeurs Theatre (un saut, un tour, un éclat) par-dessus la position que calcule `damp`. La séquence finit à zéro, donc le relais est invisible.

Liens : [`INDEX`](INDEX.md) · [`SEQUENCES`](SEQUENCES.md) · [`CORE_A_LA_MAIN`](CORE_A_LA_MAIN.md) · [`../react-three-fiber/ATELIER_CARTES`](../react-three-fiber/ATELIER_CARTES.md) · [`../react-three-fiber/ETAT_RESEAU`](../react-three-fiber/ETAT_RESEAU.md)

🧪 Labo : aucun pour l'instant. Le module ci-dessous tient en un fichier, plus `./theatre`.

---

## 1. Le problème : une tête de lecture par feuille

Dans [`../react-three-fiber/ATELIER_CARTES`](../react-three-fiber/ATELIER_CARTES.md), une carte jouée **glisse** vers le plateau avec `damp`. C'est juste, mais sans éclat. On voudrait qu'en plus elle **bondisse**, fasse un tour sur elle-même et brille un instant : une petite chorégraphie qu'on règle dans la timeline.

Premier réflexe : une feuille `"Carte jouée"` et un `play()`. Mais si deux cartes sont jouées à 0.3 s d'écart (un combo), le second `play()` **interrompt** le premier ([`SEQUENCES`](SEQUENCES.md), §2) : la première carte se fige au milieu de son saut.

La solution, c'est le second argument de `project.sheet` :

```text
project.sheet("Carte jouée")           ← la feuille « modèle », que tu édites dans le studio
project.sheet("Carte jouée", "c7")     ┐
project.sheet("Carte jouée", "c12")    ┘ des instances : mêmes keyframes, têtes de lecture à part
```

Vérifié en testant ces fiches : deux instances de la même feuille, l'une placée à 0.5 s et l'autre à 2 s, donnent bien deux valeurs différentes (0.318 et 3), avec les **mêmes** keyframes.

🔗 **Lien C++.** La feuille, c'est la **classe** : ses keyframes sont partagées, comme les méthodes. L'instance, c'est l'**objet** : chacune a son propre état, ici sa position de lecture. `project.sheet("Carte jouée", id)`, c'est le `new` de Theatre.

---

## 2. Le partage des rôles

| Qui | Calcule quoi | Pourquoi lui |
|---|---|---|
| **`damp`** (ATELIER_CARTES) | la position **de base** : dans la main, puis sur le plateau | elle dépend de l'état et peut changer à tout moment |
| **Theatre** | un **décalage** ajouté par-dessus : saut, tour, éclat | c'est de la mise en scène, écrite d'avance |

```text
                position affichée = base (damp)   +   décalage (Theatre)
 t = 0 (jouée)       main                              0
 t = 0.3 s           à mi-chemin                       saut de 1.2, demi-tour
 t = 0.8 s           presque au plateau                0   ← la séquence se termine à zéro
 après               plateau                           0   ← damp seul, sans à-coup
```

La règle qui rend le relais invisible : **la séquence commence et finit à zéro** sur toutes ses pistes. Au début et à la fin, Theatre n'ajoute rien, donc rien ne saute.

🔗 **Lien C.** C'est une addition de deux signaux, comme un son et son écho. Tant que l'écho vaut 0 aux deux bouts, on ne l'entend ni commencer ni finir.

---

## 3. Le module

```tsx
import { useEffect, useMemo, useRef } from "react";
import { useFrame } from "@react-three/fiber";
import { types } from "@theatre/core";
import { MathUtils, type Group, type MeshStandardMaterial } from "three";
import { project } from "./theatre";

const PROPS_EFFET = {
    saut: types.number(0, { range: [0, 3] }),
    tour: types.number(0, { nudgeMultiplier: 0.05 }),
    eclat: types.number(0, { range: [0, 4] }),
};

const VITESSE = 8;

type Props = {
    id: string;
    jouee: boolean;
    cible: [number, number, number];
};

export function CarteJouable({ id, jouee, cible }: Props) {
    const groupe = useRef<Group>(null);
    const face = useRef<MeshStandardMaterial>(null);

    const effet = useMemo(() => {
        const sheet = project.sheet("Carte jouée", id);
        return { sheet, obj: sheet.object("Effet", PROPS_EFFET) };
    }, [id]);

    useEffect(() => {
        if (!jouee) return;
        const seq = effet.sheet.sequence;
        seq.position = 0;
        void project.ready.then(() => seq.play());
        return () => seq.pause();
    }, [jouee, effet]);

    useFrame((_, dt) => {
        const g = groupe.current;
        if (!g) return;
        const base = g.userData as { x?: number; y?: number; z?: number };
        base.x = MathUtils.damp(base.x ?? cible[0], cible[0], VITESSE, dt);
        base.y = MathUtils.damp(base.y ?? cible[1], cible[1], VITESSE, dt);
        base.z = MathUtils.damp(base.z ?? cible[2], cible[2], VITESSE, dt);

        const v = effet.obj.value;
        g.position.set(base.x, base.y + v.saut, base.z);
        g.rotation.y = v.tour;
        if (face.current) face.current.emissiveIntensity = v.eclat;
    });

    return (
        <group ref={groupe}>
            <mesh>
                <boxGeometry args={[1.4, 2, 0.02]} />
                <meshStandardMaterial ref={face} color="ivory" emissive="gold" emissiveIntensity={0} />
            </mesh>
        </group>
    );
}
```

---

## 4. Démontage

### L'instance, une fois par carte

```text
const effet = useMemo(() => { … project.sheet("Carte jouée", id) … }, [id]);
```

- `useMemo` : on ne recrée l'instance que si `id` change. Même si React rappelle le corps deux fois (`StrictMode`), ce n'est pas grave : `project.sheet` avec le même couple (id, instance) renvoie la même feuille, et `sheet.object` avec la même clé et les mêmes props renvoie le même objet (vérifié).
- `PROPS_EFFET` est déclaré **hors** du composant, pour la même raison qu'en [`EDITABLE`](EDITABLE.md), §3 : un objet de props neuf à chaque rendu, c'est une reconfiguration à chaque rendu.
- L'`id` de la carte sert d'**identifiant d'instance**. Il doit être stable : c'est le même `key={c.id}` qu'en ATELIER_CARTES, pour la même raison.

### Jouer quand l'état change

- Le `useEffect` dépend de `jouee` : il se déclenche quand la carte **devient** jouée, pas à chaque rendu.
- `seq.position = 0` puis `play()` : on rejoue toujours depuis le début, même si une lecture précédente s'était arrêtée au milieu.
- `project.ready.then(…)` : un `play()` avant `ready` ne fait rien et renvoie quand même `true` ([`SEQUENCES`](SEQUENCES.md), §1). Une carte jouée au tout premier rendu tomberait dans ce piège.
- Le `return () => seq.pause()` : si la carte disparaît pendant son effet (fin de partie), on arrête sa lecture.

### Lire Theatre dans `useFrame`

- On lit `effet.obj.value` dans `useFrame`, et non `onValuesChange` : la carte bouge de toute façon à chaque image à cause de `damp`, et il faut **additionner** les deux. C'est le cas d'usage de la deuxième colonne du tableau de [`CORE_A_LA_MAIN`](CORE_A_LA_MAIN.md), §5.
- La position de base est rangée dans `g.userData`, un objet libre que three.js met à disposition sur chaque `Object3D`. On ne peut pas la relire dans `g.position`, puisqu'on y écrit `base + saut`. Si on dampait `g.position.y` directement, le saut serait « avalé » par `damp` à l'image suivante.
- `?? cible[0]` : à la toute première image, il n'y a pas encore de base, on part donc directement de la cible.

### Régler l'effet dans le studio

Les keyframes appartiennent à la **feuille**, pas à l'instance : en éditer une, c'est les éditer toutes. Sélectionne dans l'outline n'importe quelle instance de `Carte jouée` (la façon exacte dont le studio les liste n'a pas été vérifiée), puis dans sa timeline :

1. **Sequence** sur `saut`, `tour` et `eclat`.
2. À 0 s : `saut 0`, `tour 0`, `eclat 0`.
3. À 0.3 s : `saut 1.2`, `tour 3.14`, `eclat 3`. Courbe `Quad Out` sur le segment.
4. À 0.8 s : tout à `0`, et `tour` à `6.28` (un tour complet, qui revient visuellement à 0). Courbe `Back Out` sur `saut`, pour un petit rebond à l'arrivée.
5. **Sequence length** : `0.8`.

🚨 **`tour` finit à 6.28, pas à 0.** C'est voulu : de π à 0, la carte reviendrait en arrière. À 2π, elle finit son tour, et elle se retrouve dans la même orientation qu'au départ. Mais ce n'est **pas** zéro : si la carte doit ensuite être retournée par `damp` (rotation π pour le dos), additionne plutôt `v.tour` à une rotation de base, comme pour la position.

---

## 5. Brancher sur le jeu

Dans Transcendance, `jouee` et `cible` viennent du store, lui-même rempli par le serveur ([`../react-three-fiber/ETAT_RESEAU`](../react-three-fiber/ETAT_RESEAU.md)) :

```text
const jouee = useJeu((s) => s.plateau.includes(id));
```

- Le serveur confirme le coup → l'état change → `jouee` passe à `true` → l'effet joue. Le composant ne sait rien du réseau.
- Un abonnement React est ici correct : `jouee` change une fois par partie et par carte, pas 60 fois par seconde.
- Un adversaire qui joue une carte déclenche **le même effet** sur son instance, avec le même code. C'est tout l'intérêt de dériver l'animation de l'état.

---

## 6. Limites

- Les instances ne se suppriment pas : l'API 0.7 n'a pas de `project.removeSheet`. Une instance par carte d'une partie (quelques dizaines) ne pose aucun problème. Une instance par **particule**, si.
- Les keyframes sont communes à toutes les instances. Pour une variante (une carte légendaire plus spectaculaire), fais une **autre feuille** (`"Carte légendaire jouée"`), pas une autre instance.
- Pour jouer l'effet un peu différemment selon la carte sans le redessiner, joue sur les options de `play()` : `rate: 0.8` pour une carte lourde, par exemple.

---

## 🧪 À essayer

1. Affiche trois `CarteJouable` et un bouton qui les passe à `jouee` à 0.2 s d'écart. Les trois effets doivent se dérouler en entier, en décalé.
2. Remplace `project.sheet("Carte jouée", id)` par `project.sheet("Carte jouée")` (sans instance) et refais l'essai 1. Que deviennent les deux premières cartes ?
3. Dampe directement `g.position.y` vers `cible[1]` puis ajoute `v.saut`, au lieu de passer par `userData`. Le saut est-il toujours visible ? Pourquoi ?
4. Mets `saut` à `0.5` sur la dernière keyframe au lieu de `0`. Observe la fin de l'effet : à quoi ressemble le relais ?
5. Passe une prop `legendaire` et utilise `rate: 0.6` pour elle. Puis crée une seconde feuille pour une vraie variante et compare les deux approches.
