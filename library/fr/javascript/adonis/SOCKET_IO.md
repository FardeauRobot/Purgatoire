# 7. Socket.IO dans Adonis : le chat en temps réel

> **TL;DR.** Adonis n'a pas de module WebSocket intégré. L'équipe branche Socket.IO sur le **serveur HTTP de Node** qu'Adonis utilise déjà (`app/services/realtime.ts`), et écrit les événements dans `start/ws.ts`, l'équivalent de `routes.ts` pour le temps réel. Le **contrat** (noms d'événements et arguments) est un fichier TypeScript partagé entre le front et le back, dans `apps/types`.
>
> Dans le code de l'équipe, la connexion **n'est pas authentifiée** : le nom vient du client. Trois corrections ont été testées dans une copie :
> - un middleware `io.use` vérifie le jeton d'accès ;
> - le nom est imposé par le serveur ;
> - le logout coupe les sockets du jeton supprimé.
>
> Pour les payloads, `vine.tryValidate` remplace les `typeof` écrits à la main.

Liens : [`INDEX`](INDEX.md) · [`AUTH`](AUTH.md) · [`VINE`](VINE.md) (§5) · [`TESTS_JAPA`](TESTS_JAPA.md) · [`DEPLOIEMENT`](DEPLOIEMENT.md) (§3) · [`../react/TEMPS_REEL`](../react/TEMPS_REEL.md) · [doc Socket.IO, middlewares](https://socket.io/docs/v4/middlewares/)

🧪 Labo : aucun. Les observations viennent de scripts `socket.io-client` 4.8.4 lancés sous Node contre une copie du backend. Le côté React (connexion, abonnements, reconnexion) est dans [`../react/TEMPS_REEL`](../react/TEMPS_REEL.md).

---

## 1. Brancher Socket.IO sur Adonis

`app/services/realtime.ts`, le code de l'équipe :

```ts
import server from '@adonisjs/core/services/server'
import { Server } from 'socket.io'
import type {
  ClientToServerEvents,
  InterServerEvents,
  ServerToClientEvents,
  SocketData,
} from '@api-starter-kit/types/ws_type'
import app from '@adonisjs/core/services/app'

export type RealtimeServer = Server<
  ClientToServerEvents,
  ServerToClientEvents,
  InterServerEvents,
  SocketData
>

class Realtime {
  io: RealtimeServer | undefined
  private booted = false

  boot() {
    if (this.booted) {
      return
    }
    this.booted = true
    this.io = new Server(server.getNodeServer(), {
      cors: app.inProduction ? undefined : { origin: ['http://localhost:5173'] },
    })
  }
}

export default new Realtime()
```

- `server.getNodeServer()` : le `http.Server` de Node qu'Adonis écoute déjà. Socket.IO s'y accroche et intercepte les URL `/socket.io/…`. Un seul port pour l'API et le temps réel.
- `export default new Realtime()` : un **singleton**. Tout fichier qui importe `#services/realtime` reçoit la même instance. C'est ce qui permet au contrôleur de logout d'atteindre `realtime.io` (§4).
- `booted` : `boot()` peut être appelé deux fois sans créer deux serveurs.
- Les quatre paramètres de type de `Server<…>` donnent le typage de `socket.on`, `socket.emit`, `socket.data`.
- `cors` en dev : seul `localhost:5173` est accepté. Si ton vite tourne sur un autre port (5174 quand 5173 est pris), la connexion échoue avec une erreur CORS dans la console du navigateur. En prod, `undefined` suffit, puisque Caddy sert tout sur la même origine ([`DEPLOIEMENT`](DEPLOIEMENT.md), §3).

`start/ws.ts` est déclaré dans `adonisrc.ts` comme fichier préchargé, **uniquement pour l'environnement `web`**. Il ne tourne donc ni dans les commandes `ace` ni sous Japa ([`TESTS_JAPA`](TESTS_JAPA.md), §6). Il démarre tout dans `app.ready(() => { realtime.boot(); … })`, une fois le serveur HTTP prêt.

🔗 **Lien C.** Une WebSocket, c'est une connexion TCP **gardée ouverte** après une requête HTTP initiale (`Upgrade`). Ensuite, les deux côtés écrivent quand ils veulent, comme un `socketpair` entre deux processus. Socket.IO ajoute par-dessus des événements nommés, des accusés de réception et la reconnexion automatique.

---

## 2. Le contrat partagé

`apps/types/ws_contract.ts`, exporté en `@api-starter-kit/types/ws_type` :

```ts
export interface SuccessJoinState {
  ok: true;
  users: string[];
}
export interface ErrorState {
  ok: false;
  error: string;
}
export type JoinState = SuccessJoinState | ErrorState
export type SendState = { ok: true } | ErrorState

export interface ClientToServerEvents {
  'room:join': ( room: string, name: string, ack: (res: JoinState)=> void) => void
  'message:send': (text: string, ack: (res: SendState) => void) => void
}

export interface ServerToClientEvents {
  'message:new': (id: number, name: string, text: string, at: number) => void
  'room:user-joined': (name: string) => void
  'room:user-left': (name: string) => void
}

export type InterServerEvents = Record<string, never>

export interface SocketData {
  name?: string
  room?: string
}
```

- `ClientToServerEvents` : ce que le client **émet** et que le serveur **écoute**. Le dernier argument `ack` est un **callback** : le serveur l'appelle pour répondre, et le client le reçoit comme résultat de `emitWithAck`.
- `JoinState = SuccessJoinState | ErrorState` : une union discriminée par `ok`. Après `if (res.ok)`, TypeScript sait que `res.users` existe.
- `SocketData` : ce que le serveur range **sur** chaque socket (`socket.data.name`). Le client ne le voit jamais.
- Le front importe le même fichier ([`../react/TEMPS_REEL`](../react/TEMPS_REEL.md)). Renommer un événement casse la compilation **des deux côtés**.

🚨 **Le typage n'est qu'une promesse du client.** `text: string` dans le contrat n'empêche personne d'envoyer `42` ou `null` avec un client écrit à la main. C'est pourquoi `ws.ts` revérifie tout (`typeof text !== 'string'`). Le §5 remplace ces vérifications par Vine.

---

## 3. Authentifier la connexion

**Le code de l'équipe** : aucun middleware. `room:join(room, name, ack)` prend le `name` tel que le client l'envoie. Observé : `room:join('salon2', 'imposteur')` → `{"ok":true,"users":["imposteur"]}`. N'importe qui peut écrire dans n'importe quel salon sous n'importe quel nom, sans compte.

**Correction testée** dans la copie, ajoutée dans `ws.ts` avant `io.on('connection', …)` :

```ts
io.use(async (socket, next) => {
  const brut = socket.handshake.auth.token
  if (typeof brut !== 'string') return next(new Error('jeton manquant'))
  const jeton = await User.accessTokens.verify(new Secret(brut))
  if (!jeton || !jeton.allows('*')) return next(new Error('jeton refusé'))
  const user = await User.find(jeton.tokenableId)
  if (!user) return next(new Error('jeton refusé'))
  socket.data.name = user.nickname ?? undefined
  await socket.join(`jeton:${jeton.identifier}`)
  next()
})
```

(Imports ajoutés : `User` depuis `#models/user`, `Secret` depuis `@adonisjs/core/helpers`.)

- `io.use` : un middleware Socket.IO. Il tourne **une fois**, à la connexion, avant l'événement `connection`. `next(new Error(…))` refuse la connexion. Le client reçoit `connect_error` avec ce message.
- `socket.handshake.auth.token` : ce que le client a passé dans l'option `auth` de `io(...)`. Pas un en-tête HTTP : un navigateur ne permet pas d'ajouter `Authorization` à une WebSocket.
- `new Secret(brut)` : enveloppe la chaîne pour qu'elle ne s'imprime pas par accident dans un log.
- `User.accessTokens.verify(...)` : retrouve le jeton par son hash et vérifie l'expiration. Il **ne vérifie pas** les abilities : d'où le `jeton.allows('*')` explicite. Observé : un jeton temporaire `2fa:verify` est refusé (`jeton refusé`). Sans ce test, il passerait.
- `socket.data.name = user.nickname` : le nom vient **de la base**.
- `socket.join(\`jeton:${jeton.identifier}\`)` : chaque socket rejoint un salon privé à son jeton (§4).

Puis, dans `room:join`, une seule ligne change :

```ts
const cName = (socket.data.name ?? '').trim()
```

Le `name` envoyé par le client est **ignoré**. Observé : `room:join('salon2', 'imposteur')` → `{"ok":true,"users":["wq57526"]}`, le pseudo réel du compte.

Côté client, il faut passer le jeton : `io(URL, { autoConnect: false, auth: (cb) => cb({ token: localStorage.getItem('auth_token') }) })` ([`../react/TEMPS_REEL`](../react/TEMPS_REEL.md), §1). Le `socket.ts` de l'équipe ne le fait pas encore.

🔗 **Lien C.** C'est la différence entre `write(fd, buf)` où le noyau sait **qui** écrit (le processus, son uid) et un protocole où chaque message dit « je suis root ». Le serveur doit tirer l'identité de la connexion, jamais du contenu.

---

## 4. Couper les sockets au logout

Sans rien de plus, une socket authentifiée **reste connectée après le logout** (observé) : le jeton est supprimé en base, mais la socket a été vérifiée une fois pour toutes, à la connexion.

Dans la copie, `AccessTokensController.destroy` (par rapport au code de l'équipe : l'import de `realtime`, la variable `id` et la ligne `realtime.io?…`) :

```ts
import User from '#models/user'
import realtime from '#services/realtime'
import type { HttpContext } from '@adonisjs/core/http'

export default class AccessTokensController {
  async destroy({ auth }: HttpContext) {
    const user = auth.getUserOrFail()
    if (user.currentAccessToken) {
      const id = user.currentAccessToken.identifier
      await User.accessTokens.delete(user, id)
      realtime.io?.in(`jeton:${id}`).disconnectSockets(true)
    }

    return {
      message: 'Logged out successfully',
    }
  }
}
```

- `io.in('jeton:…')` vise uniquement les sockets ouvertes **avec ce jeton**, grâce au salon rejoint dans le middleware (§3). Les autres appareils du même utilisateur, qui ont leur propre jeton, restent connectés.
- `disconnectSockets(true)` : `true` ferme aussi la connexion sous-jacente.
- `realtime.io?.` : sous Japa, `ws.ts` n'est pas chargé et `io` vaut `undefined`. Le `?.` évite que le logout plante en test.

Observé : `après logout : disconnect « io server disconnect »`. Côté client, cette raison précise signifie que **Socket.IO ne se reconnecte pas tout seul** ([`../react/TEMPS_REEL`](../react/TEMPS_REEL.md)).

🚨 Le même problème existe pour un jeton qui **expire** pendant que la socket est ouverte, si on ajoute une expiration ([`AUTH`](AUTH.md), §6). Le middleware ne revérifie rien après la connexion.

---

## 5. Valider les payloads avec Vine

`validate()` et `tryValidate()` fonctionnent hors HTTP ([`VINE`](VINE.md), §5). `app/validators/ws.ts`, dans la copie :

```ts
import vine, { SimpleMessagesProvider } from '@vinejs/vine'

export const messageValidator = vine.create({
  text: vine.string().trim().minLength(1).maxLength(500),
})

messageValidator.messagesProvider = new SimpleMessagesProvider({
  'required': 'Message vide.',
  'string': 'Le message doit être du texte.',
  'minLength': 'Message vide.',
  'maxLength': 'Message trop long (500 caractères max).',
})
```

Et dans `ws.ts`, `message:send` devient :

```ts
socket.on('message:send', async (text, ack) => {
  if (typeof ack !== 'function') return

  const { room, name } = socket.data
  if (!room || !name) {
    return ack({ ok: false, error: 'Join a room first' })
  }

  const [erreur, valide] = await messageValidator.tryValidate({ text })
  if (erreur) {
    return ack({ ok: false, error: erreur.messages[0].message })
  }
  const cText = valide.text
  io.to(room).emit('message:new', nextId++, name, cText, Date.now())
  ack({ ok: true })
})
```

- `tryValidate` ne **lève** pas : il renvoie `[erreur, null]` ou `[null, données]`, comme le `.safe()` de Tuyau ([`TUYAU`](TUYAU.md), §4). C'est plus pratique ici qu'un `try/catch`, puisqu'il faut répondre par `ack`.
- `{ text }` : Vine valide un **objet**, d'où l'emballage de l'argument.
- `typeof ack !== 'function'` reste à la main : sans `ack`, on ne peut même pas répondre.
- Le handler devient `async`. Socket.IO l'accepte sans rien changer.

Observé :

```
message:send avant join   → {"ok":false,"error":"Join a room first"}
message:send "   "        → {"ok":false,"error":"Message vide."}
message:send 42           → {"ok":false,"error":"Le message doit être du texte."}
message:send null         → {"ok":false,"error":"Message vide."}
message:send "x" × 501    → {"ok":false,"error":"Message trop long (500 caractères max)."}
message:send "  bonjour  " → {"ok":true}     message:new reçu : "bonjour"
```

- `"   "` est d'abord réduit à `""` par `trim`, puis refusé par `minLength(1)`.
- `42` est refusé : `vine.string()` ne convertit **pas** les nombres en chaînes. `vine.number()`, à l'inverse, convertit `"26"` en `26` ([`VINE`](VINE.md), §3).
- Le texte diffusé est la version **nettoyée** (`valide.text`), sans espaces autour.

---

## 6. Ce qui reste fragile

- `nextId` est une variable en **mémoire** : elle repart à 1 à chaque redémarrage, et les messages ne sont pas enregistrés. La table `messages` existe ([`LUCID`](LUCID.md), §4), mais `ws.ts` ne s'en sert pas encore.
- `io.in(room).fetchSockets()` lit l'état **de ce processus**. Avec un seul conteneur backend, c'est exact. Avec deux, il faudrait un *adapter* (Redis, par exemple). Hors sujet pour Transcendence, mais c'est la raison pour laquelle l'état d'un salon ne doit pas vivre ailleurs que dans Socket.IO.
- Pas de limite de débit : un client peut envoyer 1000 messages par seconde. Socket.IO n'en propose pas de base.

---

## 🧪 À essayer

1. Lance deux clients (deux onglets, ou un script Node), connecte-les au même salon, et coupe le backend. Que reçoit chacun, et que se passe-t-il au redémarrage ?
2. Ajoute à `room:join` un validateur Vine pour `room` (`trim`, `minLength(1)`, `maxLength(32)`, `regex(/^[a-z0-9-]+$/)`), puis supprime les `typeof`.
3. Enregistre chaque message avec `Message.create({ roomId, userId, content })` avant l'`emit`, et utilise son `id` à la place de `nextId`.
4. Dans le middleware `io.use`, refuse aussi un jeton dont `isExpired()` est vrai, puis teste avec un jeton créé avec `expiresIn: '1 minute'`.
