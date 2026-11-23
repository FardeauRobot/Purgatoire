# 0. Installation : un projet React + TypeScript avec Vite

> **TL;DR.** `npm create vite@latest mon-app -- --template react-ts` crée un projet complet : `index.html` (la seule page), `src/main.tsx` (le point d'entrée qui accroche React à la `<div id="root">`), `src/App.tsx` (ton premier composant). `npm run dev` lance un serveur qui recharge à chaque sauvegarde ; `npm run build` vérifie les types **puis** produit `dist/`. Garde `<StrictMode>` : en dev, il appelle deux fois tes composants et tes effets pour faire sortir les bugs tôt. Seules les variables d'environnement préfixées `VITE_` arrivent dans le navigateur, et elles y sont **lisibles par tout le monde**.

Liens : [`INDEX`](INDEX.md) · [`COMPOSANTS_JSX`](COMPOSANTS_JSX.md) · [`../OUTILS`](../OUTILS.md) · [`../react-three-fiber/INSTALLATION`](../react-three-fiber/INSTALLATION.md)

🧪 Labo : aucun. Crée le projet dans un dossier jetable, ou lis `apps/frontend/` dans le dépôt de l'équipe.

---

## 1. Créer le projet

```sh
npm create vite@latest mon-app -- --template react-ts
cd mon-app
npm install
npm run dev
```

- `npm create vite@latest` télécharge et lance le générateur de Vite. Le `--` sépare les options de `npm` de celles du générateur.
- `--template react-ts` : React, en TypeScript. Sans lui, le générateur pose des questions.
- `npm run dev` affiche une adresse (`http://localhost:5173`). Ouvre-la : la page se met à jour à chaque sauvegarde, sans perdre l'état des composants quand c'est possible (*Fast Refresh*).

🔗 **Lien C.** `npm run dev`, c'est un `make` qui recompilerait à chaque sauvegarde et rechargerait le binaire déjà lancé. Il n'y a pas d'exécutable à produire en dev : le navigateur reçoit les fichiers un par un, transformés à la volée.

---

## 2. Les fichiers, un par un

```text
mon-app/
├── index.html            ← la seule page HTML ; contient <div id="root"> et charge src/main.tsx
├── package.json          ← dépendances et scripts (dev, build, preview, lint)
├── vite.config.ts        ← plugins de Vite (React, et Tailwind dans l'équipe)
├── tsconfig.json         ← renvoie vers les deux suivants
├── tsconfig.app.json     ← règles TypeScript pour src/ (le code du navigateur)
├── tsconfig.node.json    ← règles pour vite.config.ts (le code qui tourne dans Node)
└── src/
    ├── main.tsx          ← point d'entrée : accroche React à #root
    ├── App.tsx           ← le premier composant
    └── index.css         ← styles globaux
```

`src/main.tsx` :

```tsx
import { StrictMode } from "react";
import { createRoot } from "react-dom/client";
import "./index.css";
import App from "./App.tsx";

createRoot(document.getElementById("root")!).render(
    <StrictMode>
        <App />
    </StrictMode>,
);
```

Ligne par ligne :
- `import "./index.css"` : Vite sait importer un CSS. Il l'injecte dans la page. Ce n'est pas du JavaScript, c'est Vite qui comprend cette ligne.
- `import App from "./App.tsx"` : l'extension `.tsx` est autorisée par l'option `allowImportingTsExtensions` du gabarit.
- `document.getElementById("root")!` : récupère la `<div id="root">` d'`index.html`. Le `!` dit à TypeScript « je sais qu'elle existe » (voir [`../TYPESCRIPT`](../TYPESCRIPT.md)). Le `main.tsx` de l'équipe vérifie à la place, et lève une erreur claire si la `div` manque : c'est plus sûr.
- `createRoot(…).render(<App />)` : à partir d'ici, React possède tout ce qu'il y a dans `#root`. Tu ne touches plus au DOM à la main.

🔗 **Lien C.** `main.tsx` est le `main()` du programme. `createRoot(...).render(...)` ne rend pas la main comme un appel bloquant : il installe React, qui réagira ensuite aux événements (clics, réponses réseau). C'est la boucle d'événements d'[`../ASYNCHRONE`](../ASYNCHRONE.md).

---

## 3. `<StrictMode>` : le mode qui dérange exprès

En dev, `<StrictMode>` fait **volontairement** des choses étranges. Observé avec ce composant :

```tsx
import { useEffect, useState } from "react";

export default function Sonde() {
    console.log("rendu");
    const [n] = useState(() => {
        console.log("init useState");
        return 0;
    });
    useEffect(() => {
        console.log("effet monté");
        return () => console.log("effet nettoyé");
    }, []);
    return <p>{n}</p>;
}
```

| Sans `StrictMode` | Avec `StrictMode` (dev) |
|---|---|
| `rendu` · `init useState` · `effet monté` | `rendu` · `init useState` · `init useState` · `rendu` · `effet monté` · `effet nettoyé` · `effet monté` |

- Le composant est **appelé deux fois** : un composant doit être une fonction **pure**, qui rend la même chose à chaque appel. S'il fait autre chose (modifier une variable globale, envoyer une requête), le double appel le montre tout de suite.
- L'effet est **monté, nettoyé, remonté** : ça simule un départ et un retour sur la page. Si ton nettoyage est faux (une connexion pas fermée), tu le vois dès le premier chargement, avec deux connexions au lieu d'une.
- En **production**, rien de tout ça : un seul appel, un seul montage.

🚨 Le `main.tsx` de l'équipe n'a **pas** de `<StrictMode>`. Le retirer ne corrige aucun bug, ça les cache jusqu'à ce qu'un vrai départ de page les déclenche. Remets-le.

🔗 **Lien C.** C'est `valgrind` pour React : il fait tourner ton code dans des conditions plus dures que la réalité, pour qu'un bug latent devienne visible tout de suite.

---

## 4. Les scripts

| Commande | Ce qu'elle fait |
|---|---|
| `npm run dev` | Serveur de dev, rechargement à chaud, `StrictMode` actif. |
| `npm run build` | `tsc -b` **puis** `vite build`. Une erreur de type bloque le build. Produit `dist/`. |
| `npm run preview` | Sert `dist/` tel quel, pour tester le vrai build en local. |
| `npm run lint` | ESLint. Dans l'équipe, avec `eslint-plugin-react-hooks`, qui signale les erreurs de hooks (voir [`EFFETS`](EFFETS.md)). |

- `vite` seul ne **vérifie pas** les types : il les efface et c'est tout. Une erreur TypeScript n'empêche pas la page de s'afficher en dev. C'est `tsc -b` dans `build`, ou ton éditeur, qui la signale.
- Le mode `strict` de TypeScript est **actif par défaut** depuis TypeScript 6 (vérifié avec 6.0.3, la version de l'équipe, et 7.0.2, celle que donne aujourd'hui le gabarit) : un paramètre sans type est une erreur, `null` n'entre pas dans un `string`. Les `tsconfig` du gabarit n'écrivent donc plus `"strict": true`.

---

## 5. Les variables d'environnement

```sh
# .env.local  (jamais commité : il est dans le .gitignore du gabarit)
VITE_API_URL=http://localhost:3333
```

```ts
export const API_URL = import.meta.env.VITE_API_URL;
```

- Vite ne transmet au code du navigateur que les variables qui commencent par **`VITE_`**. Observé : `SECRET_DB` dans le même fichier vaut `undefined` côté navigateur, et n'apparaît pas dans le build.
- 🚨 Tout ce qui commence par `VITE_` est **écrit en clair** dans les fichiers de `dist/` (observé : l'URL s'y trouve). Jamais de secret dans une variable `VITE_`. Un secret vit sur le serveur (Adonis), pas dans le navigateur.
- La valeur est figée **au moment du build**. Changer `.env` après `npm run build` ne change rien : il faut rebuilder. C'est pourquoi l'image Docker du front doit recevoir `VITE_API_URL` pendant le build, pas au lancement (voir [`API`](API.md)).
- `import.meta.env.DEV` et `import.meta.env.PROD` : vrais respectivement en dev et en build.

🔗 **Lien C.** Une variable `VITE_` se comporte comme un `#define` passé avec `-D` à la compilation : la valeur est collée dans le binaire, et on peut la lire avec `strings`.

---

## 🧪 À essayer

1. Crée un projet, lance-le, puis modifie le texte d'`App.tsx`. La page change-t-elle sans rechargement complet ?
2. Copie le composant `Sonde` du §3 dans `App.tsx` et ouvre la console. Retire `<StrictMode>` de `main.tsx` : combien de lignes restent ?
3. Écris `const n: number = "trois";` dans `App.tsx`. La page s'affiche-t-elle en dev ? Que dit `npm run build` ?
4. Mets `SECRET=abc` et `VITE_PUBLIC=xyz` dans `.env.local`, affiche les deux avec `console.log(import.meta.env)`, puis cherche `xyz` dans `dist/assets/*.js` après un build.
