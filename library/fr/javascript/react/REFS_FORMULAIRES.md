# 4. Les refs et les formulaires

> **TL;DR.** `useRef` donne une boîte `{ current }` qui **survit** aux rendus mais dont la modification **ne déclenche pas** de rendu : parfait pour un élément du DOM (donner le focus à un champ) ou un identifiant de minuteur. En React 19, `ref` se passe à un composant comme **une prop ordinaire**, sans `forwardRef`. Pour un formulaire, deux écoles : le champ **contrôlé** (`value` + `onChange`, l'état React fait foi), utile quand chaque frappe compte ; ou l'**action** de formulaire (`<form action={fn}>` + `useActionState`), qui reçoit un `FormData`, gère l'état « en cours » et vide le formulaire après l'envoi.

Liens : [`INDEX`](INDEX.md) · [`ETAT`](ETAT.md) · [`EFFETS`](EFFETS.md) · [`API`](API.md) · [`../react-three-fiber/ANIMATION`](../react-three-fiber/ANIMATION.md)

🧪 Labo : aucun.

---

## 1. `useRef` : une mémoire qui ne redessine pas

```tsx
import { useRef, useState } from "react";

export function Pseudo() {
    const champ = useRef<HTMLInputElement>(null);
    const essais = useRef(0);
    const [message, setMessage] = useState("");

    function valider() {
        essais.current++;
        const v = champ.current?.value.trim() ?? "";
        if (v.length < 3) {
            setMessage(`Trop court (essai ${essais.current})`);
            champ.current?.focus();
            return;
        }
        setMessage(`Bienvenue ${v}`);
    }

    return (
        <>
            <input ref={champ} placeholder="Pseudo" />
            <button onClick={valider}>Valider</button>
            <p>{message}</p>
        </>
    );
}
```

- `useRef<HTMLInputElement>(null)` : une boîte vide. `ref={champ}` demande à React d'y ranger l'élément `<input>` réel une fois affiché.
- `champ.current?.focus()` : on parle directement au DOM. C'est la porte de sortie pour ce que React ne sait pas exprimer en JSX (focus, mesure, défilement, un `<canvas>`).
- `essais.current++` : un compteur qui survit aux rendus, mais l'incrémenter ne redessine rien. S'il devait s'afficher, ce serait un état.
- 🚨 Ne lis et n'écris pas `.current` **pendant le rendu** (dans le corps du composant) : seulement dans un gestionnaire ou un effet. Pendant le rendu, l'élément n'existe peut-être pas encore, et ta valeur échapperait à React.

| | `useState` | `useRef` |
|---|---|---|
| Survit aux rendus | oui | oui |
| La modifier redessine | **oui** | **non** |
| Lecture | la valeur de ce rendu | toujours la dernière valeur |
| Pour… | ce qui s'affiche | le DOM, un id de minuteur, une connexion |

🔗 **Lien C.** Une ref, c'est un pointeur vers une `struct` que tu possèdes en dehors de la boucle d'affichage : tu peux écrire dedans quand tu veux, l'écran n'en sait rien. C'est exactement ce que fait `useFrame` en R3F pour bouger un mesh 60 fois par seconde sans rendu React ([`../react-three-fiber/ANIMATION`](../react-three-fiber/ANIMATION.md)).

---

## 2. Passer une ref à son propre composant (React 19)

```tsx
import { useRef, type Ref } from "react";

type ChampProps = { label: string; ref?: Ref<HTMLInputElement> };

function Champ({ label, ref }: ChampProps) {
    return (
        <label>
            {label} <input ref={ref} />
        </label>
    );
}

export function Connexion() {
    const pseudo = useRef<HTMLInputElement>(null);
    return (
        <>
            <Champ label="Pseudo" ref={pseudo} />
            <button onClick={() => pseudo.current?.focus()}>Aller au pseudo</button>
        </>
    );
}
```

- En React 19, `ref` arrive dans les props comme les autres : on la déclare dans le type, on la transmet à l'`<input>`. Observé : le focus atteint bien le champ.
- Avant React 19, il fallait envelopper `Champ` dans `forwardRef(...)`. Tu le verras dans du code ancien, dans des tutoriels, et dans le bac à sable de Transcendance, qui est en React 18.3.1. En React 19 il marche encore, mais il n'est plus nécessaire.

---

## 3. Le champ contrôlé

```tsx
import { useState } from "react";

export function Recherche({ noms }: { noms: string[] }) {
    const [texte, setTexte] = useState("");
    const trouves = noms.filter((n) => n.toLowerCase().includes(texte.toLowerCase()));

    return (
        <>
            <input value={texte} onChange={(e) => setTexte(e.target.value)} placeholder="Chercher un joueur" />
            <p>{trouves.length} résultat(s)</p>
            <ul>{trouves.map((n) => <li key={n}>{n}</li>)}</ul>
        </>
    );
}
```

- `value={texte}` : le champ affiche **ce que dit l'état**, rien d'autre.
- `onChange` : à chaque frappe, on met l'état à jour, React redessine, le champ affiche la nouvelle valeur.
- Le filtrage se fait **pendant** la frappe : c'est le cas où le champ contrôlé est le bon outil.
- 🚨 `value` sans `onChange` : le champ est bloqué, on ne peut plus rien taper, et la console le signale (observé) : ``You provided a `value` prop to a form field without an `onChange` handler. This will render a read-only field.`` Pour une valeur initiale qu'on laisse ensuite libre, c'est `defaultValue`.

🔗 **Lien C.** Un champ contrôlé, c'est un terminal en mode non canonique (`minishell` avec `readline` désactivé) : chaque touche passe par ton programme, qui décide de ce qui s'affiche.

---

## 4. Les actions de formulaire (React 19)

Pour un formulaire qu'on **envoie** (inscription, connexion, création d'un salon), React 19 propose plus simple que de contrôler chaque champ :

