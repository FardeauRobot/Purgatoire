# 12. Performance et débogage

> **TL;DR.** Un composant se redessine quand **son état change** ou quand **son parent se redessine**. C'est presque toujours assez rapide : ne cherche à l'éviter que si tu **mesures** un problème. L'outil, c'est l'extension **React Developer Tools** (onglets *Components* et *Profiler*). Les remèdes, du plus simple au plus fin : **déplacer l'état** au plus près de qui l'utilise, lire un store avec un **sélecteur** précis, puis `memo` + `useCallback` / `useMemo`. `memo` seul ne sert à rien si on lui passe une fonction ou un objet créés pendant le rendu. Et pour les bugs, la seconde moitié de la fiche fait correspondre chaque message d'erreur exact à sa cause.

Liens : [`INDEX`](INDEX.md) · [`ETAT`](ETAT.md) · [`EFFETS`](EFFETS.md) · [`ZUSTAND`](ZUSTAND.md) · [`ATELIER_PARTIE`](ATELIER_PARTIE.md) · [`../react-three-fiber/PERFORMANCE_DEBUG`](../react-three-fiber/PERFORMANCE_DEBUG.md) (la 3D, qui a ses propres règles)

🧪 Labo : aucun. Les comptes de rendus viennent d'une sonde React 19.3.1 dans Chrome ; les messages d'erreur ont été déclenchés exprès et recopiés tels quels.

---

## 1. Quand un composant se redessine

- Son **état** change (`useState`, `useReducer`, un store qu'il lit avec un sélecteur dont le résultat change, un contexte qu'il lit).
- Son **parent** se redessine. Par défaut, **tous** ses enfants se redessinent avec lui, que leurs props aient changé ou non.

« Se redessiner » veut dire **rappeler la fonction** du composant. React compare ensuite le JSX obtenu avec le précédent et ne touche au DOM que là où il diffère. Un rendu inutile coûte donc l'appel de la fonction, pas une modification de la page.

🔗 **Lien C.** C'est un `make` sans dépendances précises : tout est recompilé, mais l'éditeur de liens ne réécrit le binaire que si quelque chose a changé. Le coût est la compilation inutile, pas le résultat.

---

## 2. Ce que fait `memo`, mesuré

Un parent avec un compteur, cliqué trois fois. Chaque enfant compte ses rendus (sans StrictMode ; avec, chaque nombre est doublé) :

```tsx
import { memo, useCallback, useState } from "react";

const Fiche = memo(function Fiche({ onChoisir }: { onChoisir: () => void }) {
    return <button onClick={onChoisir}>Choisir</button>;
});

export function Parent() {
    const [n, setN] = useState(0);
    const choisir = useCallback(() => console.log("choisi"), []);
    return (
        <>
            <button onClick={() => setN((x) => x + 1)}>{n}</button>
            <Fiche onChoisir={choisir} />
        </>
    );
}
```

| Enfant | Rendus en plus après 3 clics |
|---|---|
| Composant normal | **+3** |
| `memo`, prop `onChoisir={() => …}` écrite dans le JSX | **+3** |
| `memo`, prop `onChoisir={choisir}` venant de `useCallback` | **+0** |
| `memo`, prop `style={{ color: "red" }}` écrite dans le JSX | **+3** |
| `memo`, sans prop | **+0** |

- `memo(Composant)` : avant de redessiner, React compare chaque prop avec la précédente par **adresse** (`Object.is`). Si toutes sont identiques, il saute le rendu.
- Une fonction ou un objet écrits dans le JSX sont **recréés à chaque rendu** du parent : nouvelle adresse, donc « changés ». `memo` est contourné, sans aucun avertissement.
- `useCallback(fn, deps)` renvoie **la même fonction** tant que `deps` ne change pas. `useMemo(() => valeur, deps)` fait de même pour un objet, un tableau ou un calcul.
- Les deux ne servent qu'en **paire** : `useCallback` sans `memo` sur l'enfant ne fait rien gagner, `memo` sans `useCallback` non plus.

🔗 **Lien C.** C'est la mutation en place d'[`ETAT`](ETAT.md), §3, vue de l'autre côté : là, même adresse avec un contenu nouveau (React ne voit rien) ; ici, adresse nouvelle avec un contenu identique (React voit un changement).

