# 11. Atelier : l'écran de partie en 2D

> **TL;DR.** On assemble tout : un **store** de partie que **seul le serveur remplit** (`appliquer`), plus une **sélection** de cartes qui reste locale au joueur ; un **écouteur Socket.IO** posé une fois, hors de React ; des composants qui lisent chacun **leur** morceau du store ; des boutons qui envoient des **intentions** (`partie:jouer`, `partie:defausser`) et attendent l'accusé ; un chrono calculé à partir de l'heure de fin **donnée par le serveur**. C'est la version HTML + Tailwind de l'écran. La version 3D est dans [`../react-three-fiber/ATELIER_CARTES`](../react-three-fiber/ATELIER_CARTES.md), et elle peut lire **le même store**.

Liens : [`INDEX`](INDEX.md) · [`ZUSTAND`](ZUSTAND.md) · [`TEMPS_REEL`](TEMPS_REEL.md) · [`TAILWIND`](TAILWIND.md) · [`EFFETS`](EFFETS.md) · [`../react-three-fiber/ETAT_RESEAU`](../react-three-fiber/ETAT_RESEAU.md)

🧪 Labo : aucun. Ce code a tourné tel quel dans Chrome, avec un état de partie injecté à la main (§5). Le **serveur** de jeu n'existe pas encore : les règles ci-dessous (8 cartes, 5 au plus par coup, un nombre limité de défausses) sont un **exemple**, à remplacer par celles que l'équipe fixera.

---

## 1. Le découpage

```text
partie.ts         le store : ce que dit le serveur + la sélection du joueur
brancher.ts       la socket, et l'écouteur "partie:etat" → store
EcranPartie.tsx   connexion au montage, et l'écran :
  ├─ Chrono       lit finTour
  ├─ Main         lit main + selection → 8 × CarteMain
  └─ Actions      lit selection, monTour, defaussesRestantes → émet les intentions
```

- Le flux ne va que dans **un** sens : serveur → store → écran. L'écran ne modifie jamais `main` ou `aura` lui-même ; il **demande** au serveur, qui répond par un nouvel état.
- La seule chose que l'écran écrit, c'est la **sélection** : elle n'intéresse que ce joueur, tant qu'il n'a pas cliqué sur « Jouer ».

---

## 2. Le store

```ts
import { create } from "zustand";

export type Carte = { id: string; nom: string; aura: number };

export type EtatServeur = {
    main: Carte[];
    aura: [number, number];
    monTour: boolean;
    finTour: number;
    defaussesRestantes: number;
};

type EtatPartie = EtatServeur & {
    selection: string[];
    appliquer: (e: EtatServeur) => void;
    basculer: (id: string) => void;
    vider: () => void;
};

export const MAX_SELECTION = 5;

const DEPART: EtatServeur & { selection: string[] } = {
    main: [],
    aura: [0, 0],
    monTour: false,
    finTour: 0,
    defaussesRestantes: 0,
    selection: [],
};

export const usePartie = create<EtatPartie>()((set) => ({
    ...DEPART,
    appliquer: (e) =>
        set((s) => ({
            ...e,
            selection: s.selection.filter((id) => e.main.some((c) => c.id === id)),
        })),
    basculer: (id) =>
        set((s) => {
            if (s.selection.includes(id)) return { selection: s.selection.filter((x) => x !== id) };
            if (s.selection.length >= MAX_SELECTION) return {};
            return { selection: [...s.selection, id] };
        }),
    vider: () => set(DEPART),
}));
```

Ligne par ligne :
- `EtatServeur` : **exactement** ce que le serveur envoie. Ce type devrait vivre dans le paquet partagé de l'équipe (`apps/types/`), comme `ws_contract.ts`, pour que le front et le back ne puissent pas diverger.
- `selection` contient des **identifiants**, pas des cartes. Si on stockait les objets `Carte`, ils deviendraient des copies périmées dès que le serveur renvoie la main.
- `appliquer` remplace tout l'état serveur **et nettoie la sélection** : une carte sélectionnée qui a quitté la main (jouée, défaussée, volée par un effet) en est retirée. Observé : la carte `c1` sélectionnée disparaît de la sélection quand le serveur renvoie une main sans elle.
- `basculer` : un deuxième clic désélectionne ; au-delà de `MAX_SELECTION`, `return {}` ne change rien. Observé : six clics donnent cinq cartes choisies.
- `finTour` : une **heure** (en millisecondes depuis 1970), pas un nombre de secondes restantes. Voir le §4.

🔗 **Lien C.** `selection` est un tableau d'indices vers un autre tableau, pas un tableau de pointeurs vers ses éléments : si le tableau principal est réalloué (un `realloc` qui déplace tout), les indices restent valables, les pointeurs pendraient.

---

## 3. La socket et l'écouteur

