# 🪜 Aborder React

> **TL;DR.** React s'apprend en **deux temps** : d'abord le cœur (composants, état, effets), qui tient en quatre fiches et une idée (`UI = f(état)`) ; ensuite la stack autour (routeur, store, API, socket, style), une brique à la fois. Avant de commencer, il te faut du JavaScript **moderne** (fonctions fléchées, déstructuration, `map`, spread, `async`/`await`, modules) et un minimum de HTML/CSS. Le piège n'est pas la syntaxe : c'est de **continuer à penser en impératif** (« quand on clique, je change ce texte ») au lieu de décrire l'écran pour un état donné.

Liens : [`INDEX`](INDEX.md) · [`PHILOSOPHIE`](PHILOSOPHIE.md) · [`SYNTHESE`](SYNTHESE.md) · [`../ABORDER`](../ABORDER.md) · [`../VERS_TRANSCENDANCE`](../VERS_TRANSCENDANCE.md) · [doc React (fr)](https://fr.react.dev/learn)

---

## 1. Ce que tu dois savoir avant

### Indispensable

| Prérequis | Où React s'en sert | Où le revoir |
|---|---|---|
| Fonctions fléchées, fonctions passées en argument | chaque `onClick={() => …}`, chaque prop `onJouer` | [`../FONCTIONS`](../FONCTIONS.md) §1, §3 |
| **Closures** | chaque rendu « voit » **ses** valeurs d'état ; la *closure périmée* des effets | [`../FONCTIONS`](../FONCTIONS.md) §4, [`EFFETS`](EFFETS.md) §5 |
| Déstructuration d'objet et de tableau | `function Carte({ rang, famille })`, `const [n, setN] = useState(0)` | [`../OBJETS_TABLEAUX`](../OBJETS_TABLEAUX.md) §5 |
| `map`, `filter`, spread `[...t, x]`, `{ ...o, k: v }` | afficher une liste ; mettre à jour un état **sans le modifier** | [`../OBJETS_TABLEAUX`](../OBJETS_TABLEAUX.md) §2, §4 |
| Références et copie superficielle | savoir pourquoi `push` sur un état ne redessine rien | [`../OBJETS_TABLEAUX`](../OBJETS_TABLEAUX.md) §2 |
| Ternaire, `&&`, `??` | l'affichage conditionnel dans le JSX | [`../VARIABLES_TYPES`](../VARIABLES_TYPES.md) §6 |
| `import` / `export` | un composant par fichier | [`../CLASSES_MODULES`](../CLASSES_MODULES.md) §4 |
| `async` / `await`, `fetch` | les appels à l'API, les loaders | [`../ASYNCHRONE`](../ASYNCHRONE.md) |
| `npm`, `package.json` | créer et lancer le projet avec Vite | [`../OUTILS`](../OUTILS.md) |
| Types, unions, `type Props = {…}` | typer les props, les stores, les événements | [`../TYPESCRIPT`](../TYPESCRIPT.md) §1–3 |

### Utile, mais ça s'apprend en route

- **HTML** : les balises de base (`div`, `button`, `ul`/`li`, `form`, `input`). React les écrit pour toi, mais tu dois savoir ce qu'elles sont.
- **CSS** : boîte, `flex`, `grid`. Tailwind n'est que du CSS écrit en classes ([`TAILWIND`](TAILWIND.md)).
- **Le DOM** : savoir qu'une page est un arbre d'éléments qu'on peut modifier en JS. Tu n'auras presque plus à y toucher, mais c'est ce que React modifie à ta place.
- **HTTP** : méthode, chemin, statut, en-tête `Authorization`. Ton webserv t'a déjà appris l'essentiel ([`API`](API.md)).

### Pas nécessaire

Les composants **classes** (`class X extends React.Component`), `componentDidMount`, Redux, `create-react-app` : c'est l'ancien React. Si un tutoriel en parle, il date. Ce dossier est en React 19, avec des fonctions et des hooks.

---

## 2. Le modèle mental à installer

Lis [`PHILOSOPHIE`](PHILOSOPHIE.md) en premier. Puis garde ces quatre phrases en tête pendant toutes les fiches :

1. **Un composant est une fonction que React rappelle** chaque fois que son état ou ses props changent. Il n'y a pas « une » exécution, il y en a une **par rendu**.
2. **L'état est une photo.** `n` ne change pas pendant un rendu ; `setN` en demande un nouveau ([`ETAT`](ETAT.md) §2).
3. **On fabrique, on ne modifie pas.** `[...main, carte]`, pas `main.push(carte)` ([`ETAT`](ETAT.md) §3).
4. **Un effet, c'est pour ce qui n'est pas React**, et il se défait au nettoyage ([`EFFETS`](EFFETS.md) §1, §4).

---

## 3. Comment s'y prendre

### Phase 1 : le cœur (fiches 0 à 5)

1. **Crée un projet jetable** avec Vite ([`INSTALLATION`](INSTALLATION.md)). Garde `StrictMode` : il te montre tes erreurs tôt.
2. **Fais une version statique d'abord.** Avant tout `useState`, affiche des cartes écrites en dur avec des props ([`COMPOSANTS_JSX`](COMPOSANTS_JSX.md)).
3. **Ajoute l'état**, puis demande-toi pour chaque donnée : « est-ce un état, ou ça se calcule ? » ([`ETAT`](ETAT.md) §4).
4. **Seulement ensuite, les effets.** Et pour chacun, vérifie qu'il ne devrait pas être un simple calcul ou un gestionnaire d'événement ([`EFFETS`](EFFETS.md) §4).
5. **Fais chaque « 🧪 À essayer »** : la plupart te font **provoquer** un bug exprès (double rendu, mutation, mauvaise `key`) pour que tu le reconnaisses plus tard.

### Phase 2 : la stack (fiches 6 à 10)

Une brique à la fois, dans cet ordre, parce que chacune s'appuie sur la précédente : **Router** (les pages) → **Zustand** (l'état partagé) → **API** (le serveur) → **Socket.IO** (le temps réel) → **Tailwind** (le style). Lis la fiche Adonis correspondante en parallèle quand une fiche le propose.

