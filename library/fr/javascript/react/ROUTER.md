# 6. React Router 8 : des pages, des layouts, des loaders

> **TL;DR.** Une application React n'a qu'**une** page HTML. React Router lit l'URL et choisit quels composants afficher, sans recharger. On décrit les routes dans un tableau donné à `createBrowserRouter` : un **layout** affiche le cadre commun et un `<Outlet />` où viennent les pages enfants ; `:id` dans un chemin devient `useParams().id`. Un **loader** charge les données **avant** d'afficher la page (l'URL ne change qu'une fois les données prêtes), peut **rediriger** (`throw redirect(…)`) ou **échouer** (`throw data(…, { status: 404 })`), et l'erreur s'affiche dans l'`ErrorBoundary` de la route. On navigue avec `<Link>`, jamais avec `<a href>`.

Liens : [`INDEX`](INDEX.md) · [`EFFETS`](EFFETS.md) (§3, charger des données) · [`CONTEXTE_HOOKS`](CONTEXTE_HOOKS.md) · [`API`](API.md) · [doc React Router](https://reactrouter.com)

🧪 Labo : aucun. Le `main.tsx` du dépôt de l'équipe en est un bon point de départ.

---

## 1. Les trois façons d'utiliser React Router

React Router 8 se présente en trois « modes ». Celui de l'équipe, et de cette fiche, est le **mode données** (*data mode*).

| Mode | Point d'entrée | Ce qu'il apporte |
|---|---|---|
| Déclaratif | `<BrowserRouter>` + `<Routes>` | Seulement l'aiguillage URL → composant. |
| **Données** | `createBrowserRouter([...])` + `<RouterProvider>` | En plus : loaders, actions, erreurs par route, état de navigation. |
| Framework | un plugin Vite + des fichiers de routes | En plus : rendu serveur, découpage automatique. C'est l'ex-Remix. |

Dans les tutoriels, tu croiseras surtout le mode déclaratif (`<Routes><Route …/></Routes>`). Il marche, mais n'a ni loaders ni `ErrorBoundary`.

🚨 Les imports : tout vient de **`"react-router"`**, sauf `RouterProvider` que l'équipe importe depuis `"react-router/dom"` (il est exporté par les deux). Le paquet `react-router-dom` des versions 6 et antérieures n'est plus nécessaire.

---

## 2. Un routeur complet

```tsx
import {
    createBrowserRouter,
    data,
    isRouteErrorResponse,
    Link,
    NavLink,
    Outlet,
    redirect,
    useLoaderData,
    useNavigation,
    useRouteError,
    type LoaderFunctionArgs,
} from "react-router";
import { RouterProvider } from "react-router/dom";

type Salon = { id: string; nom: string; joueurs: number };

async function chargerSalon({ params }: LoaderFunctionArgs): Promise<Salon> {
    const r = await fetch(`/api/v1/rooms/${params.id}`);
    if (r.status === 404) throw data("Salon introuvable", { status: 404 });
    if (!r.ok) throw new Error(`HTTP ${r.status}`);
    return (await r.json()) as Salon;
}

function exigerConnexion() {
    if (!localStorage.getItem("auth_token")) throw redirect("/connexion");
    return null;
}

function Cadre() {
    const navigation = useNavigation();
    return (
        <div>
            <nav>
                <NavLink to="/">Accueil</NavLink> · <NavLink to="/profil">Profil</NavLink>
            </nav>
            {navigation.state === "loading" && <p className="barre-chargement">Chargement…</p>}
            <main>
                <Outlet />
            </main>
        </div>
    );
}

function Accueil() {
    return (
        <ul>
            <li><Link to="/salon/1">Salon 1</Link></li>
            <li><Link to="/salon/2">Salon 2</Link></li>
        </ul>
    );
}

function PageSalon() {
    const salon = useLoaderData<typeof chargerSalon>();
    return <h1>{salon.nom} : {salon.joueurs}/4 joueurs</h1>;
}

function ErreurSalon() {
    const e = useRouteError();
    if (isRouteErrorResponse(e)) return <h1>{e.status} : {String(e.data)}</h1>;
    return <h1>Erreur inattendue</h1>;
}

const routeur = createBrowserRouter([
    {
        path: "/",
        Component: Cadre,
        children: [
            { index: true, Component: Accueil },
            { path: "salon/:id", loader: chargerSalon, Component: PageSalon, ErrorBoundary: ErreurSalon },
            { path: "profil", loader: exigerConnexion, element: <h1>Mon profil</h1> },
            { path: "connexion", element: <h1>Connexion</h1> },
            { path: "*", element: <h1>Page introuvable</h1> },
        ],
    },
]);

export default function App() {
    return <RouterProvider router={routeur} />;
}
```

---

## 3. Démontage

### Les routes imbriquées et l'`Outlet`

```text
URL /salon/1
  └─ route "/"          → Cadre      (le menu, toujours là)
       └─ "salon/:id"   → PageSalon  (affichée à la place de <Outlet />)
```

- `children` : les routes **enfants** s'affichent **dans** le parent, à l'endroit de son `<Outlet />`. Le cadre (menu, barre de chargement) ne se redessine pas d'une page à l'autre.
- `index: true` : l'enfant affiché quand l'URL est exactement celle du parent (`/`).
- `path: "*"` : tout ce qui n'a pas trouvé de route.
- `Component: Cadre` ou `element: <Cadre />` : les deux marchent. `Component` reçoit le composant lui-même, `element` un élément déjà écrit.

### `Link` et `NavLink`

- `<Link to="/salon/1">` produit un vrai `<a href>`, mais intercepte le clic : pas de rechargement, l'état de l'application est conservé.
- 🚨 Un `<a href="/salon/1">` brut **recharge toute la page** : le store est vidé, la connexion Socket.IO est coupée et refaite.
- `<NavLink>` ajoute la classe `active` quand son lien correspond à l'URL courante, pour surligner l'onglet ouvert. Observé : le lien `/` est actif sur l'accueil, et **plus** sur `/salon/1`. Le lien racine n'est actif que sur la racine exacte.
- Pour naviguer depuis du code (après une connexion réussie) : `const naviguer = useNavigate(); naviguer("/profil");`.

### Le loader : les données avant la page

Observé avec un loader qui met 300 ms :

| Moment | URL | Écran | `navigation.state` |
|---|---|---|---|
| Juste après le clic | `/` | encore l'accueil | `loading` |
| Loader terminé | `/salon/1` | `Salon du Fou` | `idle` |

- La page **n'apparaît qu'avec ses données** : pas d'état « chargement » à gérer dans `PageSalon`, pas de `null` à tester.
- `useNavigation().state` sert à afficher un indicateur **global** pendant ce temps.
- `params.id` : la valeur du `:id` du chemin. Dans un composant, c'est `useParams().id`. C'est toujours une `string` (ou `undefined`) : à convertir si tu attends un nombre.
- `useLoaderData<typeof chargerSalon>()` : TypeScript déduit le type de retour du loader. Le mot-clé `typeof` donne le type de la fonction, pas sa valeur.
- Le loader remplace le `useEffect` + `fetch` d'[`EFFETS`](EFFETS.md), §3 : pas de double requête en StrictMode, pas de course entre deux réponses, le routeur s'en charge.

🔗 **Lien C.** Le loader, c'est charger la map **avant** d'ouvrir la fenêtre dans `so_long` : si le `.ber` est invalide, on n'affiche jamais une fenêtre à moitié remplie, on affiche l'erreur.

### Rediriger et échouer

- `throw redirect("/connexion")` : observé, l'URL devient `/connexion` sans jamais afficher le profil. C'est une **route protégée** sans composant spécial.
- `throw data("Salon introuvable", { status: 404 })` : observé, l'`ErrorBoundary` de la route affiche `404 Salon introuvable`, et le cadre reste en place.
- `isRouteErrorResponse(e)` distingue une erreur **lancée exprès** (`data(…)`, avec un statut) d'une vraie exception (un bug, un réseau coupé).
- Une route sans `ErrorBoundary` fait remonter l'erreur jusqu'au parent le plus proche qui en a une. Sans aucune, React Router affiche sa propre page d'erreur, peu engageante : mets-en au moins une sur la route racine.

🚨 **La redirection du loader n'est pas une sécurité.** Elle ne fait que décider quoi **afficher**. Les données protégées doivent être refusées par le **serveur** (le middleware `auth` d'Adonis). Un jeton présent dans le `localStorage` peut aussi être expiré : le loader ne le saura qu'en appelant l'API, qui répondra `401`.