---

## 3. Avant `memo` : les remèdes simples

Dans l'ordre où les essayer :

1. **Descendre l'état.** Un état qui change souvent (un chrono, la position de la souris, le texte d'un champ) doit vivre dans le **plus petit** composant qui l'affiche. Dans [`ATELIER_PARTIE`](ATELIER_PARTIE.md), le tic de 250 ms est dans `Chrono` et ne redessine que lui.
2. **Lire un store avec un sélecteur précis.** `usePartie((s) => s.aura)` ne redessine que quand l'aura change ([`ZUSTAND`](ZUSTAND.md)). `usePartie()` sans sélecteur redessine à **chaque** changement du store.
3. **Passer des `children`.** Un composant qui reçoit `children` ne les redessine pas quand son propre état change : ils ont été créés par **son** parent.
4. Seulement ensuite : `memo` + `useCallback` / `useMemo`, sur le composant que le profileur montre comme coûteux (une longue liste, un graphique).

`useMemo` sert aussi à éviter un **calcul** coûteux à chaque rendu :

```tsx
import { useMemo } from "react";

type Partie = { id: number; adversaire: string; victoire: boolean };

export function Statistiques({ parties }: { parties: Partie[] }) {
    const ratio = useMemo(() => {
        const victoires = parties.filter((p) => p.victoire).length;
        return parties.length === 0 ? 0 : victoires / parties.length;
    }, [parties]);
    return <p>{Math.round(ratio * 100)} % de victoires</p>;
}
```

- Le calcul n'est refait que si le tableau `parties` change d'adresse. Pour quelques dizaines de parties, c'est inutile ; pour un historique de milliers, ça se mesure.

---

## 4. Mesurer : React Developer Tools

Extension de navigateur (Chrome, Firefox). Elle ajoute deux onglets aux DevTools :

- **Components** : l'arbre des composants, avec les props, l'état et les hooks de celui que tu sélectionnes, modifiables à la main. Dans ses réglages, **« Highlight updates when components render »** entoure chaque composant au moment où il se redessine : un chrono qui fait clignoter tout l'écran se voit immédiatement.
- **Profiler** : enregistre une interaction, puis montre pour chaque rendu quels composants ont été rappelés, combien de temps ils ont pris et **pourquoi** (réglage « Record why each component rendered »).

Ces onglets n'ont pas été testés dans cette fiche (l'extension se pilote à la main) : leur description vient de la documentation.

🚨 Mesure sur un build de **production** (`npm run build && npm run preview`) avant de conclure : le mode dev est plus lent (vérifications en plus, double rendu de StrictMode).

---

## 5. Message d'erreur → cause

Tous ces messages ont été déclenchés exprès dans une sonde et recopiés tels quels. La colonne « Fiche » renvoie à la notion en cause.

