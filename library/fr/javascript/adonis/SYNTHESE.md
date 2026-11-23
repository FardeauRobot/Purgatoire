# 📒 Synthèse : AdonisJS et le backend de Transcendance

> **TL;DR.** La fiche de révision de [`adonis/`](INDEX.md). Le trajet d'une requête, puis chaque brique (routes, contrôleurs, Vine, Lucid, auth, middlewares, Tuyau, Socket.IO, Japa, déploiement) réduite à ce qu'il faut savoir par cœur, avec la fiche et le paragraphe qui l'expliquent. Les blocs de code sont recopiés de [`ROUTES_CONTROLEURS`](ROUTES_CONTROLEURS.md) §1–2 et [`VINE`](VINE.md) §1, raccourcis par `// …`.

Liens : [`INDEX`](INDEX.md) · [`ABORDER`](ABORDER.md) · [`../SYNTHESE`](../SYNTHESE.md) · [`../react/SYNTHESE`](../react/SYNTHESE.md)

Versions : `@adonisjs/core` 7.6, Lucid 22.4, auth 10.1, Vine 4.4, Tuyau 1.3, Japa 5.4, Socket.IO 4.8, Postgres 18 (voir [`INDEX`](INDEX.md), « Ce qui a été vérifié »).

---

## 1. L'idée en cinq lignes

- **Le besoin** : un backend complet (routes, validation, base, auth, tests) sans assembler dix bibliothèques soi-même.
- **La réponse** : un framework **« piles incluses »**, à conventions, proche de Laravel. Chaque chose a son dossier.
- **Le trajet d'une requête** est toujours le même : kernel → route → contrôleur → validateur → modèle → JSON.
- **Tout ce qui entre se valide** (Vine), parce que les types TypeScript n'existent plus à l'exécution.
- **L'identité vient du jeton**, jamais de ce que le client écrit.

---

## 2. Où va quoi

| Dossier / fichier | Rôle | Fiche |
|---|---|---|
| `start/routes.ts` | méthode + chemin → méthode de contrôleur | [`ROUTES_CONTROLEURS`](ROUTES_CONTROLEURS.md) §1 |
| `start/kernel.ts` | les trois piles de middlewares | [`MIDDLEWARE_ERREURS`](MIDDLEWARE_ERREURS.md) §1 |
| `start/ws.ts` | les événements Socket.IO (le `routes.ts` du temps réel) | [`SOCKET_IO`](SOCKET_IO.md) §1 |
| `app/controllers/` | une classe par ressource, une méthode `async` par route | [`ROUTES_CONTROLEURS`](ROUTES_CONTROLEURS.md) §2–3 |
| `app/validators/` | les schémas Vine | [`VINE`](VINE.md) |
| `app/models/` | les modèles Lucid (relations, méthodes) | [`LUCID`](LUCID.md) §2 |
| `database/migrations/` | l'historique du schéma ; `database/schema.ts` est **généré** | [`LUCID`](LUCID.md) §1–2 |
| `app/exceptions/handler.ts` | transforme une erreur en réponse JSON | [`MIDDLEWARE_ERREURS`](MIDDLEWARE_ERREURS.md) §3–5 |
| `tests/**/*.spec.ts` | les tests Japa | [`TESTS_JAPA`](TESTS_JAPA.md) |
| `apps/types` | le contrat Socket.IO partagé front/back | [`SOCKET_IO`](SOCKET_IO.md) §2 |
| `.adonisjs/client/registry/` | le registre Tuyau, **généré**, non commité | [`TUYAU`](TUYAU.md) §2 |

---

## 3. Les briques

### Routes et contrôleurs ([`ROUTES_CONTROLEURS`](ROUTES_CONTROLEURS.md))
```ts
router
  .group(() => {
    router.post('signup', [controllers.NewAccount, 'store'])
    router.post('login', [controllers.AccessTokens, 'store'])
    router.post('login/verify', [controllers.AccessTokens, 'verify2fa']).use(middleware.auth())
  })
  .prefix('auth')
  .as('auth')
```
- Un groupe ajoute un préfixe de chemin (`.prefix`), un préfixe de **nom** (`.as`), des middlewares (`.use`). Les préfixes s'empilent (§1).
- Une méthode de contrôleur reçoit **un** objet, le `HttpContext` : `request`, `response`, `params`, `auth`, `serialize`, `logger` (§2).
- Ce qu'elle **renvoie** devient le corps JSON ; `response.created(x)`, `.notFound(x)`… fixent le statut (§2).
- `:id` toujours avec `.where('id', router.matchers.number())`, sinon `/games/abc` donne une 500 (§4).
- `serialize(…)` + un **transformer** choisissent les champs exposés (§5).

### Validation : Vine ([`VINE`](VINE.md))
```ts
export const signupValidator = vine.create({
  nickname: vine
    .string()
    .trim()
    .minLength(3)
    .maxLength(20)
    .alphaNumeric()
    .unique({ table: 'users', column: 'nickname' }),
  // …
})
```
```ts
const { nickname, email, password, avatar } = await request.validateUsing(signupValidator)
```
- Valide → objet **typé et nettoyé** (champs inconnus retirés, `trim`, conversions). Invalide → **422** avec toutes les erreurs, le contrôleur s'arrête (§1–2).
- Tout champ est **obligatoire** par défaut ; `.optional()` pour l'inverse (§1).
- `.unique` / `exists` interrogent la base, après les règles précédentes (§1, §3).
- `request.only([...])` **ne valide rien** (§2).

