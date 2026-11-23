# 6. Tuyau : le client typé entre le front et le back

> **TL;DR.** `node ace codegen` lit les routes, les validateurs et les contrôleurs du backend, et écrit un **registre** dans `apps/backend/.adonisjs/client/registry/`. Le front l'importe (`@api-starter-kit/backend/registry`) et appelle `api.request('auth.access_tokens.store', { body })` : le **nom** de la route, le **corps** et la **réponse** sont tous typés.
>
> Le type du corps vient du **validateur**, celui de la réponse vient du **`return` du contrôleur**. Un contrôleur sans validateur donne un corps typé `{}`. Une réponse incohérente donne un type union, que TypeScript refuse de lire sans vérification.
>
> Le registre n'est **pas commité** : il faut relancer `codegen` après chaque changement de route ou de réponse.

Liens : [`INDEX`](INDEX.md) · [`INSTALLATION`](INSTALLATION.md) (§5) · [`ROUTES_CONTROLEURS`](ROUTES_CONTROLEURS.md) · [`VINE`](VINE.md) · [`AUTH`](AUTH.md) (§3, §6) · [`../react/API`](../react/API.md) · [doc Tuyau](https://tuyau.julr.dev)

🧪 Labo : aucun. Les fichiers `sonde.ts` et `sonde_err.ts` ont été typecheckés avec la configuration TypeScript du front de l'équipe, puis lancés sous Node contre une copie du backend.

---

## 1. Le client de l'équipe

`apps/frontend/src/lib/api.ts` :

```ts
import { createTuyau } from '@tuyau/core/client'
import { registry } from '@api-starter-kit/backend/registry'

export const api = createTuyau({
  baseUrl: import.meta.env.VITE_API_URL || 'http://localhost:3333',
  registry,
  headers: { Accept: 'application/json' },
  hooks: {
    beforeRequest: [
      (request) => {
        const token = localStorage.getItem('auth_token')
        if (token) {
          request.headers.set('Authorization', `Bearer ${token}`)
        }
      }
    ]
  }
})
```

- `@api-starter-kit/backend/registry` : un import **d'un autre workspace** du monorepo. Le `package.json` du backend l'expose par `"exports": { "./registry": "./.adonisjs/client/registry/index.ts" }`. Le front importe donc un **fichier TypeScript du backend**, non compilé.
- `baseUrl` : `VITE_API_URL` en dev (vite sur 5173, Adonis sur 3333). En prod, Caddy sert les deux sur la même origine, mais le build Docker n'a pas de `.env` : le repli `'http://localhost:3333'` est alors **figé dans le bundle** dès qu'un écran importe `api` ([`DEPLOIEMENT`](DEPLOIEMENT.md), §3, avec la correction).
- `hooks.beforeRequest` : avant **chaque** requête, on lit le jeton et on l'ajoute. Le jeton n'est donc pas figé à la création du client : un login suivi d'un `localStorage.setItem` prend effet tout de suite.

🚨 **Le front compile des fichiers du backend.** Les modèles utilisent des décorateurs (`@column`). Sans `experimentalDecorators` dans le tsconfig du front, on obtient `TS1206 Decorators are not valid here` (observé). L'équipe a recopié les options du backend sous le commentaire `/* same of adonisjs */` du `tsconfig.app.json`. Ne les retire pas en « nettoyant ».

🔗 **Lien C.** Le registre joue le rôle d'un `.h` partagé : le serveur et le client lisent le **même** prototype, et le compilateur refuse un appel qui ne correspond pas. Sans lui, front et back se mettent d'accord par écrit, comme deux fichiers `.c` qui recopient chacun le prototype à la main.

---

## 2. Ce que génère `codegen`

Pour la route de login, `schema.d.ts` contient (observé) :

```ts
'auth.access_tokens.store': {
  methods: ["POST"]
  pattern: '/api/v1/auth/login'
  types: {
    body: ExtractBody<InferInput<(typeof import('#validators/user').loginValidator)>>
    response: ExtractResponse<Awaited<ReturnType<import('#controllers/access_tokens_controller').default['store']>>>
    errorResponse: … | { status: 422; response: { errors: SimpleError[] } }
  }
}
```

- La clé `'auth.access_tokens.store'` est le **nom** de la route, celui qu'affiche `node ace list:routes` ([`ROUTES_CONTROLEURS`](ROUTES_CONTROLEURS.md), §2).
- `body` est **déduit** du validateur que le contrôleur passe à `validateUsing`. Si le contrôleur fait `request.only([...])`, Tuyau ne trouve pas de validateur, et le corps est typé `{}`. Observé avec le code de l'équipe : impossible de passer `{ email, password }` sans erreur TypeScript. Brancher `loginValidator` ([`VINE`](VINE.md), §2), puis relancer `codegen`, règle le problème.
- `response` est le **type de retour** de la méthode du contrôleur. Chaque `return response.ok({...})` y ajoute un membre.
- `errorResponse` ajoute automatiquement le 422 quand il y a un validateur.

🚨 **Oublier `codegen`, c'est typer contre l'ancien back.** Le dossier `.adonisjs/` n'est pas commité. Après un `git pull` qui touche des routes, le front typecheck contre le **dernier registre généré chez toi**. `node ace serve --watch` le régénère au démarrage et à chaque modification (observé : la date de `schema.d.ts` change dès qu'on touche un contrôleur). C'est le hook `generateRegistry()` déclaré dans `adonisrc.ts`. Un `tsc` lancé seul ne le fait pas.

