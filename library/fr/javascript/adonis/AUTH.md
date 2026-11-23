# 4. L'authentification : jetons d'accès et 2FA

> **TL;DR.** Le guard par défaut s'appelle `api`. Il lit `Authorization: Bearer oat_…` et retrouve le jeton dans la table `auth_access_tokens`, où seul son **hash** est stocké.
>
> Un jeton porte des **abilities** :
> - `["*"]` : accès complet ;
> - `["2fa:verify"]` : seulement l'étape 2FA, valable 5 minutes.
>
> Deux middlewares se partagent le travail : `auth()` vérifie que le jeton **existe et n'est pas expiré**, `requireFullAccess()` vérifie qu'il vaut `*`.
>
> Le flux 2FA de l'équipe fonctionne de bout en bout (observé). Trois points restent à corriger :
> - les **noms de clé** du jeton diffèrent selon la route : `data.token`, `temp`, `tempToken`, `token` ;
> - régénérer le secret **après** activation verrouille l'utilisateur dehors ;
> - les jetons pleins **n'expirent jamais**.

Liens : [`INDEX`](INDEX.md) · [`ROUTES_CONTROLEURS`](ROUTES_CONTROLEURS.md) · [`MIDDLEWARE_ERREURS`](MIDDLEWARE_ERREURS.md) · [`TUYAU`](TUYAU.md) · [`SOCKET_IO`](SOCKET_IO.md) (§3) · [`../react/API`](../react/API.md) · [doc AdonisJS, access tokens](https://docs.adonisjs.com/guides/authentication/access-tokens-guard)

🧪 Labo : aucun. Le flux complet (inscription, activation 2FA, login, vérification) a été joué par un script Node sur une copie du backend. Les codes TOTP ont été calculés avec `otplib`, comme le ferait une application d'authentification. La correction du §6 n'existe que dans cette copie.

---

## 1. La configuration

`config/auth.ts` de l'équipe, sans les commentaires :

```ts
const authConfig = defineConfig({
  default: 'api',
  guards: {
    api: tokensGuard({
      provider: tokensUserProvider({
        tokens: 'accessTokens',
        model: () => import('#models/user'),
      }),
    }),
    web: sessionGuard({
      useRememberMeTokens: false,
      provider: sessionUserProvider({ model: () => import('#models/user') }),
    }),
  },
})
```

- **Guard** : une façon de reconnaître l'utilisateur. `api` lit l'en-tête `Authorization`, `web` lit un cookie de session.
- `default: 'api'` : `middleware.auth()` sans argument utilise `api`. Le guard `web` est déclaré mais **aucune route ne s'en sert**.
- `tokens: 'accessTokens'` : le nom de la propriété **statique** du modèle, `User.accessTokens = DbAccessTokensProvider.forModel(User)` ([`LUCID`](LUCID.md), §2).

🔗 **Lien C.** Le jeton est une **clé** : le serveur n'en garde que le hash, comme `/etc/shadow` ne garde que le hash des mots de passe. Quelqu'un qui lit la base ne peut pas se connecter avec.

---

## 2. Inscription et connexion sans 2FA

```ts
const user = await User.verifyCredentials(email, password)
const token = await User.accessTokens.create(user)
return response.ok({ message: 'Login OK.', requires2fa: false, temp: token.value!.release() })
```

- `User.verifyCredentials` vient du mixin `withAuthFinder` : il cherche l'e-mail et compare le hash du mot de passe. En cas d'échec, il lève `E_INVALID_CREDENTIALS`, que le gestionnaire d'erreurs transforme en **400** `{"errors":[{"message":"Invalid user credentials"}]}` (observé). Le message est le **même** pour un e-mail inconnu et pour un mauvais mot de passe : on ne révèle pas quels e-mails existent.
- `User.accessTokens.create(user)` : abilities `["*"]` par défaut, et **pas d'expiration**. En base, on observe `expires_at` à `null` pour tous les jetons pleins.
- `token.value!.release()` : la seule fois où le jeton existe **en clair**. `release()` est explicite pour éviter qu'un `console.log(token)` l'imprime par accident.

En base, après quelques connexions (observé) :

```
 id | tokenable_id |   abilities    | sans_expiration
 37 |           39 | ["2fa:verify"] | f
 36 |           39 | ["*"]          | t
 35 |           39 | ["*"]          | t
```

Chaque login crée une **nouvelle** ligne, et aucune n'expire. Seul le logout (`POST /api/v1/account/logout`) en supprime une, celle de la requête.

---

## 3. Quatre noms pour le même jeton

Observé, en suivant le code de l'équipe :

| Route | Où est le jeton dans la réponse |
|---|---|
| `POST /auth/signup` | `data.token` (passe par `serialize`) |
| `POST /auth/login`, sans 2FA | `temp` |
| `POST /auth/login`, avec 2FA | `tempToken` |
| `POST /auth/login/verify` | `token` |

Le frontend de l'équipe n'appelle pas encore le login : `localStorage.getItem('auth_token')` est lu dans `lib/api.ts`, mais rien ne l'écrit. La première personne qui branchera l'écran de connexion lira `res.token` et obtiendra `undefined` pour un login sans 2FA. Avec Tuyau, TypeScript le signale avant l'exécution ([`TUYAU`](TUYAU.md), §3).

Le nom `temp` est aussi trompeur : ce jeton-là est **plein** et n'expire pas. Le jeton vraiment temporaire est `tempToken`.

---

## 4. Le flux 2FA, observé de bout en bout

```
POST /account/2fa/generate     (jeton plein)          → 200 {"message":"2FA secret generated. Verify to enable.","data":{"secret":"…","qrCodeUrl":"data:image/png;base64,…"}}
POST /account/2fa/enable  {"code":"000000"}           → 401 {"message":"Invalid 2FA code"}
POST /account/2fa/enable  {"code":<code TOTP>}        → 200 {"message":"2FA is now enabled"}
POST /auth/login                                      → 200 {"message":"2FA code required. …","requires2fa":true,"tempToken":"oat_…"}
GET  /account/profile     (tempToken)                 → 401 {"message":"Unauthorized access. (meow)"}
POST /auth/login/verify   {"code":"000000"} (tempToken) → 401 {"message":"Invalid 2FA code. (lol)"}
POST /auth/login/verify   {"code":<code TOTP>} (tempToken) → 200 {"message":"Login OK.","token":"oat_…"}
GET  /account/profile     (token)                     → 200 {"data":{…}}
GET  /account/profile     (tempToken, après verify)   → 401
```

Comment c'est construit :
- `generate` stocke un secret dans `users.two_factor_secret` et renvoie un QR code (`otpauth://…` encodé en image) à scanner avec une application TOTP.
- `enable` exige un code valide **avant** d'activer. Ainsi, on ne peut pas activer la 2FA avec un secret qu'on n'a pas réellement enregistré.
- `login` d'un compte 2FA crée un jeton aux abilities `['2fa:verify']` qui expire en 5 minutes.
- `login/verify` n'a que `middleware.auth()` : le jeton temporaire y **passe**. Partout ailleurs, `requireFullAccess` le refuse :

```ts
export default class RequireFullAccessMiddleware {
  async handle({ auth, response }: HttpContext, next: NextFn) {
    const token = auth.user?.currentAccessToken
    if (!token?.allows('*')) {
      return response.unauthorized({ message: 'Unauthorized access. (meow)' })
    }
    return next()
  }
}
```

- `verify2fa` **supprime** le jeton temporaire puis crée un jeton plein. Le temporaire ne sert donc qu'une fois (observé : 401 ensuite).

🔗 **Lien C.** TOTP, c'est `HMAC(secret, floor(now / 30))` tronqué à 6 chiffres. Le serveur et le téléphone font le même calcul de leur côté, sans jamais communiquer. Ils n'ont besoin que du secret partagé et d'horloges à peu près à l'heure.

---

## 5. Les pièges de ce flux

🚨 **`generate` après activation verrouille l'utilisateur.** Observé : sur un compte où la 2FA est **déjà active**, un nouvel appel à `generate` répond 200 et remplace le secret. Le code de l'ancienne application devient faux : `verify` répond 401 `Invalid 2FA code. (lol)`. Et `enable` ne peut pas servir à rattraper le coup : il répond `2FA already enabled` dès que la 2FA est active (lu dans le code, ligne 36 du contrôleur). L'utilisateur qui n'a pas rescanné le nouveau QR ne peut plus se connecter. Correction : refuser `generate` quand `user.isTwoFactorEnabled` est vrai, et prévoir un `disable` qui exige un code.

🚨 **Aucune limite de tentatives** (lu dans le code, non testé en charge). Le paquet `@adonisjs/limiter` n'est pas installé et aucune route ne limite les essais. Avec le mot de passe en main, un attaquant dispose de 5 minutes par jeton temporaire, et peut en redemander autant qu'il veut. Le login lui-même n'est pas limité non plus.

🚨 **Le secret TOTP sort si on renvoie le modèle brut** ([`ROUTES_CONTROLEURS`](ROUTES_CONTROLEURS.md), §5). Le transformer le protège aujourd'hui.

- `verify2fa` accepte aussi un jeton **plein** (observé : 200). Il le supprime et en crée un autre. Ce n'est pas dangereux, mais la route ne vérifie pas `allows('2fa:verify')`.
- `auth.user!` dans `TwoFactorAuthsController` : le `!` est sûr seulement parce que la route a `middleware.auth()`. Déplacer la route hors du groupe la casserait sans erreur de compilation. `auth.getUserOrFail()` est plus sûr.

---

## 6. Une réponse de login cohérente

Correction testée dans la copie : **une** clé `token`, et `requires2fa` dit ce qu'est ce jeton.

```ts
import User from '#models/user'
import { loginValidator } from '#validators/user'
import type { HttpContext } from '@adonisjs/core/http'

export default class AccessTokensController {
  async store({ request, response }: HttpContext) {
    const { email, password } = await request.validateUsing(loginValidator)
    const user = await User.verifyCredentials(email, password)

    if (user.isTwoFactorEnabled) {
      const temp = await User.accessTokens.create(user, ['2fa:verify'], { expiresIn: '5 mins' })
      return response.ok({ requires2fa: true as const, token: temp.value!.release() })
    }

    const token = await User.accessTokens.create(user)
    return response.ok({ requires2fa: false as const, token: token.value!.release() })
  }
}
```

- `true as const` / `false as const` : le type de la réponse devient une **union discriminée**. Côté front, `if (res.requires2fa)` suffit à savoir dans quelle branche on est ([`TUYAU`](TUYAU.md), §3).
- `loginValidator` est enfin utilisé : un corps vide donne un 422 explicite ([`VINE`](VINE.md), §2).

Pour les jetons pleins, une expiration se règle en un seul endroit, dans le modèle :

```ts
static accessTokens = DbAccessTokensProvider.forModel(User, { expiresIn: '30 days' })
```

(Option lue dans les types d'`@adonisjs/auth`. Elle typecheck dans la copie, mais l'expiration n'a **pas** été observée en base.)

---

## 7. Côté front : où ranger le jeton

L'équipe le lit dans `localStorage` (`auth_token`) et l'ajoute en `Bearer` à chaque requête Tuyau ([`TUYAU`](TUYAU.md), §1).

- **Avantage** : simple, et le même jeton sert pour Socket.IO (`auth: { token }`, [`SOCKET_IO`](SOCKET_IO.md), §3).
- **Risque** : un script injecté dans la page (XSS) lit `localStorage`. React échappe le texte affiché par défaut, donc le risque vient surtout de `dangerouslySetInnerHTML` et des dépendances.
- Au **logout**, il faut appeler la route (pour supprimer le jeton en base) **et** vider `localStorage`. L'un sans l'autre laisse un jeton valide quelque part.

---

## 🧪 À essayer

1. Fais un login, copie le jeton, fais un logout, puis réutilise le jeton sur `/account/profile`.
2. Active la 2FA sur un compte, appelle `generate` une seconde fois, déconnecte-toi, et essaie de te reconnecter avec ton application TOTP.
3. Ajoute un garde `if (user.isTwoFactorEnabled) return response.badRequest(...)` dans `generate`, et rejoue l'essai 2.
4. Ouvre `auth_access_tokens` dans pgAdmin (`localhost:8880`) et compte les lignes de ton utilisateur après dix logins.
