# 5. Le contexte et les hooks personnalisés

> **TL;DR.** Le **contexte** transmet une valeur à tous les composants d'un sous-arbre sans la passer d'étage en étage : `createContext`, un fournisseur `<MonContexte value={…}>` en haut (React 19 : plus besoin de `.Provider`), et `use(MonContexte)` ou `useContext(MonContexte)` en bas. Il convient aux valeurs qui changent **rarement** (l'utilisateur connecté, la langue, le thème). Pour un état qui change souvent, comme une partie, un store Zustand est mieux adapté. Un **hook personnalisé** est une fonction `useQuelqueChose` qui regroupe des hooks pour les réutiliser : c'est la façon React de factoriser de la logique.

Liens : [`INDEX`](INDEX.md) · [`ETAT`](ETAT.md) (§5, remonter l'état) · [`EFFETS`](EFFETS.md) · [`ZUSTAND`](ZUSTAND.md) · [`ROUTER`](ROUTER.md)

🧪 Labo : aucun.

---

## 1. Le problème : le *prop drilling*

```text
App (connaît l'utilisateur)
 └─ Page
     └─ Table
         └─ BarreJoueur
             └─ Avatar   ← seul à avoir besoin de l'utilisateur
```

Passer `utilisateur` en prop à `Page`, puis `Table`, puis `BarreJoueur`, juste pour qu'il arrive à `Avatar` : chaque étage intermédiaire doit déclarer et transmettre une prop dont il ne fait rien. C'est le *prop drilling*.

---

## 2. Un contexte pour la session

```tsx
import { createContext, use, useState, type ReactNode } from "react";

export type Utilisateur = { id: number; username: string };

type Session = {
    utilisateur: Utilisateur | null;
    connecter: (u: Utilisateur, jeton: string) => void;
    deconnecter: () => void;
};

const SessionContexte = createContext<Session | null>(null);

export function SessionFournisseur({ children }: { children: ReactNode }) {
    const [utilisateur, setUtilisateur] = useState<Utilisateur | null>(null);

    function connecter(u: Utilisateur, jeton: string) {
        localStorage.setItem("auth_token", jeton);
        setUtilisateur(u);
    }

    function deconnecter() {
        localStorage.removeItem("auth_token");
        setUtilisateur(null);
    }

    return <SessionContexte value={{ utilisateur, connecter, deconnecter }}>{children}</SessionContexte>;
}

export function useSession(): Session {
    const s = use(SessionContexte);
    if (!s) throw new Error("useSession doit être appelé sous <SessionFournisseur>");
    return s;
}
```

Ligne par ligne :
- `createContext<Session | null>(null)` : crée le canal. `null` est la valeur lue par un composant **hors** de tout fournisseur.
- `<SessionContexte value={…}>` : en React 19, le contexte s'utilise directement comme fournisseur. Observé : un composant placé dessous lit la valeur fournie, un composant placé à côté lit la valeur par défaut. Dans du code React 18, tu verras `<SessionContexte.Provider value={…}>`.
- `useSession()` : un **hook personnalisé** qui lit le contexte et transforme l'oubli du fournisseur en erreur claire, au lieu d'un `null` qui explose plus loin. Les composants n'importent que lui, jamais `SessionContexte` directement.

🚨 **Un fichier, un rôle.** Ici, le hook et le composant sont dans le même bloc pour tenir sur l'écran. Le lint de l'équipe (`reactRefresh.configs.vite`) refuse un fichier `.tsx` qui exporte **à la fois** un composant et autre chose (vérifié) : `Fast refresh only works when a file only exports components. Use a new file to share constants or functions between components`. C'est une **erreur**, `npm run lint` échoue. Dans le projet, sépare : `session.ts` (le contexte et `useSession`) et `SessionFournisseur.tsx` (le composant).
- `use(SessionContexte)` : la nouvelle façon de lire un contexte en React 19. Contrairement aux autres hooks, `use` peut être appelé **après** un `return` anticipé ou dans un `if` (observé). `useContext(SessionContexte)` fait la même chose avec les règles habituelles des hooks.

Brancher et lire :

```tsx
import { SessionFournisseur, useSession } from "./session";

function Avatar() {
    const { utilisateur, deconnecter } = useSession();
    if (!utilisateur) return <a href="/connexion">Se connecter</a>;
    return (
        <span>
            {utilisateur.username} <button onClick={deconnecter}>Quitter</button>
        </span>
    );
}

export default function App() {
    return (
        <SessionFournisseur>
            <header><Avatar /></header>
        </SessionFournisseur>
    );
}
```

🔗 **Lien C++.** Un contexte ressemble à une variable globale, mais **portée par l'arbre** : deux sous-arbres peuvent avoir deux fournisseurs avec deux valeurs différentes, et un test peut fournir une fausse session. C'est de l'injection de dépendance : `Avatar` ne sait pas d'où vient la session, il la demande.

🚨 **Le jeton dans `localStorage`** : c'est ce que fait le dépôt de l'équipe (`auth_token`). C'est simple, mais tout script qui s'exécute sur la page (une faille XSS, une dépendance compromise) peut le lire. L'alternative, un cookie `HttpOnly` posé par le serveur, est invisible au JavaScript. Le choix se fait côté Adonis ; voir le dossier Adonis.

---

## 3. Quand ne pas utiliser le contexte

Quand la `value` d'un fournisseur change, **tous** les composants qui lisent ce contexte se redessinent. Ici, `value={{ … }}` est même un objet neuf à chaque rendu de `SessionFournisseur`. Pour une session qui change deux fois par visite, c'est sans importance.

Pour un état de partie qui change à chaque coup, à chaque seconde du chrono et à chaque message du serveur, le contexte redessinerait tout l'écran de jeu à chaque fois. Zustand permet à chaque composant de ne s'abonner qu'au morceau qui l'intéresse ([`ZUSTAND`](ZUSTAND.md)).

| Valeur | Outil |
|---|---|
| Utilisée par un composant et ses enfants proches | `useState` + props |
| Lue partout, change rarement (session, langue, thème) | Contexte |
| Lue partout, change souvent (état de partie, chat) | Store Zustand |
| Liée à l'URL (id du salon, onglet ouvert) | L'URL elle-même ([`ROUTER`](ROUTER.md)) |

---

## 4. Les hooks personnalisés

Dès qu'un même groupe de `useState` + `useEffect` apparaît dans deux composants, on l'extrait dans une fonction `use…` :

```tsx
import { useEffect, useState } from "react";

export function useChrono(secondesMax: number, actif: boolean): number {
    const [restant, setRestant] = useState(secondesMax);

    useEffect(() => {
        if (!actif) return;
        const id = setInterval(() => setRestant((r) => Math.max(0, r - 1)), 1000);
        return () => clearInterval(id);
    }, [actif]);

    return restant;
}

export function ChronoTour({ monTour }: { monTour: boolean }) {
    const restant = useChrono(30, monTour);
    return <p className={restant <= 5 ? "urgent" : ""}>{restant} s</p>;
}
```

🚨 **Un fichier, un rôle.** Ici, le hook et le composant sont dans le même bloc pour tenir sur l'écran. Le lint de l'équipe (`reactRefresh.configs.vite`) refuse un fichier `.tsx` qui exporte **à la fois** un composant et autre chose (vérifié) : `Fast refresh only works when a file only exports components. Use a new file to share constants or functions between components`. C'est une **erreur**, `npm run lint` échoue. Dans le projet, sépare : `useChrono.ts` d'un côté, `ChronoTour.tsx` de l'autre.

- Un hook personnalisé **commence par `use`** : c'est ce qui permet au lint de vérifier les règles des hooks à l'intérieur ([`EFFETS`](EFFETS.md), §7).
- Il partage de la **logique**, pas de l'**état** : deux composants qui appellent `useChrono` ont chacun leur propre `restant`. C'est une fonction, pas un singleton.
- Il renvoie ce qu'on veut : une valeur, un tableau `[valeur, setter]`, un objet.

🚨 `useChrono` compte côté client. Dans le jeu, le serveur doit rester l'arbitre du temps : le client affiche un chrono, le serveur décide quand le tour est fini. Sinon, il suffit de figer l'onglet pour avoir un temps infini.

🔗 **Lien C.** Un hook personnalisé, c'est une fonction utilitaire de `libft` : on la sort du code dès qu'on l'a écrite deux fois. La seule contrainte en plus : elle doit être appelée en haut du composant, comme les hooks qu'elle contient.

---

## 🧪 À essayer

1. Affiche `<Avatar />` **en dehors** de `<SessionFournisseur>`. Lis le message d'erreur : c'est celui de `useSession`.
2. Ajoute un `console.log("rendu Avatar")` et un bouton, dans `App`, qui incrémente un compteur sans rapport avec la session. `Avatar` est-il redessiné ? Et si le compteur est un état de `SessionFournisseur` ?
3. Écris `useToucheEchap` d'[`EFFETS`](EFFETS.md) §6 dans son propre fichier, et utilise-le dans deux modales différentes.
4. Affiche deux `ChronoTour` côte à côte, l'un actif, l'autre non. Ont-ils le même `restant` ?
5. Dans `useChrono`, que se passe-t-il quand `secondesMax` change en cours de route ? Propose une correction sans effet (indice : [`EFFETS`](EFFETS.md), §4, la `key`).
