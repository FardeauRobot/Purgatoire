# 10. État du jeu et réseau : zustand + WebSocket

> **TL;DR.** Dès que plusieurs composants (le HUD en HTML, la main de cartes en 3D, les personnages) lisent le même état de partie, on sort cet état des composants et on le met dans un **store** : `zustand` est le choix naturel avec R3F (même équipe, pmndrs). Les composants **lisent** le store avec un sélecteur (`useJeu((s) => s.pv)`) et ne se re-rendent que si **cette** valeur change. Dans `useFrame`, on lit avec `useJeu.getState()`, **sans** abonnement, donc sans re-rendu. Pour un jeu en ligne, **le serveur fait autorité** : le client envoie des **intentions** (« je joue la carte 3 »), le serveur renvoie l'**état**, et le store ne fait que le recopier.

Liens : [`INDEX`](INDEX.md) · [`INTERACTION`](INTERACTION.md) · [`SQUELETTE_ANIMATIONS`](SQUELETTE_ANIMATIONS.md) · [`ATELIER_CARTES`](ATELIER_CARTES.md) · [`../VERS_TRANSCENDANCE`](../VERS_TRANSCENDANCE.md)

🧪 Labo : aucune leçon. Exemples vérifiés au typecheck avec `zustand` 5.

---

## 1. Pourquoi un store

Avec `useState`, l'état vit **dans un composant**, et on le fait descendre par les props (voir [`INTERACTION`](INTERACTION.md), section 7). Ça marche pour un compteur, pas pour une partie :

```text
App
├─ Hud (HTML)              lit : pv, aura, tour, chrono
└─ Canvas
   ├─ Arene
   │  ├─ Personnage J1     lit : animation de J1
   │  └─ Personnage J2     lit : animation de J2
   └─ Main
      └─ Carte ×7          lit : cartes en main, carte survolée ; écrit : « jouer »
```

Faire descendre tout ça à travers 4 niveaux de props (le *prop drilling*) devient vite illisible, et chaque changement re-rend toute la chaîne. Un store est une **variable globale observable** : chaque composant s'abonne **directement** aux morceaux qui l'intéressent.

🔗 **Lien C.** C'est une variable globale, mais avec une liste de *callbacks* appelés à chaque modification, et une seule fonction autorisée à la modifier. L'idée d'un `setter` qui notifie des observateurs.

---

## 2. Un store zustand

```tsx
import { create } from "zustand";

export type Carte = { id: string; nom: string; aura: number };
export type Anim = "Idle" | "Attaque" | "Aura" | "Coup" | "Victoire";

type EtatJeu = {
    pv: [number, number];
    aura: [number, number];
    main: Carte[];
    survolee: string | null;
    anims: [Anim, Anim];
    survoler: (id: string | null) => void;
    appliquerServeur: (etat: Partial<Pick<EtatJeu, "pv" | "aura" | "main" | "anims">>) => void;
};

export const useJeu = create<EtatJeu>()((set) => ({
    pv: [30, 30],
    aura: [0, 0],
    main: [],
    survolee: null,
    anims: ["Idle", "Idle"],
    survoler: (id) => set({ survolee: id }),
    appliquerServeur: (etat) => set(etat),
}));
```

- **`create<EtatJeu>()(…)`** : la double paire de parenthèses est voulue. C'est la forme que la doc de zustand impose en TypeScript pour que l'inférence des types fonctionne.
- **`set({ survolee: id })`** **fusionne** l'objet donné dans l'état (au premier niveau), comme le `setState` des anciens composants React. Pas besoin de recopier les autres champs.
- **Les actions vivent dans le store** (`survoler`, `appliquerServeur`) : les composants appellent `useJeu.getState().survoler(id)` ou la récupèrent avec un sélecteur. Toute modification passe par une fonction nommée, facile à retrouver.
- `useJeu` est **à la fois** un hook React et un objet avec des méthodes (`getState`, `setState`, `subscribe`).

---

## 3. Lire le store : les trois façons

| Où | Comment | Re-rendu ? |
|---|---|---|
| Composant React (HUD, carte) | `const pv = useJeu((s) => s.pv)` | Oui, **seulement** quand `s.pv` change. |
| Dans `useFrame` / un handler | `useJeu.getState().survolee` | Non : lecture ponctuelle, sans abonnement. |
| Hors React (le code WebSocket) | `useJeu.getState()`, `useJeu.setState(...)`, `useJeu.subscribe(fn)` | Sans objet. |

### Le sélecteur, et le piège de l'objet neuf