```ts
import { io, type Socket } from "socket.io-client";
import { usePartie, type EtatServeur } from "./partie";

type ServeurVersClient = { "partie:etat": (e: EtatServeur) => void };
type Reponse = { ok: true } | { ok: false; erreur: string };
type ClientVersServeur = {
    "partie:jouer": (ids: string[], ack: (r: Reponse) => void) => void;
    "partie:defausser": (ids: string[], ack: (r: Reponse) => void) => void;
};

export const socket: Socket<ServeurVersClient, ClientVersServeur> = io({
    autoConnect: false,
    auth: (cb) => cb({ token: localStorage.getItem("auth_token") }),
});

socket.on("partie:etat", (e) => usePartie.getState().appliquer(e));
```

- L'écouteur est posé **au chargement du module**, une seule fois, hors de tout composant ([`TEMPS_REEL`](TEMPS_REEL.md), §5). Il n'a donc ni `off` ni problème de StrictMode.
- `usePartie.getState()` : l'accès au store **hors de React**. Pas de hook ici : ce n'est pas un composant.
- Les intentions envoient des **identifiants** de cartes. Le serveur vérifie qu'elles sont bien dans la main de ce joueur, que c'est son tour et qu'il reste des défausses : le client ne fait que **proposer**.

---

## 4. L'écran

```tsx
import { useEffect, useState } from "react";
import { useShallow } from "zustand/react/shallow";
import { socket } from "./brancher";
import { MAX_SELECTION, usePartie, type Carte } from "./partie";

function useSecondesRestantes(fin: number): number {
    const [maintenant, setMaintenant] = useState(() => Date.now());
    useEffect(() => {
        const id = setInterval(() => setMaintenant(Date.now()), 250);
        return () => clearInterval(id);
    }, []);
    return Math.max(0, Math.ceil((fin - maintenant) / 1000));
}

function Chrono() {
    const finTour = usePartie((s) => s.finTour);
    const restant = useSecondesRestantes(finTour);
    return <p className={`font-mono text-2xl ${restant <= 5 ? "text-red-500" : ""}`}>{restant} s</p>;
}

function CarteMain({ carte, choisie, onBasculer }: { carte: Carte; choisie: boolean; onBasculer: (id: string) => void }) {
    return (
        <button
            onClick={() => onBasculer(carte.id)}
            aria-pressed={choisie}
            className={`h-32 w-24 rounded-xl border-2 bg-slate-800 p-2 text-left transition ${choisie ? "-translate-y-4 border-aura" : "border-slate-600"}`}
        >
            <span className="block text-sm">{carte.nom}</span>
            <span className="text-xl font-bold text-aura">{carte.aura}</span>
        </button>
    );
}

function Main() {
    const main = usePartie((s) => s.main);
    const selection = usePartie((s) => s.selection);
    const basculer = usePartie((s) => s.basculer);
    return (
        <div className="flex gap-2">
            {main.map((c) => (
                <CarteMain key={c.id} carte={c} choisie={selection.includes(c.id)} onBasculer={basculer} />
            ))}
        </div>
    );
}

function Actions() {
    const { selection, monTour, defaussesRestantes } = usePartie(
        useShallow((s) => ({ selection: s.selection, monTour: s.monTour, defaussesRestantes: s.defaussesRestantes })),
    );
    const [envoi, setEnvoi] = useState(false);
    const [erreur, setErreur] = useState<string | null>(null);

    async function envoyer(evenement: "partie:jouer" | "partie:defausser") {
        setEnvoi(true);
        setErreur(null);
        try {
            const r = await socket.timeout(5000).emitWithAck(evenement, selection);
            if (!r.ok) setErreur(r.erreur);
        } catch {
            setErreur("Le serveur ne répond pas");
        } finally {
            setEnvoi(false);
        }
    }

    const bloque = !monTour || envoi || selection.length === 0;
    return (
        <div className="flex items-center gap-3">
            <button disabled={bloque} onClick={() => envoyer("partie:jouer")} className="rounded bg-aura px-4 py-2 disabled:opacity-40">
                Jouer ({selection.length}/{MAX_SELECTION})
            </button>
            <button
                disabled={bloque || defaussesRestantes === 0}
                onClick={() => envoyer("partie:defausser")}
                className="rounded border px-4 py-2 disabled:opacity-40"
            >
                Défausser ({defaussesRestantes})
            </button>
            {erreur && <p role="alert" className="text-red-400">{erreur}</p>}
        </div>
    );
}

export function EcranPartie() {
    const aura = usePartie((s) => s.aura);
    const monTour = usePartie((s) => s.monTour);

    useEffect(() => {
        socket.connect();
        return () => {
            socket.disconnect();
            usePartie.getState().vider();
        };
    }, []);

    return (
        <section className="flex flex-col items-center gap-6 p-6">
            <header className="flex w-full justify-between">
                <p>Aura : {aura[0]} contre {aura[1]}</p>
                <p>{monTour ? "À toi" : "Tour adverse"}</p>
                <Chrono />
            </header>
            <Main />
            <Actions />
        </section>
    );
}
```