### Phase 3 : assembler et mesurer (fiches 11 et 12)

L'[`ATELIER_PARTIE`](ATELIER_PARTIE.md) remet tout ensemble. Installe **React Developer Tools** avant [`PERFORMANCE_DEBUG`](PERFORMANCE_DEBUG.md) et garde son tableau « message d'erreur → cause » sous la main.

### Ensuite

La 3D : [`../react-three-fiber/ABORDER`](../react-three-fiber/ABORDER.md). Tout ce que tu as appris ici s'y applique.

---

## 4. Les erreurs de débutant à éviter

- **Commencer par la stack.** Router, Zustand et Tailwind en même temps que `useState` : tu ne sauras plus quelle brique cause quel bug.
- **Mettre tout dans l'état.** Un total, un filtre appliqué, une liste triée : ça se **calcule** pendant le rendu.
- **Utiliser `useEffect` pour tout.** « Quand X change, mets Y à jour » est presque toujours un calcul, pas un effet.
- **Optimiser trop tôt.** `memo`, `useMemo`, `useCallback` n'ont de sens qu'après une mesure ([`PERFORMANCE_DEBUG`](PERFORMANCE_DEBUG.md) §2).
- **Copier un tutoriel React 16–17.** `forwardRef`, `<Contexte.Provider>`, `ReactDOM.render` : ça marche parfois encore, mais React 19 a plus simple ([`REFS_FORMULAIRES`](REFS_FORMULAIRES.md) §2, [`CONTEXTE_HOOKS`](CONTEXTE_HOOKS.md) §2).
- **Faire confiance au client.** Une page cachée par le routeur n'est pas protégée : c'est le serveur qui refuse ([`ROUTER`](ROUTER.md)).

---

## 5. Tu es prêt pour la 3D et le projet si…

> [!question]- Pourquoi une variable `let n = 0` dans un composant ne peut-elle pas servir de compteur ?
> Elle est recréée à 0 à **chaque** appel du composant, et la modifier ne prévient pas React qu'il faut redessiner. ([`ETAT`](ETAT.md) §1)

> [!question]- Après `setN(n + 1); setN(n + 1);`, de combien `n` a-t-il augmenté ?
> De 1. Les deux appels lisent le même `n` de ce rendu. Pour enchaîner : `setN((x) => x + 1)` deux fois. ([`ETAT`](ETAT.md) §2)

> [!question]- Quelle est la différence entre `onClick={jouer()}` et `onClick={() => jouer()}` ?
> La première **appelle** `jouer` pendant le rendu. La seconde donne une fonction à appeler au clic. ([`ETAT`](ETAT.md) §1)

> [!question]- Pourquoi l'effet tourne-t-il deux fois au démarrage en dev ?
> `StrictMode` monte, nettoie et remonte exprès pour vérifier que ton nettoyage défait bien l'effet. En production, il ne tourne qu'une fois. ([`EFFETS`](EFFETS.md) §3)

> [!question]- Le prix total du panier doit-il être un état ?
> Non : il se calcule à partir de la liste à chaque rendu. ([`ETAT`](ETAT.md) §4)

> [!question]- Contexte ou Zustand pour l'état d'une partie en cours ?
> Zustand : la partie change souvent, et un sélecteur ne redessine que les composants qui lisent le morceau modifié. Le contexte convient aux valeurs qui changent rarement (utilisateur, langue). ([`CONTEXTE_HOOKS`](CONTEXTE_HOOKS.md) §3)

> [!question]- `fetch` renvoie un 401. Ton `try`/`catch` l'attrape-t-il ?
> Non. `fetch` ne lève une erreur que si le réseau échoue ; il faut tester `r.ok`. ([`API`](API.md) §1)
