# 3. Les effets : `useEffect`, le nettoyage, et quand s'en passer

> **TL;DR.** Un **effet** synchronise ton composant avec quelque chose qui **n'est pas React** : une connexion réseau, un minuteur, un écouteur sur `window`, une requête `fetch`. Il s'exécute **après** l'affichage, puis de nouveau chaque fois qu'une valeur de son tableau de **dépendances** change. Il renvoie une fonction de **nettoyage**, qui défait ce qu'il a fait. En dev, `StrictMode` le monte, le nettoie et le remonte exprès : si ton nettoyage est faux, ça se voit tout de suite. Et surtout : un calcul à partir des props ou de l'état **n'est pas** un effet.

Liens : [`INDEX`](INDEX.md) · [`ETAT`](ETAT.md) · [`INSTALLATION`](INSTALLATION.md) (§3, StrictMode) · [`API`](API.md) · [`TEMPS_REEL`](TEMPS_REEL.md) · [`../ASYNCHRONE`](../ASYNCHRONE.md)

🧪 Labo : aucun.

---

## 1. La forme d'un effet

```tsx
import { useEffect, useState } from "react";

export function Chrono({ actif }: { actif: boolean }) {
    const [secondes, setSecondes] = useState(0);

    useEffect(() => {
        if (!actif) return;
        const id = setInterval(() => setSecondes((s) => s + 1), 1000);
        return () => clearInterval(id);
    }, [actif]);

    return <p>{secondes} s</p>;
}
```

- `useEffect(fonction, [dépendances])` : React appelle `fonction` **après** avoir mis l'écran à jour.
- `[actif]` : il la rappelle seulement si `actif` a changé depuis le rendu précédent. Avant de la rappeler, il exécute le **nettoyage** de la fois d'avant.
- `return () => clearInterval(id)` : le nettoyage. Il tourne aussi quand le composant disparaît.
- `if (!actif) return;` : rien à faire, donc rien à nettoyer.

Le déroulé quand `actif` passe de `true` à `false` :

```text
rendu (actif = true)   → effet : setInterval(#1)
rendu (actif = false)  → nettoyage : clearInterval(#1) → effet : return tout de suite
démontage              → (rien à nettoyer)
```

| Tableau de dépendances | L'effet tourne… |
|---|---|
| absent | après **chaque** rendu |
| `[]` | une fois au montage (deux fois en dev, voir §3) |
| `[a, b]` | au montage, puis quand `a` ou `b` change (comparés avec `Object.is`) |

🔗 **Lien C.** Effet + nettoyage, c'est `open` + `close`, `malloc` + `free`, `signal` + remise à `SIG_DFL`. Chaque ressource prise dans l'effet est rendue dans le nettoyage. Un effet sans nettoyage qui ouvre quelque chose, c'est une fuite.

---

## 2. Les dépendances ne se choisissent pas

Les dépendances, ce sont **toutes** les valeurs du composant (props, état, fonctions déclarées dedans) que l'effet utilise. Ce n'est pas une option « quand veux-tu que ça se relance ».

Le lint de l'équipe (`eslint-plugin-react-hooks`) les vérifie. Observé sur un effet qui utilise `n` et `salon` avec `[]` :

```text
warning  React Hook useEffect has missing dependencies: 'n' and 'salon'. Either include them
         or remove the dependency array. You can also do a functional update 'setN(n => ...)'
         if you only need 'n' in the 'setN' call.
```

🚨 C'est un **warning**, pas une erreur : `npm run lint` passe quand même. Traite-le comme une erreur.

Si ajouter une dépendance relance l'effet trop souvent, ne la retire pas : change le code pour ne plus en avoir besoin. Le message le dit : la forme `setN(n => …)` supprime la dépendance à `n`.

---

## 3. Pourquoi l'effet tourne deux fois en dev

Rappel d'[`INSTALLATION`](INSTALLATION.md), §3 : sous `StrictMode`, React fait **monté → nettoyé → monté**. Observé avec une requête annulable :

