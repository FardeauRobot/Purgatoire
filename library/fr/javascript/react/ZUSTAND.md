# 7. Zustand : l'état partagé de l'application

> **TL;DR.** Un store Zustand est un état **hors des composants**, lisible par n'importe lequel avec `useStore((s) => s.morceau)`. Le composant ne se redessine que si **son** morceau change. Les bases (créer le store, les trois façons de le lire, `useShallow`, `getState()` hors React, le serveur qui fait autorité) sont déjà dans [`../react-three-fiber/ETAT_RESEAU`](../react-three-fiber/ETAT_RESEAU.md) : lis-la d'abord. Cette fiche ajoute ce qui sert dans l'application web autour du jeu : plusieurs petits stores plutôt qu'un gros, `persist` pour les préférences, la remise à zéro à la déconnexion, et les messages d'erreur exacts.

Liens : [`INDEX`](INDEX.md) · [`../react-three-fiber/ETAT_RESEAU`](../react-three-fiber/ETAT_RESEAU.md) · [`CONTEXTE_HOOKS`](CONTEXTE_HOOKS.md) (§3, contexte ou store) · [`ETAT`](ETAT.md) · [`TEMPS_REEL`](TEMPS_REEL.md)

🧪 Labo : aucun.

---

## 1. Rappel en dix lignes

```tsx
import { create } from "zustand";

type EtatScore = {
    score: number;
    ajouter: (points: number) => void;
};

export const useScore = create<EtatScore>()((set) => ({
    score: 0,
    ajouter: (points) => set((s) => ({ score: s.score + points })),
}));

export function AfficheScore() {
    const score = useScore((s) => s.score);
    return <p>{score}</p>;
}
```

- `create<T>()(…)` : les doubles parenthèses sont la forme TypeScript imposée par Zustand.
- `set((s) => ({ score: … }))` : on renvoie **seulement** les champs qui changent, Zustand les fusionne avec le reste. Comme pour `useState`, on fabrique de nouvelles valeurs, on ne mute pas `s`.
- `useScore((s) => s.score)` : un **sélecteur**. Observé : cinq changements d'un **autre** champ du même store ne redessinent pas ce composant.
- Hors d'un composant (un écouteur Socket.IO, un loader de [`ROUTER`](ROUTER.md)) : `useScore.getState().ajouter(10)`.
- Dans le projet, le store va dans son propre fichier (`stores/score.ts`) et le composant dans le sien : le lint de l'équipe refuse un `.tsx` qui exporte à la fois un composant et un store (`react-refresh/only-export-components`, vérifié). Les fiches les regroupent seulement pour la lecture.

---

## 2. Le piège du sélecteur qui fabrique un objet

```tsx
import { create } from "zustand";
import { useShallow } from "zustand/react/shallow";

type EtatJoueur = { pseudo: string; score: number; chrono: number };
const useJoueur = create<EtatJoueur>()(() => ({ pseudo: "", score: 0, chrono: 0 }));

export function BandeauFaux() {
    const { pseudo, score } = useJoueur((s) => ({ pseudo: s.pseudo, score: s.score }));
    return <p>{pseudo} : {score}</p>;
}

export function BandeauJuste() {
    const { pseudo, score } = useJoueur(useShallow((s) => ({ pseudo: s.pseudo, score: s.score })));
    return <p>{pseudo} : {score}</p>;
}
```

Observé avec `BandeauFaux`, dès le premier affichage :

```text
The result of getSnapshot should be cached to avoid an infinite loop
Error: Maximum update depth exceeded. This can happen when a component repeatedly calls setState …
```

- Le sélecteur renvoie un objet **neuf** à chaque appel. Zustand compare par adresse (`Object.is`), voit « ça a changé », redessine, rappelle le sélecteur, obtient un autre objet neuf… La page plante.
- `useShallow` compare les **champs** un par un. Observé : avec lui, cinq changements de `chrono` ne redessinent pas le bandeau.
- Le premier message est le meilleur indice : quand tu lis `getSnapshot should be cached`, cherche un sélecteur qui fabrique un objet ou un tableau (`.map`, `.filter`, `{ … }`, `[ … ]`).

Autre solution, souvent la plus lisible : deux sélecteurs simples.

```text
const pseudo = useJoueur((s) => s.pseudo);
const score  = useJoueur((s) => s.score);
```

🔗 **Lien C.** C'est l'erreur de [`ETAT`](ETAT.md), §3, à l'envers. Là, on gardait la même adresse avec un contenu différent : React ne voyait rien. Ici, on donne une adresse neuve à un contenu identique : React voit un changement à chaque fois.

