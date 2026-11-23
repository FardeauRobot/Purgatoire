# 🪜 Aborder Docker

> **TL;DR.** Un conteneur **n'est pas une machine** : c'est un processus Linux ordinaire à qui le noyau **ment** sur ce qu'il voit. Il a ses propres PID, sa propre racine `/` et sa propre pile réseau, sur **ton** noyau. Tu as déjà écrit les deux appels qui le créent : `fork()` + `execve()` dans minishell. Docker utilise `clone()` avec des drapeaux `CLONE_NEW*`. Prérequis : minishell (processus, `execve`, signaux), un peu de réseau (ports, sockets de webserv) et un shell Linux à l'aise. Aborde-le **dans l'ordre du [`00_ROADMAP`](../../en/docker/00_ROADMAP.md)** (EN) : le modèle d'abord, PID 1 ensuite, Compose en dernier. Celui qui ouvre `docker-compose.yml` le premier jour se perd.

Liens : [`SYNTHESE`](SYNTHESE.md) · [`GLOSSAIRE`](GLOSSAIRE.md) · hub [`docker/INDEX`](../../en/docker/INDEX.md) (EN) · [`00_ROADMAP`](../../en/docker/00_ROADMAP.md) (EN) · [`13_DEFENSE`](../../en/docker/13_DEFENSE.md) (EN)

> 🇬🇧 Le dossier Docker du vault est en anglais, sauf le [`GLOSSAIRE`](GLOSSAIRE.md), traduit. Les liens marqués **(EN)** pointent vers `en/docker/`. Cette fiche-ci n'a pas d'original anglais.

---

## 1. À quel besoin il répond

[`01_PHILOSOPHY`](../../en/docker/01_PHILOSOPHY.md) §1 (EN) liste quatre problèmes réels :

| Problème | En clair |
|---|---|
| La dérive d'environnement | « ça marche sur ma machine » |
| L'enfer des dépendances | deux applications qui veulent deux versions de la même bibliothèque |
| Une installation lente et non reproductible | monter un serveur à la main, étape par étape |
| L'écart entre dev, préproduction et production | ce qui tourne en test n'est pas ce qui tourne en vrai |

Docker y répond en livrant **l'environnement avec l'application** : une image construite une fois, lancée partout.

Ce qu'il **ne** résout **pas** ([`01_PHILOSOPHY`](../../en/docker/01_PHILOSOPHY.md) §2), et que tu dois savoir dire en défense :
- **ce n'est pas une barrière de sécurité** contre du code hostile, puisque le noyau est partagé ;
- une image arm64 ne tourne pas sur amd64 sans émulation ;
- **les données restent ton problème** : les volumes sont un mécanisme, pas une stratégie ;
- ce n'est pas de l'orchestration : Docker gère une seule machine.

Pour toi, le besoin concret s'appelle **Inception** : trois services (nginx, WordPress + php-fpm, MariaDB), construits depuis Alpine, reliés par un réseau privé, avec deux volumes et le port 443 comme seule entrée ([`docker/INDEX`](../../en/docker/INDEX.md), « Project facts »). C'est le premier projet 42 où rien ne compile : la note dépend de ta capacité à **expliquer chaque ligne** de ta configuration.

---

## 2. Ce que tu dois savoir avant

### Indispensable : ton C système

| Prérequis | Où Docker s'en sert | Où le revoir |
|---|---|---|
| `fork()` + `execve()` (minishell) | un conteneur = `clone()` avec `CLONE_NEWPID \| CLONE_NEWNS \| CLONE_NEWNET`, puis `execve()` ; « les drapeaux sont toute la différence » | [`minishell`](../../en/projects/LEVEL3/minishell.md) (EN), [`02_UNDER_THE_HOOD`](../../en/docker/02_UNDER_THE_HOOD.md) §1 (EN) |
| `execve()` **remplace** le processus et garde son PID | c'est exactement `exec "$@"` à la fin d'un entrypoint | [`03_IMAGES_AND_DOCKERFILES`](../../en/docker/03_IMAGES_AND_DOCKERFILES.md) §3 (EN) |
| Les signaux (`SIGTERM`, `SIGKILL`) et leur action par défaut | `docker stop` envoie `SIGTERM` à PID 1, qui **n'a pas** d'action par défaut | [`02_UNDER_THE_HOOD`](../../en/docker/02_UNDER_THE_HOOD.md) §2 (EN) |
| Les processus zombies et `wait()` | PID 1 doit récupérer les orphelins | [`03_IMAGES_AND_DOCKERFILES`](../../en/docker/03_IMAGES_AND_DOCKERFILES.md) §3 |
| Sockets, `bind()`, `listen()`, ports (webserv) | « la question est de savoir **dans quel espace de ports** tu fais ton `bind` » | [`05_NETWORKING`](../../en/docker/05_NETWORKING.md) (EN), en tête de page |