Démontage :
- **`useSecondesRestantes`** recalcule `fin - maintenant` à chaque tic, au lieu de **décompter** (`r - 1`) comme `useChrono` dans [`CONTEXTE_HOOKS`](CONTEXTE_HOOKS.md), §4. Un onglet en arrière-plan ralentit ses `setInterval` : un décompte prendrait du retard, une différence d'heures reste juste. Et c'est le serveur qui fixe `finTour`, donc l'arbitre du temps. Observé : avec une fin à `+3000` ms, l'écran affiche `2 s` 1,3 s plus tard.
- Le tic à 250 ms, et non 1000, évite qu'un tic tombe juste avant le changement de seconde et laisse un chiffre affiché presque une seconde de trop.
- **`Chrono`** est le seul composant redessiné quatre fois par seconde. Mis dans `EcranPartie`, il redessinerait tout l'écran à chaque tic ([`PERFORMANCE_DEBUG`](PERFORMANCE_DEBUG.md)).
- **`Main`** lit trois morceaux avec trois sélecteurs simples. `basculer` est une fonction du store : son adresse ne change jamais, elle ne provoque aucun rendu.
- **`key={c.id}`** : l'identifiant de la carte, jamais l'index. Quand une carte est jouée, les autres glissent dans le tableau, et l'index ferait passer l'état d'une carte à sa voisine ([`COMPOSANTS_JSX`](COMPOSANTS_JSX.md)).
- **`aria-pressed`** : dit aux lecteurs d'écran que la carte est un bouton à bascule. Le sujet de Transcendance a un module accessibilité, et ça sert aussi aux tests (on sélectionne `[aria-pressed="true"]`).
- **`Actions`** lit trois champs en un sélecteur, donc avec `useShallow` ([`ZUSTAND`](ZUSTAND.md), §2).
- **`bloque`** : un booléen **dérivé** à chaque rendu, pas un état de plus ([`ETAT`](ETAT.md), §4). Observé : les deux boutons sont désactivés au départ (rien de choisi), puis pendant l'envoi.
- Le **nettoyage** de `EcranPartie` déconnecte **et** vide le store : en quittant la partie, rien ne reste de l'ancienne main.

---

## 5. Ce qui a été observé

L'écran a été affiché dans Chrome avec un état injecté par `usePartie.getState().appliquer(…)` : 8 cartes, mon tour, fin dans 7 s, 2 défausses.

| Action | Résultat |
|---|---|
| Départ | aucune carte choisie, `Jouer (0/5)` et `Défausser (2)` désactivés, `7 s` |
| 6 clics sur 6 cartes | 5 cartes choisies, `Jouer (5/5)` actif |
| Nouveau clic sur la 1ʳᵉ | désélectionnée, `Jouer (4/5)` |
| Le serveur renvoie une main sans `c1`, au tour adverse | `c1` sort de la sélection (3 restent), boutons désactivés, `Tour adverse`, `Défausser (1)` |
| Clic sur `Jouer`, aucun serveur | les deux boutons désactivés pendant l'envoi ; après 5 s : `Le serveur ne répond pas`, boutons réactivés |

- La sélection **survit** au tour adverse (les cartes 2, 3 et 4 restent choisies). C'est un choix : on peut aussi la vider dans `appliquer` quand `monTour` passe à `false`.
- Le passage du serveur réel n'a pas été testé : il est à écrire côté Adonis.

---

## 6. Brancher la 3D sur le même store

La scène R3F de [`../react-three-fiber/ATELIER_CARTES`](../react-three-fiber/ATELIER_CARTES.md) peut remplacer `<Main />` sans toucher au reste :

```text
const main      = usePartie((s) => s.main);
const selection = usePartie((s) => s.selection);
const basculer  = usePartie((s) => s.basculer);
… <CarteMesh onClick={() => basculer(c.id)} soulevee={selection.includes(c.id)} />
```

- Le store ne sait pas **qui** l'affiche. HTML, 3D, ou les deux en même temps : ils lisent la même source.
- Le chrono, les boutons et les messages d'erreur restent en HTML, par-dessus le `<Canvas>` : le texte et les boutons y sont plus nets et plus accessibles qu'en 3D.

---

## 🧪 À essayer

1. Monte `EcranPartie` avec un faux état (le `appliquer` du §5) et un `console.log("rendu", nom)` dans chaque composant. Lesquels sont redessinés à chaque tic du chrono ? Et au clic sur une carte ?
2. Déplace `<Chrono />` hors de son composant : mets `useSecondesRestantes` directement dans `EcranPartie`. Compte de nouveau les rendus.
3. Remplace `key={c.id}` par `key={i}`, ajoute une animation d'entrée sur `CarteMain`, puis renvoie une main sans la première carte. Quelles cartes rejouent leur animation ?
4. Vide la sélection quand ce n'est plus ton tour : où le faire, dans `appliquer` ou dans un `useEffect` ? Pourquoi le store est-il le meilleur endroit ?
5. Stocke des objets `Carte` dans `selection` au lieu d'identifiants, puis renvoie une main où la carte `c2` a une `aura` différente. Que montre la sélection ?