```tsx
import { useEffect, useState } from "react";

export function Profil({ id }: { id: number }) {
    const [nom, setNom] = useState<string | null>(null);
    const [erreur, setErreur] = useState<string | null>(null);

    useEffect(() => {
        const ctrl = new AbortController();
        fetch(`/api/v1/users/${id}`, { signal: ctrl.signal })
            .then((r) => {
                if (!r.ok) throw new Error(`HTTP ${r.status}`);
                return r.json() as Promise<{ username: string }>;
            })
            .then((u) => setNom(u.username))
            .catch((e: unknown) => {
                if (e instanceof DOMException && e.name === "AbortError") return;
                setErreur(e instanceof Error ? e.message : String(e));
            });
        return () => ctrl.abort();
    }, [id]);

    if (erreur) return <p>Erreur : {erreur}</p>;
    return <p>{nom ?? "Chargement…"}</p>;
}
```

Console observée en dev :

```text
requête lancée
requête lancée
requête annulée      ← la première, coupée par le nettoyage
réponse utilisée     ← une seule réponse arrive dans l'état
```

- `AbortController` : `ctrl.abort()` coupe la requête, et sa promesse échoue avec une `AbortError`. On l'ignore : ce n'est pas une vraie erreur.
- Le même nettoyage règle un bug de **production** : si `id` passe de 1 à 2 pendant que la requête 1 est en route, la réponse 1 pourrait arriver **après** la 2 et afficher le mauvais profil. Avec `abort()`, la requête 1 est annulée dès que `id` change.
- Les `setNom` / `setErreur` sont appelés **dans un `.then`**, donc plus tard, quand la réponse arrive. C'est permis (voir §4).

🔗 **Lien C.** Deux requêtes en vol dont la plus lente gagne, c'est une *race condition*, comme deux threads de `philosophers` qui écrivent la même variable. Ici, pas de mutex : on annule l'ancienne.

En pratique, pour charger des données au changement de page, React Router a les **loaders**, qui font ce travail pour toi ([`ROUTER`](ROUTER.md)).

---

## 4. Vous n'avez peut-être pas besoin d'un effet

L'erreur la plus fréquente : utiliser un effet pour calculer un état à partir d'un autre.

```tsx
import { useEffect, useState } from "react";

export function TotalFaux({ main }: { main: number[] }) {
    const [total, setTotal] = useState(0);
    useEffect(() => {
        setTotal(main.reduce((s, x) => s + x, 0));
    }, [main]);
    return <p>{total}</p>;
}

export function TotalJuste({ main }: { main: number[] }) {
    const total = main.reduce((s, x) => s + x, 0);
    return <p>{total}</p>;
}
```

- `TotalFaux` : un premier rendu avec un total **faux** (0), puis l'effet, puis un second rendu. Deux rendus, et un instant où l'écran ment.
- `TotalJuste` : le calcul se fait pendant le rendu. Un rendu, toujours juste ([`ETAT`](ETAT.md), §4).