### Indispensable : le shell et Linux

- **Le shell POSIX** : tes scripts d'entrypoint commencent par `#!/bin/sh` et utilisent `set -e`, `exec "$@"`, des tests `[ -f … ]`. Alpine fournit les outils `busybox`, à qui il manque des options des outils GNU ([`01_PHILOSOPHY`](../../en/docker/01_PHILOSOPHY.md) §5) ; et le sujet interdit `bash` comme entrypoint ([`10_INCEPTION`](../../en/docker/10_INCEPTION.md) §2, R2).
- **L'arborescence Linux** : `/etc`, `/var`, `/run`, `/home`. Les montages : `mount`, un *bind mount*.
- **Les permissions** (`chmod`, propriétaire) : un 403 de nginx peut venir d'un mauvais propriétaire dans le volume ([`12_TROUBLESHOOTING`](../../en/docker/12_TROUBLESHOOTING.md) §4, EN).
- **Un `Makefile`** : c'est lui qui pilote `docker compose` ([`07_COMPOSE`](../../en/docker/07_COMPOSE.md) §7, EN).

### Utile

- **HTTP et TLS** dans les grandes lignes : un certificat, une clé, une version de TLS. nginx termine TLS sur le 443 ([`GLOSSAIRE`](GLOSSAIRE.md) §9).
- **YAML** : l'indentation porte le sens, comme Python.
- **Comment marche une page PHP** : nginx ne l'exécute pas, il la passe à php-fpm sur le port 9000 ([`05_NETWORKING`](../../en/docker/05_NETWORKING.md) §6, EN).
- **Un peu de SQL** : créer une base et un utilisateur dans MariaDB.

### Pas nécessaire

Kubernetes, Docker Swarm, le cloud. Le C++ ou le Rust. Écrire un serveur web : nginx le fait pour toi.

---

## 3. Le modèle mental à installer

Les quatre idées du hub [`docker/INDEX`](../../en/docker/INDEX.md) (« The mental model ») :

1. **Un conteneur est un processus, pas une machine.** Les *namespaces* décident de ce qu'il voit, les *cgroups* de ce qu'il consomme. Rien n'est émulé. C'est pour ça qu'il démarre vite, et pour ça que macOS a besoin d'une VM Linux cachée.
2. **Une image est une pile de couches en lecture seule ; le conteneur ajoute une couche inscriptible.** Quand le conteneur meurt, sa couche meurt avec lui. Ce qui doit survivre va dans un **volume**.
3. **Un service par conteneur.** Pas un processus (nginx crée des workers) : une **responsabilité**. Trois services = trois conteneurs, trois Dockerfiles.
4. **Le conteneur vit exactement aussi longtemps que son PID 1.** Ton `CMD` **est** PID 1. Les services doivent tourner au **premier plan**.

```text
  docker run  →  dockerd  →  containerd  →  runc  →  clone() + execve()
                                                         │
                         un processus de TON noyau, avec des namespaces
```

La chaîne vient de [`02_UNDER_THE_HOOD`](../../en/docker/02_UNDER_THE_HOOD.md) §6 (EN).

Et trois réflexes ([`01_PHILOSOPHY`](../../en/docker/01_PHILOSOPHY.md) §6) :
- **immutabilité** : on ne répare pas un conteneur avec `docker exec`, on reconstruit l'image ;
- **une responsabilité** par conteneur ;
- **déclaratif** : le Dockerfile et le compose décrivent l'état voulu, pas les étapes.

---

## 4. Comment s'y prendre

