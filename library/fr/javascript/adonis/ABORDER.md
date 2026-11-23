# 🪜 Aborder AdonisJS

> **TL;DR.** AdonisJS est un framework backend **« piles incluses »** : là où Express te laisse choisir et brancher dix bibliothèques, Adonis arrive avec le routeur, la validation (Vine), l'ORM (Lucid), l'authentification, les tests (Japa) et une ligne de commande (`node ace`), tous rangés à une place précise. On l'aborde donc comme on aborde un **projet existant** : apprendre **où va chaque chose**, puis suivre le trajet d'une requête de `routes.ts` jusqu'à la base. Les prérequis : TypeScript avec `async`/`await` et les classes, HTTP (que webserv t'a appris), un peu de SQL, et Docker pour Postgres.

Liens : [`INDEX`](INDEX.md) · [`SYNTHESE`](SYNTHESE.md) · [`../ABORDER`](../ABORDER.md) · [`../VERS_TRANSCENDANCE`](../VERS_TRANSCENDANCE.md) §3 · [`../react/ABORDER`](../react/ABORDER.md) · [doc AdonisJS](https://docs.adonisjs.com)

---

## 1. À quel besoin il répond

Un backend de jeu en ligne doit, pour **chaque** requête :

```text
  HTTP entrant ─► quelle route ? ─► qui est-ce ? ─► les données sont-elles valides ? ─► lire/écrire la base ─► JSON sortant
                  (routeur)         (auth)          (validation)                       (ORM)
```

Avec Node « nu » ou Express, chacune de ces flèches est une bibliothèque à choisir, à brancher et à faire tenir avec les autres. Adonis fait ces choix pour toi, avec des **conventions** (un dossier pour les contrôleurs, un pour les validateurs, un pour les modèles) et des outils qui se parlent : le validateur produit un objet **typé**, le modèle connaît ses colonnes, le client du front (Tuyau) est **généré** à partir des routes.

🔗 **Lien C.** C'est la différence entre écrire webserv de zéro (parser la requête, router, gérer les erreurs) et recevoir un squelette où il ne reste qu'à remplir les fonctions de traitement. Le prix : il faut apprendre **les conventions** avant d'être efficace.

---

## 2. Ce que tu dois savoir avant

### Indispensable

| Prérequis | Où Adonis s'en sert | Où le revoir |
|---|---|---|
| `async` / `await`, `try` / `catch` | chaque méthode de contrôleur, chaque requête en base | [`../ASYNCHRONE`](../ASYNCHRONE.md) |
| Classes, `static`, `extends`, `export default` | contrôleurs, modèles, middlewares sont des classes | [`../CLASSES_MODULES`](../CLASSES_MODULES.md), [`../lexique/STATIC_GET_SET`](../lexique/STATIC_GET_SET.md) |
| Déstructuration | `async show({ auth, request }: HttpContext)` | [`../OBJETS_TABLEAUX`](../OBJETS_TABLEAUX.md) §5 |
| TypeScript : types, unions, génériques, **effacement des types** | comprendre pourquoi Vine existe | [`../TYPESCRIPT`](../TYPESCRIPT.md) §6 |
| **HTTP** : méthodes, chemins, statuts (200, 201, 401, 403, 404, 422, 500), en-têtes | lire `routes.ts` et les réponses de `curl` | ton webserv ; [`ROUTES_CONTROLEURS`](ROUTES_CONTROLEURS.md) §2 |
| **JSON** | le corps de toutes les requêtes et réponses | [`../OBJETS_TABLEAUX`](../OBJETS_TABLEAUX.md) §6 |
| `npm`, `package.json`, scripts | lancer, tester, builder | [`../OUTILS`](../OUTILS.md) |

### Utile, à revoir si besoin

- **SQL relationnel** : table, clé primaire, **clé étrangère**, jointure, table de liaison (*pivot*). Lucid te l'épargne en partie, mais les relations mal déclarées de l'équipe ([`LUCID`](LUCID.md) §5) viennent toutes d'une clé étrangère mal placée.
- **Docker et Compose** : Postgres tourne dans un conteneur en dev, tout le projet en production ([`INSTALLATION`](INSTALLATION.md) §4, [`DEPLOIEMENT`](DEPLOIEMENT.md)). Ta doc Inception : [`../../../en/docker/INDEX`](../../../en/docker/INDEX.md).
- **`curl`** : tester une route sans front.
- **Les décorateurs** (`@column()`) : une syntaxe TypeScript qui annote une propriété. Il suffit de savoir les lire.
- **Hachage et jetons** : savoir qu'on stocke le **hash** d'un mot de passe ou d'un jeton, jamais l'original ([`AUTH`](AUTH.md) §1).

### Pas nécessaire

Express, Laravel ou Rails. Mais si tu en connais un, Adonis ressemble beaucoup plus à **Laravel** qu'à Express.

---

## 3. Le modèle mental à installer

**Une requête suit toujours le même trajet.** Si tu sais où elle passe, tu sais où chercher un bug.

```text
 requête
   │
   ▼  start/kernel.ts      middlewares serveur, puis routeur          (MIDDLEWARE_ERREURS)
   ▼  start/routes.ts      méthode + chemin → [controllers.X, 'méthode'], + middlewares nommés   (ROUTES_CONTROLEURS)
   ▼  app/controllers/     méthode async qui reçoit le HttpContext
   │     ├─ request.validateUsing(…)   app/validators/   → 422 si invalide          (VINE)
   │     ├─ auth.getUserOrFail()       le jeton           → 401 si absent           (AUTH)
   │     └─ Modele.query()…            app/models/        → SQL vers Postgres       (LUCID)
   ▼  ce que la méthode renvoie devient le JSON de la réponse
   ▼  une exception non rattrapée passe par app/exceptions/handler.ts                (MIDDLEWARE_ERREURS)
```

Et deux règles qui ne se discutent pas :
1. **Tout ce qui entre est validé** par Vine, même si le front est en TypeScript. Les types n'existent plus à l'exécution.
2. **L'identité vient du jeton** (`auth.user`), jamais du corps de la requête ni d'un message de socket.

---

## 4. Comment s'y prendre

1. **Lance le backend tel quel** en suivant [`INSTALLATION`](INSTALLATION.md), dans **ta propre copie** du dépôt avec ta propre base. Rien ne s'apprend sur un projet qui ne démarre pas.
2. **Fais `node ace list:routes`** et appelle deux ou trois routes avec `curl`. Tu vois le trajet du §3 de l'extérieur avant de lire le code.
3. **Suis une requête de bout en bout** : `POST /auth/signup`, de `routes.ts` au contrôleur, au validateur, au modèle, à la table. Lis [`ROUTES_CONTROLEURS`](ROUTES_CONTROLEURS.md), [`VINE`](VINE.md) et [`LUCID`](LUCID.md) dans cet ordre, avec le code ouvert à côté.
4. **Puis l'identité** : [`AUTH`](AUTH.md) et [`MIDDLEWARE_ERREURS`](MIDDLEWARE_ERREURS.md). Provoque un 401, un 403, un 422, une 404, une 500, et regarde la forme de chaque réponse.
5. **Puis le lien avec le front** : [`TUYAU`](TUYAU.md), lu en même temps que [`../react/API`](../react/API.md) ; [`SOCKET_IO`](SOCKET_IO.md) avec [`../react/TEMPS_REEL`](../react/TEMPS_REEL.md).
6. **Écris un test Japa** pour chaque chose que tu as vérifiée à la main ([`TESTS_JAPA`](TESTS_JAPA.md)). C'est ce qui empêche une régression de revenir.
7. **Termine par le déploiement** ([`DEPLOIEMENT`](DEPLOIEMENT.md)) et garde [`LIMITES_DEBUG`](LIMITES_DEBUG.md) ouvert pour chaque panne.

---

## 5. Les erreurs de débutant à éviter

- **Suivre un tutoriel AdonisJS 5.** La structure, les imports et l'ORM ont changé ; ce dossier est en Adonis 7 ([`LIMITES_DEBUG`](LIMITES_DEBUG.md) §3).
- **Lire `request.only([...])` comme une validation.** Ça ne fait que choisir des champs ; seul `validateUsing` valide ([`VINE`](VINE.md) §2).
- **Modifier une migration déjà jouée.** On en ajoute une nouvelle ([`LUCID`](LUCID.md) §1).
- **Croire qu'une relation qui compile marche.** Seul un `preload` l'exécute ; un test l'attrape ([`LUCID`](LUCID.md) §5, [`TESTS_JAPA`](TESTS_JAPA.md) §5).
- **Tester sur la base de dev.** Sans `.env.test`, les tests dépendent de tes données ([`TESTS_JAPA`](TESTS_JAPA.md) §3).
- **Oublier `codegen`** après un changement de route : le front utilise un registre périmé ([`TUYAU`](TUYAU.md) §2).

---

## 6. Tu es prêt pour le projet si…

> [!question]- Une requête arrive sur `POST /api/v1/auth/login`. Dans quels fichiers passe-t-elle, dans l'ordre ?
> `start/kernel.ts` (middlewares serveur et routeur) → `start/routes.ts` (la route, ses middlewares nommés) → la méthode `store` du contrôleur `AccessTokens` → son validateur → le modèle `User` → la réponse. ([`ROUTES_CONTROLEURS`](ROUTES_CONTROLEURS.md), [`MIDDLEWARE_ERREURS`](MIDDLEWARE_ERREURS.md) §1)

> [!question]- Que répond Adonis quand `validateUsing` échoue, et le code du contrôleur s'exécute-t-il ?
> Un **422** avec la liste de toutes les erreurs ; le reste du contrôleur ne s'exécute pas. ([`VINE`](VINE.md) §2)

> [!question]- Un `Game` a une colonne `winner_id`. `belongsTo` ou `hasMany` côté `Game` ?
> `belongsTo` : la clé étrangère est dans **ma** table. ([`LUCID`](LUCID.md) §4)

> [!question]- Qu'est-ce qui est stocké dans `auth_access_tokens` : le jeton ou son hash ?
> Son hash. Le jeton complet n'est montré qu'une fois, au login. ([`AUTH`](AUTH.md) §1)

> [!question]- Différence entre le middleware `auth()` et `requireFullAccess()` ?
> `auth()` vérifie que le jeton existe et n'est pas expiré. `requireFullAccess()` vérifie en plus qu'il porte l'ability `*`, pas seulement `2fa:verify`. ([`AUTH`](AUTH.md), TL;DR et §4)

> [!question]- Pourquoi `:id` sans `matchers.number()` donne-t-il une 500 au lieu d'une 404 ?
> `"abc"` part tel quel dans la requête SQL sur une colonne entière : Postgres lève une erreur, qui devient une 500. ([`ROUTES_CONTROLEURS`](ROUTES_CONTROLEURS.md) §4)

> [!question]- Un message de socket contient `{ "joueur": "alice" }`. Faut-il croire ce champ ?
> Non : l'identité vient du jeton vérifié à la connexion. ([`SOCKET_IO`](SOCKET_IO.md) §3)
