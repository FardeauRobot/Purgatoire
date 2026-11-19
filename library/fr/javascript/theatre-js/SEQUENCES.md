# 5. Jouer une séquence depuis le code

> **TL;DR.** `sheet.sequence.play()` lance la timeline et renvoie une **promesse** : `true` si elle est allée au bout, `false` si un `pause()` ou un autre `play()` l'a interrompue. Options : `iterationCount` (`Infinity` pour boucler), `range: [début, fin]` en secondes, `rate` (vitesse), `direction` (`"normal"`, `"reverse"`, `"alternate"`…). `sheet.sequence.position` lit **et écrit** la tête de lecture, en secondes. Écrire `position` à chaque image, c'est piloter l'animation par autre chose que le temps : le **scroll**, une barre de progression, l'état du jeu.

Liens : [`INDEX`](INDEX.md) · [`STUDIO`](STUDIO.md) · [`INSTANCES_CARTES`](INSTANCES_CARTES.md) · [`../ASYNCHRONE`](../ASYNCHRONE.md) · [`../react-three-fiber/DREI`](../react-three-fiber/DREI.md)

🧪 Labo : aucun pour l'instant.

---

## 1. `play()` et sa promesse

```ts
import { project } from "./theatre";

const intro = project.sheet("Intro");

export async function lancerIntro(): Promise<void> {
    await project.ready;
    const finie = await intro.sequence.play();
    if (finie) console.log("intro terminée, la partie commence");
}
```

- `await project.ready` : le projet charge son état (le `state.json` en production, le `localStorage` en dev) de façon asynchrone. 🚨 Un `play()` lancé avant **ne joue rien et renvoie quand même `true`** (lu dans le code du core, puis observé) : ta suite démarre comme si l'intro avait été vue. Attends toujours `ready` avant la première lecture.
- `play()` renvoie `Promise<boolean>`. Elle se résout **à la fin** de la lecture, pas au démarrage.
- `finie === false` quand la lecture a été coupée : par `intro.sequence.pause()`, ou par un nouveau `play()` sur la même séquence (les deux observés). Utile pour ne pas enchaîner la suite d'une intro que le joueur a passée.
- `play()` part de la **position actuelle** de la tête. Si elle est déjà à la fin, la lecture repart de 0 (observé).

🔗 **Lien C.** C'est le `waitpid` de la timeline : tu lances, tu continues ta vie, et la promesse te prévient de la fin. Le booléen joue le rôle du statut de sortie : fini normalement, ou interrompu.

---

## 2. Les options de `play()`

| Option | Exemple | Effet |
|---|---|---|
| `iterationCount` | `2`, `Infinity` | Nombre de passages. Avec `Infinity`, la promesse ne se résout jamais d'elle-même. |
| `range` | `[1, 2.5]` | Ne joue que cet intervalle, en secondes. |
| `rate` | `0.5`, `2` | Vitesse : `0.5` deux fois plus lent, `2` deux fois plus rapide. |
| `direction` | `"normal"`, `"reverse"`, `"alternate"`, `"alternateReverse"` | Sens de lecture. `"alternate"` fait un aller, puis un retour, puis un aller… (utile avec `iterationCount`). |

```ts
import { project } from "./theatre";

const halo = project.sheet("Halo");

export function pulser(): void {
    void halo.sequence.play({ iterationCount: Infinity, direction: "alternate", rate: 1.5 });
}

export function rejouerLaFin(): Promise<boolean> {
    return project.sheet("Intro").sequence.play({ range: [2, 3] });
}
```

- `void` devant `play(…)` : on ignore volontairement la promesse, et ça se voit. Sans lui, un linter strict signalerait une promesse flottante.
- Une séquence a **une seule** tête de lecture. Deux `play()` à la suite ne jouent pas deux fois en parallèle : le second remplace le premier (dont la promesse se résout à `false`). Pour jouer la même animation plusieurs fois en même temps, il faut des **instances** de feuille ([`INSTANCES_CARTES`](INSTANCES_CARTES.md)).

---

## 3. La tête de lecture : `position`

```ts
import { project } from "./theatre";

const seq = project.sheet("Intro").sequence;

seq.pause();
seq.position = 1.2;
console.log(seq.position);
```

- `position` est en **secondes**. L'écrire déplace la tête : toutes les props de la feuille prennent leur valeur à cet instant, et les `onValuesChange` sont appelés.
- `pause()` arrête une lecture en cours sans revenir au début. Un `play()` ensuite reprend là où elle s'était arrêtée (observé : pause à 0.50 s, reprise à 0.50 s).

Pour **observer** la tête (afficher une barre de progression), on passe par son *pointer* :

```ts
import { onChange, val } from "@theatre/core";
import { project } from "./theatre";

const seq = project.sheet("Intro").sequence;

export function suivreProgression(afficher: (ratio: number) => void): () => void {
    return onChange(seq.pointer.position, (position) => {
        afficher(position / val(seq.pointer.length));
    });
}
```

