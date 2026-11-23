# 🟪 AdonisJS : le backend de Transcendance

> Rédigé directement en français, comme le reste de [`javascript/`](../INDEX.md).

**AdonisJS** est un framework backend en TypeScript, plus proche de Laravel ou de Rails que d'Express : il fournit d'office le routeur, les contrôleurs, la validation (**Vine**), l'ORM (**Lucid**), l'authentification, les tests (**Japa**) et une ligne de commande (`node ace`). La stack de l'équipe y ajoute **Tuyau** (le client typé du front) et **Socket.IO** (le temps réel), puis déploie le tout avec Docker, Caddy et Postgres.

Ce dossier suit le backend du dépôt de l'équipe, de `npm ci` jusqu'au conteneur de production. Chaque fiche part du **code réel** de l'équipe, montre ce qu'il fait vraiment (réponses `curl`, sorties de tests), et propose une correction quand il ne fait pas ce qu'on croit.

Liens : [`../INDEX`](../INDEX.md) · [`../VERS_TRANSCENDANCE`](../VERS_TRANSCENDANCE.md) · [`../react/INDEX`](../react/INDEX.md) · [`../TYPESCRIPT`](../TYPESCRIPT.md) · [`../ASYNCHRONE`](../ASYNCHRONE.md) · [sujet ft_transcendence](../../projects/LEVEL6/ft_transcendence-subject.md) · [doc AdonisJS](https://docs.adonisjs.com)

---

## 🧭 Ordre de lecture

| Étape | Fiche | Pourquoi maintenant |
|---|---|---|
| 0 | [`INSTALLATION`](INSTALLATION.md) | Le monorepo, les versions, le `.env`, Postgres en dev, `migration:run` et `codegen`, `node ace`. |
| 1 | [`ROUTES_CONTROLEURS`](ROUTES_CONTROLEURS.md) | Lire `routes.ts`, le `HttpContext`, un contrôleur complet, les paramètres de route, `serialize`. |
| 2 | [`VINE`](VINE.md) | Valider les entrées, la forme des 422, `exists` en base, un validateur qui dépend de l'utilisateur. |
| 3 | [`LUCID`](LUCID.md) | Migrations, schéma généré, requêtes, les quatre relations, l'audit des relations de l'équipe, les pivots. |
| 4 | [`AUTH`](AUTH.md) | Jetons d'accès, abilities, le flux 2FA observé de bout en bout, une réponse de login cohérente. |
| 5 | [`MIDDLEWARE_ERREURS`](MIDDLEWARE_ERREURS.md) | Le kernel, deux middlewares maison, les formes d'erreur, dev et prod, masquer les 500. |
| 6 | [`TUYAU`](TUYAU.md) | Le registre généré, le bug de login que TypeScript voit, `.safe()`. |
| 7 | [`SOCKET_IO`](SOCKET_IO.md) | Brancher Socket.IO sur Adonis, authentifier la connexion, couper au logout, valider les payloads. |
| 8 | [`TESTS_JAPA`](TESTS_JAPA.md) | Un fichier de test complet, une base de test séparée, la migration au démarrage, l'audit des relations. |
| 9 | [`DEPLOIEMENT`](DEPLOIEMENT.md) | `compose.yaml`, les dépendances manquantes en prod, Caddy et les avatars, le healthcheck, les migrations. |
| 10 | [`LIMITES_DEBUG`](LIMITES_DEBUG.md) | Les outils de diagnostic, un tableau message d'erreur → cause, ce que ce dossier ne couvre pas. |

> 🌟 Si tu n'as le temps que pour trois fiches : **LUCID**, **AUTH**, **TESTS_JAPA**.

> 🪜 **Avant l'étape 0** : [`ABORDER`](ABORDER.md), les prérequis, le modèle mental et l'ordre d'étude. 📒 **Pour réviser** : [`SYNTHESE`](SYNTHESE.md), tout le dossier en une fiche, avec quiz.

Côté front, chaque fiche renvoie à son pendant dans [`../react/`](../react/INDEX.md) : [`API`](../react/API.md) pour les appels, [`TEMPS_REEL`](../react/TEMPS_REEL.md) pour Socket.IO, [`REFS_FORMULAIRES`](../react/REFS_FORMULAIRES.md) pour afficher les erreurs de Vine.

---

## 🧪 Pas de labo

- Ce dossier n'a pas de labo : chaque fiche se termine par **« 🧪 À essayer »**. Fais les exercices dans **ta propre copie** du dépôt (`cp -R`, ou une branche à toi), avec ta propre base Postgres ([`INSTALLATION`](INSTALLATION.md), §4).
- Le dépôt de l'équipe sert d'exemple. Les fiches décrivent son code et proposent des corrections, mais **ne le modifient pas** : toutes les corrections ont été écrites et testées dans une copie.

---

## ✅ Ce qui a été vérifié

Versions lues dans `node_modules` le **20/11/2026** : `@adonisjs/core` 7.6.0, `@adonisjs/lucid` 22.4.2, `@adonisjs/auth` 10.1.0, `@vinejs/vine` 4.4.0, `@tuyau/core` 1.3.0, `@japa/runner` 5.4.0, `socket.io` 4.8.4, `pg` 8.23.1, `typescript` 6.0.3. Images : `postgres:18-alpine`, `caddy:2.11.9-alpine`.

Tout a été rejoué sur une **copie** du dépôt, avec un Postgres jetable dans Docker. Ni le dépôt de l'équipe ni sa base n'ont été modifiés.

- **Observé.** Ce que les fiches disent « observé » a été lancé : `curl` sur le serveur de dev et sur le build de prod, scripts Node (flux 2FA, client Tuyau, `socket.io-client`), tests Japa, des conteneurs Postgres et Caddy lancés avec `docker run`. Les sorties sont recopiées telles quelles, en tronquant parfois les lignes longues (`…`).
- **Messages d'erreur.** Tous ceux du tableau de [`LIMITES_DEBUG`](LIMITES_DEBUG.md) §2 ont été déclenchés et recopiés.
- **Code de l'équipe, tel quel.** Les comportements du code d'origine (login sans validateur, deux formes de réponse au login, relations mal déclarées, socket non authentifiée, otplib absent du backend, Caddy qui renvoie `index.html` pour les avatars…) ont été observés **avant** toute correction.
- **Corrections, toutes dans la copie.** Chacune a été lancée, et le test ou la requête qui montrait le problème a été rejoué :
  - le login unifié et validé ([`AUTH`](AUTH.md) §6, [`VINE`](VINE.md) §2) ;
  - le gestionnaire d'exceptions qui masque les 500 ([`MIDDLEWARE_ERREURS`](MIDDLEWARE_ERREURS.md) §5) ;
  - l'authentification, la coupure au logout et la validation Vine des sockets ([`SOCKET_IO`](SOCKET_IO.md) §3–5) ;
  - les modèles corrigés ([`LUCID`](LUCID.md) §5) ;
  - `.env.test` et le hook de migration de `tests/bootstrap.ts` ([`TESTS_JAPA`](TESTS_JAPA.md) §3–4) ;
  - la route `/health` ([`DEPLOIEMENT`](DEPLOIEMENT.md) §6), le Caddyfile (§4), les dépendances déplacées dans le `package.json` du backend (§2), l'URL de base du front (§3).
- **Suite de tests.** Dans la copie corrigée : `node ace test` → **24 tests verts** ([`TESTS_JAPA`](TESTS_JAPA.md)), et `tsc --noEmit` sans erreur.
- **Non testé.**
  - Le build des images Docker de l'équipe : [`DEPLOIEMENT`](DEPLOIEMENT.md) en a rejoué les étapes à la main (installation, `node ace build`, lancement en prod), sans `docker build`.
  - La correction de l'URL de base du front : le build passe, mais elle n'a pas été chargée dans un navigateur ([`DEPLOIEMENT`](DEPLOIEMENT.md) §3).
  - `APP_URL` en dur et le volume des avatars ([`DEPLOIEMENT`](DEPLOIEMENT.md) §5).
  - Le `request_id` dans les logs Docker ([`LIMITES_DEBUG`](LIMITES_DEBUG.md) §1) : vu en local seulement.
- **`ABORDER` et `SYNTHESE`** ne contiennent pas de code nouveau : leurs extraits sont recopiés des fiches du dossier, qui indiquent leur source. Ce sont des **extraits**, parfois raccourcis par `// …` ou privés de leurs imports : ils ne sont pas des modules complets et ne comptent pas dans la vérification ci-dessus.
- Si la doc officielle contredit une fiche, c'est la doc qui a raison. Corrige la fiche. Méfie-toi des exemples écrits pour **AdonisJS 5** ([`LIMITES_DEBUG`](LIMITES_DEBUG.md) §3).

---

## 🔁 C ↔ AdonisJS

| En C | Dans Adonis | Fiche |
|---|---|---|
| Un `switch` sur la commande reçue | `routes.ts` : méthode + chemin → méthode de contrôleur | [`ROUTES_CONTROLEURS`](ROUTES_CONTROLEURS.md) |
| Vérifier chaque argument avant de l'utiliser | Un validateur Vine, appelé en première ligne | [`VINE`](VINE.md) |
| Une `struct` et des fonctions qui la lisent ou l'écrivent | Un modèle Lucid | [`LUCID`](LUCID.md) |
| Qui tient le pointeur dans une liste chaînée ? | `belongsTo` / `hasMany` / `manyToMany` | [`LUCID`](LUCID.md) §4 |
| L'uid du processus fourni par le noyau | `auth.user`, tiré du jeton, jamais du corps de la requête | [`AUTH`](AUTH.md), [`SOCKET_IO`](SOCKET_IO.md) |
| Un `.h` partagé entre deux `.c` | Le registre de Tuyau | [`TUYAU`](TUYAU.md) |
| `bind()` sur `0.0.0.0` ou `127.0.0.1` | `HOST` dans le conteneur | [`DEPLOIEMENT`](DEPLOIEMENT.md) §1 |

---

## 🚨 Les pièges, en une liste

1. Une migration déjà jouée ne se modifie pas : on en ajoute une nouvelle ([`LUCID`](LUCID.md) §1).
2. Une relation mal déclarée compile : seul un `preload` l'attrape, d'où l'audit ([`LUCID`](LUCID.md) §5, [`TESTS_JAPA`](TESTS_JAPA.md) §5).
3. `request.only([...])` ne valide rien : utilise `validateUsing` ([`VINE`](VINE.md) §2).
4. Un paramètre `:id` sans `matchers.number()` produit une 500 au lieu d'une 404 ([`ROUTES_CONTROLEURS`](ROUTES_CONTROLEURS.md) §4).
5. Deux `return` aux clés différentes dans le login : le front ne sait pas lire la réponse ([`AUTH`](AUTH.md) §6, [`TUYAU`](TUYAU.md) §3).
6. `accessTokens.verify` ne vérifie pas les abilities : teste `allows` toi-même ([`SOCKET_IO`](SOCKET_IO.md) §3).
7. Le nom d'un joueur vient du jeton, jamais du message ([`SOCKET_IO`](SOCKET_IO.md) §3).
8. Le registre de Tuyau n'est pas commité : relance `codegen` après un `git pull` ([`TUYAU`](TUYAU.md) §2).
9. En prod, une 500 garde son message brut (chemin de fichier, nom de paquet, SQL) si on ne la masque pas ([`MIDDLEWARE_ERREURS`](MIDDLEWARE_ERREURS.md) §4–5).
10. Sans `POSTGRES_DB` dans `.env.test`, `node ace test` tourne sur la base de **dev** : les tests dépendent de tes données ([`TESTS_JAPA`](TESTS_JAPA.md) §3).
11. Une dépendance déclarée à la racine du monorepo manque dans l'image du backend ([`DEPLOIEMENT`](DEPLOIEMENT.md) §2).
12. Une variable `VITE_` absente au build tombe sur sa valeur par défaut, figée dans le bundle ([`DEPLOIEMENT`](DEPLOIEMENT.md) §3).
13. Un chemin que Caddy ne relaie pas renvoie `index.html` avec un **200** ([`DEPLOIEMENT`](DEPLOIEMENT.md) §4).
14. Un healthcheck qui ne touche pas la base répond « ok » quand elle est tombée ([`DEPLOIEMENT`](DEPLOIEMENT.md) §6).