---

## 3. Le bug de login que TypeScript aurait vu

Le contrôleur de l'équipe renvoie `temp` sans 2FA et `tempToken` avec ([`AUTH`](AUTH.md), §3). Sur le front, quelqu'un écrira naturellement :

```ts
const login = await api.request('auth.access_tokens.store', { body: { email, password } })
const jeton: string = login.token
```

Résultat du typecheck contre le registre généré depuis le code de l'équipe (observé) :

```
error TS2339: Property 'token' does not exist on type 'ExtractResponse<
  { __response: { message: string; requires2fa: boolean; tempToken: string; }; __status: 200; } |
  { __response: { message: string; requires2fa: boolean; temp: string; }; __status: 200; }>'.
```

- TypeScript voit les **deux** `return` et refuse `login.token` : la clé n'existe dans aucune des branches.
- `requires2fa` est typé `boolean`, pas `true` ou `false`. Même `if (login.requires2fa)` ne permet donc **pas** de savoir dans quelle branche on est : il faudrait tester `'tempToken' in login`.

Avec la correction d'[`AUTH`](AUTH.md) §6 (une seule clé `token`, `true as const` / `false as const`), puis `codegen`, le même code front typecheck. Il tourne aussi : `login → connecté | jeton : oat_…` (observé).

C'est l'intérêt principal de Tuyau dans ce projet : une incohérence du back devient une **erreur de compilation** dans le front, au lieu d'un `undefined` à l'exécution.

---

## 4. Gérer les erreurs : `.safe()`

Par défaut, `api.request` **lève** une `TuyauHTTPError` dès que le code n'est pas 2xx. Observé sur une erreur de validation : `name: TuyauHTTPError`, `status: 422`, `response: {"errors":[…]}`.

Pour ne pas multiplier les `try/catch`, `.safe()` renvoie un couple `[données, erreur]`. Module complet, typecheck avec la config du front :

```ts
import { createTuyau } from '@tuyau/core/client'
import { registry } from '@api-starter-kit/backend/registry'

const api = createTuyau({ baseUrl: 'http://localhost:3399', registry })

export async function connecter(email: string, password: string) {
  const [res, err] = await api
    .request('auth.access_tokens.store', { body: { email, password } })
    .safe()

  if (err) {
    if (err.isValidationError()) {
      return { ok: false as const, champs: err.response?.errors.map((e) => e.field) ?? [] }
    }
    return { ok: false as const, champs: [], message: err.message }
  }

  localStorage.setItem('auth_token', res.token)
  return { ok: true as const, etape: res.requires2fa ? ('code-2fa' as const) : ('connecte' as const) }
}
```

- `const [res, err] = await … .safe()` : si `err` vaut `null`, alors `res` est forcément rempli. TypeScript le sait : après le `if (err)`, `res.token` est un `string`.
- `err.isValidationError()` : un **garde de type**. Dans le `if`, `err.response` est typé `{ errors: SimpleError[] }`, donc `e.field` est connu.
- `res.token` et `res.requires2fa` ne typecheckent **qu'avec** la réponse corrigée du §3.

Observé (le `localStorage` est simulé sous Node) :

```
connecter('pas-un-mail', '')          → {"ok":false,"champs":["email","password"]}
connecter('inconnu@x.io', 'motdepasse') → {"ok":false,"champs":[],"message":"Request failed with status code 400: POST /api/v1/auth/login"}
connecter(<compte créé>, 'motdepasse')  → {"ok":true,"etape":"connecte"}   jeton stocké : oat_…
```

🚨 `err.message` est le message **de Tuyau** (« Request failed with status code 400 »), pas celui du serveur. Le texte « Invalid user credentials » se trouve dans `err.response`, et sa forme dépend de la route ([`MIDDLEWARE_ERREURS`](MIDDLEWARE_ERREURS.md), §3).

---

## 🧪 À essayer

1. Dans `GamesController.show`, ajoute un champ à la réponse sans relancer `codegen`, puis lis-le côté front. Relance `codegen` et recommence.
2. Remets `request.only(['email', 'password'])` dans le login, lance `codegen`, et lis le type de `body` au survol dans VS Code.
3. Écris `deconnecter()` avec `.safe()` : appel de `profile.access_tokens.destroy`, puis `localStorage.removeItem('auth_token')`, **même si** l'appel échoue.
4. Dans `connecter`, ajoute une branche `err.isStatus(400)` et affiche le message du serveur.