```text
const pv = useJeu((s) => s.pv);                     ✅ re-rendu quand pv change
const { pv, aura } = useJeu((s) => ({ pv: s.pv, aura: s.aura }));   ❌ boucle infinie en zustand 5
```

Le sélecteur est rappelé à chaque changement du store, et zustand compare son résultat à l'ancien avec `Object.is` (égalité de **référence**). Un objet créé dans le sélecteur est **neuf à chaque fois** : il est toujours « différent », et zustand 5 le signale par une erreur de boucle de rendu. Deux solutions :

```tsx
import { useShallow } from "zustand/react/shallow";
import { useJeu } from "./store";

export function Hud() {
    const pv = useJeu((s) => s.pv);
    const { aura, main } = useJeu(useShallow((s) => ({ aura: s.aura, main: s.main })));

    return (
        <div className="hud">
            PV {pv[0]} / {pv[1]} — Aura {aura[0]} — {main.length} cartes
        </div>
    );
}
```

- Un sélecteur **par valeur** (première ligne) : le plus simple.
- **`useShallow`** compare le contenu de l'objet **champ par champ**, et pas sa référence.

---

## 4. Le store et `useFrame` : ne pas re-rendre 60 fois par seconde

Règle de [`ANIMATION`](ANIMATION.md) : ce qui change à chaque image ne passe pas par React. Le store suit la même règle. Une carte qui doit se soulever quand elle est survolée **lit** le store dans `useFrame` :

```tsx
import { useRef, type ReactNode } from "react";
import { useFrame } from "@react-three/fiber";
import { MathUtils, type Group } from "three";
import { useJeu } from "./store";

export function CarteEnMain({ id, children }: { id: string; children: ReactNode }) {
    const ref = useRef<Group>(null);

    useFrame((_, delta) => {
        if (!ref.current) return;
        const cible = useJeu.getState().survolee === id ? 0.4 : 0;
        ref.current.position.y = MathUtils.damp(ref.current.position.y, cible, 10, delta);
    });

    return (
        <group
            ref={ref}
            onPointerOver={(e) => {
                e.stopPropagation();
                useJeu.getState().survoler(id);
            }}
            onPointerOut={() => useJeu.getState().survoler(null)}
        >
            {children}
        </group>
    );
}
```

- **Aucun abonnement** : survoler une carte ne re-rend **aucun** composant. La valeur est lue 60 fois par seconde, et l'animation suit.
- `getState()` dans un handler : pas besoin d'abonner le composant à `survoler`, une action qui ne change jamais.
- `children: ReactNode` : le contenu de la carte (son mesh, son texte) est passé entre les balises, comme un `<div>` qui enveloppe d'autres éléments.

---

## 5. Le serveur fait autorité

Dans un jeu en ligne, **le client ment** : n'importe qui peut ouvrir la console et appeler `useJeu.setState({ pv: [999, 0] })`. Si le serveur croyait ce que dit le client, la triche serait triviale.

```text
 Client A                     Serveur (AdonisJS)                   Client B
    │  « je joue la carte c7 »        │                                │
    ├────────────────────────────────►│  vérifie : c'est ton tour ?    │
    │                                 │  la carte est dans ta main ?   │
    │                                 │  tu as assez d'aura ?          │
    │                                 │  → calcule le nouvel état      │
    │◄──────── nouvel état ───────────┼──────── nouvel état ──────────►│
    │  appliquerServeur(état)         │         appliquerServeur(état) │
    │  → anims : Attaque / Coup       │                                │
```

