# 📒 Synthèse : Docker et Inception

> **TL;DR.** La fiche de révision du dossier [`docker/`](../../en/docker/INDEX.md) (EN). Le modèle (processus, couches, PID 1), puis le Dockerfile, le réseau, le stockage, les secrets et les règles du sujet, chacun réduit à ce qu'il faut savoir par cœur, avec sa fiche et son paragraphe. Pour l'entraînement à l'oral (questions et réponses), c'est [`13_DEFENSE`](../../en/docker/13_DEFENSE.md) (EN). Les blocs de code sont **recopiés tels quels** de [`03_IMAGES_AND_DOCKERFILES`](../../en/docker/03_IMAGES_AND_DOCKERFILES.md) §3, [`04_CONTAINERS`](../../en/docker/04_CONTAINERS.md) §4, [`06_STORAGE`](../../en/docker/06_STORAGE.md) §4 et [`12_TROUBLESHOOTING`](../../en/docker/12_TROUBLESHOOTING.md) §0, commentaires anglais compris.

Liens : [`ABORDER`](ABORDER.md) · [`GLOSSAIRE`](GLOSSAIRE.md) · hub [`docker/INDEX`](../../en/docker/INDEX.md) (EN) · [`00_ROADMAP`](../../en/docker/00_ROADMAP.md) (EN) · [`10_INCEPTION`](../../en/docker/10_INCEPTION.md) (EN)

Image de base : Alpine, **avant-dernière version stable**, à vérifier toi-même, jamais `latest` ([`10_INCEPTION`](../../en/docker/10_INCEPTION.md) §3). Cette fiche ne donne volontairement aucun numéro de version.

---

## 1. L'idée en cinq lignes

- **Un conteneur est un processus**, pas une machine : namespaces (ce qu'il voit) + cgroups (ce qu'il consomme), sur le noyau de l'hôte.
- **Une image = des couches en lecture seule** ; le conteneur ajoute une couche inscriptible, qui meurt avec lui. Ce qui doit durer va dans un volume.
- **Un service par conteneur** : une responsabilité, pas un processus.
- **Le conteneur vit aussi longtemps que son PID 1** : le service tourne au premier plan, en PID 1.
- **On ne répare pas, on reconstruit**, et on décrit l'état voulu plutôt que les étapes ([`01_PHILOSOPHY`](../../en/docker/01_PHILOSOPHY.md) §6).

> 🎤 Le résumé de 60 secondes, à avoir prêt ([`13_DEFENSE`](../../en/docker/13_DEFENSE.md)) : trois conteneurs sur un réseau Docker privé ; nginx termine TLS sur le 443, WordPress + php-fpm sur le 9000, MariaDB sur le 3306 ; seul nginx est exposé ; deux volumes nommés, adossés à `/home/<login>/data` ; tout construit depuis Alpine, orchestré par docker compose, piloté par un Makefile.

---

## 2. Sous le capot ([`02_UNDER_THE_HOOD`](../../en/docker/02_UNDER_THE_HOOD.md))