Le lint de l'équipe l'interdit (observé, c'est une **erreur**) :

```text
error  Error: Calling setState synchronously within an effect can trigger cascading renders
       … Avoid calling setState() directly within an effect          react-hooks/set-state-in-effect
```

| Tu veux… | Pas d'effet, plutôt… |
|---|---|
| Calculer une valeur à partir des props ou de l'état | Un calcul dans le corps du composant |
| Réagir à un clic (envoyer le coup joué) | Le code dans le **gestionnaire** `onClick` |
| Remettre un état à zéro quand une prop change | Une `key` sur le composant : `<Partie key={idPartie} />` le recrée |
| Charger les données d'une page | Un loader de React Router ([`ROUTER`](ROUTER.md)) |

Un effet reste le bon outil pour : un minuteur, une connexion Socket.IO ([`TEMPS_REEL`](TEMPS_REEL.md)), un écouteur sur `window` ou `document`, une bibliothèque qui n'est pas React (un canvas, un graphique).

🔗 **Lien C.** Un effet qui recalcule un état, c'est un `Makefile` dont une règle regénère un `.h` à partir d'un autre `.h` à chaque compilation, ce qui relance une compilation. Le calcul direct, c'est une fonction `inline`.

---

## 5. La closure périmée

Chaque rendu a ses propres valeurs ([`ETAT`](ETAT.md), §1). Une fonction créée pendant un rendu **voit les valeurs de ce rendu-là**, pour toujours.

```tsx
import { useEffect, useState } from "react";

export function ChronoFige() {
    const [s, setS] = useState(0);
    useEffect(() => {
        const id = setInterval(() => setS(s + 1), 100);
        return () => clearInterval(id);
        // eslint-disable-next-line react-hooks/exhaustive-deps
    }, []);
    return <p>{s}</p>;
}
```

Observé au bout d'une seconde : **1**. Avec `setS((x) => x + 1)` : **10**.

- L'intervalle est créé au premier rendu, où `s` vaut 0. Il fait donc `setS(0 + 1)` dix fois de suite.
- Le `eslint-disable` est là **pour l'exemple** : c'est précisément le warning du §2 qui aurait signalé le bug. Un `eslint-disable-next-line react-hooks/exhaustive-deps` dans du vrai code doit faire tiquer en relecture.

---

## 6. Un écouteur sur la fenêtre

```tsx
import { useEffect, useState } from "react";

export function useToucheEchap(fermer: () => void) {
    useEffect(() => {
        function surTouche(e: KeyboardEvent) {
            if (e.key === "Escape") fermer();
        }
        window.addEventListener("keydown", surTouche);
        return () => window.removeEventListener("keydown", surTouche);
    }, [fermer]);
}

export function Modale({ onFermer }: { onFermer: () => void }) {
    useToucheEchap(onFermer);
    return <div role="dialog">Règles du jeu</div>;
}

export function Page() {
    const [ouverte, setOuverte] = useState(false);
    return (
        <>
            <button onClick={() => setOuverte(true)}>Règles</button>
            {ouverte && <Modale onFermer={() => setOuverte(false)} />}
        </>
    );
}
```

- `removeEventListener` doit recevoir **la même** fonction qu'`addEventListener` : c'est pourquoi `surTouche` est déclarée dans l'effet, et non écrite en ligne deux fois.
- `useToucheEchap` est un **hook personnalisé** : une fonction qui commence par `use` et appelle des hooks. On en reparle dans [`CONTEXTE_HOOKS`](CONTEXTE_HOOKS.md). Dans le projet, il va dans son propre fichier `useToucheEchap.ts` : le lint de l'équipe refuse un `.tsx` qui exporte à la fois un hook et des composants (`react-refresh/only-export-components`, vérifié).
- `onFermer={() => setOuverte(false)}` crée une fonction neuve à chaque rendu de `Page`, donc l'effet se réabonne à chaque rendu. Ici c'est sans conséquence (un `add` et un `remove` sur `window`). Ce serait gênant pour une connexion réseau : voir [`TEMPS_REEL`](TEMPS_REEL.md).

---

## 7. Les règles des hooks

`useState`, `useEffect` et tous les `use…` obéissent à deux règles :

1. Les appeler **au niveau du composant**, jamais dans un `if`, une boucle ou après un `return` anticipé.
2. Les appeler seulement dans un composant ou dans un autre hook.

React repère les hooks par leur **ordre d'appel**, pas par leur nom. Un `if` qui saute un `useState` décale tous les suivants. Le lint l'attrape (observé) :

```text
error  React Hook "useState" is called conditionally. React Hooks must be called in the exact same
       order in every component render. Did you accidentally call a React Hook after an early return?
```

Et si le code passe quand même, la console affiche à l'exécution : `React has detected a change in the order of Hooks called by …`.

🔗 **Lien C.** React range les états d'un composant dans un tableau, rempli dans l'ordre des appels : `etats[0]`, `etats[1]`… Sauter un appel, c'est lire `etats[1]` en croyant lire `etats[0]`.

---

## 🧪 À essayer

1. Reproduis `ChronoFige`, puis retire le commentaire `eslint-disable` et lance `npm run lint`. Corrige en suivant le message.
2. Dans `Profil`, retire `return () => ctrl.abort()` et regarde l'onglet **Réseau** des DevTools en dev : combien de requêtes ? Puis change `id` très vite (un bouton `id + 1`) avec et sans le nettoyage.
3. Écris `TotalFaux` et lance le lint. Lis l'erreur en entier : elle cite les deux cas où un effet est légitime.
4. Dans `useToucheEchap`, remplace `removeEventListener("keydown", surTouche)` par `removeEventListener("keydown", (e) => surTouche(e))`. Ouvre et ferme la modale cinq fois, puis appuie sur Échap : combien de fois `fermer` est-il appelé ?
5. Écris `if (!actif) return null;` **avant** le `useState` de `Chrono`. Que dit le lint ? Et la console, si tu bascules `actif` ?
