# 1. Routes, contrôleurs, réponses

> **TL;DR.** `start/routes.ts` associe une **méthode + un chemin** à une **méthode de contrôleur** : `router.post('login', [controllers.AccessTokens, 'store'])`. Les groupes ajoutent :
> - un préfixe (`.prefix('auth')`) ;
> - un préfixe de **nom** (`.as('auth')`) ;
> - des middlewares (`.use(middleware.auth())`).
>
> Un contrôleur est une classe dont chaque méthode reçoit le `HttpContext` : `request`, `response`, `auth`, `params`, `serialize`. Ce que la méthode **renvoie** devient le corps JSON. `response.created(…)`, `response.notFound(…)` et les autres fixent le statut. Chez l'équipe, `serialize(…)` enveloppe tout dans `{ "data": … }`, et un **transformer** choisit les champs exposés.

Liens : [`INDEX`](INDEX.md) · [`INSTALLATION`](INSTALLATION.md) · [`VINE`](VINE.md) · [`MIDDLEWARE_ERREURS`](MIDDLEWARE_ERREURS.md) · [`TUYAU`](TUYAU.md) · [`../react/API`](../react/API.md) · [doc AdonisJS, routing](https://docs.adonisjs.com/guides/basics/routing)

🧪 Labo : aucun. Les réponses citées ont été obtenues avec `curl` sur une copie du backend. Le contrôleur `Games` du §3 **n'existe pas chez l'équipe** (leur `GamesController` est une classe vide) : il a été écrit pour cette fiche, typechecké et appelé.

---

## 1. Lire `routes.ts`

Le fichier de l'équipe, auquel s'ajoute un groupe `games` (marqué ⊕) écrit pour l'exemple :

```ts
import { middleware } from '#start/kernel'
import router from '@adonisjs/core/services/router'
import { controllers } from '#generated/controllers'

router.get('/', () => {
  return { hello: 'world' }
})

router
  .group(() => {
    router
      .group(() => {
        router.post('signup', [controllers.NewAccount, 'store'])
        router.post('login', [controllers.AccessTokens, 'store'])
        router.post('login/verify', [controllers.AccessTokens, 'verify2fa']).use(middleware.auth())
      })
      .prefix('auth')
      .as('auth')

    router.get('card', [controllers.Cards, 'show'])

    router                                                                        // ⊕
      .group(() => {
        router.post('/', [controllers.Games, 'store'])
        router.get('/:id', [controllers.Games, 'show']).where('id', router.matchers.number())
      })
      .prefix('games')
      .as('games')
      .use(middleware.auth())
      .use(middleware.requireFullAccess())

    router
      .group(() => {
        router.get('profile', [controllers.Profile, 'show'])
        router.post('avatar', [controllers.Profile, 'uploadAvatar'])
        router.patch('profile', [controllers.Profile, 'update'])
        router.post('logout', [controllers.AccessTokens, 'destroy'])
        router.post('2fa/generate', [controllers.TwoFactorAuths, 'generate'])
        router.post('2fa/enable', [controllers.TwoFactorAuths, 'enable'])
      })
      .prefix('account')
      .as('profile')
      .use(middleware.auth())
      .use(middleware.requireFullAccess())
  })
  .prefix('/api/v1')
```

Ligne par ligne :
- `router.get('/', () => …)` : une route avec une **fonction** au lieu d'un contrôleur. Le healthcheck Docker appelle cette route ([`DEPLOIEMENT`](DEPLOIEMENT.md), §3).
- `router.group(() => { … })` : tout ce qui est déclaré **dans** la fonction reçoit ce qu'on chaîne **après** le groupe.
- `.prefix('/api/v1')` sur le groupe extérieur, `.prefix('auth')` sur le groupe intérieur : les préfixes **s'empilent**. `login` devient `/api/v1/auth/login`.
- `[controllers.AccessTokens, 'store']` : un **tableau** à deux cases, le contrôleur et le nom de sa méthode. `controllers` est généré par `codegen` ([`INSTALLATION`](INSTALLATION.md), §5). La chaîne `'store'` est vérifiée par TypeScript : `'stroe'` ne compile pas.
- `.use(middleware.auth())` posé sur **une** route (`login/verify`) ou sur un **groupe** (`account`). L'ordre compte : `auth` d'abord (qui es-tu ?), puis `requireFullAccess` (as-tu le droit ?) ([`AUTH`](AUTH.md), §4).
- `.where('id', router.matchers.number())` : la route ne correspond **que** si `:id` est un nombre ([§4](#4-les-paramètres-de-route)).

`node ace list:routes` affiche ce que le routeur a vraiment compris. Quand sa sortie part dans un pipe, il écrit une ligne JSON par route (observé, extrait, `handler` retiré) :

```
{"method":"POST","pattern":"/api/v1/auth/login","name":"auth.access_tokens.store"}
{"method":"POST","pattern":"/api/v1/auth/login/verify","name":"auth.access_tokens.verify_2_fa","middleware":["auth"]}
{"method":"POST","pattern":"/api/v1/games","name":"games.games.store","middleware":["auth","requireFullAccess"]}
{"method":"GET","pattern":"/api/v1/games/:id","name":"games.games.show","middleware":["auth","requireFullAccess"]}
{"method":"POST","pattern":"/api/v1/account/logout","name":"profile.access_tokens.destroy","middleware":["auth","requireFullAccess"]}
```

- Le **nom** d'une route se compose ainsi : préfixe `.as(…)` + nom du contrôleur en snake_case + méthode. D'où `games.games.store`, avec un `games` en double. Ces noms comptent : c'est par eux que le front appelle l'API avec Tuyau ([`TUYAU`](TUYAU.md), §2).
- `.as('profile')` sur le groupe `account` produit `profile.access_tokens.destroy` pour le **logout**. Le nom ne dit pas ce que fait la route : lis le `pattern`.

🔗 **Lien C.** `routes.ts` est une **table de dispatch** : un tableau de paires (clé → pointeur de fonction), comme le tableau `{ "cd", builtin_cd }` d'un minishell. Le routeur cherche la ligne qui correspond et appelle la fonction.

---

## 2. Le contrôleur et le `HttpContext`

Un contrôleur de l'équipe, en entier :

```ts
import UserTransformer from '#transformers/user_transformer'
import { avatarValidator, updateProfileValidator } from '#validators/user'
import type { HttpContext } from '@adonisjs/core/http'
import app from '@adonisjs/core/services/app'
import { unlink } from 'node:fs/promises'

export default class ProfileController {
  async show({ auth, serialize }: HttpContext) {
    return serialize(UserTransformer.transform(auth.getUserOrFail()))
  }
  // uploadAvatar, update…
}
```

- `export default class` : un fichier, un contrôleur. Le nom du fichier (`profile_controller.ts`) donne la clé `controllers.Profile`.
- `async show({ auth, serialize }: HttpContext)` : Adonis passe **un seul** objet, le contexte de la requête, et on y prend par déstructuration ce dont on a besoin.
- `auth.getUserOrFail()` : l'utilisateur authentifié par le middleware `auth`. « OrFail » : s'il n'y en a pas, cela lève une exception qui devient un 401.

Ce qu'il y a dans le contexte, et à quoi ça sert :

| Champ | Usage |
|---|---|
| `request` | `request.validateUsing(v)`, `request.input('code')`, `request.only([...])`, `request.file('avatar')` |
| `response` | `response.ok(x)`, `.created(x)`, `.badRequest(x)`, `.unauthorized(x)`, `.notFound(x)`, `.status(418).send(x)` |
| `params` | les `:id` du chemin |
| `auth` | `auth.user`, `auth.getUserOrFail()` |
| `serialize` | ajouté par l'équipe dans `providers/api_provider.ts` (§5) |
| `logger` | `logger.info(…)` lié à la requête (l'`x-request-id` apparaît dans le log) |

🔗 **Lien C.** C'est le `t_data *data` qu'on passe à toutes les fonctions d'un projet 42 au lieu de dix paramètres. Ici, le framework le construit pour chaque requête et le jette après.

---

## 3. Un contrôleur complet : `GamesController`

Écrit pour cette fiche : créer une partie contre un adversaire, puis la lire. Typechecké et appelé.

```ts
import Game from '#models/game'
import { createGameValidator } from '#validators/game'
import type { HttpContext } from '@adonisjs/core/http'

export default class GamesController {
  async store({ auth, request, response }: HttpContext) {
    const moi = auth.getUserOrFail()
    const { adversaireId } = await request.validateUsing(createGameValidator)
    if (adversaireId === moi.id) {
      return response.badRequest({ message: 'Tu ne peux pas jouer contre toi-même.' })
    }

    const game = await Game.create({})
    await game.related('users').attach([moi.id, adversaireId])
    return response.created({ id: game.id })
  }

  async show({ params, auth, response }: HttpContext) {
    const moi = auth.getUserOrFail()
    const game = await Game.query()
      .where('id', params.id)
      .whereHas('users', (q) => q.where('users.id', moi.id))
      .preload('users')
      .first()

    if (!game) {
      return response.notFound({ message: 'Partie introuvable.' })
    }
    return {
      id: game.id,
      joueurs: game.users.map((u) => ({
        id: u.id,
        nickname: u.nickname,
        score: u.$extras.pivot_score as number,
      })),
    }
  }
}
```

- `request.validateUsing(createGameValidator)` : valide le corps **ou lève** une erreur 422. Après cette ligne, `adversaireId` est un `number` qui existe dans la table `users` ([`VINE`](VINE.md), §3).
- `return response.badRequest({...})` : le `return` est important. Sans lui, la fonction continue et crée quand même la partie.
- `game.related('users').attach([...])` : remplit la table pivot `games_users`. Cela ne marche qu'avec une relation `manyToMany`, que le modèle `Game` de l'équipe **n'a pas** ([`LUCID`](LUCID.md), §5).
- `.whereHas('users', …)` : « seulement les parties où je joue ». Sans cette ligne, n'importe qui lit n'importe quelle partie en changeant l'`id`. C'est une faille IDOR classique.
- `return { … }` : un objet ordinaire renvoyé tel quel devient la réponse **200** en JSON, sans enveloppe `data`.

Observé :

```
POST /api/v1/games {"adversaireId":26}      → 201 {"id":5}
POST /api/v1/games {"adversaireId":25}      → 400 {"message":"Tu ne peux pas jouer contre toi-même."}
POST /api/v1/games {"adversaireId":999999}  → 422 {"errors":[{"message":"The selected adversaireId is invalid","rule":"database.exists","field":"adversaireId"}]}
GET  /api/v1/games/6                        → 200 {"id":6,"joueurs":[{"id":25,"nickname":"ga76604","score":0},{"id":26,…}]}
GET  /api/v1/games/1   (pas ma partie)      → 404 {"message":"Partie introuvable."}
POST /api/v1/games     (sans jeton)         → 401 {"errors":[{"message":"Unauthorized access"}]}
```

`score: 0` vient du `defaultTo(0)` de la migration `games_users`.

---

## 4. Les paramètres de route

Sans le `.where('id', router.matchers.number())`, observé avec un jeton valide :

```
GET /api/v1/games/abc  → 500  {"message":"select * from \"games\" where \"id\" = $1 and exists (…) - invalid input syntax for type integer: \"abc\"", …}
```

Deux problèmes d'un coup :
- un **500** pour une simple faute de frappe dans l'URL ;
- la **requête SQL** renvoyée au client (en dev, le gestionnaire d'erreurs est en mode debug, [`MIDDLEWARE_ERREURS`](MIDDLEWARE_ERREURS.md), §3).

Avec le matcher, `/games/abc` ne correspond plus à la route : **404** `Cannot GET:/api/v1/games/abc`, sans requête en base. `matchers.number()` convertit aussi `params.id` en nombre.

🔗 **Lien C.** C'est `ft_atoi` sans vérifier que la chaîne est un nombre : la donnée part plus loin et c'est Postgres qui proteste. Mieux vaut refuser à l'entrée.

---

## 5. `serialize` et les transformers

L'équipe ne renvoie pas les modèles bruts. Elle passe par deux couches.

**Le transformer** choisit les champs :

```ts
import type User from '#models/user'
import { BaseTransformer } from '@adonisjs/core/transformers'
import env from '#start/env'

export default class UserTransformer extends BaseTransformer<User> {
  toObject() {
    const data = this.pick(this.resource, [
      'id', 'nickname', 'email', 'avatarUrl', 'createdAt', 'updatedAt', 'initials',
    ])
    const appUrl = env.get('APP_URL') as string
    if (data.avatarUrl) {
      data.avatarUrl = `${appUrl}${data.avatarUrl}`
    } else {
      data.avatarUrl = `${appUrl}/uploads/avatars/default-avatar.png`
    }
    return data
  }
}
```

- `this.pick(this.resource, [...])` : une **liste blanche**. `password`, `twoFactorSecret` et `isTwoFactorEnabled` ne sortent jamais, parce qu'ils ne sont pas dans la liste. Une colonne ajoutée plus tard au modèle reste privée tant qu'on ne l'ajoute pas ici.
- `initials` est un **getter** du modèle (`get initials()`), pas une colonne : `pick` le lit comme une propriété.
- Les champs `undefined` disparaissent. Observé sur `/api/v1/card`, dont la carte est construite en mémoire sans `id` ni dates : la réponse ne contient que `name`, `value` et `imageUrl`.

**`serialize`**, ajouté au contexte par `providers/api_provider.ts`, enveloppe le résultat :

```ts
return serialize({ user: UserTransformer.transform(user), token: token.value!.release() })
```

Observé (signup) :

```json
{"data":{"token":"oat_…","user":{"id":1,"nickname":"alice","email":"a@x.io","avatarUrl":"http://localhost:3399/uploads/avatars/default-avatar.png","createdAt":"…","updatedAt":"…","initials":"AL"}}}
```

Le front lit donc `res.data.user`, pas `res.user`. Les routes qui renvoient avec `response.ok({...})` (login, 2FA) **n'ont pas** cette enveloppe. Les deux formes coexistent dans l'API ([`AUTH`](AUTH.md), §3).

🚨 **Renvoyer un modèle Lucid directement** (`return user`) appelle sa sérialisation par défaut : **toutes** les colonnes, sauf celles marquées `serializeAs: null`. Dans `database/schema.ts`, seul `password` l'est. Observé avec `user.serialize()` : les clés sont `id, nickname, email, createdAt, updatedAt, avatarUrl, twoFactorSecret, isTwoFactorEnabled`. Le **secret TOTP** sort en clair, et avec lui n'importe qui peut générer les codes 2FA de l'utilisateur. Passe toujours par le transformer.

---

## 🧪 À essayer

1. Ajoute `router.get('ping', () => 'pong')` dans le groupe `/api/v1`, puis appelle-la avec `curl -i`. Quel `content-type` ?
2. Écris `[controllers.Profile, 'shwo']` dans `routes.ts` et lance `npx tsc --noEmit`.
3. Retire le `return` devant `response.badRequest(...)` dans `store`, et joue contre toi-même. Combien de parties sont créées ?
4. Retire le `.whereHas(...)` de `show`, et lis la partie d'un autre joueur.
5. Ajoute `'twoFactorSecret'` à la liste de `pick` (sans le garder !) : que voit le front après `2fa/generate` ?