| À savoir | § |
|---|---|
| minishell : `fork()` + `execve()`. Docker : `clone()` avec `CLONE_NEW*` + `execve()`. « Les drapeaux sont toute la différence » | 1 |
| Namespaces : **PID** (ton processus est PID 1), **Mount** (`/` est l'image), **Network** (deux conteneurs peuvent tous deux écouter le 443), UTS, IPC, User, Cgroup | 2 |
| PID 1 : récupère les orphelins ; **n'a pas d'action par défaut sur les signaux** (un `SIGTERM` non géré est ignoré) ; s'il meurt, tout le namespace meurt | 2 |
| `/` du conteneur : `pivot_root` dans un système de fichiers assemblé à partir des couches, pas `chroot` | 2 |
| Cgroups : limites de CPU, mémoire (OOM-kill), E/S, nombre de processus | 3 |
| Couches : un système de fichiers en union (overlay) empile les couches de l'image et la couche inscriptible | 4 |
| **macOS n'a ni namespaces ni cgroups** (noyau Darwin) : Docker Desktop, colima, OrbStack font tourner une **VM Linux** ; la CLI parle au démon par un socket | 5 |
| Chaîne : `docker run` → dockerd → containerd → runc → `clone()` + `execve()` | 6 |

---

## 3. Images et Dockerfile ([`03_IMAGES_AND_DOCKERFILES`](../../en/docker/03_IMAGES_AND_DOCKERFILES.md))

| À savoir | § |
|---|---|
| **Règle du cache** : au premier défaut, l'instruction et **toutes les suivantes** sont refaites. Le stable en haut, le volatile (`conf/`, `tools/`) en bas | 1 |
| `RUN` est mis en cache sur la **chaîne de commande littérale**, pas sur son résultat | 1 |
| `docker build .` envoie tout le répertoire au démon. **`.dockerignore` est un contrôle de sécurité** : sans lui, `COPY . .` fige un `.env` dans une couche, pour toujours | 1 |
| `EXPOSE` ne publie rien : c'est de la documentation | 2 |
| Deux exemples commentés, nginx et MariaDB (la forme, pas les fichiers entiers : les écrire, c'est le projet) ; builds multi-étapes ; checklist de chaque Dockerfile | 4–6 |

### PID 1, la section qui décide du projet (§3)

| Service | Au premier plan |
|---|---|
| nginx | `nginx -g "daemon off;"` |
| php-fpm | `php-fpm82 -F` |
| MariaDB | `mariadbd` (pas `mariadbd_safe` : relancer en cas de crash, c'est le travail de la politique de redémarrage) |

```sh
#!/bin/sh
set -e

# ... initialization: create DB, users, wp-config, whatever ...

exec "$@"      # ← the important line
```

- **`exec`** remplace le shell par le service, qui hérite du PID 1. C'est le même `execve()` que dans minishell.
- **`set -e`** : arrêt à la première erreur, sinon le service démarre sur un état cassé.
- **Idempotence** : le script tourne à **chaque** démarrage ; chaque étape est « crée-le s'il n'existe pas ».
- **Forme exec** (`CMD ["nginx", "-g", "daemon off;"]`), jamais la forme shell, qui fait de `/bin/sh -c` le PID 1. Corollaire : `CMD ["echo", "$HOME"]` affiche `$HOME` tel quel.
- **Les 4 raisons contre `tail -f /dev/null`** : les signaux ne sont pas transmis ; PID 1 ignore ceux qu'il ne gère pas ; Docker ne voit plus le service (pas de redémarrage) ; les zombies s'accumulent.

---

## 4. Conteneurs ([`04_CONTAINERS`](../../en/docker/04_CONTAINERS.md))

| À savoir | § |
|---|---|
| `restart: unless-stopped` : redémarre après un crash et après un reboot de la VM, mais un arrêt manuel est respecté. `always` relance même ce que tu as arrêté à la main | 3 |
| `Restarting (1) 3 seconds ago` dans `docker ps` = une boucle de démarrage : lire les logs | 3 |
| **`depends_on` règle l'ordre, pas la disponibilité.** Un processus qui existe n'est pas un service prêt | 4 |

```yaml
services:
  mariadb:
    healthcheck:
      test: ["CMD", "mariadb-admin", "ping", "-h", "localhost"]
      interval: 10s
      timeout: 5s
      retries: 5
      start_period: 30s

  wordpress:
    depends_on:
      mariadb:
        condition: service_healthy
```

`start_period` : la période de grâce pendant laquelle les échecs ne comptent pas (§4). Autres solutions, moins propres : une boucle d'attente dans l'entrypoint, ou des tentatives répétées côté application (§4 b et c).

---

## 5. Réseau ([`05_NETWORKING`](../../en/docker/05_NETWORKING.md))

| À savoir | § |
|---|---|
| Le **bridge par défaut n'a pas de DNS** ; un **bridge défini par l'utilisateur** a un DNS intégré en **`127.0.0.11`** qui résout les noms de services | 3 |
| C'est pour ça que WordPress joint `mariadb` sans rien configurer ; Compose crée toujours un réseau utilisateur | 3 |
| Des conteneurs sur deux réseaux utilisateur différents ne se voient pas : le réseau **est** la frontière | 3 |
| `ports:` publie vers l'hôte ; `EXPOSE` et `expose:` ne publient rien. Sur un même réseau, **tous** les ports sont joignables | 4 |
| **Seul nginx publie, et seulement le 443** ; MariaDB (3306) et php-fpm (9000) restent internes | 4 |
| Règle N5 : bloc `networks:` déclaré explicitement ; N6 : `network_mode: host`, `--link`, `links:` interdits | 5 |
| 9000 parle **FastCGI**, pas HTTP : `curl` n'y donne rien, tester avec `nc -zv wordpress 9000` | 6 |

---

## 6. Stockage ([`06_STORAGE`](../../en/docker/06_STORAGE.md))

| | Volume nommé | Bind mount | tmpfs |
|---|---|---|---|
| Vit | dans une zone gérée par Docker | dans un chemin de l'hôte que tu choisis | en RAM |
| Survit au conteneur | oui | oui | non |
| Pour | bases de données, état d'application | éditer du code en dev | secrets, brouillons |

(§2.) **Le piège** (§3) : un volume **vide** monté sur un répertoire rempli reçoit le contenu de l'image ; un **bind** le **masque** (le répertoire paraît vide). La copie n'a lieu que si le volume est vide : ensuite, le volume gagne toujours sur l'image.

### Le motif Inception (§4)

```yaml
volumes:
  wordpress:
    driver: local
    driver_opts:
      type: none
      device: /home/<login>/data/wordpress
      o: bind
  mariadb:
    driver: local
    driver_opts:
      type: none
      device: /home/<login>/data/mariadb
      o: bind
```

- Ce sont les arguments de `mount(2)` : `type: none` (pas de système de fichiers), `o: bind`, `device` (la source).
- **Oui, il y a un bind en dessous** ; ne le nie pas en défense. Ce qui en fait un volume nommé : déclaré dans `volumes:`, référencé par son nom, visible dans `docker volume ls`, avec un cycle de vie Docker.
- **Le répertoire doit exister** : `mkdir -p` dans le Makefile, sinon « ça marche chez moi » seulement.

---

## 7. Compose et Makefile ([`07_COMPOSE`](../../en/docker/07_COMPOSE.md))

| À savoir | § |
|---|---|
| Chaque bloc du compose a son équivalent `docker run` : si tu ne sais pas le retrouver, tu copies au lieu d'écrire | Gate (§9) |
| Le bloc de service, champ par champ ; volumes et réseaux ; secrets | 3–5 |
| Un squelette complet pour Inception | 6 |
| Le Makefile pilote `docker compose -f srcs/docker-compose.yml …` | 7 |

---

## 8. Secrets et variables d'environnement ([`08_SECRETS_AND_ENV`](../../en/docker/08_SECRETS_AND_ENV.md))

| | Variables d'environnement | Secrets Docker |
|---|---|---|
| Exposés comme | l'environnement du processus | un fichier `/run/secrets/<nom>` |
| Visibles dans `docker inspect` | **oui, en clair** | non |
| Hérités par les processus enfants | oui | non |
| Peuvent finir dans une image | oui (`ENV` / `ARG`) | non |
| Pour | la configuration | les identifiants |

(§1.) Règle courte : **si sa fuite compte, c'est un secret.**

| À savoir | § |
|---|---|
| Le secret est monté sur un **tmpfs**, dans **chaque service qui le déclare**, et seulement ceux-là | 3, 5 |
| **Avec docker compose, il n'est pas chiffré au repos** : ça, c'est Swarm et Kubernetes. Savoir dire le bénéfice **et** la limite | 3 |
| WordPress exige le mot de passe de la base **en clair** dans `wp-config.php`, dans le volume | 5 |
| `.gitignore` **avant** le premier identifiant ; `git rm` après coup n'efface pas l'historique | 6 |
| Convention `_FILE` : l'entrypoint lit le fichier plutôt qu'une valeur | 7 |

---

## 9. Mac et Linux ([`09_MAC_AND_LINUX`](../../en/docker/09_MAC_AND_LINUX.md))

| À savoir | § |
|---|---|
| macOS : Docker Desktop, colima ou OrbStack, toujours une VM Linux en dessous | 2 |
| `/home/<login>/data` n'existe pas sur ton Mac | 5 |
| Ton Mac construit des images **arm64** ; la machine d'évaluation est très probablement **amd64** | 6 |
| Le domaine `<login>.42.fr` passe par une entrée dans `/etc/hosts` | 8 |

---

## 10. Les règles du sujet ([`10_INCEPTION`](../../en/docker/10_INCEPTION.md) §2)

| Groupe | À retenir |
|---|---|
| Structure | une VM ; tout dans `srcs/` ; Makefile à la racine ; un Dockerfile par service ; nom d'image = nom de service |
| Images | Alpine ou Debian avant-dernière stable ; pas de `latest` ; images construites par toi ; **aucun mot de passe dans un Dockerfile** |
| Services | TLS 1.2 ou 1.3 seulement ; nginx **seule entrée, port 443** ; redémarrage sur crash ; deux utilisateurs WordPress, l'admin sans `admin` dans son nom |
| Stockage et réseau | deux volumes **nommés** dans `/home/<login>/data` ; un réseau déclaré ; pas de `host` ni de `links` |
| Hygiène | pas de boucle infinie (`tail -f`, `bash`, `sleep infinity`, `while true`) ; `.env` obligatoire ; un identifiant dans git = **échec du projet** |

Documentation notée, condition de validation : `README.md`, `USER_DOC.md`, `DEV_DOC.md` à la racine du projet, `LEVEL5/Inception/` (§4–5). Le README contient les **quatre comparaisons** de [`11_COMPARISONS`](../../en/docker/11_COMPARISONS.md) (EN) :

| Comparaison | En une phrase (§1–4) |
|---|---|
| VM / Docker | la VM virtualise le **matériel** et démarre son noyau ; le conteneur virtualise le **système** et emprunte le noyau de l'hôte |
| Secrets / variables | les variables sont héritées, inspectables, fuyantes par nature ; les secrets sont des fichiers montés, limités aux services qui en ont besoin |
| Réseau Docker / hôte | un réseau utilisateur donne une pile réseau propre + un DNS ; le mode `host` supprime cette séparation |
| Volumes / bind mounts | le bind associe un chemin **que tu choisis** ; le volume est un objet **que Docker crée, nomme et gère** |

---

## 11. Quand ça casse ([`12_TROUBLESHOOTING`](../../en/docker/12_TROUBLESHOOTING.md))

```bash
docker compose ps -a                    # what's running, exited, restarting?
docker compose logs --tail 50 <service> # what did it say before dying?
docker compose config                   # what does compose actually see?
```

Toujours ces trois-là, **avant** de modifier un fichier (§0). Ensuite, par symptôme : le conteneur s'arrête tout de suite (§1), 502 Bad Gateway (§2), « Error establishing a database connection » (§3), répertoire WordPress vide ou 403 (§4), montage (§5), TLS (§6), marche au premier `up` mais casse au second (§7), ports et permissions (§8), build (§9).

---

## 🧠 Questions de révision

> [!question]- Pourquoi `docker stop` met-il 10 secondes sur un conteneur mal construit ?
> PID 1 n'a pas d'action par défaut sur `SIGTERM` : s'il ne le gère pas, il l'ignore. Docker attend le délai de grâce, puis envoie `SIGKILL`. ([`02_UNDER_THE_HOOD`](../../en/docker/02_UNDER_THE_HOOD.md) §2)

> [!question]- Pourquoi `mariadbd` et pas `mariadbd_safe` ?
> `_safe` est une enveloppe qui relance le serveur après un crash. C'est le rôle de la politique de redémarrage ; un superviseur dans le conteneur cache les pannes à Docker. ([`13_DEFENSE`](../../en/docker/13_DEFENSE.md), « PID 1 »)

> [!question]- Ton entrypoint fait `CREATE DATABASE wordpress;`. Que se passe-t-il au deuxième `up` ?
> Il échoue : la base existe déjà dans le volume, et `set -e` arrête le script. Chaque étape doit être idempotente. ([`03_IMAGES_AND_DOCKERFILES`](../../en/docker/03_IMAGES_AND_DOCKERFILES.md) §3)

> [!question]- WordPress plante au démarrage, puis marche au troisième essai. Le bug est-il corrigé ?
> Non : `depends_on` ne règle que l'ordre, et le redémarrage masque la course. Il faut un healthcheck sur MariaDB et `condition: service_healthy`. ([`04_CONTAINERS`](../../en/docker/04_CONTAINERS.md) §4)

> [!question]- MariaDB ne publie pas le 3306. Comment WordPress la joint-il, et pourquoi l'extérieur ne peut-il pas ?
> Sur un même réseau, tous les ports sont joignables ; `ports:` ne sert qu'à ouvrir vers l'hôte. Seul nginx publie, le 443. ([`05_NETWORKING`](../../en/docker/05_NETWORKING.md) §4)

> [!question]- Tu modifies un fichier dans l'image et tu reconstruis : rien ne change dans le conteneur. Pourquoi ?
> Le volume a déjà du contenu, et il gagne sur l'image ; la copie image → volume n'a lieu que si le volume est vide. `docker compose down -v` puis `up -d --build`. ([`06_STORAGE`](../../en/docker/06_STORAGE.md) §3, [`12_TROUBLESHOOTING`](../../en/docker/12_TROUBLESHOOTING.md) §4)

> [!question]- « Les secrets Docker sont chiffrés » : vrai dans Inception ?
> Non. Avec docker compose, `secrets:` lit un fichier local et le monte sur un tmpfs, sans chiffrement au repos. Le gain : hors de l'environnement, hors de `docker inspect`, hors des images, un seul dossier à ignorer dans git. ([`08_SECRETS_AND_ENV`](../../en/docker/08_SECRETS_AND_ENV.md) §3)

> [!question]- Pourquoi le sujet interdit-il `network_mode: host` ?
> Le conteneur partage alors la pile réseau de l'hôte : MariaDB écouterait le 3306 **de l'hôte**, et nginx ne serait plus la seule entrée. ([`05_NETWORKING`](../../en/docker/05_NETWORKING.md) §5)
