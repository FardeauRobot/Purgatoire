# 1. Composants et JSX

> **TL;DR.** Un composant est une **fonction** dont le nom commence par une majuscule, qui reçoit un objet de **props** et renvoie du **JSX**, c'est-à-dire la description de ce qu'il faut afficher. Le JSX ressemble à du HTML, avec quelques différences : `className` au lieu de `class`, `onClick` en camelCase, un seul élément racine (ou un fragment `<>…</>`), et `{…}` pour insérer n'importe quelle expression JavaScript. Une liste se fait avec `.map()`, et chaque élément reçoit une **`key` stable**, jamais l'index si la liste peut changer.

Liens : [`INDEX`](INDEX.md) · [`INSTALLATION`](INSTALLATION.md) · [`ETAT`](ETAT.md) · [`../FONCTIONS`](../FONCTIONS.md) · [`../OBJETS_TABLEAUX`](../OBJETS_TABLEAUX.md)

🧪 Labo : aucun. Colle les modules dans `src/` d'un projet créé selon [`INSTALLATION`](INSTALLATION.md).

---

## 1. Un premier composant

```tsx
type Famille = "Coupes" | "Deniers" | "Épées" | "Bâtons";

type CarteProps = {
    rang: string;
    famille: Famille;
    grisee?: boolean;
};

const SYMBOLE: Record<Famille, string> = { Coupes: "♥", Deniers: "◆", Épées: "♠", Bâtons: "♣" };

export function Carte({ rang, famille, grisee = false }: CarteProps) {
    return (
        <div className={grisee ? "carte grisee" : "carte"} title={`${rang} de ${famille}`}>
            <span className="rang">{rang}</span>
            <span className="famille">{SYMBOLE[famille]}</span>
        </div>
    );
}
```

Ligne par ligne :
- `type CarteProps` : la liste des props et leur type. `grisee?` est **optionnelle**.
- `function Carte({ rang, famille, grisee = false }: CarteProps)` : React appelle la fonction avec **un seul** argument, un objet. On le déstructure directement, avec une valeur par défaut pour `grisee` (voir [`../OBJETS_TABLEAUX`](../OBJETS_TABLEAUX.md)).
- La **majuscule** à `Carte` est obligatoire : `<carte />` serait compris comme une balise HTML inconnue, `<Carte />` comme ton composant.
- `className` : `class` est un mot réservé en JavaScript.
- `{grisee ? "carte grisee" : "carte"}` : entre accolades, n'importe quelle **expression** (un calcul, un appel, un ternaire). Pas d'instruction : pas de `if`, pas de `for`.
- `{SYMBOLE[famille]}` : une constante déclarée **hors** du composant. Elle n'a pas besoin d'être recréée à chaque appel.

Pour l'utiliser :

```tsx
import { Carte } from "./Carte";

export default function App() {
    return (
        <>
            <Carte rang="R" famille="Coupes" />
            <Carte rang="7" famille="Épées" grisee />
        </>
    );
}
```

- `grisee` sans valeur vaut `true`, comme un attribut HTML booléen.
- `<>…</>` est un **fragment** : un parent invisible, parce qu'un composant ne renvoie qu'**un** élément racine. Il n'ajoute aucune `div` dans la page.
- TypeScript vérifie l'appel : `<Carte rang="R" />` sans `famille`, ou `famille="Trèfle"`, est une erreur de compilation.

🔗 **Lien C++.** Un composant, c'est une fonction `render(const Props&)` qui renvoie une description de l'écran. Les props sont des paramètres **en lecture seule** : on ne les modifie jamais dans le composant. Pour changer ce qui s'affiche, on passe par l'**état** ([`ETAT`](ETAT.md)).

---

## 2. Ce qu'est vraiment le JSX

Le JSX n'est pas du HTML : c'est une écriture plus courte pour des appels de fonction. Vite le transforme avant que le navigateur ne le voie :

```text
<Carte rang="R" famille="Coupes" />
        devient à peu près
jsx(Carte, { rang: "R", famille: "Coupes" })
```

Conséquences :
- Une balise JSX est une **valeur**. On peut la ranger dans une variable, la renvoyer depuis une fonction, la mettre dans un tableau.
- Les attributs sont des **propriétés d'objet**, donc en camelCase : `onClick`, `tabIndex`, `htmlFor` (au lieu de `for`).
- `style` prend un **objet**, pas une chaîne : `style={{ opacity: 0.5, marginTop: 8 }}`. Les doubles accolades, ce sont les accolades du JSX, puis celles de l'objet.
- `{…}` dans le texte affiche une chaîne, un nombre, une balise ou un tableau de balises, **pas un objet**. Observé avec `<p>{joueur}</p>` : `Objects are not valid as a React child (found: object with keys {a})`. Affiche un champ : `{joueur.nom}`. TypeScript le refuse déjà à la compilation.