Le [`00_ROADMAP`](../../en/docker/00_ROADMAP.md) (EN) découpe tout en phases, avec trois temps chacune : **lire, faire, puis passer le 🚪 Gate**. Le Gate est une question à laquelle tu réponds **à voix haute, sans notes** ; si tu bloques, tu n'avances pas. Compte **10 à 14 jours** en tout. Le résumé :

| Phases | Quoi | Fiches |
|---|---|---|
| 0–1 | Lancer un conteneur Alpine, puis comprendre qu'un conteneur est un processus | [`01`](../../en/docker/01_PHILOSOPHY.md), [`02`](../../en/docker/02_UNDER_THE_HOOD.md) (EN) |
| 2–3 | Images, cache, Dockerfile, **puis PID 1** (« la demi-journée la plus rentable ») | [`03`](../../en/docker/03_IMAGES_AND_DOCKERFILES.md) (EN) |
| 4–5 | Réseau, puis stockage | [`05`](../../en/docker/05_NETWORKING.md), [`06`](../../en/docker/06_STORAGE.md) (EN) |
| 6–7 | Compose, puis secrets et variables d'environnement | [`07`](../../en/docker/07_COMPOSE.md), [`08`](../../en/docker/08_SECRETS_AND_ENV.md) (EN) |
| 8 | Construire Inception **de bas en haut** : squelette → MariaDB → WordPress → nginx → TLS → persistance | [`10_INCEPTION`](../../en/docker/10_INCEPTION.md) (EN) |
| 9–11 | Vérifier chaque règle du sujet, écrire la doc notée, préparer la défense | [`10`](../../en/docker/10_INCEPTION.md), [`11`](../../en/docker/11_COMPARISONS.md), [`12`](../../en/docker/12_TROUBLESHOOTING.md), [`13`](../../en/docker/13_DEFENSE.md) (EN) |

Trois conseils tirés de la roadmap :
- **Un service au vert avant le suivant** (Phase 8). Trois inconnues et un seul symptôme, c'est deviner.
- **« Au vert » veut dire** : il redémarre proprement après `down -v` puis `up --build`, sans geste manuel.
- **Tiens le journal d'utilisation de l'IA** dès le début : le README doit le contenir (Phase 10, [`10_INCEPTION`](../../en/docker/10_INCEPTION.md) §6).

Sur Mac, lis [`09_MAC_AND_LINUX`](../../en/docker/09_MAC_AND_LINUX.md) (EN) tôt : `/home/<login>/data` n'existe pas sur ton Mac (§5), et ton Mac est arm64 alors que la machine d'évaluation peut être amd64 (§6).

Un terme t'échappe ? Le [`GLOSSAIRE`](GLOSSAIRE.md) en français couvre chaque mot du sujet.

---

## 5. Les erreurs de débutant à éviter

- **Commencer par `docker-compose.yml`.** Compose orchestre ; sans savoir ce qu'il orchestre, il n'a pas de sens ([`00_ROADMAP`](../../en/docker/00_ROADMAP.md), introduction).
- **`tail -f /dev/null`** (ou `sleep infinity`, `while true`) pour garder le conteneur en vie : interdit par le sujet. La vraie correction est de mettre le service au premier plan en PID 1 ([`03_IMAGES_AND_DOCKERFILES`](../../en/docker/03_IMAGES_AND_DOCKERFILES.md) §3).
- **Oublier `exec`** à la fin de l'entrypoint : le shell reste PID 1 et tu as recréé le problème de `tail -f` (§3).
- **Un script d'init non idempotent** : un `CREATE DATABASE` sans condition échoue au deuxième `up`, c'est-à-dire exactement ce que fait l'évaluateur (§3).
- **Croire que `depends_on` attend que la base soit prête** : il règle seulement l'ordre de démarrage. Il faut un healthcheck ([`04_CONTAINERS`](../../en/docker/04_CONTAINERS.md) §4, EN).
- **Installer WordPress dans l'image puis monter un bind dessus** : le répertoire paraît vide ([`06_STORAGE`](../../en/docker/06_STORAGE.md) §3, EN).
- **Ne pas créer `/home/<login>/data` dans le Makefile** : « ça marche chez moi, pas chez l'évaluateur » (§4).
- **Un mot de passe dans git**, ou un `COPY . .` sans `.dockerignore` qui fige un `.env` dans une couche d'image ([`08_SECRETS_AND_ENV`](../../en/docker/08_SECRETS_AND_ENV.md) §6, [`03_IMAGES_AND_DOCKERFILES`](../../en/docker/03_IMAGES_AND_DOCKERFILES.md) §1, EN).
- **Copier un Inception sur GitHub** : il tournera, et tu échoueras à la défense, parce que chaque question porte sur **ton** raisonnement ([`00_ROADMAP`](../../en/docker/00_ROADMAP.md), « The honest shortcut warning »).

