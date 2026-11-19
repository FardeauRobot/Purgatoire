# 0. Installation : créer un projet R3F et comprendre chaque fichier

> **TL;DR.** `npm create vite@latest` puis `npm install three @react-three/fiber @react-three/drei`. Le `<Canvas>` remplit son parent : sans `height: 100%` sur toute la chaîne `html > body > #root`, il s'écrase. Monte toujours React et R3F **ensemble**, car R3F 9.8 n'accepte que `react >=19 <19.4`.

Liens : [`INDEX`](INDEX.md) · [`ANATOMIE`](ANATOMIE.md) · [`../OUTILS`](../OUTILS.md)

🧪 Labo : `~/Labo/TestThreeFiber/` (`package.json`, `vite.config.js`, `index.html`, `src/main.jsx`, `src/styles.css`)

---

## 1. Créer le projet

```sh
npm create vite@latest aura-3d -- --template react-ts
cd aura-3d
npm install three @react-three/fiber @react-three/drei
npm install -D @types/three
npm run dev
```

Le template `react` (sans `-ts`) donne un projet JavaScript, comme le labo. Pour Transcendance, prends `react-ts`.

| Paquet | Rôle |
|---|---|
| `three` | Le moteur 3D lui-même : géométries, matériaux, lumières, rendu WebGL. |
| `@react-three/fiber` | Le pont React ↔ three : `<Canvas>`, les balises `<mesh>`, `useFrame`, `useThree`. |
| `@react-three/drei` | Des composants tout faits pour R3F (voir [`DREI`](DREI.md)). |
| `@types/three` | Les types TypeScript de three (three est écrit en JavaScript). |
| `leva` | Optionnel : le panneau de réglages du labo (voir [`LEVA`](LEVA.md)). |
| `vite` + `@vitejs/plugin-react` | Le serveur de dev et le *bundler* : ton `make`, avec rechargement à chaud. |

🔗 **Lien C++.** `npm install` remplit `node_modules/` comme un `git submodule` de libft, mais pour toutes les dépendances, avec leurs propres dépendances. `package-lock.json` fige les versions exactes : commite-le, ne commite jamais `node_modules/`.

---

## 2. La contrainte de version

R3F s'accroche aux **internes** de React (son *reconciler*). Chaque version de R3F ne supporte donc qu'une plage de React. On le lit dans ses `peerDependencies` :

```sh
npm view @react-three/fiber peerDependencies
# → react: '>=19 <19.4', react-dom: '>=19 <19.4', three: '>=0.156', …
```

| Situation | Conséquence |
|---|---|
| `npm install react@latest` alors qu'une React 19.4 sort | `npm` refuse (`ERESOLVE`) ou installe un arbre incohérent. |
| R3F 8 avec React 19 | Erreurs obscures au démarrage : R3F 8 vise React 18. |
| drei 10 avec R3F 8 | Incompatible : drei 10 demande `@react-three/fiber ^9`. |

Règle : **mets à jour React, R3F et drei dans la même commande**, après avoir lu leurs `peerDependencies`.

---

## 3. Le chemin d'exécution : de `index.html` à ta scène

```text
index.html                    ← la seule page, contient <div id="root">
  └─ <script src="/src/main.jsx">
       └─ createRoot(#root).render(<App />)     ← React prend le contrôle du div
            └─ <App>                             ← menu des leçons + zone de scène
                 └─ <Lesson />                   ← contient un <Canvas>
                      └─ <mesh>, <ambientLight>… ← la scène three.js
```

Un `main.tsx` minimal, autonome :

```tsx
import { StrictMode } from "react";
import { createRoot } from "react-dom/client";
import { Canvas } from "@react-three/fiber";

function App() {
    return (
        <Canvas>
            <mesh>
                <boxGeometry />
                <meshNormalMaterial />
            </mesh>
        </Canvas>
    );
}

createRoot(document.getElementById("root")!).render(
    <StrictMode>
        <App />
    </StrictMode>
);
```

- `document.getElementById("root")!` : le `!` dit à TypeScript « je garantis que ce n'est pas `null` ». Le labo, en JS, n'en a pas besoin.
- `meshNormalMaterial` colore chaque face selon son orientation. Il ne demande **aucune lumière**, c'est le matériau idéal pour vérifier qu'une scène s'affiche.
- **`<StrictMode>`**, en développement seulement, monte chaque composant **deux fois** (montage, démontage, remontage) pour débusquer les effets mal nettoyés. Si un `console.log` dans un `useEffect` s'affiche deux fois, c'est normal. En production, une seule fois.