---

## 3. Afficher sous condition

```tsx
type Props = { defausses: number; enAttente: boolean };

export function Statut({ defausses, enAttente }: Props) {
    if (enAttente) return <p>En attente de l'adversaire…</p>;

    return (
        <div>
            {defausses > 0 && <p>Il te reste {defausses} défausses.</p>}
            {defausses === 0 ? <p>Plus de défausse.</p> : null}
        </div>
    );
}
```

- Un `if` avec un `return` anticipé, **avant** le JSX : c'est le plus lisible quand tout l'affichage change.
- `condition && <…/>` : si la condition est fausse, rien ne s'affiche.
- `condition ? <A/> : <B/>` : l'un ou l'autre.
- `null`, `undefined`, `false` et `true` n'affichent **rien**.

🚨 **`{defausses && …}` affiche `0`.** Observé : avec `defausses = 0`, la page affiche le chiffre « 0 ». `0 && x` vaut `0`, et React affiche les nombres. Écris toujours une **vraie** condition : `defausses > 0 && …`.

---

## 4. Les listes et la `key`

```tsx
import { Carte } from "./Carte";

type CarteJeu = { id: string; rang: string; famille: "Coupes" | "Deniers" | "Épées" | "Bâtons" };

export function Main({ cartes }: { cartes: CarteJeu[] }) {
    return (
        <div className="main">
            {cartes.map((c) => (
                <Carte key={c.id} rang={c.rang} famille={c.famille} />
            ))}
        </div>
    );
}
```

- `.map()` transforme le tableau de données en tableau d'éléments, et React sait afficher un tableau.
- `key={c.id}` : un identifiant **stable et unique parmi les frères**. React s'en sert pour savoir quel élément est lequel d'un rendu à l'autre. `key` n'est pas une prop : `Carte` ne la reçoit pas.
- Sans `key`, la console affiche (observé) : `Each child in a list should have a unique "key" prop.`

### Pourquoi pas l'index

Avec `key={i}`, React associe l'état d'un composant à sa **position**, pas à sa donnée. Observé avec une liste `As`, `Roi`, `Dame`, où chaque ligne a son propre champ de texte : on tape une note sur la ligne `As`, puis on retire `As` de la liste.

| `key` | Ce qui reste à l'écran |
|---|---|
| `key={i}` | `Roi = 'note de l'As'`, `Dame = ''` : la note de l'As a glissé sur le Roi. |
| `key={nom}` | `Roi = ''`, `Dame = ''` : correct. |

L'index n'est acceptable que pour une liste qui ne bouge **jamais** (ni ajout au milieu, ni suppression, ni tri). Une main de cartes bouge tout le temps : donne à chaque carte un `id` dès la donne (c'est ce que fait déjà le moteur du bac à sable).

🔗 **Lien C.** La `key`, c'est une clé primaire. Comparer deux listes par position plutôt que par clé, c'est comparer deux tableaux de structures en supposant que l'élément `i` est toujours le même : faux dès qu'on retire le premier.

---

## 5. `children` : un composant qui en entoure d'autres

```tsx
import type { ReactNode } from "react";

type CadreProps = { titre: string; children: ReactNode };

export function Cadre({ titre, children }: CadreProps) {
    return (
        <section className="cadre">
            <h2>{titre}</h2>
            {children}
        </section>
    );
}

export function Exemple() {
    return (
        <Cadre titre="Manche 2">
            <p>Score : 153</p>
        </Cadre>
    );
}
```

- Ce qui est écrit **entre** `<Cadre>` et `</Cadre>` arrive dans la prop `children`.
- `ReactNode` : le type de « tout ce que React sait afficher » (élément, texte, nombre, tableau, `null`).
- C'est ainsi qu'on fait des mises en page réutilisables (un cadre, une modale, une colonne) sans savoir d'avance ce qu'elles contiendront.

---

## 🧪 À essayer

1. Affiche trois `Carte`, dont une grisée. Passe `famille="Trèfle"` : que dit l'éditeur ?
2. Remplace `{defausses > 0 && …}` par `{defausses && …}` et affiche `<Statut defausses={0} enAttente={false} />`. Qu'apparaît-il ?
3. Retire la `key` de `Main` et ouvre la console. Puis mets `key={c.rang}` avec deux Rois dans la main : que dit React ?
4. Reproduis le tableau du §4 : une liste de trois noms, un `<input>` par ligne, un bouton qui retire le premier. Compare `key={i}` et `key={nom}`.
5. Écris `<Cadre>` sans `titre`. Puis écris-le sans enfant : TypeScript accepte-t-il ? Rends `children` optionnel (`children?: ReactNode`).