---

## 6. Tu es prêt pour la suite si…

Ces questions sont les 🚪 Gates des premières phases du [`00_ROADMAP`](../../en/docker/00_ROADMAP.md) ; les réponses viennent des fiches citées.

> [!question]- Pourquoi un conteneur démarre-t-il en 50 ms quand une VM met 30 secondes ? (Phase 1)
> La VM démarre **son propre noyau** sur du matériel virtualisé. Le conteneur n'est qu'un processus de plus sur le noyau déjà démarré : rien à booter. ([`13_DEFENSE`](../../en/docker/13_DEFENSE.md), « Conceptual questions » ; [`11_COMPARISONS`](../../en/docker/11_COMPARISONS.md) §1)

> [!question]- Tu modifies un fichier de config et tu reconstruis. Quelles couches sont refaites ? (Phase 2)
> L'instruction qui copie ce fichier, et **toutes celles qui suivent** : au premier défaut de cache, le cache est mort. D'où la règle : le stable en haut, le volatile en bas. ([`03_IMAGES_AND_DOCKERFILES`](../../en/docker/03_IMAGES_AND_DOCKERFILES.md) §1)

> [!question]- Pourquoi `CMD ["nginx"]` s'arrête-t-il tout de suite, et pas `CMD ["nginx", "-g", "daemon off;"]` ? (Phase 3)
> nginx se démonise : il crée une copie en arrière-plan et le parent se termine. Le parent était PID 1, donc le conteneur s'arrête. `daemon off;` le garde au premier plan. ([`03_IMAGES_AND_DOCKERFILES`](../../en/docker/03_IMAGES_AND_DOCKERFILES.md) §3)

> [!question]- Et pourquoi `tail -f /dev/null` est-il une mauvaise correction, pas seulement une moche ? (Phase 3)
> `SIGTERM` arrive à `tail`, qui ne le transmet pas ; PID 1 ignore les signaux qu'il ne gère pas ; Docker ne voit plus le vrai service et ne peut pas le redémarrer ; les zombies ne sont pas récupérés. Deux raisons suffisent en défense. ([`13_DEFENSE`](../../en/docker/13_DEFENSE.md), « PID 1 »)

> [!question]- WordPress joint la base sous le nom `mariadb`, et aucun fichier ne définit ce nom. Qui le résout ? (Phase 4)
> Le DNS intégré du **réseau bridge défini par l'utilisateur**, en `127.0.0.11`. Le nom du service devient un nom DNS. Le bridge par défaut, lui, n'a pas de DNS. ([`05_NETWORKING`](../../en/docker/05_NETWORKING.md) §3)

> [!question]- Le sujet interdit les bind mounts mais exige les données dans `/home/<login>/data`. Comment est-ce compatible ? (Phase 5)
> Un **volume nommé** du driver `local`, avec `driver_opts` (`type: none`, `o: bind`, `device: …`). Il y a bien un bind en dessous, mais l'objet est un volume déclaré et géré par Docker. ([`06_STORAGE`](../../en/docker/06_STORAGE.md) §4)

> [!question]- Suis un mot de passe du fichier sur le disque jusqu'à MariaDB. Qui d'autre peut le lire ? (Phase 7)
> `secrets/db_password.txt` (ignoré par git) → bloc `secrets:` du compose → `/run/secrets/db_password` sur un tmpfs, **seulement** dans les services qui le déclarent (ici mariadb et wordpress) → lu par l'entrypoint, jamais exporté → haché dans la table des utilisateurs de MariaDB, et **en clair** dans `wp-config.php` (WordPress l'exige), dans le volume. ([`08_SECRETS_AND_ENV`](../../en/docker/08_SECRETS_AND_ENV.md) §5)
