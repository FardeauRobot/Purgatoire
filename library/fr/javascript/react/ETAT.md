# 2. L'état : `useState`, l'immutabilité, `useReducer`

> **TL;DR.** L'**état** est la mémoire d'un composant entre deux rendus. `const [n, setN] = useState(0)` : `n` est la valeur **de ce rendu-ci**, `setN` demande un **nouveau rendu** avec une nouvelle valeur. Trois règles : (1) appeler le setter ne change pas `n` tout de suite ; (2) quand la nouvelle valeur dépend de l'ancienne, passe une **fonction** (`setN(x => x + 1)`) ; (3) ne **modifie jamais** un objet ou un tableau d'état : fabriques-en un nouveau (`[...main, carte]`). Ce qui se calcule à partir de l'état n'est pas un état. Quand plusieurs actions modifient le même état, regroupe-les dans un `useReducer`.

Liens : [`INDEX`](INDEX.md) · [`COMPOSANTS_JSX`](COMPOSANTS_JSX.md) · [`EFFETS`](EFFETS.md) · [`ZUSTAND`](ZUSTAND.md) · [`../VERS_TRANSCENDANCE`](../VERS_TRANSCENDANCE.md) (§2, la règle d'or) · [`../OBJETS_TABLEAUX`](../OBJETS_TABLEAUX.md)

🧪 Labo : aucun. Les observations de cette fiche viennent de sondes lancées dans Chrome (voir [`INDEX`](INDEX.md)).

---

## 1. Un compteur, et ce qui se passe vraiment

```tsx
import { useState } from "react";

export function Compteur() {
    const [n, setN] = useState(0);

    return (
        <button onClick={() => setN(n + 1)}>
            Cliqué {n} fois
        </button>
    );
}
```

- `useState(0)` : « réserve-moi une case mémoire, qui vaut 0 au premier rendu ». La fonction renvoie un tableau de deux éléments, qu'on déstructure.
- Au clic, `setN(n + 1)` **ne modifie pas** `n`. Il range la nouvelle valeur et **programme** un nouvel appel de `Compteur`. Lors de ce nouvel appel, `useState(0)` renvoie 1 : le `0` ne sert qu'au tout premier rendu.
- Le nom `n` est une `const`. Il ne change jamais pendant un rendu : chaque rendu a **son propre** `n`.
- 🚨 `onClick={() => setN(n + 1)}` donne **une fonction** à appeler plus tard. `onClick={setN(n + 1)}` appelle `setN` **pendant le rendu**, qui programme un rendu, qui rappelle `setN`… Observé : `Too many re-renders. React limits the number of renders to prevent an infinite loop.` TypeScript le refuse d'ailleurs aussi : `setN(…)` renvoie `void`, pas une fonction.

Pourquoi pas une simple variable ?

```text
let n = 0;            ← dans le composant : remis à 0 à chaque appel de la fonction
n++;                  ← et React ne sait pas qu'il faut ré-afficher
```

🔗 **Lien C.** Une variable locale meurt à la fin de la fonction. Une `static` survivrait, mais serait partagée par toutes les instances du composant, et personne ne préviendrait l'écran. `useState`, c'est une case `static` **par instance**, avec un « préviens l'écran quand ça change » intégré.

---

## 2. La valeur ne change pas tout de suite

Observé avec un bouton qui fait trois fois `setN(n + 1)`, puis un `console.log(n)` :

```tsx
import { useState } from "react";

export function Triple() {
    const [n, setN] = useState(0);
    const [m, setM] = useState(0);

    function cliquer() {
        setN(n + 1);
        setN(n + 1);
        setN(n + 1);
        console.log("n juste après :", n);

        setM((x) => x + 1);
        setM((x) => x + 1);
        setM((x) => x + 1);
    }

    return <button onClick={cliquer}>n = {n}, m = {m}</button>;
}
```

| Après un clic | Résultat observé |
|---|---|
| `n` | **1**, pas 3 |
| `console.log` | `n juste après : 0` (l'ancienne valeur) |
| `m` | **3** |
| Nombre de rendus | **un seul** pour les six appels |

- `setN(n + 1)` trois fois, c'est trois fois `setN(0 + 1)` : `n` vaut 0 pendant tout ce rendu.
- `setM(x => x + 1)` passe une **fonction de mise à jour**. React les applique en file : 0 → 1 → 2 → 3.
- React **regroupe** tous les setters d'un même événement en un seul rendu (*batching*). Observé aussi dans un `setTimeout` : deux setters, un seul rendu.

**Règle :** dès que la nouvelle valeur dépend de l'ancienne, utilise la forme `setX(x => …)`. C'est indispensable dans un intervalle ou un écouteur réseau ([`EFFETS`](EFFETS.md), §4), où le `n` vu par la fonction peut être très vieux.

---

## 3. L'immutabilité : fabriquer, pas modifier

`VERS_TRANSCENDANCE`, §2, pose la règle d'or. Voici ce qui se passe quand on l'enfreint.

```tsx
import { memo, useState } from "react";

const Compte = memo(function Compte({ liste }: { liste: number[] }) {
    return <p>{liste.length} cartes</p>;
});

export function Mutation() {
    const [liste, setListe] = useState<number[]>([]);

    return (
        <>
            <button onClick={() => { liste.push(1); setListe(liste); }}>muter</button>
            <button onClick={() => setListe([...liste, 1])}>copier</button>
            <Compte liste={liste} />
        </>
    );
}
```

Observé : trois clics sur « muter » → l'écran reste à **0 carte**. Puis un clic sur « copier » → l'écran passe directement à **4** : les trois `push` étaient bien dans le tableau, invisibles.

- React compare l'ancien et le nouvel état avec `Object.is`, c'est-à-dire **par adresse**. `liste.push(1)` puis `setListe(liste)`, c'est la même adresse : pour React, rien n'a changé.
- Selon le moment, React peut tout de même rappeler le composant une fois (observé dans une autre sonde), mais les enfants protégés par `memo` comparent aussi par adresse et ne bougent pas. Le comportement n'est pas fiable : c'est un bug, même quand « ça a l'air de marcher ».

Les copies à connaître :

| Intention | Mutation (❌) | Copie (✅) |
|---|---|---|
| Ajouter | `main.push(c)` | `[...main, c]` |
| Retirer | `main.splice(i, 1)` | `main.filter((x) => x.id !== id)` |
| Remplacer un élément | `main[i] = c` | `main.map((x) => (x.id === c.id ? c : x))` |
| Trier | `main.sort(f)` | `[...main].sort(f)` ou `main.toSorted(f)` |
| Changer un champ | `joueur.score = 10` | `{ ...joueur, score: 10 }` |
| Champ imbriqué | `partie.joueur.score = 10` | `{ ...partie, joueur: { ...partie.joueur, score: 10 } }` |

🚨 `.sort()` et `.reverse()` **modifient** le tableau en place, alors qu'ils ont l'air de « renvoyer un résultat ». Copie d'abord.

🔗 **Lien C.** Comparer deux états par adresse, c'est `if (ancien == nouveau)` sur deux pointeurs. Si tu modifies la zone pointée puis repasses le même pointeur, le test dit « identique ». React a choisi cette comparaison parce qu'elle coûte une instruction ; en échange, c'est à toi de fournir une nouvelle adresse quand le contenu change.

---

## 4. Ce qui n'est pas un état

```tsx
import { useState } from "react";

type CarteJeu = { id: string; valeur: number };

export function Selection({ main }: { main: CarteJeu[] }) {
    const [choisies, setChoisies] = useState<string[]>([]);

    const total = main
        .filter((c) => choisies.includes(c.id))
        .reduce((s, c) => s + c.valeur, 0);
    const peutJouer = choisies.length > 0 && choisies.length <= 5;

    function basculer(id: string) {
        setChoisies((avant) =>
            avant.includes(id) ? avant.filter((x) => x !== id) : [...avant, id],
        );
    }

    return (
        <div>
            {main.map((c) => (
                <button key={c.id} onClick={() => basculer(c.id)} aria-pressed={choisies.includes(c.id)}>
                    {c.valeur}
                </button>
            ))}
            <p>Total : {total}</p>
            <button disabled={!peutJouer}>Jouer</button>
        </div>
    );
}
```

- `total` et `peutJouer` sont **calculés** à chaque rendu à partir de `main` et `choisies`. Ce ne sont pas des états.
- En faire des états (`const [total, setTotal] = useState(0)`) obligerait à les tenir à jour à chaque changement de sélection : un oubli, et l'affichage ment.
- On range les **identifiants** des cartes choisies, pas des copies des cartes : si la carte change dans `main`, la sélection ne garde pas une vieille version.

**Règle :** garde le **minimum** dans l'état, et calcule le reste. Si le calcul est vraiment coûteux, il y a `useMemo` ([`PERFORMANCE_DEBUG`](PERFORMANCE_DEBUG.md)), mais un `filter` sur huit cartes ne l'est pas.

🔗 **Lien C++.** Ne stocke pas ce que tu peux recalculer : c'est la même règle que de ne pas garder un champ `_size` dans une classe dont on peut toujours demander la taille au conteneur. Deux sources de vérité finissent toujours par diverger.

---

## 5. Remonter l'état

Deux composants frères qui doivent voir la même donnée ne peuvent pas se la passer directement. On place l'état chez leur **parent commun**, qui le donne aux deux par les props, avec une fonction pour le modifier :

```tsx
import { useState } from "react";

function Main({ choisie, onChoisir }: { choisie: string | null; onChoisir: (id: string) => void }) {
    return (
        <div>
            {["c1", "c2", "c3"].map((id) => (
                <button key={id} onClick={() => onChoisir(id)} aria-pressed={id === choisie}>{id}</button>
            ))}
        </div>
    );
}

function Apercu({ choisie }: { choisie: string | null }) {
    return <p>{choisie ? `Carte ${choisie} sélectionnée` : "Aucune carte"}</p>;
}

export function Table() {
    const [choisie, setChoisie] = useState<string | null>(null);
    return (
        <>
            <Main choisie={choisie} onChoisir={setChoisie} />
            <Apercu choisie={choisie} />
        </>
    );
}
```

- Les données **descendent** (props), les événements **remontent** (fonctions passées en props, nommées `onQuelqueChose` par convention).
- `Main` ne sait pas ce qui se passe quand on choisit une carte. Il prévient, c'est tout.
- Quand l'état doit être vu par des composants très éloignés (le pseudo du joueur connecté, l'état de la partie), le passer d'étage en étage devient pénible : c'est le rôle du contexte ([`CONTEXTE_HOOKS`](CONTEXTE_HOOKS.md)) ou d'un store ([`ZUSTAND`](ZUSTAND.md)).

---

## 6. `useReducer` : toutes les transitions au même endroit

Quand un état a plusieurs façons de changer (sélectionner, défausser, jouer, piocher), les `setX` éparpillés dans les gestionnaires deviennent difficiles à suivre. `useReducer` rassemble les règles dans une **fonction pure** :

```tsx
import { useReducer } from "react";

type CarteJeu = { id: string; valeur: number };
type Etat = { main: CarteJeu[]; choisies: string[]; defausses: number };
type Action =
    | { type: "basculer"; id: string }
    | { type: "defausser" }
    | { type: "piocher"; cartes: CarteJeu[] };

function reduire(etat: Etat, action: Action): Etat {
    switch (action.type) {
        case "basculer": {
            const deja = etat.choisies.includes(action.id);
            if (!deja && etat.choisies.length >= 5) return etat;
            return {
                ...etat,
                choisies: deja ? etat.choisies.filter((x) => x !== action.id) : [...etat.choisies, action.id],
            };
        }
        case "defausser":
            if (etat.defausses === 0 || etat.choisies.length === 0) return etat;
            return {
                main: etat.main.filter((c) => !etat.choisies.includes(c.id)),
                choisies: [],
                defausses: etat.defausses - 1,
            };
        case "piocher":
            return { ...etat, main: [...etat.main, ...action.cartes] };
    }
}

const DEPART: Etat = { main: [], choisies: [], defausses: 3 };

export function Partie() {
    const [etat, dispatch] = useReducer(reduire, DEPART);

    return (
        <div>
            {etat.main.map((c) => (
                <button key={c.id} onClick={() => dispatch({ type: "basculer", id: c.id })}>{c.valeur}</button>
            ))}
            <button onClick={() => dispatch({ type: "defausser" })} disabled={etat.defausses === 0}>
                Défausser ({etat.defausses})
            </button>
        </div>
    );
}
```

- `reduire(etat, action)` : prend l'état courant et une action, renvoie le **nouvel** état. Jamais de mutation, jamais d'appel réseau : c'est une fonction pure, testable sans navigateur.
- `return etat` : action refusée (6ᵉ carte, plus de défausse). Même adresse, donc React n'a rien à refaire.
- `dispatch({ type: "defausser" })` : le composant **décrit** ce qui s'est passé, le réducteur décide de ce que ça change.
- Le type `Action` est une **union discriminée** ([`../TYPESCRIPT`](../TYPESCRIPT.md)) : dans `case "basculer"`, TypeScript sait que `action.id` existe. Et si tu ajoutes une action sans son `case`, la fonction ne renvoie plus toujours un `Etat` : erreur de compilation (vérifié : `TS2366: Function lacks ending return statement and return type does not include 'undefined'`).

Le bac à sable de Transcendance fonctionne déjà ainsi (`dispatch({ type: 'toggle', … })` dans `Table.tsx`).

🔗 **Lien C.** Le réducteur, c'est une machine à états avec un `switch` sur l'événement, exactement comme le traitement des touches dans `so_long` ou le parseur de `minishell`. La différence : au lieu de modifier une `struct` globale, il renvoie une nouvelle `struct`.

---

## 🧪 À essayer

1. Reproduis le composant `Triple` du §2. Remplace les trois `setN(n + 1)` par `setN(n + 1); setN(n + 2);` : que vaut `n` après un clic ? Pourquoi ?
2. Reproduis `Mutation` du §3. Clique trois fois sur « muter », puis une fois sur « copier ». Retire le `memo` autour de `Compte` : le comportement change-t-il ?
3. Dans `Selection`, transforme `total` en état mis à jour dans `basculer`. Puis fais changer `main` depuis le parent (une carte retirée) : le total est-il encore juste ?
4. Dans `reduire`, écris `etat.choisies.push(action.id); return etat;` pour le cas `basculer`. Que voit-on à l'écran ?
5. Ajoute une action `{ type: "jouer" }` au type `Action` sans écrire son `case`. Lis l'erreur de TypeScript.
