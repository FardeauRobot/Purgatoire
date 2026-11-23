# 📒 Synthèse : React et sa stack

> **TL;DR.** La fiche de révision de [`react/`](INDEX.md). D'abord l'idée, puis le cœur (composants, état, effets, refs, contexte) et la stack (Router, Zustand, API, Socket.IO, Tailwind), chacun réduit à ce qu'il faut savoir par cœur, avec la fiche et le paragraphe qui l'expliquent. Les deux blocs de code sont recopiés de [`ETAT`](ETAT.md) §1 et [`EFFETS`](EFFETS.md) §1 : ce sont les deux formes à savoir réécrire les yeux fermés.

Liens : [`INDEX`](INDEX.md) · [`PHILOSOPHIE`](PHILOSOPHIE.md) · [`ABORDER`](ABORDER.md) · [`../SYNTHESE`](../SYNTHESE.md) · [`../react-three-fiber/SYNTHESE`](../react-three-fiber/SYNTHESE.md)

Versions : React 19.3, Vite 8, React Router 8, Zustand 5, Tailwind 4, Socket.IO 4.8 (voir [`INDEX`](INDEX.md), « Ce qui a été vérifié »).

---

## 1. L'idée en cinq lignes ([`PHILOSOPHIE`](PHILOSOPHIE.md))

- **Le besoin** : un écran qui reflète toujours les données, sans recopier chaque changement à la main dans le DOM.
- **La réponse** : `UI = f(état)`. Tu décris l'écran pour un état ; React calcule les modifications.
- **Le rendu est pur** : on calcule du JSX, on ne **fait** rien pendant le rendu.
- **Données qui descendent, événements qui remontent.**
- **React n'est que l'affichage** : routeur, store, réseau, style et 3D se branchent autour.

---

## 2. Le cœur

### Composants et JSX ([`COMPOSANTS_JSX`](COMPOSANTS_JSX.md))
| À savoir | § |
|---|---|
| Un composant = une fonction **avec une majuscule** qui reçoit **un** objet de props et renvoie du JSX | 1 |
| `className`, `onClick` (camelCase), un seul élément racine ou un fragment `<>…</>` | 1–2 |
| `{…}` accepte une **expression**, pas une instruction (pas de `if`, pas de `for`) | 1 |
| Condition : ternaire ou `&&` ; attention, `{n && …}` avec `n = 0` affiche « 0 » | 3 |
| Liste : `.map()` + une `key` **stable** (un id), jamais l'index d'une liste qui change | 4 |
| `children` : un composant qui en entoure d'autres | 5 |

### État ([`ETAT`](ETAT.md))
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
| Règle | § |
|---|---|
| `n` est la valeur **de ce rendu** ; `setN` programme un **nouveau** rendu | 1–2 |
| Nouvelle valeur calculée à partir de l'ancienne : `setN((x) => x + 1)` | 2 |
| **Ne jamais muter** : `[...main, carte]`, `main.filter(…)`, `{ ...joueur, pv: 3 }` | 3 |
| Ce qui se calcule n'est pas un état | 4 |
| Deux frères partagent une donnée : on la remonte au parent commun | 5 |
| Plusieurs actions sur le même état : `useReducer`, toutes les transitions au même endroit | 6 |

### Effets ([`EFFETS`](EFFETS.md))
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
| Règle | § |
|---|---|
| Un effet synchronise avec ce qui **n'est pas React** (socket, minuteur, `window`) | 1 |
| Il tourne **après** l'affichage, et de nouveau quand une dépendance change | 1 |
| `[]` : au montage ; absent : après chaque rendu ; `[a, b]` : quand `a` ou `b` change | 1 |
| Les dépendances **ne se choisissent pas** : tout ce que l'effet lit | 2 |
| En dev, `StrictMode` monte → nettoie → remonte : un nettoyage faux se voit tout de suite | 3 |
| Un calcul n'est **pas** un effet ; une réaction à un clic non plus | 4 |
| *Closure périmée* : l'effet voit les valeurs du rendu où il a été créé | 5 |
| Hooks : au premier niveau du composant, jamais dans un `if` ou une boucle | 7 |