---

## 4. Recharger la page sur `/salon/1`

En dev, Vite renvoie `index.html` pour n'importe quelle URL, donc `F5` sur `/salon/1` marche. En production, le serveur de fichiers doit faire de même, sinon il répond `404` : le fichier `/salon/1` n'existe pas.

Le Caddyfile de l'équipe le fait déjà, avec `try_files` :

```text
handle {
    root * /srv
    try_files {path} /index.html
    file_server
}
```

- `try_files {path} /index.html` : sert le fichier s'il existe (`/assets/index-abc.js`), sinon `index.html`, et React Router lit l'URL.
- 🚨 Une conséquence : une faute de frappe dans l'URL d'un **fichier** (`/assets/oubli.js`) renvoie `index.html` avec un statut `200`. Si une image ou un script « contient du HTML », c'est ce repli.

---

## 5. État dans l'URL

Ce que l'utilisateur voudrait **partager** ou **retrouver** après un `F5` va dans l'URL plutôt que dans un état :

```tsx
import { useSearchParams } from "react-router";

export function Classement() {
    const [params, setParams] = useSearchParams();
    const tri = params.get("tri") ?? "victoires";

    return (
        <>
            <button onClick={() => setParams({ tri: "victoires" })} aria-pressed={tri === "victoires"}>Victoires</button>
            <button onClick={() => setParams({ tri: "ratio" })} aria-pressed={tri === "ratio"}>Ratio</button>
            <p>Tri : {tri}</p>
        </>
    );
}
```

- `?tri=ratio` dans l'URL : le lien envoyé à un ami ouvre le même classement.
- `setParams` navigue (une entrée dans l'historique), donc le bouton « précédent » du navigateur annule le tri.

---

## 🧪 À essayer

1. Remplace un `<Link>` par un `<a href>` et ajoute un compteur `useState` dans `Cadre`. Incrémente-le, puis clique sur le lien : le compteur survit-il ? Et avec `<Link>` ?
2. Ajoute `await new Promise((r) => setTimeout(r, 1500))` dans `chargerSalon`, puis clique sur un salon. Où est l'URL pendant l'attente ? Que montre `navigation.state` ?
3. Retire `ErrorBoundary: ErreurSalon` et va sur `/salon/999`. Quelle page s'affiche ? Ajoute un `ErrorBoundary` sur la route `/` et recommence.
4. Écris un loader `exigerConnexion` qui garde aussi l'URL demandée : `redirect("/connexion?retour=/profil")`. Après la connexion, renvoie l'utilisateur à `retour`.
5. Lance `npm run build && npm run preview`, va sur `/salon/1`, appuie sur `F5`. Puis sers `dist/` avec un serveur sans repli (`python3 -m http.server -d dist 8000`) et recommence.
