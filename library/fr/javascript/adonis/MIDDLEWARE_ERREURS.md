# 5. Middlewares et gestion des erreurs

> **TL;DR.** `start/kernel.ts` déclare trois piles de middlewares :
> - **serveur** : chaque requête, même vers une route qui n'existe pas ;
> - **routeur** : seulement si une route correspond ;
> - **nommés** : seulement sur les routes qui les demandent (`middleware.auth()`).
>
> Quand une erreur remonte, `app/exceptions/handler.ts` la transforme en réponse JSON :
> - **en dev**, il ajoute la stack et le chemin des fichiers (`debug`) ;
> - **en prod**, il les retire, mais le **message** d'une erreur imprévue part encore au client.
>
> Le back de l'équipe produit **au moins quatre formes** d'erreur différentes. Le front doit les connaître toutes, ou l'équipe doit les unifier.

Liens : [`INDEX`](INDEX.md) · [`ROUTES_CONTROLEURS`](ROUTES_CONTROLEURS.md) · [`VINE`](VINE.md) · [`AUTH`](AUTH.md) · [`DEPLOIEMENT`](DEPLOIEMENT.md) · [`../react/API`](../react/API.md) · [doc AdonisJS, exception handling](https://docs.adonisjs.com/guides/basics/exception-handling)

🧪 Labo : aucun. Les réponses viennent de `curl` sur une copie du backend, lancée en dev (`node ace serve`) et en prod (`node build/bin/server.js` avec `NODE_ENV=production`). Le gestionnaire du §5 n'existe que dans cette copie.

---

## 1. Le kernel de l'équipe

```ts
import router from '@adonisjs/core/services/router'
import server from '@adonisjs/core/services/server'

server.errorHandler(() => import('#exceptions/handler'))

server.use([
  () => import('#middleware/force_json_response_middleware'),
  () => import('#middleware/container_bindings_middleware'),
  () => import('@adonisjs/cors/cors_middleware'),
  () => import('@adonisjs/static/static_middleware'),
])

router.use([
  () => import('@adonisjs/core/bodyparser_middleware'),
  () => import('@adonisjs/session/session_middleware'),
  () => import('@adonisjs/shield/shield_middleware'),
  () => import('@adonisjs/auth/initialize_auth_middleware'),
  () => import('#middleware/silent_auth_middleware'),
])

export const middleware = router.named({
  requireFullAccess: () => import('#middleware/require_full_access_middleware'),
  auth: () => import('#middleware/auth_middleware'),
})
```

Une requête traverse les piles **dans l'ordre**, comme des poupées russes : chaque middleware fait son travail, appelle `next()`, puis peut agir sur la réponse au retour.

- `server.use` : tourne **avant** la recherche de route. CORS doit y être, puisqu'une requête `OPTIONS` de pré-vol ne correspond à aucune route. `static` y est aussi : il sert `public/` (les avatars, [`DEPLOIEMENT`](DEPLOIEMENT.md), §4) sans passer par le routeur.
- `router.use` : tourne une fois la route trouvée. `bodyparser` lit le corps. `initialize_auth` prépare `ctx.auth`.
- `silent_auth` fait `await ctx.auth.check()` sur **toutes** les routes. Si le jeton est valide, `auth.user` est rempli, sinon rien ne se passe. Coût : une requête SQL par appel, même sur une route publique.
- `router.named` : n'agit que là où on l'écrit, `.use(middleware.auth())` ([`ROUTES_CONTROLEURS`](ROUTES_CONTROLEURS.md), §1).

🔗 **Lien C.** C'est une chaîne de pointeurs de fonction où chaque maillon reçoit le suivant (`next`). Ne pas appeler `next()`, c'est court-circuiter le reste : `requireFullAccess` fait exactement ça quand il répond 401 ([`AUTH`](AUTH.md), §4).

---

## 2. Deux middlewares maison

```ts
import type { HttpContext } from '@adonisjs/core/http'
import type { NextFn } from '@adonisjs/core/types/http'

export default class ForceJsonResponseMiddleware {
  handle(ctx: HttpContext, next: NextFn) {
    ctx.request.request.headers.accept = 'application/json'
    return next()
  }
}
```

- Il **réécrit** l'en-tête `Accept` de la requête. Le gestionnaire d'erreurs choisit son format selon `Accept`. Sans ce middleware, un navigateur (`Accept: text/html`) recevrait une page d'erreur HTML. Observé : avec `-H 'accept: text/html'`, la réponse 401 reste en `application/json`.
- `ctx.request.request` : le premier `request` est celui d'Adonis, le second est l'objet `IncomingMessage` de Node.

Le second, `silent_auth`, tient en deux lignes : `await ctx.auth.check()` puis `return next()`.

Côté sécurité, `config/shield.ts` laisse le **CSRF désactivé**. C'est correct pour une API à jeton `Bearer` : un site tiers ne peut pas ajouter cet en-tête à la place du navigateur. Les en-têtes `x-frame-options: DENY` et `x-content-type-options: nosniff` sont actifs (observé).

🚨 Le middleware `session` pose un cookie `adonis-session` sur **chaque** réponse (observé), alors qu'aucune route n'utilise la session. Ce n'est pas dangereux, mais si un jour quelqu'un branche le guard `web`, le CSRF désactivé **deviendra** un problème.

---

## 3. Les formes d'erreur, observées

| Cause | Code | Corps |
|---|---|---|
| Validation Vine | 422 | `{"errors":[{"message","rule","field","meta"?}]}` |
| `auth()` sans jeton valide | 401 | `{"errors":[{"message":"Unauthorized access"}]}` |
| `verifyCredentials` échoue | 400 | `{"errors":[{"message":"Invalid user credentials"}]}` |
| `requireFullAccess` (code de l'équipe) | 401 | `{"message":"Unauthorized access. (meow)"}` |
| Erreur 2FA (code de l'équipe) | 401 / 400 | `{"message":"Invalid 2FA code. (lol)"}` |
| `response.notFound(...)` dans un contrôleur | 404 | `{"message":"Partie introuvable."}` |
| Route inexistante | 404 | `{"message":"Cannot GET:/api/v1/nope"}` + stack en dev |
| JSON mal formé | 400 | `{"message":"Unexpected end of JSON input"}` + stack **et corps brut** en dev |
| Exception imprévue | 500 | `{"message":"…"}` + stack en dev |

On voit **deux familles** :
- les erreurs d'Adonis renvoient `errors: [...]`, un tableau ;
- les réponses écrites à la main renvoient `message`, une chaîne.

Le front doit donc lire `body.errors?.[0]?.message ?? body.message` pour afficher quoi que ce soit ([`../react/API`](../react/API.md)).

🔗 **Lien C.** Même problème qu'une libft où certaines fonctions renvoient `-1` et d'autres `NULL` en cas d'échec : chaque appelant doit se souvenir de la convention de chaque fonction. Une seule convention supprime une classe entière de bugs.

Correction simple, côté contrôleurs : toujours `response.unauthorized({ errors: [{ message: '…' }] })`, et pas `{ message }`.

---

## 4. Dev et prod

Le gestionnaire de l'équipe :

```ts
import app from '@adonisjs/core/services/app'
import { type HttpContext, ExceptionHandler } from '@adonisjs/core/http'

export default class HttpExceptionHandler extends ExceptionHandler {
  protected debug = !app.inProduction

  async handle(error: unknown, ctx: HttpContext) {
    return super.handle(error, ctx)
  }

  async report(error: unknown, ctx: HttpContext) {
    return super.report(error, ctx)
  }
}
```

- `handle` **répond** au client. `report` **journalise** (log `"level":50` avec la stack et un `request_id`). Adonis appelle les deux pour chaque erreur.
- `debug = !app.inProduction` : en dev, le corps contient `name`, `frames`, `stack`. Le JSON est parfois si long qu'il vaut mieux lire le terminal de `node ace serve`.

Ce qui sort, observé :

```
dev  GET /api/v1/nope            → {"message":"Cannot GET:/api/v1/nope","name":"Exception","frames":[{"functionName":…,"fileName":"/…/node_modules/@adonisjs/http-server/…
prod GET /api/v1/nope            → {"message":"Cannot GET:/api/v1/nope"}
dev  POST /auth/login  '{"email":'  → {"message":"Unexpected end of JSON input",…,"raw":{"status":400,"body":"{\"email\":…
prod POST /auth/login  '{"email":'  → {"message":"Unexpected end of JSON input"}
prod POST /auth/login  (otplib absent) → 500 {"message":"Cannot find package 'otplib' imported from /…/build/app/controllers/access_tokens_controller.js"}
```

🚨 **En dev, l'erreur de JSON mal formé renvoie le corps brut de la requête.** Si ce corps contenait un mot de passe, il revient dans la réponse, puis dans les outils de développement du navigateur. Ce n'est pas grave en local, mais c'est une raison de plus pour ne **jamais** déployer avec `NODE_ENV=development`.

🚨 **En prod, une erreur imprévue garde son message**, et ce message peut contenir un chemin de fichier, un nom de paquet ou une requête SQL ([`ROUTES_CONTROLEURS`](ROUTES_CONTROLEURS.md), §4). Le dernier exemple ci-dessus vient d'un build de prod du code de l'équipe ([`DEPLOIEMENT`](DEPLOIEMENT.md), §2).

---

## 5. Masquer les 500 en prod

Testé dans la copie : on a forcé `debug = false` en dev, déclenché une erreur, puis remis la ligne d'origine. Le fichier ci-dessous est la version finale et typecheck.

```ts
import app from '@adonisjs/core/services/app'
import { type HttpContext, ExceptionHandler } from '@adonisjs/core/http'

export default class HttpExceptionHandler extends ExceptionHandler {
  protected debug = !app.inProduction

  async handle(error: unknown, ctx: HttpContext) {
    const status = (error as { status?: number }).status ?? 500
    if (status >= 500 && !this.debug) {
      return ctx.response.status(500).send({
        errors: [{ message: 'Erreur interne.', requestId: ctx.request.id() }],
      })
    }
    return super.handle(error, ctx)
  }

  async report(error: unknown, ctx: HttpContext) {
    return super.report(error, ctx)
  }
}
```

- `(error as { status?: number }).status ?? 500` : les erreurs d'Adonis portent leur code HTTP. Une erreur JavaScript ordinaire n'en a pas, donc c'est un 500.
- Seuls les **500** sont masqués. Les 4xx gardent leur message, qui est fait pour le client (« Invalid user credentials »).
- `ctx.request.id()` : l'identifiant de la requête. On le donne au client et on le retrouve dans le log.

Observé, avec une route qui lève `new Error('/secret/chemin/fichier.js introuvable')` :

```
réponse → 500 {"errors":[{"message":"Erreur interne.","requestId":"2e518bf3-9d16-46c2-871e-6ec620069edc"}]}
log     → {"level":50,…,"request_id":"2e518bf3-9d16-46c2-871e-6ec620069edc","err":{"message":"/secret/chemin/fichier.js introuvable","stack":"Error: …
```

Les 404 et les 422 sont restés identiques. Un utilisateur qui signale « erreur 2e518bf3 » permet de retrouver la stack exacte dans `docker compose logs backend`.

---

## 🧪 À essayer

1. Envoie `curl -X OPTIONS -H 'Origin: http://localhost:5173' -H 'Access-Control-Request-Method: POST' -i localhost:3333/api/v1/auth/login` et lis les en-têtes `access-control-*`. Puis envoie la même requête vers une route qui n'existe pas.
2. Commente `force_json_response_middleware` dans le kernel et rappelle `/api/v1/nope` depuis le navigateur.
3. Remplace `{ message: 'Unauthorized access. (meow)' }` par la forme `errors: [...]` dans `require_full_access_middleware`, puis écris le `lireErreur(body)` du front qui ne lit plus qu'une seule forme.
4. Ajoute une route qui fait `throw new Error('boum')`, lance un build de prod, et compare la réponse avant et après le gestionnaire du §5.
