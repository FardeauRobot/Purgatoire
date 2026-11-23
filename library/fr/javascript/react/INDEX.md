# ⚛️ React : l'interface web de Transcendance

> Rédigé directement en français, comme le reste de [`javascript/`](../INDEX.md).

**React** décrit l'interface comme des **fonctions** : elles reçoivent des données (*props*), gardent un **état**, et renvoient du JSX. Quand l'état change, React rappelle la fonction et met la page à jour. Autour, la stack de l'équipe ajoute **React Router** (les pages), **Zustand** (l'état partagé), **Tailwind** (le style), **Tuyau** (les appels typés à Adonis) et **Socket.IO** (le temps réel).

Ce dossier suit cette stack, version par version, jusqu'à l'écran de partie du jeu de cartes.

Liens : [`../INDEX`](../INDEX.md) · [`../VERS_TRANSCENDANCE`](../VERS_TRANSCENDANCE.md) · [`../react-three-fiber/INDEX`](../react-three-fiber/INDEX.md) · [`../TYPESCRIPT`](../TYPESCRIPT.md) · [`../ASYNCHRONE`](../ASYNCHRONE.md) · [sujet ft_transcendence](../../projects/LEVEL6/ft_transcendence-subject.md) · [doc React](https://react.dev)

---

## 🧭 Ordre de lecture

| Étape | Fiche | Pourquoi maintenant |
|---|---|---|
| 0 | [`INSTALLATION`](INSTALLATION.md) | Créer le projet avec Vite, lire chaque fichier, StrictMode, les variables `VITE_`. |
| 1 | [`COMPOSANTS_JSX`](COMPOSANTS_JSX.md) | Un composant, ses props, le JSX, l'affichage conditionnel, les listes et leurs `key`. |
| 2 | [`ETAT`](ETAT.md) | `useState`, le regroupement des mises à jour, l'immutabilité, l'état dérivé, `useReducer`. |
| 3 | [`EFFETS`](EFFETS.md) | `useEffect`, le nettoyage, charger des données, et quand **ne pas** utiliser d'effet. |
| 4 | [`REFS_FORMULAIRES`](REFS_FORMULAIRES.md) | `useRef`, champs contrôlés ou non, `useActionState` et `useFormStatus`. |
| 5 | [`CONTEXTE_HOOKS`](CONTEXTE_HOOKS.md) | Le contexte, `use()`, écrire ses propres hooks. |
| 6 | [`ROUTER`](ROUTER.md) | Les pages, les layouts, les loaders, les redirections, `F5` en production. |
| 7 | [`ZUSTAND`](ZUSTAND.md) | Les stores, le sélecteur qui plante, `persist`, la remise à zéro. |
| 8 | [`API`](API.md) | `fetch` et ses erreurs silencieuses, le jeton, CORS et le proxy, l'URL de base figée au build. |
| 9 | [`TEMPS_REEL`](TEMPS_REEL.md) | Socket.IO : une socket par module, `on`/`off`, accusés de réception, authentification. |
| 10 | [`TAILWIND`](TAILWIND.md) | Tailwind 4, `@theme`, et les classes construites qui n'existent pas. |
| 11 | [`ATELIER_PARTIE`](ATELIER_PARTIE.md) | Tout assembler : l'écran de partie, piloté par le serveur. |
| 12 | [`PERFORMANCE_DEBUG`](PERFORMANCE_DEBUG.md) | `memo` mesuré, React DevTools, et un tableau message d'erreur → cause. |

> 🌟 Si tu n'as le temps que pour trois fiches : **ETAT**, **EFFETS**, **TEMPS_REEL**.

> 🪜 **Avant l'étape 0** : [`PHILOSOPHIE`](PHILOSOPHIE.md), à quel besoin React répond et comment penser en React, puis [`ABORDER`](ABORDER.md), les prérequis, le modèle mental et l'ordre d'étude. 📒 **Pour réviser** : [`SYNTHESE`](SYNTHESE.md), tout le dossier en une fiche, avec quiz.

---

## 🧪 Pas de labo

- Ce dossier n'a pas de labo : chaque fiche se termine par **« 🧪 À essayer »**, des exercices courts qui disent quoi observer. Fais-les dans un projet jetable (`npm create vite@latest`, voir [`INSTALLATION`](INSTALLATION.md)) ou dans une branche à toi.
- Le dépôt de l'équipe sert d'exemple : les fiches citent ses fichiers (`main.tsx`, `api.ts`, `socket.ts`, `Chat.tsx`, le Caddyfile) pour montrer **où** chaque notion apparaît. Elles décrivent le code, ne le modifient pas.

---

## ✅ Ce qui a été vérifié

Versions installées et vérifiées le **19/11/2026** : `react` / `react-dom` 19.3.1, `@types/react` 19.3.0, `vite` 8.3.8, `@vitejs/plugin-react` 6.3.0, `react-router` 8.5.2, `zustand` 5.1.0, `tailwindcss` / `@tailwindcss/vite` 4.3.5, `socket.io-client` / `socket.io` 4.8.4, `typescript` 6.0.3, `eslint` 10.15.0, `eslint-plugin-react-hooks` 7.1.1, `eslint-plugin-react-refresh` 0.5.8, Node 26.

- **Typecheck.** Les 48 blocs ` ```ts ` / ` ```tsx ` ont été extraits et passés à `tsc` avec la config du gabarit Vite (`noUnusedLocals`, `verbatimModuleSyntax`, `erasableSyntaxOnly`…) : **zéro erreur**.
- **Lint.** Avec la config ESLint **de l'équipe**, il reste :
  - une erreur voulue, `set-state-in-effect`, sur le contre-exemple `TotalFaux` d'[`EFFETS`](EFFETS.md) ;
  - quatre erreurs `react-refresh/only-export-components`, sur des blocs qui regroupent un hook ou un store avec un composant pour la lecture. Chacune est signalée dans sa fiche, avec la façon de découper les fichiers.
- **Comportements observés.** Ce que les fiches disent « observé » a été lancé dans Chrome (piloté par un script) : double rendu de StrictMode, regroupement des `set`, mutation en place, clés par index, closure figée, formulaires React 19, loaders et redirections du routeur, sélecteurs Zustand, `persist`, CORS et proxy Vite, Socket.IO avec un jeton, classes Tailwind dans le CSS produit, `memo` et `useCallback`, l'écran de l'atelier.
- **Messages d'erreur.** Tous ceux cités ont été déclenchés exprès et recopiés tels quels.
- **Lu dans le code source** des paquets : `pingInterval` à 25 000 ms (`engine.io`), le sens de `socket.active`, le `@deprecated` de `FormEvent`.
- **Observé plus tard**, avec un `vite build` du front de l'équipe sans `.env` : l'URL `localhost:3333` est figée dans le bundle dès qu'un fichier importe `api.ts` (aujourd'hui, aucun ne le fait, donc elle n'y est pas encore). Détails dans [`API`](API.md) §4 et [`../adonis/DEPLOIEMENT`](../adonis/DEPLOIEMENT.md) §3. Le build Docker de l'équipe, lui, n'a pas été lancé.
- **Non vérifié** : les onglets de React Developer Tools (décrits d'après leur documentation) ; le serveur de jeu de l'atelier, qui n'existe pas encore.
- **`PHILOSOPHIE`, `ABORDER` et `SYNTHESE`** ne contiennent pas de code nouveau : leurs extraits sont recopiés des fiches du dossier, qui indiquent leur source. Ce sont des **extraits**, parfois raccourcis par `// …` ou privés de leurs imports : ils ne sont pas des modules complets et ne comptent pas dans la vérification ci-dessus. Exception : les deux petits extraits de `PHILOSOPHIE` §1 (le DOM mis à jour à la main et son équivalent React) n'ont pas été passés à `tsc`.
- Si la doc officielle contredit une fiche, c'est la doc qui a raison. Corrige la fiche.

---

## 🔁 JavaScript « à la main » ↔ React

| Sans React | Avec React | Fiche |
|---|---|---|
| `document.createElement`, `appendChild` | Un composant qui renvoie du JSX | [`COMPOSANTS_JSX`](COMPOSANTS_JSX.md) |
| Une variable + `el.textContent = …` à chaque changement | `useState` : on change l'état, React met la page à jour | [`ETAT`](ETAT.md) |
| `addEventListener` / `removeEventListener` | `onClick` dans le JSX ; `useEffect` + nettoyage pour `window` | [`EFFETS`](EFFETS.md) |
| `document.querySelector("#champ")` | `useRef` | [`REFS_FORMULAIRES`](REFS_FORMULAIRES.md) |
| Une variable globale partagée | Un contexte, ou un store Zustand | [`CONTEXTE_HOOKS`](CONTEXTE_HOOKS.md), [`ZUSTAND`](ZUSTAND.md) |
| Une page HTML par URL | Des routes, une seule page | [`ROUTER`](ROUTER.md) |
| Une feuille CSS à côté | Des classes Tailwind dans le JSX | [`TAILWIND`](TAILWIND.md) |

---

## 🚨 Les pièges, en une liste

1. `onClick={f()}` appelle `f` **pendant** le rendu : écris `onClick={() => f()}` ([`ETAT`](ETAT.md)).
2. Muter un tableau ou un objet d'état (`push`, `obj.x = …`) : React ne voit rien ([`ETAT`](ETAT.md), §3).
3. `key={index}` dans une liste qui change : l'état passe à l'élément voisin ([`COMPOSANTS_JSX`](COMPOSANTS_JSX.md)).
4. `{n && …}` avec `n = 0` affiche « 0 » ([`COMPOSANTS_JSX`](COMPOSANTS_JSX.md)).
5. Un effet sans nettoyage : tout arrive en double sous StrictMode ([`EFFETS`](EFFETS.md), [`TEMPS_REEL`](TEMPS_REEL.md)).
6. Un état calculé dans un effet au lieu de pendant le rendu ([`EFFETS`](EFFETS.md), §4).
7. Un sélecteur Zustand qui renvoie un objet neuf : boucle infinie ([`ZUSTAND`](ZUSTAND.md), §2).
8. `fetch` ne lève pas d'erreur sur un 404 ou un 401 : teste `r.ok` ([`API`](API.md)).
9. Une variable `VITE_` absente au build tombe sur sa valeur par défaut, en production ([`API`](API.md), §4).
10. Rien de secret dans une variable `VITE_` ni dans `localStorage` ([`INSTALLATION`](INSTALLATION.md), [`ZUSTAND`](ZUSTAND.md)).
11. `socket.off("evt")` sans la fonction retire les écouteurs **de tout le monde** ([`TEMPS_REEL`](TEMPS_REEL.md)).
12. `bg-${couleur}-500` n'est jamais généré par Tailwind ([`TAILWIND`](TAILWIND.md)).
13. `memo` + une fonction écrite dans le JSX ne sert à rien ([`PERFORMANCE_DEBUG`](PERFORMANCE_DEBUG.md)).
14. Une redirection côté client n'est pas une sécurité : c'est le serveur qui refuse ([`ROUTER`](ROUTER.md)).