---

## 3. Plusieurs petits stores

```text
useSession    ← utilisateur connecté, jeton                (change à la connexion)
usePrefs      ← volume, langue, animations réduites         (persisté dans localStorage)
usePartie     ← main, plateau, scores, tour, chrono         (rempli par le serveur, effacé en fin de partie)
useChat       ← messages par salon                          (rempli par Socket.IO)
```

- Chaque store a **une** raison de changer. Les remettre à zéro, les tester, les lire dans une revue de code : tout est plus simple.
- Le store de partie **recopie** ce que dit le serveur, il ne décide rien ([`../react-three-fiber/ETAT_RESEAU`](../react-three-fiber/ETAT_RESEAU.md), §5).
- La session peut aussi être un contexte ([`CONTEXTE_HOOKS`](CONTEXTE_HOOKS.md), §2). Un store a un avantage : il est lisible **hors** de React, dans `api.ts` par exemple, pour ajouter le jeton à chaque requête.

---

## 4. `persist` : survivre au rechargement

```tsx
import { create } from "zustand";
import { persist } from "zustand/middleware";

type Prefs = {
    volume: number;
    animationsReduites: boolean;
    regler: (p: Partial<Pick<Prefs, "volume" | "animationsReduites">>) => void;
};

export const usePrefs = create<Prefs>()(
    persist(
        (set) => ({
            volume: 5,
            animationsReduites: false,
            regler: (p) => set(p),
        }),
        { name: "transcendance-prefs" },
    ),
);
```

Observé : après `regler({ volume: 9 })`, le `localStorage` contient

```text
transcendance-prefs = {"state":{"volume":9},"version":0}
```

et après un rechargement de la page, le volume affiché est bien 9.

- `persist(…, { name })` : enregistre l'état dans `localStorage` sous cette clé à chaque changement, et le relit au démarrage. Les fonctions (`regler`) ne sont pas enregistrées.
- `version` : si la forme de l'état change dans une future version (un champ renommé), on incrémente `version` et on fournit une fonction `migrate`. Sinon, l'ancien JSON est rechargé tel quel dans la nouvelle forme.
- `Partial<Pick<…>>` : « certains de ces champs, pas forcément tous » ([`../TYPESCRIPT`](../TYPESCRIPT.md)).

🚨 Ne persiste **pas** l'état de partie : il appartient au serveur. Après un rechargement, le client redemande l'état au serveur, il ne le reconstruit pas depuis un `localStorage` qui peut être périmé ou modifié à la main.

🚨 Tout ce qui est dans `localStorage` est lisible et modifiable par l'utilisateur (DevTools → Application) et par tout script de la page. Des préférences, oui. Un secret, non.

---

## 5. Remettre à zéro à la déconnexion

```tsx
import { create } from "zustand";

type CarteJeu = { id: string; valeur: number };
type EtatPartie = { idPartie: string | null; main: CarteJeu[]; scores: Record<string, number> };

const DEPART: EtatPartie = { idPartie: null, main: [], scores: {} };

export const usePartie = create<EtatPartie & { appliquer: (e: EtatPartie) => void; vider: () => void }>()((set) => ({
    ...DEPART,
    appliquer: (e) => set(e),
    vider: () => set(DEPART),
}));
```

- `DEPART` est déclaré **une fois**, hors du store, et sert deux fois : à la création et dans `vider`.
- À la déconnexion : `usePartie.getState().vider()`, de même pour le chat. Sinon le joueur suivant sur le même navigateur voit la dernière main de l'autre.
- `set(DEPART)` **fusionne** : les fonctions `appliquer` et `vider` restent dans le store.

---

## 🧪 À essayer

1. Affiche `BandeauFaux` et lis la console. Remplace par `BandeauJuste`, puis par deux sélecteurs simples.
2. Ajoute un `console.log("rendu")` dans `AfficheScore`, et un bouton qui change un **autre** champ du store. Le composant se redessine-t-il ?
3. Avec `usePrefs`, change le volume, recharge la page, puis ouvre DevTools → Application → Local Storage. Modifie la valeur à la main et recharge.
4. Renomme `volume` en `volumeMusique` dans `usePrefs` sans toucher à `version`. Que contient le store au rechargement ? Ajoute `version: 1` et une fonction `migrate`.
5. Écris `vider: () => set(DEPART, true)` (le second argument remplace au lieu de fusionner). Appelle `vider()` puis `appliquer(...)` : que se passe-t-il ?