```tsx
import { useActionState } from "react";
import { useFormStatus } from "react-dom";

type Etat = { erreur: string | null; pseudo: string | null };

async function inscrire(_avant: Etat, data: FormData): Promise<Etat> {
    const pseudo = String(data.get("pseudo") ?? "").trim();
    if (pseudo.length < 3) return { erreur: "3 caractères minimum", pseudo: null };
    const r = await fetch("/api/v1/auth/register", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ username: pseudo, password: String(data.get("motdepasse") ?? "") }),
    });
    if (!r.ok) return { erreur: `Refusé (${r.status})`, pseudo: null };
    return { erreur: null, pseudo };
}

function BoutonEnvoyer() {
    const { pending } = useFormStatus();
    return <button disabled={pending}>{pending ? "Envoi…" : "S'inscrire"}</button>;
}

export function Inscription() {
    const [etat, action, enCours] = useActionState(inscrire, { erreur: null, pseudo: null });

    if (etat.pseudo) return <p>Bienvenue {etat.pseudo} !</p>;

    return (
        <form action={action}>
            <input name="pseudo" placeholder="Pseudo" />
            <input name="motdepasse" type="password" placeholder="Mot de passe" />
            <BoutonEnvoyer />
            {etat.erreur && <p role="alert">{etat.erreur}</p>}
            {enCours && <p>Vérification…</p>}
        </form>
    );
}
```

Ligne par ligne :
- `inscrire(_avant, data)` : l'**action**. Elle reçoit l'état précédent et le `FormData` du formulaire, et renvoie le nouvel état. Elle peut être `async`.
- `data.get("pseudo")` : la valeur du champ `name="pseudo"`. Aucun `useState` par champ, aucun `onChange`.
- `useActionState(inscrire, état initial)` renvoie `[état, action à brancher, en cours ?]`.
- `<form action={action}>` : au submit, React appelle l'action, sans rechargement de page (pas besoin de `e.preventDefault()`).
- `useFormStatus()` : lu dans un composant **enfant** du `<form>`, il dit si ce formulaire est en cours d'envoi. Il ne marche pas dans le composant qui affiche le `<form>` lui-même.

Observé avec une action qui attend 300 ms :

| Moment | Écran |
|---|---|
| Pendant l'envoi | bouton `Envoi…` désactivé, `enCours` vrai |
| Après `"ab"` | `3 caractères minimum`, et **le champ est vidé** |
| Après `"Fardeau"` | `bienvenue Fardeau`, champ vidé |

🚨 **Le formulaire est vidé après chaque action, même en cas d'erreur.** C'est le comportement de React 19 pour un `<form action>`. Pour une erreur, c'est agaçant : le joueur doit tout retaper. Deux solutions : renvoyer la saisie dans l'état et la remettre avec `defaultValue={etat.saisie}`, ou passer au champ contrôlé (§3) pour ce formulaire.

🚨 La validation dans le navigateur (longueur, format) **ne protège rien** : n'importe qui peut appeler l'API sans passer par ce formulaire. Elle sert le confort du joueur. La vraie validation est sur le serveur (VineJS, voir le dossier Adonis).

### Avec `onSubmit`, comme dans le dépôt de l'équipe

```tsx
import { useState, type SubmitEvent } from "react";

export function Message({ envoyer }: { envoyer: (texte: string) => void }) {
    const [erreur, setErreur] = useState<string | null>(null);

    function surEnvoi(e: SubmitEvent<HTMLFormElement>) {
        e.preventDefault();
        const form = e.currentTarget;
        const texte = String(new FormData(form).get("texte") ?? "").trim();
        if (!texte) return setErreur("Message vide");
        setErreur(null);
        envoyer(texte);
        form.reset();
    }

    return (
        <form onSubmit={surEnvoi}>
            <input name="texte" />
            <button>Envoyer</button>
            {erreur && <p>{erreur}</p>}
        </form>
    );
}
```

- `e.preventDefault()` : sans lui, le navigateur recharge la page (comportement HTML de base d'un `<form>`).
- `new FormData(form)` : la même lecture par `name` que les actions, à la main.
- `e.currentTarget` : le `<form>` sur lequel l'écouteur est posé. `e.target` serait l'élément qui a déclenché l'événement.
- `SubmitEvent<HTMLFormElement>` : le type de l'événement. Beaucoup de tutoriels écrivent `FormEvent`, mais `@types/react` 19.3 le marque `@deprecated` (« FormEvent doesn't actually exist »).
- C'est la forme utilisée dans `Chat.tsx` de l'équipe. Les deux formes sont correctes : `onSubmit` pour un envoi synchrone (un `emit` Socket.IO), l'action quand il y a une attente à afficher.

---

## 🧪 À essayer

1. Dans `Pseudo`, affiche `{essais.current}` dans le JSX. Cliquez trois fois sur Valider avec un pseudo trop court : le nombre affiché suit-il ? Pourquoi ?
2. Dans `Recherche`, retire `onChange`. Essaie de taper, puis lis la console. Remplace `value` par `defaultValue`.
3. Dans `Inscription`, tape un pseudo de deux lettres et envoie : le champ se vide. Ajoute `saisie` à `Etat`, renvoie-la en cas d'erreur, et remets-la avec `defaultValue={etat.saisie ?? ""}`. Observé : la saisie reste affichée après l'envoi.
4. Déplace `useFormStatus()` directement dans `Inscription`, au-dessus du `<form>`. Le bouton se désactive-t-il encore pendant l'envoi ?
5. Retire `e.preventDefault()` de `Message`. Que se passe-t-il à l'envoi ? Regarde l'URL.