### Refs et formulaires ([`REFS_FORMULAIRES`](REFS_FORMULAIRES.md))
- `useRef` : une boîte `{ current }` qui **survit** aux rendus sans en **déclencher** (élément du DOM, id de minuteur) (§1).
- React 19 : `ref` se passe comme une prop ordinaire, plus besoin de `forwardRef` (§2).
- Champ **contrôlé** : `value` + `onChange`, l'état fait foi (§3).
- **Action** de formulaire : `<form action={fn}>` + `useActionState` (+ `useFormStatus`), reçoit un `FormData` (§4).

### Contexte et hooks maison ([`CONTEXTE_HOOKS`](CONTEXTE_HOOKS.md))
- Contexte : `createContext`, `<MonContexte value={…}>` en haut (React 19, sans `.Provider`), `use(MonContexte)` en bas (§2).
- Pour ce qui change **rarement** (session, langue, thème). Ce qui change souvent → Zustand (§3).
- Hook personnalisé : une fonction `useQuelqueChose` qui regroupe des hooks (§4).

---

## 3. La stack

| Brique | L'essentiel | Le piège | Fiche |
|---|---|---|---|
| **Vite** | `npm run dev` (rechargement), `npm run build` (typecheck puis `dist/`) | Seules les variables `VITE_` arrivent au navigateur, **lisibles par tous** | [`INSTALLATION`](INSTALLATION.md) §4–5 |
| **React Router 8** | `createBrowserRouter([...])`, layout + `<Outlet />`, `:id` → `useParams()`, **loader** avant l'affichage, `throw redirect(…)`, `<Link>` | La redirection n'est pas une sécurité ; `F5` sur `/salon/1` demande une config serveur | [`ROUTER`](ROUTER.md) §2, §4 |
| **Zustand 5** | `create<T>()((set) => ({…}))`, `useStore((s) => s.morceau)`, `getState()` hors React | Un sélecteur qui renvoie un objet neuf → boucle infinie (`useShallow`) | [`ZUSTAND`](ZUSTAND.md) §2 |
| **API** | **Une** fonction `requete()` : jeton, JSON, statut → exception | `fetch` ne lève rien sur 404/401 : tester `r.ok` ; l'URL de base est figée au build | [`API`](API.md) §1–2, §4 |
| **Tuyau** | Client typé généré par le backend : nom de route, corps et réponse typés | Le registre se régénère après un `git pull` | [`API`](API.md) §5, [`../adonis/TUYAU`](../adonis/TUYAU.md) |
| **Socket.IO** | Une socket dans un module, `autoConnect: false`, jeton dans `auth` ; rejoindre le salon dans `connect` | `socket.off("evt")` sans la fonction retire les écouteurs **de tous** | [`TEMPS_REEL`](TEMPS_REEL.md) §1–4 |
| **Tailwind 4** | Classes utilitaires dans le JSX, `@import "tailwindcss";`, couleurs du projet dans `@theme` | `bg-${couleur}-500` n'est jamais généré : écrire les classes en entier | [`TAILWIND`](TAILWIND.md) §3–4 |

### L'architecture de l'écran de partie ([`ATELIER_PARTIE`](ATELIER_PARTIE.md))
```text
 serveur ── "partie:etat" ──► écouteur Socket.IO (hors React) ──► store.appliquer(état)
                                                                       │
                       chaque composant lit SON morceau ◄──────────────┘
                                   │
 bouton ── "partie:jouer" (intention) ──► serveur décide ──► renvoie l'état
```
Le chrono se calcule à partir de l'heure de fin **envoyée par le serveur**, pas d'un compteur local.

---

## 4. Performance et débogage ([`PERFORMANCE_DEBUG`](PERFORMANCE_DEBUG.md))