| Message exact (début) | Cause | Fiche |
|---|---|---|
| `Too many re-renders. React limits the number of renders to prevent an infinite loop.` | Un `setX(…)` appelé **pendant** le rendu, souvent `onClick={setX(1)}` au lieu de `onClick={() => setX(1)}` : le `set` part à chaque rendu, qui en provoque un autre. | [`ETAT`](ETAT.md) |
| `Maximum update depth exceeded.` | Observé avec un sélecteur Zustand qui fabrique un objet. Autre cause classique (non déclenchée ici) : un effet qui modifie une de ses propres dépendances. | [`ZUSTAND`](ZUSTAND.md), [`EFFETS`](EFFETS.md) |
| `The result of getSnapshot should be cached to avoid an infinite loop` | Un sélecteur de store qui renvoie un objet ou un tableau neuf : `useShallow`. | [`ZUSTAND`](ZUSTAND.md) |
| `Objects are not valid as a React child (found: object with keys {a})` | Un objet affiché tel quel dans le JSX : affiche un de ses champs. | [`COMPOSANTS_JSX`](COMPOSANTS_JSX.md) |
| `Each child in a list should have a unique "key" prop.` | Un `.map` sans `key`, ou avec des clés en double. | [`COMPOSANTS_JSX`](COMPOSANTS_JSX.md) |
| `React has detected a change in the order of Hooks called by …` | Un hook dans un `if`, après un `return` anticipé, ou dans une boucle. | [`EFFETS`](EFFETS.md) |
| ``You provided a `value` prop to a form field without an `onChange` handler.`` | Un champ contrôlé sans `onChange` : ajoute-le, ou utilise `defaultValue`. | [`REFS_FORMULAIRES`](REFS_FORMULAIRES.md) |
| (lint) `Calling setState synchronously within an effect can trigger cascading renders` | Un état dérivé calculé dans un effet : calcule-le pendant le rendu. | [`EFFETS`](EFFETS.md) |
| (lint) `React Hook useEffect has missing dependencies: …` | Une valeur lue dans l'effet absente du tableau de dépendances. C'est un **warning** : il ne bloque pas le build, mais il a presque toujours raison. | [`EFFETS`](EFFETS.md) |
| (lint) `Fast refresh only works when a file only exports components.` | Un `.tsx` qui exporte un composant **et** un hook, un store ou une constante : un fichier par rôle. | [`CONTEXTE_HOOKS`](CONTEXTE_HOOKS.md) |
| `TypeError: Failed to fetch` | Réseau coupé, serveur arrêté, ou réponse bloquée par **CORS** (lis la ligne au-dessus dans la console). | [`API`](API.md) |

### Symptômes sans message

| Symptôme | Cause probable | Fiche |
|---|---|---|
| Je clique, l'état change (vu avec `console.log`), l'écran non | Mutation en place (`tab.push`, `obj.x = …`) suivie d'un `set` avec la même adresse. | [`ETAT`](ETAT.md), §3 |
| Trois `setN(n + 1)` n'ajoutent que 1 | `n` est figé dans ce rendu : `setN((x) => x + 1)`. | [`ETAT`](ETAT.md), §2 |
| Un intervalle affiche toujours la même valeur | Fermeture figée dans un effet à `[]` : forme fonction, ou dépendances. | [`EFFETS`](EFFETS.md), §5 |
| Tout arrive en double en dev, pas en prod | StrictMode monte deux fois : un effet sans nettoyage. | [`EFFETS`](EFFETS.md), [`TEMPS_REEL`](TEMPS_REEL.md) |
| L'état d'un élément de liste passe à son voisin | `key={index}` dans une liste qui change. | [`COMPOSANTS_JSX`](COMPOSANTS_JSX.md) |
| Un `0` s'affiche tout seul | `{liste.length && …}` : écris `liste.length > 0 &&`. | [`COMPOSANTS_JSX`](COMPOSANTS_JSX.md) |
| Une classe Tailwind n'a aucun effet | Classe construite (`bg-${x}-500`) : jamais générée. | [`TAILWIND`](TAILWIND.md), §4 |
| Une variable d'environnement vaut `undefined` | Elle ne commence pas par `VITE_`, ou Vite n'a pas été relancé. | [`INSTALLATION`](INSTALLATION.md) |
| Le front marche en local, pas une fois déployé | URL de l'API figée au build, ou chemin non relayé par Caddy. | [`API`](API.md), §4 |
| `F5` sur une page donne `404` en prod | Pas de repli vers `index.html` sur le serveur de fichiers. | [`ROUTER`](ROUTER.md), §4 |

---

## 🧪 À essayer

1. Recopie le `Parent` du §2 avec un `console.log` dans `Fiche`. Remplace `onChoisir={choisir}` par `onChoisir={() => console.log("choisi")}` et compte les rendus.
2. Retire `memo` de `Fiche` mais garde `useCallback`. Le gain disparaît-il ?
3. Installe React Developer Tools, active **Highlight updates**, ouvre l'écran d'[`ATELIER_PARTIE`](ATELIER_PARTIE.md). Qu'est-ce qui clignote à chaque tic du chrono ? Puis déplace le chrono dans `EcranPartie` et regarde de nouveau.
4. Dans le **Profiler**, enregistre un clic sur une carte. Quels composants ont été rappelés, et pourquoi ?
5. Choisis trois messages du tableau du §5 et déclenche-les toi-même dans un projet de test, sans regarder la fiche indiquée.
