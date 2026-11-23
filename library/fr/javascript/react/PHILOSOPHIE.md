# 🧭 Philosophie de React : à quoi il sert, comment il pense

> **TL;DR.** Une interface doit **toujours refléter les données**. Écrite à la main, on la modifie morceau par morceau (`el.textContent = …`), et elle finit par **se désynchroniser** de l'état réel. React inverse le problème : tu décris **à quoi l'écran doit ressembler pour un état donné** (`UI = f(état)`), et React calcule lui-même les modifications du DOM. Tout le reste en découle : des **composants** (des fonctions), des données qui **descendent** (props), des événements qui **remontent** (callbacks), un rendu **pur**, un état **jamais modifié en place**, et des **effets** pour tout ce qui sort de React. React n'est **que** la couche d'affichage : routeur, store, appels réseau et style viennent d'autres outils.

Liens : [`INDEX`](INDEX.md) · [`ABORDER`](ABORDER.md) · [`SYNTHESE`](SYNTHESE.md) · [`COMPOSANTS_JSX`](COMPOSANTS_JSX.md) · [`ETAT`](ETAT.md) · [`EFFETS`](EFFETS.md) · [`../PHILOSOPHIE`](../PHILOSOPHIE.md) · [doc React, « Penser en React »](https://fr.react.dev/learn/thinking-in-react)

> ℹ️ Cette fiche est **conceptuelle**. Ses deux petits extraits (§1) illustrent une idée et n'ont pas été passés à `tsc` ; le code vérifié est dans les fiches numérotées.

---

## 1. Le problème : l'interface qui se désynchronise

Une main de cartes, écrite en JavaScript « à la main » :

```js
let main = [];

function ajouterCarte(carte) {
    main.push(carte);
    const li = document.createElement("li");
    li.textContent = carte.nom;
    document.querySelector("#main").appendChild(li);
    document.querySelector("#compteur").textContent = main.length;
}
```

Ça marche. Puis viennent « retirer une carte », « trier la main », « griser les cartes trop chères », « la partie redémarre », « le serveur renvoie une main corrigée ». Chaque action doit savoir **quels morceaux de page** toucher, et dans quel ordre. Oublie une ligne, et le compteur affiche 5 quand la main en contient 4 : l'écran **ment**.

C'est un problème de **synchronisation** entre deux copies de la vérité : la variable `main` et le DOM.

Avec React, il n'y a plus qu'**une** copie :

```tsx
function Main({ cartes }: { cartes: { id: string; nom: string }[] }) {
    return (
        <>
            <ul>{cartes.map((c) => <li key={c.id}>{c.nom}</li>)}</ul>
            <p>{cartes.length} cartes</p>
        </>
    );
}
```

Tu ne dis plus **comment passer** d'un écran à l'autre, tu dis **ce que l'écran est** pour une liste de cartes donnée. Ajouter, retirer, trier, recevoir une main du serveur : c'est toujours la même fonction, appelée avec d'autres données. Le compteur ne peut plus mentir, il est **calculé**.

🔗 **Lien C.** Dans `so_long`, tu ne « déplaces » pas le sprite du joueur en effaçant quelques pixels : à chaque frame, tu **redessines** la carte à partir de l'état du jeu (`t_game`). React applique cette idée à une page web. La différence : au lieu de tout repeindre, il compare l'ancien résultat au nouveau et ne touche que ce qui a changé dans le DOM (§4).

---

## 2. L'idée centrale : `UI = f(état)`

```text
   données (props + état)  ──►  ton composant (une fonction)  ──►  description de l'écran (JSX)
                                                                         │
                                                         React compare avec la fois d'avant
                                                                         ▼
                                                            modifications minimales du DOM
```

Trois conséquences, qui sont les trois règles qu'on retrouve partout dans le dossier :

| Principe | Ce que ça veut dire | Où c'est détaillé |
|---|---|---|
| **Déclaratif** | Tu décris le résultat, pas les étapes. Pas de `appendChild`, pas de `querySelector`. | [`COMPOSANTS_JSX`](COMPOSANTS_JSX.md) |
| **Rendu pur** | La fonction ne fait **rien d'autre** que calculer du JSX. Même entrée → même sortie. Pas de requête, pas de minuteur, pas de modification d'une variable extérieure **pendant** le rendu. | [`INSTALLATION`](INSTALLATION.md) §3 (StrictMode), [`EFFETS`](EFFETS.md) §4 |
| **L'état est une photo** | Une valeur d'état ne change pas pendant un rendu. Pour la changer, tu demandes un **nouveau** rendu avec une **nouvelle** valeur (`setX`), jamais en modifiant l'ancienne. | [`ETAT`](ETAT.md) §2–3 |

🔗 **Lien C++.** Un rendu pur, c'est une fonction `const` sans effet de bord : elle peut être appelée deux fois, ou pas du tout, sans que rien ne casse. C'est exactement ce que `StrictMode` vérifie en dev en appelant tes composants **deux fois**.

---

## 3. Les composants : découper l'écran comme on découpe un programme

Un composant est une **fonction** qui reçoit des **props** et renvoie du JSX. On les emboîte comme des appels de fonctions :

```text
<App>
 ├── <Layout>                 (le cadre commun : barre du haut, menu)
 │    └── <SalonPage>
 │         ├── <ListeJoueurs joueurs={…} />
 │         └── <Chat messages={…} onEnvoyer={…} />
 └── …
```

- **Une responsabilité par composant**, comme une fonction de 25 lignes à 42 : si tu dois écrire « et » pour décrire ce qu'il fait, découpe.
- **Composer plutôt qu'hériter.** React n'utilise pas l'héritage de classes : un composant en **contient** d'autres (`children`, [`COMPOSANTS_JSX`](COMPOSANTS_JSX.md) §5). C'est l'inverse des `ClapTrap → ScavTrap` du C++.
- **Réutiliser la logique** se fait avec un **hook personnalisé** (`useQuelqueChose`), pas avec une classe mère ([`CONTEXTE_HOOKS`](CONTEXTE_HOOKS.md) §4).

---

## 4. Le flux de données à sens unique

```text
           props (données)
   Parent ───────────────────►  Enfant
          ◄───────────────────
           appel d'une fonction reçue en prop (onJouer, onEnvoyer…)
```

- Les **données descendent**, toujours. Un enfant ne modifie jamais ses props : elles appartiennent au parent.
- Les **événements remontent** : le parent passe une fonction (`onJouer`), l'enfant l'appelle. C'est le parent qui change **son** état, et le nouvel affichage redescend.
- Quand deux composants frères ont besoin de la même donnée, on la **remonte** chez leur parent commun ([`ETAT`](ETAT.md) §5). Quand beaucoup de composants éloignés en ont besoin : un **contexte** (valeur qui change rarement) ou un **store Zustand** (valeur qui change souvent) ([`CONTEXTE_HOOKS`](CONTEXTE_HOOKS.md), [`ZUSTAND`](ZUSTAND.md)).

Pourquoi ce sens unique ? Parce que pour comprendre **pourquoi l'écran affiche X**, il suffit de remonter les props jusqu'à l'état qui les a produites. Il n'y a qu'un chemin.

🔗 **Lien C.** C'est la différence entre passer une `struct` par valeur (l'enfant lit) et passer un pointeur de fonction de rappel (l'enfant prévient). Personne ne reçoit de pointeur pour écrire directement dans l'état du parent.

### Et la comparaison du DOM ?

À chaque nouvel état, React rappelle les composants concernés, obtient un nouvel arbre JSX, le **compare** à l'ancien (*réconciliation*) et n'applique au vrai DOM que les différences. Tu n'as pas à y penser, sauf sur un point : dans une liste, la `key` dit à React **quel élément est lequel** d'un rendu à l'autre. Une mauvaise `key` (l'index d'une liste qui change) et l'état passe à l'élément voisin ([`COMPOSANTS_JSX`](COMPOSANTS_JSX.md) §4).

---

## 5. Les effets : la porte de sortie

Un rendu pur ne peut pas ouvrir une socket, lancer un minuteur ni écouter `window`. Pour tout ce qui **n'est pas React**, il y a `useEffect` : « après l'affichage, synchronise-toi avec ce système extérieur, et donne-moi de quoi tout défaire » ([`EFFETS`](EFFETS.md) §1).

| Cas | Outil |
|---|---|
| Calculer une valeur à partir de l'état ou des props | **rien** : calcule-la pendant le rendu |
| Réagir à un clic, à une saisie | un **gestionnaire d'événement** (`onClick`) |
| Rester branché à quelque chose d'extérieur (socket, minuteur, `window`) | un **effet** avec nettoyage |
| Charger des données avant d'afficher une page | un **loader** de React Router ([`ROUTER`](ROUTER.md)) |

La moitié des bugs React de débutant viennent d'un effet utilisé là où il ne fallait **rien** ([`EFFETS`](EFFETS.md) §4).

---

## 6. Ce que React n'est pas

React est une **bibliothèque d'affichage**, pas un framework complet. Il ne fournit pas :

| Besoin | Hors de React | Dans la stack de l'équipe |
|---|---|---|
| Plusieurs pages, l'URL | un routeur | React Router ([`ROUTER`](ROUTER.md)) |
| État partagé qui change souvent | un store | Zustand ([`ZUSTAND`](ZUSTAND.md)) |
| Parler au serveur | `fetch`, un client typé | `requete()` + Tuyau ([`API`](API.md)) |
| Temps réel | une socket | Socket.IO ([`TEMPS_REEL`](TEMPS_REEL.md)) |
| Le style | CSS, un utilitaire | Tailwind ([`TAILWIND`](TAILWIND.md)) |
| Compiler, servir, recharger | un outil de build | Vite ([`INSTALLATION`](INSTALLATION.md)) |
| Dessiner en 3D | un autre *renderer* | React Three Fiber ([`../react-three-fiber/`](../react-three-fiber/INDEX.md)) |

La dernière ligne est importante : le modèle de React (composants, props, état, effets) ne dépend pas du DOM. R3F remplace « créer des `<div>` » par « créer des objets three.js », et **tout ce que tu apprends ici s'applique à la 3D**.

---

## 7. Comment l'utiliser : la méthode « Penser en React »

La doc officielle propose cinq étapes. Appliquées à l'écran de partie ([`ATELIER_PARTIE`](ATELIER_PARTIE.md)) :

1. **Découpe la maquette en composants.** Entoure chaque bloc sur un croquis : `Plateau`, `MainJoueur`, `Carte`, `Chrono`, `BarrePV`. Une hiérarchie apparaît.
2. **Écris une version statique.** Uniquement des props, aucune interaction, aucun état. Les données viennent d'un objet écrit en dur. Tu vérifies que l'affichage est juste.
3. **Trouve l'état minimal.** Pour chaque donnée, pose trois questions : elle ne change jamais ? → pas un état. Elle arrive par les props ? → pas un état. Elle se **calcule** à partir d'autre chose ? → pas un état. Ce qui reste est l'état (ici : la partie, envoyée par le serveur, et la sélection locale de cartes).
4. **Décide où vit chaque état.** Le plus proche possible du composant qui l'utilise. Partagé par des frères ? Chez le parent commun. Partagé par toute l'application et changeant souvent ? Un store.
5. **Ajoute le flux inverse.** Les enfants reçoivent des fonctions (`onSelectionner`, `onJouer`) et les appellent ; seul le propriétaire de l'état le modifie.

🔗 **Lien 42.** C'est la démarche d'un projet bien commencé : d'abord la structure de données (`t_game`), ensuite l'affichage, enfin les événements clavier. Écrire les `mlx_hook` avant de savoir ce qu'est l'état du jeu, c'est la meilleure façon de recommencer.

---

## 8. Quand React est le bon choix, et quand il ne l'est pas

**Bon choix** quand l'interface est **riche et change souvent** en fonction de données : une application avec des écrans qui dépendent d'un utilisateur, d'une partie, d'un chat. C'est le cas de Transcendance.

**Moins bon choix** pour :
- une page **statique** (un CV, une documentation) : du HTML suffit ;
- un site dont le contenu doit être lisible **avant** le JavaScript (référencement) : il faut alors un rendu serveur (Next.js, React Router en mode framework), hors du périmètre de ce dossier ;
- une boucle d'animation à 60 images par seconde **dans l'état React** : c'est le rôle de `useFrame` et des refs en 3D ([`../react-three-fiber/ANIMATION`](../react-three-fiber/ANIMATION.md) §4).

---

## ⚠️ À retenir

- Le problème que React résout : **l'écran désynchronisé des données**. Sa réponse : `UI = f(état)`.
- Un composant est une **fonction pure** de ses props et de son état. Pendant le rendu, on **calcule**, on ne **fait** rien.
- On ne modifie jamais un état : on en **fabrique** un nouveau et on le donne au setter.
- Les données descendent, les événements remontent.
- Un effet sert à se brancher sur ce qui **n'est pas React**, avec un nettoyage. Un calcul n'est pas un effet.
- React n'est que l'affichage : le reste de la stack (routeur, store, réseau, style, 3D) vient se brancher autour.