- Un composant se redessine quand **son état** change ou quand **son parent** se redessine (§1).
- Remèdes, dans l'ordre : déplacer l'état plus bas → sélecteur Zustand précis → `memo` + `useCallback` / `useMemo` (§3).
- `memo` + une fonction écrite dans le JSX ne sert à rien : la prop est neuve à chaque rendu (§2).
- On **mesure** avec React Developer Tools (*Components*, *Profiler*) avant d'optimiser (§4).
- `Too many re-renders` → un setter appelé pendant le rendu (`onClick={f()}`) (§5).

---

## 5. Les 14 pièges, en une ligne chacun

Recopiés de [`INDEX`](INDEX.md) : `onClick={f()}` · mutation d'état · `key={index}` · `{0 && …}` · effet sans nettoyage · état calculé dans un effet · sélecteur qui fabrique un objet · `fetch` silencieux · `VITE_` absente au build · secret côté client · `socket.off` sans fonction · classe Tailwind concaténée · `memo` + fonction inline · redirection prise pour une sécurité.

---

## 🧠 Questions de révision

> [!question]- Quelles sont les trois questions pour savoir si une donnée est un état ?
> Elle ne change jamais ? Elle vient des props ? Elle se calcule à partir d'autre chose ? Si la réponse est oui à l'une d'elles, ce n'est pas un état. ([`PHILOSOPHIE`](PHILOSOPHIE.md) §7)

> [!question]- Pourquoi `main.push(carte); setMain(main);` ne redessine-t-il pas ?
> C'est le **même** tableau : React compare avec `Object.is`, voit la même référence et conclut que rien n'a changé. Il faut `setMain([...main, carte])`. ([`ETAT`](ETAT.md) §3)

> [!question]- Une liste de joueurs utilise `key={index}`. Le premier quitte. Que se passe-t-il ?
> Chaque élément prend la `key` de son voisin : l'état local (un champ, une sélection) passe au joueur d'à côté. ([`COMPOSANTS_JSX`](COMPOSANTS_JSX.md) §4)

> [!question]- Que renvoie la fonction passée à `useEffect`, et quand est-elle appelée ?
> Une fonction de nettoyage, appelée avant la prochaine exécution de l'effet et au démontage. ([`EFFETS`](EFFETS.md) §1)

> [!question]- Différence entre `useRef` et `useState` ?
> Les deux survivent aux rendus. Changer un état déclenche un rendu ; changer `ref.current` non. ([`REFS_FORMULAIRES`](REFS_FORMULAIRES.md) §1)

> [!question]- À quoi sert un loader de React Router, et que se passe-t-il si les données ne sont pas prêtes ?
> Il charge les données **avant** d'afficher la page : l'URL ne change qu'une fois les données prêtes. Il peut aussi rediriger ou lever une erreur, affichée par l'`ErrorBoundary` de la route. ([`ROUTER`](ROUTER.md) §2)

> [!question]- Dans un composant, comment lire le score dans un store Zustand sans redessiner à chaque autre changement ?
> Avec un sélecteur : `useJeu((s) => s.score)`. Le composant ne se redessine que si `score` change. ([`ZUSTAND`](ZUSTAND.md) §1)

> [!question]- Pourquoi la socket est-elle créée dans un module et pas dans un composant ?
> Un composant est rappelé à chaque rendu et monté deux fois en dev : on créerait plusieurs connexions. Le module est évalué une seule fois. ([`TEMPS_REEL`](TEMPS_REEL.md) §1)

> [!question]- Pourquoi le client n'envoie-t-il pas « j'ai maintenant 25 PV » ?
> Parce que le serveur fait autorité : le client envoie une **intention** (« je joue la carte 3 »), le serveur calcule et renvoie l'état. Sinon, n'importe qui triche avec la console. ([`ATELIER_PARTIE`](ATELIER_PARTIE.md), [`../react-three-fiber/ETAT_RESEAU`](../react-three-fiber/ETAT_RESEAU.md) §5)