---

## 4. Le piège du canvas écrasé

`<Canvas>` prend **100 % de la taille de son parent**. Un pourcentage se calcule par rapport au parent, qui se calcule par rapport au sien, et ainsi de suite. Si un seul maillon n'a pas de hauteur, toute la chaîne s'effondre.

```css
/* styles.css du labo */
html, body, #root { height: 100%; margin: 0; }
.layout { display: flex; height: 100%; }
.stage  { flex: 1; position: relative; }
```

| Symptôme | Cause |
|---|---|
| Rien ne s'affiche, aucune erreur | Un parent du `<Canvas>` a une hauteur nulle. |
| Une bande de quelques pixels ou d'environ 150 px | La hauteur par défaut d'un `<canvas>` HTML, faute de mieux. |
| Une barre de défilement inutile | `margin` par défaut du `body` : le `margin: 0` la retire. |

Pour déboguer : DevTools (`Cmd+Option+I`), onglet **Elements**, survole chaque parent du `<canvas>` et lis sa hauteur. Le premier qui affiche `0` est le coupable.

---

## 5. Les réglages du labo, expliqués

```js
// vite.config.js
export default defineConfig({
    plugins: [react()],
    server: { port: 5180, strictPort: true },
})
```

- `plugins: [react()]` : compile le JSX et active le *Fast Refresh*. Quand tu sauvegardes une leçon, seul ce composant est remplacé, sans recharger toute la page.
- `port: 5180` : le port par défaut de Vite (5173) est pris par un autre projet sur ta machine.
- `strictPort: true` : si 5180 est occupé, Vite **échoue** au lieu de prendre silencieusement 5181. Tu sais toujours quelle URL ouvrir.

```json
// jsconfig.json (extrait)
{ "compilerOptions": { "checkJs": true, "strict": false, "jsx": "react-jsx" } }
```

- `checkJs: true` : VS Code vérifie aussi les `.jsx` et souligne les erreurs de type, même sans TypeScript. Tu profites des types de `@react-three/fiber` gratuitement.
- `strict: false` : pas d'erreur pour un `useRef(null)` non typé. En TypeScript (Transcendance), on passe en `strict: true` et on écrit `useRef<Mesh>(null)`.

### Le remontage par `key` (dans `App.jsx`)

```jsx
<Lesson key={currentId} />
```

Quand `key` change, React **détruit** l'ancien composant et en crée un neuf, au lieu de le mettre à jour. Le `<Canvas>` de la leçon précédente est démonté : sa caméra, sa scène et ses ressources GPU sont libérées, et la nouvelle leçon repart de zéro.

Subtilité : ici, chaque leçon est **une fonction différente** (`Lesson1`, `Lesson2`…). Or React remonte déjà tout composant dont le *type* change. La `key` est donc une ceinture de sécurité : elle rend l'intention explicite et protège le jour où deux entrées du menu partageraient le même composant avec des props différentes. Dans ce cas, sans `key`, React **réutiliserait** le même `<Canvas>`, avec la caméra déplacée de l'entrée précédente.

---

## 6. Scripts du quotidien

| Commande | Effet |
|---|---|
| `npm run dev` | Serveur de dev avec rechargement à chaud. Lit le code source directement. |
| `npm run build` | Produit `dist/` : JS minifié, prêt à servir. Fichiers générés, à ne pas éditer. |
| `npm run preview` | Sert `dist/` en local pour tester la version de production. |
| `npx tsc --noEmit` | Vérifie les types sans rien produire (projet TypeScript). |

> ⚠️ `dist/` est un résultat de compilation, comme un `.o`. Il est régénéré à chaque `build` : ajoute-le au `.gitignore` s'il n'y est pas.

---

## 🧪 À essayer

1. Dans `styles.css`, retire `height: 100%` de la règle `html, body, #root`. Observe le canvas s'écraser, puis remets la règle.
2. Lance une deuxième fois `npm run dev` dans un autre terminal. Grâce à `strictPort`, la deuxième instance doit échouer avec un message clair.
3. Dans `src/lessons/index.jsx`, donne à la leçon 7 le composant `Lesson6`. Retire `key={currentId}` dans `App.jsx`, tourne la caméra à la souris en leçon 6, puis clique sur la leçon 7 : la caméra garde sa position, car React a réutilisé le même `<Canvas>`. Remets `key` : elle repart de zéro. Annule ensuite les deux changements.
4. Ajoute `console.log("monté")` dans un `useEffect` d'une leçon. Compte les affichages en dev : `StrictMode` en provoque deux.