- `seq.pointer.position` n'est **pas** un nombre : c'est un *pointer*, une adresse vers une valeur qui change. `onChange(pointer, cb)` appelle `cb` à chaque changement et renvoie la fonction de désabonnement (même motif que `onValuesChange`).
- `val(pointer)` lit la valeur actuelle d'un pointer. 🚨 `"durée : " + seq.pointer.length` ne donne pas la durée : ça lève `object is not a function` (observé). Passe toujours par `val()`.
- 🔗 **Lien C.** Un pointer, c'est littéralement un pointeur : `seq.pointer.length` est l'adresse, `val()` le déréférencement `*`.

---

## 4. Piloter par le scroll

Le cas classique d'une page d'accueil : la caméra avance quand on fait défiler. Ici on n'appelle jamais `play()` : on **écrit** `position` à chaque image, à partir du scroll.

```tsx
import { Canvas, useFrame } from "@react-three/fiber";
import { ScrollControls, useScroll } from "@react-three/drei";
import { val } from "@theatre/core";
import { editable as e, PerspectiveCamera, SheetProvider } from "@theatre/r3f";
import { project } from "./theatre";

const sheet = project.sheet("Accueil");

function SuivreScroll() {
    const scroll = useScroll();

    useFrame(() => {
        const duree = val(sheet.sequence.pointer.length);
        sheet.sequence.position = scroll.offset * duree;
    });

    return null;
}

export default function Accueil() {
    return (
        <Canvas>
            <ScrollControls pages={4}>
                <SheetProvider sheet={sheet}>
                    <SuivreScroll />
                    <PerspectiveCamera theatreKey="Caméra" makeDefault position={[0, 2, 10]} />
                    <ambientLight intensity={0.5} />
                    <e.mesh theatreKey="Logo">
                        <torusKnotGeometry args={[1, 0.3, 128, 16]} />
                        <meshNormalMaterial />
                    </e.mesh>
                </SheetProvider>
            </ScrollControls>
        </Canvas>
    );
}
```

Ligne par ligne :
- `<ScrollControls pages={4}>` (drei) : crée une zone qui défile sur 4 hauteurs d'écran par-dessus le canvas.
- `useScroll()` : donne `scroll.offset`, de `0` (tout en haut) à `1` (tout en bas). Il faut être **dans** `<ScrollControls>`.
- `scroll.offset * duree` : on convertit un ratio en secondes de timeline. En haut de la page, la tête est à 0 s ; en bas, à la fin.
- `SuivreScroll` ne rend rien (`return null`) : c'est un composant qui ne sert qu'à brancher un `useFrame`. C'est un motif courant en R3F.
- `scroll.offset` est déjà lissé par drei (prop `damping` de `ScrollControls`), donc la caméra ne saccade pas même si la molette avance par crans.

Dans le studio, tu animes la caméra sur, disons, 10 secondes. Ces 10 secondes deviennent « toute la hauteur de la page ».

---

## 5. Synchroniser avec un son

```ts
import { project } from "./theatre";

const intro = project.sheet("Intro");

export async function introAvecMusique(): Promise<void> {
    await intro.sequence.attachAudio({ source: "/sons/intro.mp3" });
    await intro.sequence.play();
}
```

- `attachAudio` charge le fichier, puis **asservit** la séquence au son : `play()` lance la musique, et la tête de lecture suit l'horloge audio. Les deux ne peuvent pas dériver.
- La promesse de `attachAudio` se résout quand le fichier est chargé et décodé. Ne lance `play()` qu'après.
- 🚨 Les navigateurs bloquent le son tant que l'utilisateur n'a pas **interagi** avec la page (un clic, une touche). Theatre attend alors ce geste avant de jouer : appelle `introAvecMusique()` depuis un gestionnaire de clic (« Jouer »), pas au chargement. La doc de `attachAudio` conseille aussi de lui passer ton propre `audioContext`, créé dans ce clic.

---

## 🧪 À essayer

1. Retire le `await project.ready` de `lancerIntro()` et appelle-la au tout début du chargement. Le message « intro terminée » s'affiche-t-il ? L'intro a-t-elle joué ? Remets la ligne.
2. Lance `pulser()`, puis appelle `halo.sequence.pause()` depuis un bouton. Relance avec `play()` : la lecture reprend-elle où elle s'était arrêtée ? Et les options (`rate`, `direction`) sont-elles conservées ?
3. Appelle deux fois `lancerIntro()` à 0.5 s d'écart. Combien de messages « intro terminée » ? Pourquoi ?
4. Branche `suivreProgression` sur une `<div>` dont la largeur représente le ratio. Mets à jour la largeur par une ref, pas par un `useState` (même raison qu'en [`CORE_A_LA_MAIN`](CORE_A_LA_MAIN.md), §4).
5. Monte la page de scroll du §4 et anime `Caméra` et `Logo` dans le studio. Change `pages={4}` en `pages={10}` : qu'est-ce qui change, et qu'est-ce qui ne change pas ?
