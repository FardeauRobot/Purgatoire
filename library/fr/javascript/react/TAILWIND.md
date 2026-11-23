# 10. Tailwind CSS 4 : styler avec des classes

> **TL;DR.** Tailwind remplace la plupart du CSS écrit à la main par des **classes utilitaires** posées directement dans le JSX : `className="p-4 rounded-xl bg-red-500 hover:scale-105"`. En version 4, l'installation tient en un plugin Vite et **une** ligne de CSS (`@import "tailwindcss";`), sans fichier `tailwind.config.js` ; les couleurs et polices du projet se déclarent dans le CSS avec `@theme`. Tailwind lit tes fichiers comme du **texte** et ne génère que les classes qu'il y trouve écrites **en entier** : une classe fabriquée par concaténation (`bg-${couleur}-500`) n'existe pas dans le CSS final.

Liens : [`INDEX`](INDEX.md) · [`INSTALLATION`](INSTALLATION.md) · [`COMPOSANTS_JSX`](COMPOSANTS_JSX.md) · [doc Tailwind](https://tailwindcss.com/docs)

🧪 Labo : aucun. Vérifié avec `tailwindcss` 4.3.5 et `@tailwindcss/vite`, par un `vite build` suivi d'une recherche des classes dans le CSS produit.

---

## 1. L'installation (déjà faite dans le dépôt de l'équipe)

```sh
npm install tailwindcss @tailwindcss/vite
```

```ts
import tailwindcss from "@tailwindcss/vite";
import react from "@vitejs/plugin-react";
import { defineConfig } from "vite";

export default defineConfig({
    plugins: [react(), tailwindcss()],
});
```

```css
@import "tailwindcss";
```

- Le plugin Vite fait tout le travail : il lit tes fichiers, repère les classes et génère le CSS correspondant.
- La ligne `@import` va dans le CSS global (`src/index.css`), lui-même importé par `main.tsx`.
- 🚨 Les tutoriels d'avant 2025 parlent de `tailwind.config.js`, `postcss.config.js` et des trois lignes `@tailwind base; @tailwind components; @tailwind utilities;`. C'est la **version 3**. Avec la version 4, ne crée aucun de ces fichiers.

---

## 2. Lire une classe Tailwind

```tsx
export function Bouton({ children }: { children: string }) {
    return (
        <button className="px-4 py-2 rounded-lg bg-indigo-600 text-white font-semibold hover:bg-indigo-500 disabled:opacity-50 md:px-6">
            {children}
        </button>
    );
}
```

| Classe | CSS équivalent |
|---|---|
| `px-4` / `py-2` | `padding-inline` / `padding-block` : 4 × 0.25 rem = 1 rem, 2 × 0.25 rem = 0.5 rem |
| `rounded-lg` | `border-radius` moyen |
| `bg-indigo-600` | couleur de fond : teinte `indigo`, intensité `600` (de 50, très clair, à 950, très foncé) |
| `font-semibold` | `font-weight: 600` |
| `hover:bg-indigo-500` | la même règle, mais seulement au survol |
| `disabled:opacity-50` | seulement quand le bouton est désactivé |
| `md:px-6` | seulement à partir de la largeur `md` (48 rem, environ 768 px) |

- Le **préfixe** (`hover:`, `focus:`, `disabled:`, `md:`) est une condition. On les empile : `md:hover:bg-indigo-500`.
- Les tailles sont **mobile d'abord** : `px-4 md:px-6` veut dire « 4 partout, 6 à partir de `md` ». Il n'y a pas de préfixe « seulement sur mobile ».

🔗 **Lien C.** Chaque classe est une petite fonction du CSS, avec un nom court et un seul effet. On compose l'apparence en les enchaînant, comme on enchaîne des appels de `libft` plutôt que de réécrire `strlen` à chaque fois.

---

## 3. Les couleurs et polices du projet : `@theme`

```css
@import "tailwindcss";

@theme {
    --color-aura: oklch(0.72 0.19 300);
    --color-plateau: oklch(0.32 0.06 160);
    --font-titre: "Cinzel", serif;
}
```

- Chaque variable `--color-xxx` crée **toutes** les classes de couleur `xxx` : `bg-aura`, `text-aura`, `border-aura`… Observé : `text-aura` est généré, et `--color-aura` est présent dans le CSS final.
- `--font-titre` crée `font-titre`.
- Ce sont aussi de vraies variables CSS : un composant three.js ou un `style={{ color: "var(--color-aura)" }}` peut les lire.
- Une seule source pour la palette du jeu : changer la teinte de l'aura, c'est une ligne.

---

## 4. Le piège : les classes construites

```tsx
type Famille = "coupes" | "deniers" | "epees" | "batons";

const FOND: Record<Famille, string> = {
    coupes: "bg-red-500",
    deniers: "bg-amber-400",
    epees: "bg-sky-500",
    batons: "bg-emerald-600",
};

export function Carte({ famille, selectionnee }: { famille: Famille; selectionnee: boolean }) {
    return (
        <div className={`h-28 w-20 rounded-xl ${FOND[famille]} ${selectionnee ? "-translate-y-3 ring-4 ring-aura" : ""}`}>
            {famille}
        </div>
    );
}
```

Observé dans le CSS produit par `vite build` :

| Écrit dans le code | Dans le CSS ? |
|---|---|
| `"bg-red-500"` dans l'objet `FOND` | ✅ oui |
| `` `bg-${teinte}-700` `` avec `teinte = "emerald"` | ❌ **non** |
| `hover:scale-105`, `md:w-40` | ✅ oui |

- Tailwind ne fait **pas** tourner ton code. Il cherche dans les fichiers des chaînes qui **ressemblent** à des classes. `bg-${teinte}-700` n'est pas une classe complète : rien n'est généré, et la carte n'a pas de fond. Aucune erreur, aucun avertissement.
- La parade : écrire chaque classe **en entier** quelque part, par exemple dans un objet de correspondance comme `FOND`. Le texte complet est dans le fichier, donc la classe existe.
- Le ternaire `selectionnee ? "…" : ""` marche pour la même raison : les deux chaînes sont écrites en entier.

🔗 **Lien C.** C'est comme le linker qui ne garde que les fonctions référencées. Une fonction appelée par un nom construit à l'exécution (`dlsym` avec une chaîne calculée) n'est pas vue par l'analyse statique, et elle n'est pas là quand on en a besoin.

---

## 5. Une valeur qui change vraiment à l'exécution

Pour une valeur continue (une position, une progression, une couleur venue du serveur), une classe ne convient pas : il y en aurait une infinité. On utilise `style` :

```tsx
export function JaugeAura({ ratio }: { ratio: number }) {
    return (
        <div className="h-2 w-full rounded bg-plateau">
            <div className="h-full rounded bg-aura transition-[width]" style={{ width: `${Math.round(ratio * 100)}%` }} />
        </div>
    );
}
```

- Les classes portent ce qui est **fixe** (forme, couleur, transition), `style` porte ce qui est **calculé**.
- `transition-[width]` : une valeur entre crochets, écrite en entier, est générée telle quelle (*valeur arbitraire*).

---

## 6. Quand la liste de classes devient illisible

- Extrais un **composant** (`<Bouton>`, `<Carte>`) plutôt que de copier une longue `className` à dix endroits. C'est la façon React de réutiliser un style.
- Pour un style vraiment global (le fond de la page, une police par défaut), écris du CSS normal dans `index.css`, sous l'`@import`.
- L'extension Tailwind de VS Code complète les classes et montre le CSS au survol : installe-la.

---

## 🧪 À essayer

1. Reproduis la `Carte` du §4, puis remplace `FOND[famille]` par `` `bg-${couleur}-500` `` avec une prop `couleur="red"`. La carte a-t-elle un fond ? Cherche `bg-red-500` dans le CSS de `dist/`.
2. Ajoute `--color-aura` dans `@theme` et utilise `bg-aura`, `text-aura` et `border-aura`. Change la teinte : tout suit-il ?
3. Écris `className="p-4 md:p-8"` et réduis la fenêtre en dessous de 768 px. Puis essaie d'obtenir l'inverse (plus grand sur mobile).
4. Compare la taille du CSS de `dist/` avec trois classes, puis avec trente. Tailwind génère-t-il les classes que tu n'utilises pas ?