### Base de données : Lucid ([`LUCID`](LUCID.md))
```text
 migration (on n'en modifie jamais une jouée) ─► migration:run ─► database/schema.ts (généré) ─► modèle (relations)
```
| Relation | Où est la clé étrangère | § |
|---|---|---|
| `belongsTo` | dans **ma** table | 4 |
| `hasMany` / `hasOne` | dans **l'autre** table | 4 |
| `manyToMany` | dans une **table pivot** | 4, 6 |
- Une relation mal déclarée compile et ne plante qu'au premier `preload` : 5 sur 11 chez l'équipe (§5).
- `snake_case` en base, `camelCase` en TypeScript (§2).

### Authentification ([`AUTH`](AUTH.md))
- Guard `api` : `Authorization: Bearer oat_…`, la table ne garde que le **hash** (§1).
- Abilities : `["*"]` (accès complet) ou `["2fa:verify"]` (l'étape 2FA seulement, 5 minutes) (§1).
- `auth()` : jeton valide et non expiré. `requireFullAccess()` : en plus, ability `*` (TL;DR, §4).
- À corriger chez l'équipe : quatre noms pour le même jeton, secret 2FA régénérable après activation, jetons pleins sans expiration (§2–3, §5).

### Middlewares et erreurs ([`MIDDLEWARE_ERREURS`](MIDDLEWARE_ERREURS.md))
- Trois piles : **serveur** (toute requête), **routeur** (si une route correspond), **nommés** (sur demande) (§1).
- En production, une 500 laisse fuir son message brut (SQL, chemins) si le handler ne la masque pas (§4–5).

### Le lien avec le front
| Outil | L'essentiel | Fiche |
|---|---|---|
| **Tuyau** | `node ace codegen` génère un registre ; le front appelle `api.request('auth.access_tokens.store', { body })`, tout typé ; `.safe()` pour gérer les erreurs | [`TUYAU`](TUYAU.md) §2, §4 |
| **Socket.IO** | branché sur le serveur HTTP d'Adonis (`app/services/realtime.ts`) ; contrat dans `apps/types` ; jeton vérifié à la connexion, sockets coupées au logout, payloads validés par Vine | [`SOCKET_IO`](SOCKET_IO.md) §1–5 |

### Tests : Japa ([`TESTS_JAPA`](TESTS_JAPA.md))
- `node ace test` : un vrai serveur en environnement `test`, les fichiers `tests/**/*.spec.ts`.
- `client.post(url).json(…)`, `.loginAs(user)`, `db.assertHas(table, {…})`, `testUtils.db().withGlobalTransaction()` pour tout annuler après chaque test.
- Une **base de test séparée** (`.env.test`), migrée au démarrage (§3–4). Un test par relation Lucid (§5).

### Déploiement ([`DEPLOIEMENT`](DEPLOIEMENT.md))
```text
 internet ─► caddy :80/:443 ─┬─ /            → front buildé (try_files … /index.html)
                             ├─ /api/v1/*    → backend :3333 (interne)
                             └─ /socket.io/* → backend :3333
                                                  │
                                              db (Postgres 18), réseau sans accès extérieur
```
- Une dépendance déclarée seulement à la racine du monorepo manque dans l'image (§2).
- Un chemin non relayé par Caddy renvoie `index.html` avec un **200** (§4).
- Un healthcheck qui ne touche pas la base répond « ok » quand elle est tombée (§6).

### Diagnostic ([`LIMITES_DEBUG`](LIMITES_DEBUG.md))
`node ace list:routes` · `node ace migration:status` · l'écouteur `db:query` · le `request_id` dans les logs.

---

## 🧠 Questions de révision

> [!question]- Quels sont les trois effets d'un `.group()` sur les routes qu'il contient ?
> Un préfixe de chemin (`.prefix`), un préfixe de nom (`.as`), et des middlewares (`.use`). ([`ROUTES_CONTROLEURS`](ROUTES_CONTROLEURS.md) §1)

> [!question]- Que fait `.trim()` dans un schéma Vine : un contrôle ou une transformation ?
> Une transformation : la valeur renvoyée est nettoyée, avant que `minLength` et `unique` s'appliquent. ([`VINE`](VINE.md) §1)

> [!question]- Tu ajoutes une colonne `elo` aux utilisateurs. Qu'écris-tu, et où ?
> Une **nouvelle** migration, puis `node ace migration:run`, qui régénère `database/schema.ts`. Rien dans le modèle. ([`LUCID`](LUCID.md) §1–2)

> [!question]- Pourquoi un utilisateur qui a seulement passé le mot de passe (2FA activée) ne peut-il pas créer une partie ?
> Son jeton temporaire porte l'ability `2fa:verify`, pas `*` : `requireFullAccess()` le refuse. ([`AUTH`](AUTH.md) §1, §4)

> [!question]- Le front affiche une erreur de type sur `api.request(…)` après ton `git pull`. Première chose à faire ?
> `node ace codegen` : le registre Tuyau n'est pas commité, il est périmé. ([`TUYAU`](TUYAU.md) §2)

> [!question]- Dans un test Japa, comment éviter qu'une inscription laisse un utilisateur en base ?
> `testUtils.db().withGlobalTransaction()` : tout ce que le test écrit est annulé à la fin. ([`TESTS_JAPA`](TESTS_JAPA.md))

> [!question]- `/avatars/x.png` renvoie du HTML avec un statut 200 en production. Pourquoi ?
> Caddy ne relaie pas ce chemin au backend : il tombe dans `try_files` et sert `index.html`. ([`DEPLOIEMENT`](DEPLOIEMENT.md) §4)

> [!question]- Un utilisateur signale « erreur 500 ». Comment retrouver la trace côté serveur ?
> Par le `request_id` : chaque erreur journalisée le porte, et avec le gestionnaire de [`MIDDLEWARE_ERREURS`](MIDDLEWARE_ERREURS.md) §5 le client reçoit le même. ([`LIMITES_DEBUG`](LIMITES_DEBUG.md) §1)