| Le client envoie… | Le serveur renvoie… |
|---|---|
| des **intentions** : `{ type: "jouer", carte: "c7" }` | l'**état** qui en résulte : PV, aura, mains, animations à jouer |
| jamais de résultat (« j'inflige 5 dégâts ») | des erreurs : `{ type: "refus", raison: "pas ton tour" }` |

Le store du client devient alors une simple **copie** de l'état du serveur. Les seules valeurs que le client possède vraiment sont celles d'interface : la carte survolée, la caméra, l'ouverture d'un menu.

---

## 6. Brancher une WebSocket sur le store

```tsx
import { useEffect } from "react";
import { useJeu, type Carte, type Anim } from "./store";

type MessageServeur =
    | { type: "etat"; pv: [number, number]; aura: [number, number]; main: Carte[]; anims: [Anim, Anim] }
    | { type: "refus"; raison: string };

type MessageClient = { type: "jouer"; carte: string } | { type: "passer" };

let socket: WebSocket | null = null;

export function envoyer(msg: MessageClient) {
    if (socket?.readyState === WebSocket.OPEN) socket.send(JSON.stringify(msg));
}

export function useConnexionPartie(url: string) {
    useEffect(() => {
        const ws = new WebSocket(url);
        socket = ws;

        ws.onmessage = (ev) => {
            const msg = JSON.parse(ev.data as string) as MessageServeur;
            if (msg.type === "etat") {
                const { type: _, ...etat } = msg;
                useJeu.getState().appliquerServeur(etat);
            } else {
                console.warn("refusé :", msg.raison);
            }
        };

        return () => {
            ws.close();
            if (socket === ws) socket = null;
        };
    }, [url]);
}
```

- **`useEffect` + nettoyage** : la connexion s'ouvre au montage et se ferme au démontage. En développement, `StrictMode` monte deux fois (voir [`INSTALLATION`](INSTALLATION.md)) : grâce au nettoyage, la première connexion est fermée proprement. Sans lui, tu aurais **deux** connexions, et chaque message arriverait en double.
- **Unions discriminées** (`type: "etat" | "refus"`) : après `if (msg.type === "etat")`, TypeScript sait que `msg.pv` existe. C'est le `switch` sur un `enum` + `union` du C, vérifié par le compilateur (voir [`../TYPESCRIPT`](../TYPESCRIPT.md)).
- **`as MessageServeur` ne vérifie rien** à l'exécution : c'est une promesse faite au compilateur. Côté serveur, il faut **valider** chaque message reçu du client (VineJS avec AdonisJS) : c'est une frontière de confiance.
- **`envoyer`** est une fonction ordinaire, appelable depuis n'importe quel handler : `onClick={() => envoyer({ type: "jouer", carte: id })}`.
- Le choix de la bibliothèque WebSocket côté AdonisJS est discuté dans [`../VERS_TRANSCENDANCE`](../VERS_TRANSCENDANCE.md), section 4. Avec `socket.io`, le principe est identique : seuls `on` / `emit` remplacent `onmessage` / `send`.

---

## 7. Du store à l'animation

Le dernier maillon : l'état reçu fait jouer les personnages. Le composant de [`SQUELETTE_ANIMATIONS`](SQUELETTE_ANIMATIONS.md) (section 5) prend une prop `etat` ; il suffit de la lire dans le store :

```tsx
import type { JSX } from "react";
import { useJeu, type Anim } from "./store";

declare function CombattantAnime(props: { etat: Anim; position: [number, number, number] }): JSX.Element;

export function Arene() {
    const anims = useJeu((s) => s.anims);
    return (
        <>
            <CombattantAnime etat={anims[0]} position={[-2, 0, 0]} />
            <CombattantAnime etat={anims[1]} position={[2, 0, 0]} />
        </>
    );
}
```

`anims` change quelques fois par tour : un abonnement React est ici le bon outil. Quand le serveur dit `["Attaque", "Coup"]`, les deux personnages font leur fondu ; le composant ne sait rien du réseau.

(La ligne `declare function` remplace l'import du composant de la fiche 9, pour que l'exemple soit vérifiable seul. Elle recopie **exactement** sa signature : le type `Etat` de la fiche 9 et le type `Anim` du store contiennent les mêmes noms d'animation. Si tu en ajoutes un d'un côté, ajoute-le de l'autre, ou mieux, fais importer `Anim` par le composant.)

---

## 8. Dans le monde à part du `<Canvas>`

R3F fait tourner le contenu du `<Canvas>` dans un **moteur de rendu React séparé**. Les **contextes** React (`createContext` / `useContext`) déclarés à l'extérieur du `<Canvas>` n'y étaient historiquement pas visibles : c'était une source classique de bugs. R3F v9 fait suivre les contextes automatiquement, mais un store zustand **ne dépend d'aucun contexte** : c'est une variable de module, visible partout, même hors de React. Raison de plus pour le préférer à un contexte.

---

## 🧪 À essayer

1. Crée le store de la section 2 et le `Hud` de la section 3. Dans la console du navigateur (avec le store exposé via `window.jeu = useJeu` en développement), appelle `jeu.setState({ pv: [12, 30] })` et regarde le HUD.
2. Remplace `useShallow(...)` par un sélecteur qui renvoie un objet neuf, sans `useShallow` : lis l'erreur dans la console.
3. Mets un `console.log("rendu")` dans `CarteEnMain` : survoler les cartes n'en affiche aucun.
4. Écris un mini-serveur WebSocket de test avec le paquet `ws` sous Node, qui renvoie un état fixe à chaque message. Branche `useConnexionPartie` dessus.
5. Change `anims` dans le store à la main : les personnages enchaînent leurs fondus.
