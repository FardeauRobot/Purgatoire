# 📖 GLOSSAIRE — Tous les termes techniques d'Inception

> Source : sujet `LEVEL5/Inception/INCEPTION.pdf`, version 5.3. Chaque terme qui apparaît dans le sujet (ou qu'il faut comprendre pour le respecter) : **définition**, puis **usage dans Inception**. Pour aller plus loin, chaque section renvoie au chapitre de la bibliothèque qui la creuse.
>
> 🇬🇧 [English version](../../en/docker/GLOSSAIRE.md)

---

## 🗂️ Sommaire

1. [[#1. Virtualisation & système|Virtualisation & système]]
2. [[#2. Docker — les briques|Docker — les briques]]
3. [[#3. Dockerfile — les instructions|Dockerfile — les instructions]]
4. [[#4. Docker Compose|Docker Compose]]
5. [[#5. Processus, PID 1 & démons|Processus, PID 1 & démons]]
6. [[#6. Réseau|Réseau]]
7. [[#7. Stockage|Stockage]]
8. [[#8. Secrets & configuration|Secrets & configuration]]
9. [[#9. Web & TLS|Web & TLS]]
10. [[#10. Les services|Les services]]
11. [[#11. Bonus|Bonus]]
12. [[#12. Rendu & évaluation|Rendu & évaluation]]

---

## 1. Virtualisation & système

> Approfondir : [`01_PHILOSOPHY.md`](../../en/docker/01_PHILOSOPHY.md), [`02_UNDER_THE_HOOD.md`](../../en/docker/02_UNDER_THE_HOOD.md)

### Administration système (sysadmin)
- **Définition** : le métier qui installe, configure et maintient des machines et des services (serveurs, réseau, bases de données, sauvegardes).
- **Usage** : c'est la catégorie du projet (« *System Administration related exercise* »). Rien ne se compile : tu livres de la **configuration**, et tu dois savoir justifier chaque ligne.

### Machine virtuelle (VM)
- **Définition** : un ordinateur entier émulé par un **hyperviseur** (VirtualBox, UTM, KVM…) — avec son propre noyau, son propre OS, son propre matériel virtuel.
- **Usage** : le sujet exige que **tout le projet tourne dans une VM**. C'est elle l'« hôte » des conteneurs. Le README doit comparer *VM vs Docker*.

### Hyperviseur
- **Définition** : le logiciel qui crée et fait tourner les VM en partageant le vrai matériel entre elles.
- **Usage** : c'est ce qui fait tourner ta VM Inception (sur Mac Apple Silicon : UTM, ou la VM cachée de Docker Desktop / OrbStack / Colima).

### Hôte (host)
- **Définition** : la machine sur laquelle tourne Docker — celle qui possède le noyau que partagent tous les conteneurs.
- **Usage** : ici, l'hôte = ta VM. Les volumes doivent vivre dans `/home/<login>/data` **sur l'hôte**.

### Noyau (kernel)
- **Définition** : le cœur de l'OS, qui gère processus, mémoire, fichiers, réseau.
- **Usage** : la différence clé VM/conteneur : une VM a **son propre noyau**, un conteneur **partage celui de l'hôte**. C'est pourquoi un conteneur démarre en millisecondes et pourquoi macOS a besoin d'une VM Linux pour Docker.

### Namespaces
- **Définition** : mécanisme du noyau Linux qui donne à un processus une **vue isolée** d'une ressource (PID, réseau, points de montage, hostname, utilisateurs…).
- **Usage** : c'est *ce qui fait* un conteneur. Le PID 1 de ton conteneur n'est PID 1 que dans son namespace PID.

### cgroups (control groups)
- **Définition** : mécanisme du noyau qui **limite et mesure** les ressources (CPU, RAM, I/O) d'un groupe de processus.
- **Usage** : invisible dans le sujet, mais à citer en défense : namespaces = *ce que le conteneur voit*, cgroups = *ce qu'il peut consommer*.

### Distribution (Alpine / Debian)
- **Définition** : une variante de Linux packagée avec son gestionnaire de paquets. **Alpine** est minuscule (~5 Mo, `musl`, `apk`), **Debian** est plus lourde mais plus standard (`glibc`, `apt`).
- **Usage** : seules images de base autorisées. Les conteneurs doivent partir de **l'avant-dernière version stable** de l'une des deux.

### Avant-dernière version stable (penultimate stable)
- **Définition** : la version stable *juste avant* la plus récente (ex. si Debian 13 est la dernière stable, l'avant-dernière est Debian 12 `bookworm`).
- **Usage** : impose un tag **explicite** (`alpine:3.21`, `debian:bookworm`). Vérifie la version au moment où tu codes et sache la justifier : elle bouge avec le temps.

### Gestionnaire de paquets (`apk` / `apt`)
- **Définition** : l'outil qui installe les logiciels d'une distribution.
- **Usage** : c'est par lui que tu installes nginx, php-fpm, mariadb dans tes Dockerfiles (`RUN apk add --no-cache nginx`).

---

## 2. Docker — les briques

> Approfondir : [`03_IMAGES_AND_DOCKERFILES.md`](../../en/docker/03_IMAGES_AND_DOCKERFILES.md), [`04_CONTAINERS.md`](../../en/docker/04_CONTAINERS.md)

### Docker
- **Définition** : plateforme qui construit, distribue et exécute des applications dans des **conteneurs**.
- **Usage** : l'outil central du projet. Le README doit expliquer *l'usage de Docker* dans ton projet.

### Docker Engine / démon `dockerd`
- **Définition** : le service d'arrière-plan qui fait réellement le travail (build, run, réseau, volumes). La commande `docker` n'est qu'un client qui lui parle via une socket.
- **Usage** : il doit tourner dans la VM. `systemctl status docker` pour vérifier.

### Image
- **Définition** : un modèle **en lecture seule**, empilement de couches (layers), contenant un système de fichiers et des métadonnées (commande de démarrage, ports, variables…).
- **Usage** : une image **par service**, construite **par toi** à partir d'un Dockerfile. Chaque image doit porter **le même nom que son service** (`nginx`, `wordpress`, `mariadb`).

### Conteneur (container)
- **Définition** : une instance **en cours d'exécution** d'une image : un processus isolé + une couche inscriptible par-dessus l'image.
- **Usage** : chaque service tourne dans **son propre conteneur dédié**. Le sujet le rappelle : *un conteneur n'est pas une VM*.

### Couche (layer)
- **Définition** : un diff de système de fichiers produit par une instruction du Dockerfile (`RUN`, `COPY`…). Les couches sont empilées via un **overlay filesystem**.
- **Usage** : explique le **cache de build** et pourquoi on regroupe les `RUN` (moins de couches, image plus légère).

### Couche inscriptible (writable layer)
- **Définition** : la couche propre à chaque conteneur où vont toutes ses écritures. Elle **disparaît** avec le conteneur.
- **Usage** : c'est *la raison* pour laquelle la base de données et les fichiers WordPress doivent être dans des **volumes**.

### Tag
- **Définition** : l'étiquette de version d'une image (`nom:tag`, ex. `debian:bookworm`).
- **Usage** : **le tag `latest` est interdit**. Il est mouvant : le même Dockerfile donnerait une image différente demain.

### `latest`
- **Définition** : le tag par défaut quand aucun n'est précisé — pas « la plus récente », juste « celle qu'on a taguée latest ».
- **Usage** : interdit, y compris implicitement (`FROM alpine` sans tag = `alpine:latest`).

### Registry / Docker Hub
- **Définition** : un serveur qui stocke et distribue des images. Docker Hub est le registry public par défaut.
- **Usage** : **interdit de tirer (`pull`) des images toutes faites** (pas d'`image: wordpress` ou `nginx:x`). Seules exceptions : les images de base Alpine/Debian.

### `pull` / `build`
- **Définition** : `pull` télécharge une image existante ; `build` en construit une depuis un Dockerfile.
- **Usage** : tu **build** tout. Le seul pull autorisé est celui implicite de ton `FROM alpine:X` / `debian:X`.

### Cache de build
- **Définition** : Docker réutilise une couche si l'instruction et son contexte n'ont pas changé.
- **Usage** : ordonne tes Dockerfiles du moins changeant (install des paquets) au plus changeant (copie des scripts) pour rebuilder vite.

### Contexte de build
- **Définition** : le dossier envoyé au démon au moment du `build` ; seuls ses fichiers sont accessibles à `COPY`.
- **Usage** : chaque `srcs/requirements/<service>/` est le contexte de son image.

### `.dockerignore`
- **Définition** : liste de fichiers à **exclure du contexte de build** (comme un `.gitignore`).
- **Usage** : présent dans chaque dossier de service dans l'arborescence d'exemple. Empêche d'embarquer des fichiers inutiles — ou sensibles — dans l'image.

---

## 3. Dockerfile — les instructions

> Approfondir : [`03_IMAGES_AND_DOCKERFILES.md`](../../en/docker/03_IMAGES_AND_DOCKERFILES.md)

### Dockerfile
- **Définition** : fichier texte de recettes qui décrit, instruction par instruction, comment construire une image.
- **Usage** : **un Dockerfile par service, écrit par toi**, appelé depuis le `docker-compose.yml`, lui-même lancé par le Makefile. **Aucun mot de passe** dedans.

| Instruction | Définition | Usage dans Inception |
|---|---|---|
| `FROM` | Image de base | `FROM alpine:3.21` / `debian:bookworm` — jamais sans tag |
| `RUN` | Exécute une commande **au build**, crée une couche | Installer les paquets |
| `COPY` | Copie un fichier du contexte dans l'image | Copier `conf/` et `tools/` |
| `ADD` | Comme `COPY` + URLs et archives | Préfère `COPY`, plus explicite |
| `WORKDIR` | Répertoire courant des instructions suivantes | `/var/www/html` pour WordPress |
| `EXPOSE` | **Documente** un port écouté (ne publie rien) | `9000`, `3306`, `443` |
| `ENV` | Variable d'env. figée dans l'image | Jamais pour un secret |
| `ARG` | Variable disponible **seulement au build** | Version de paquet, pas un mot de passe (visible dans `docker history`) |
| `ENTRYPOINT` | Programme lancé au démarrage | Ton script d'init (`tools/entrypoint.sh`) |
| `CMD` | Arguments par défaut (ou commande si pas d'ENTRYPOINT) | `["nginx", "-g", "daemon off;"]` |
| `USER` | Utilisateur pour les instructions suivantes / l'exécution | Optionnel, bon réflexe sécurité |
| `HEALTHCHECK` | Commande qui teste la santé du conteneur | Utile pour `depends_on: condition: service_healthy` |

### Forme exec vs forme shell
- **Définition** : `CMD ["nginx","-g","daemon off;"]` (exec, JSON) lance le programme **directement** ; `CMD nginx -g "daemon off;"` (shell) le lance via `/bin/sh -c`.
- **Usage** : la forme exec fait de ton programme **le PID 1** et lui permet de recevoir `SIGTERM`. La forme shell met `sh` en PID 1.

### Script d'entrypoint
- **Définition** : script shell exécuté au démarrage du conteneur pour préparer l'environnement (créer la base, installer WordPress…) avant de lancer le service.
- **Usage** : il **doit finir par `exec <service>`** pour céder le PID 1 au vrai démon. Interdit d'y mettre une boucle infinie.

---

## 4. Docker Compose

> Approfondir : [`07_COMPOSE.md`](../../en/docker/07_COMPOSE.md)

### Docker Compose (`docker compose`)
- **Définition** : outil qui décrit et pilote **une application multi-conteneurs** à partir d'un fichier YAML.
- **Usage** : obligatoire. `docker compose` (v2, plugin) — l'ancien `docker-compose` (v1, Python) est obsolète.

### `docker-compose.yml`
- **Définition** : le fichier YAML qui déclare services, réseaux, volumes, secrets.
- **Usage** : dans `srcs/`. Le Makefile l'appelle (`docker compose -f srcs/docker-compose.yml up -d --build`).

### YAML
- **Définition** : format de données lisible où **l'indentation (espaces, jamais de tabulations)** définit la structure.
- **Usage** : format du compose. Une indentation fausse = erreur de parsing.

### Service
- **Définition** : dans compose, la définition d'un conteneur (image, build, ports, volumes, réseaux…).
- **Usage** : `nginx`, `wordpress`, `mariadb` (+ bonus). Nom de service = nom d'image = **nom DNS** sur le réseau.

| Clé compose | Définition | Usage dans Inception |
|---|---|---|
| `build` | Chemin du contexte/Dockerfile | `build: requirements/nginx` |
| `image` | Nom donné à l'image construite | `image: nginx` — **même nom que le service** |
| `container_name` | Nom fixe du conteneur | Pratique pour `docker exec -it mariadb` |
| `ports` | **Publie** un port du conteneur sur l'hôte | Seulement `"443:443"` sur nginx |
| `expose` | Port accessible **sur le réseau interne seulement** | `9000`, `3306` |
| `networks` | Réseaux rejoints | **Obligatoire** : la ligne réseau doit apparaître |
| `volumes` | Montages | Volumes nommés uniquement |
| `env_file` | Charge un fichier de variables | `env_file: .env` |
| `environment` | Variables explicites | Non-secrètes uniquement |
| `secrets` | Secrets montés dans `/run/secrets/` | `db_password`, `db_root_password`… |
| `restart` | Politique de redémarrage | `unless-stopped` ou `always` |
| `depends_on` | Ordre de démarrage | `wordpress` dépend de `mariadb` |

### Politique de redémarrage (restart policy)
- **Définition** : règle qui dit à Docker quand relancer un conteneur arrêté : `no`, `on-failure`, `always`, `unless-stopped`.
- **Usage** : « *Your containers have to restart in case of a crash* » → `restart: unless-stopped` (ou `always`). Testable en défense en tuant le processus.

### `depends_on`
- **Définition** : ordonne le démarrage des conteneurs. Par défaut il attend que le conteneur **démarre**, pas qu'il soit **prêt**.
- **Usage** : WordPress doit attendre MariaDB. Combine-le avec un `healthcheck` (`condition: service_healthy`) ou une boucle d'attente bornée dans l'entrypoint (qui *attend* puis `exec` — pas une boucle infinie).

### Makefile
- **Définition** : fichier de règles lu par `make`, qui automatise des commandes.
- **Usage** : obligatoire **à la racine**. Il doit **tout monter** : créer `/home/<login>/data/...`, builder les images via compose, lancer la stack. Cibles habituelles : `all`, `down`, `clean`, `fclean`, `re`.

---

## 5. Processus, PID 1 & démons

> Approfondir : [`03_IMAGES_AND_DOCKERFILES.md`](../../en/docker/03_IMAGES_AND_DOCKERFILES.md) (section PID 1)

### Processus
- **Définition** : un programme en cours d'exécution, identifié par un PID.
- **Usage** : un conteneur **est** un processus (et ses enfants). Il vit exactement aussi longtemps que son PID 1.

### PID 1
- **Définition** : le premier processus d'un namespace PID (`init` sur une machine normale). Il a deux devoirs spéciaux : **recevoir les signaux** destinés au conteneur et **récolter les zombies**.
- **Usage** : le sujet dit « *Read about PID 1* ». Ton PID 1 doit être **le vrai service** (nginx, php-fpm, mysqld), lancé en avant-plan. Quand il meurt, le conteneur meurt — et la restart policy le relance.

### `exec` (shell)
- **Définition** : remplace le processus shell courant par le programme donné, **sans créer de nouveau processus** (même PID).
- **Usage** : la dernière ligne de chaque entrypoint : `exec php-fpm83 -F`. Le script disparaît, le service devient PID 1.

### Démon (daemon)
- **Définition** : un programme de service qui tourne en arrière-plan. Classiquement, il **se détache** (fork) du terminal.
- **Usage** : le piège central. Si nginx se « démonise », le PID 1 rend la main, le conteneur s'arrête. Il faut lancer chaque service **en avant-plan** : `nginx -g 'daemon off;'`, `php-fpm -F`, `mysqld` (ou `mariadbd`).

### Avant-plan / arrière-plan (foreground / background)
- **Définition** : en avant-plan, le processus garde la main ; en arrière-plan, il rend la main immédiatement.
- **Usage** : dans un conteneur, le service principal doit être en **avant-plan**.

### Hacky patches interdits
- **Définition** : des commandes qui maintiennent un conteneur artificiellement en vie : `tail -f`, `bash`, `sleep infinity`, `while true`.
- **Usage** : **interdites**, y compris dans l'entrypoint et ses scripts. Elles masquent le vrai service : s'il crashe, le conteneur reste « up » et le restart ne se déclenche jamais.

### Signal (`SIGTERM`, `SIGKILL`)
- **Définition** : message envoyé par le noyau à un processus. `docker stop` envoie `SIGTERM`, puis `SIGKILL` après 10 s.
- **Usage** : si ton PID 1 est un shell qui ne transmet pas `SIGTERM`, chaque `docker stop` prend 10 s et MariaDB est tuée sale (risque de corruption).

### Processus zombie
- **Définition** : un processus terminé dont le parent n'a pas lu le code de retour (`wait`).
- **Usage** : c'est au PID 1 de les récolter. Option : `init: true` dans compose (ajoute `tini`).

---

## 6. Réseau

> Approfondir : [`05_NETWORKING.md`](../../en/docker/05_NETWORKING.md)

### docker-network / réseau bridge
- **Définition** : un réseau virtuel privé créé par Docker ; les conteneurs qui y sont attachés se voient entre eux. `bridge` est le driver par défaut.
- **Usage** : **obligatoire**, déclaré dans le compose (`networks: inception: driver: bridge`). Les trois conteneurs y sont branchés.

### DNS interne de Docker
- **Définition** : sur un réseau défini par l'utilisateur, Docker résout **le nom d'un service** en l'IP de son conteneur.
- **Usage** : WordPress contacte la base à l'adresse `mariadb:3306`, nginx contacte `wordpress:9000`. Aucune IP en dur.

### Host network (`network_mode: host`)
- **Définition** : le conteneur partage directement la pile réseau de l'hôte, sans isolation.
- **Usage** : **interdit**. À comparer dans le README (*Docker Network vs Host Network*).

### `--link` / `links:`
- **Définition** : ancien mécanisme (obsolète) pour relier des conteneurs par nom.
- **Usage** : **interdit**. Le DNS du réseau bridge fait la même chose proprement.

### Port
- **Définition** : numéro (0–65535) qui identifie un service sur une adresse IP.
- **Usage** : **443** (HTTPS, nginx, seul ouvert vers l'extérieur), **9000** (php-fpm, interne), **3306** (MariaDB, interne).

### Publier un port (port mapping)
- **Définition** : `ports: "443:443"` relie un port de l'hôte à un port du conteneur, le rendant accessible de l'extérieur.
- **Usage** : **seul nginx publie un port**, et seulement 443. Ne pas publier 3306 ni 9000.

### Point d'entrée de l'infrastructure (entrypoint)
- **Définition** : *ici, au sens réseau* : la seule porte d'entrée vers ta stack depuis l'extérieur. (À ne pas confondre avec `ENTRYPOINT` du Dockerfile.)
- **Usage** : nginx est **l'unique** entrypoint, via 443, en TLS 1.2/1.3.

### Adresse IP locale / `localhost`
- **Définition** : `127.0.0.1` désigne la machine elle-même ; l'IP locale de la VM est son adresse sur le réseau.
- **Usage** : ton nom de domaine doit **pointer vers ton IP locale**.

### Nom de domaine / `/etc/hosts`
- **Définition** : un nom lisible qui se résout en IP. `/etc/hosts` est un fichier qui force cette résolution localement, avant le DNS.
- **Usage** : `login.42.fr` → ajoute `127.0.0.1 login.42.fr` dans `/etc/hosts` de la VM. La variable `DOMAIN_NAME` dans `.env` le porte.

---

## 7. Stockage

> Approfondir : [`06_STORAGE.md`](../../en/docker/06_STORAGE.md)

### Persistance
- **Définition** : le fait que des données **survivent** à l'arrêt/suppression du conteneur.
- **Usage** : la base et les fichiers du site doivent survivre à `docker compose down` puis `up`. Classique en défense.

### Volume (volume nommé / named volume)
- **Définition** : stockage géré par Docker, référencé par un **nom**, indépendant du cycle de vie des conteneurs.
- **Usage** : **deux volumes nommés obligatoires** : un pour la base WordPress (`/var/lib/mysql`), un pour les fichiers du site (`/var/www/html`).

### Bind mount
- **Définition** : on monte **directement un chemin de l'hôte** dans le conteneur (`./data:/var/lib/mysql`). Docker ne le gère pas.
- **Usage** : **interdit** pour ces deux volumes. À comparer dans le README (*Docker Volumes vs Bind Mounts*).

### `driver_opts` (driver `local`, `type: none`, `o: bind`)
- **Définition** : options du driver de volume `local` qui permettent à un volume **nommé** de stocker ses données à un emplacement précis de l'hôte.
- **Usage** : la façon de concilier « volume nommé » et « données dans `/home/<login>/data` » :
  ```yaml
  volumes:
    db:
      driver: local
      driver_opts: { type: none, o: bind, device: /home/login/data/mariadb }
  ```
  Le dossier doit exister **avant** (`mkdir -p` dans le Makefile).

### Point de montage (mount point)
- **Définition** : le chemin, dans le conteneur, où un volume apparaît.
- **Usage** : `/var/lib/mysql` (mariadb), `/var/www/html` (wordpress **et** nginx, qui partagent le même volume pour servir les fichiers statiques).

### `/home/login/data`
- **Définition** : le dossier de l'hôte imposé par le sujet.
- **Usage** : là où les deux volumes stockent réellement leurs données. Remplace `login` par le tien.

---

## 8. Secrets & configuration

> Approfondir : [`08_SECRETS_AND_ENV.md`](../../en/docker/08_SECRETS_AND_ENV.md)

### Variable d'environnement
- **Définition** : paire `NOM=valeur` transmise à un processus et lisible par lui (`getenv`, `$NOM`).
- **Usage** : **obligatoires** pour configurer les services (nom de domaine, nom de la base, utilisateur…).

### `.env`
- **Définition** : fichier de variables `CLE=valeur` lu par Docker Compose (substitution `${VAR}` dans le YAML, ou injecté via `env_file`).
- **Usage** : **obligatoire**, dans `srcs/`. Contient `DOMAIN_NAME`, `MYSQL_USER`, etc. **Ignoré par git**.

### Secret (Docker secrets)
- **Définition** : donnée sensible montée **comme un fichier** en lecture seule dans `/run/secrets/<nom>`, au lieu d'être une variable d'environnement.
- **Usage** : **fortement recommandé** pour les mots de passe. Fichiers dans `secrets/` à la racine (`db_password.txt`, `db_root_password.txt`, `credentials.txt`), déclarés dans le compose, lus par les scripts (`$(cat /run/secrets/db_password)`). Le README compare *Secrets vs Environment Variables* (une variable d'env. fuit dans `docker inspect`, les logs, les processus enfants).

### Credentials / API keys
- **Définition** : tout ce qui authentifie : mots de passe, clés d'API, jetons.
- **Usage** : **aucun dans le dépôt git** (hors secrets correctement configurés) → **échec immédiat du projet**. Aucun mot de passe dans un Dockerfile.

### `.gitignore`
- **Définition** : liste des fichiers que git ne doit pas suivre.
- **Usage** : doit contenir `.env` et `secrets/`. Vérifie avec `git status` *avant* le premier commit — un secret commité reste dans l'historique.

---

## 9. Web & TLS

> Approfondir : [`10_INCEPTION.md`](../../en/docker/10_INCEPTION.md)

### HTTP / HTTPS
- **Définition** : HTTP est le protocole du web (en clair, port 80). HTTPS = HTTP **chiffré par TLS** (port 443).
- **Usage** : seul HTTPS sur 443 est autorisé. Pas de port 80.

### TLS (Transport Layer Security)
- **Définition** : protocole qui chiffre et authentifie une connexion. Successeur de SSL (le nom « SSL » est resté dans les configs).
- **Usage** : **TLSv1.2 ou TLSv1.3 uniquement** → `ssl_protocols TLSv1.2 TLSv1.3;` dans nginx. Testable avec `openssl s_client -connect login.42.fr:443 -tls1_1` (doit échouer).

### Certificat (SSL/TLS) & clé privée
- **Définition** : le certificat prouve l'identité du serveur et contient sa clé publique ; la clé privée, secrète, permet de déchiffrer.
- **Usage** : nginx en a besoin pour HTTPS (`ssl_certificate`, `ssl_certificate_key`).

### Certificat auto-signé (self-signed)
- **Définition** : un certificat signé par sa propre clé, sans autorité de certification. Le navigateur affiche un avertissement.
- **Usage** : la norme pour Inception. Généré **au build ou au démarrage** avec `openssl req -x509 -nodes -newkey rsa:2048 ...` — jamais commité.

### OpenSSL
- **Définition** : boîte à outils cryptographique en ligne de commande.
- **Usage** : générer le certificat ; tester la version TLS.

### Serveur web
- **Définition** : programme qui reçoit des requêtes HTTP et renvoie des réponses (fichiers ou contenu généré).
- **Usage** : nginx est **le seul** serveur web de la stack.

### Reverse proxy
- **Définition** : serveur qui reçoit les requêtes des clients et les **relaie** à un serveur d'arrière-plan.
- **Usage** : nginx relaie les requêtes `.php` à php-fpm (`fastcgi_pass wordpress:9000;`) et sert lui-même les fichiers statiques.

### CGI / FastCGI
- **Définition** : protocoles par lesquels un serveur web délègue l'exécution d'un script à un autre programme. FastCGI garde des processus vivants au lieu d'en lancer un par requête.
- **Usage** : nginx ↔ php-fpm parlent **FastCGI** sur le port 9000. Ce n'est pas du HTTP : tu ne peux pas `curl` php-fpm.

---

## 10. Les services

### NGINX
- **Définition** : serveur web et reverse proxy léger et rapide.
- **Usage** : conteneur `nginx` : TLS 1.2/1.3, port 443, sert `/var/www/html`, transmet le PHP à `wordpress:9000`. Lancé avec `daemon off;`.

### WordPress
- **Définition** : CMS (système de gestion de contenu) écrit en PHP, qui stocke ses données dans une base MySQL/MariaDB.
- **Usage** : conteneur `wordpress` **avec php-fpm, sans nginx**. Doit être **installé et configuré** automatiquement au premier démarrage.

### PHP
- **Définition** : langage de script côté serveur, dans lequel WordPress est écrit.
- **Usage** : installé dans le conteneur wordpress (+ extensions : `mysqli`, etc.).

### php-fpm (FastCGI Process Manager)
- **Définition** : implémentation FastCGI de PHP : un pool de processus PHP qui exécutent les scripts envoyés par le serveur web.
- **Usage** : PID 1 du conteneur wordpress, en avant-plan (`php-fpm -F`), écoute sur `0.0.0.0:9000` (à configurer dans `www.conf`, par défaut il écoute sur `127.0.0.1` → injoignable depuis nginx).

### WP-CLI
- **Définition** : outil en ligne de commande pour installer et administrer WordPress (`wp core download`, `wp config create`, `wp core install`, `wp user create`).
- **Usage** : le moyen standard d'automatiser l'installation dans l'entrypoint, sans passer par le formulaire web.

### `wp-config.php`
- **Définition** : fichier de configuration de WordPress (connexion à la base, clés de sel…).
- **Usage** : généré par WP-CLI avec `DB_HOST=mariadb:3306` et les identifiants lus depuis les secrets.

### Utilisateurs WordPress / administrateur
- **Définition** : les comptes du site ; l'administrateur a tous les droits.
- **Usage** : **deux utilisateurs** obligatoires dont **un admin**, dont le nom **ne doit pas contenir** `admin`/`Admin`/`administrator`/`Administrator`.

### Panneau d'administration (`wp-admin`)
- **Définition** : l'interface de gestion de WordPress, à `https://login.42.fr/wp-admin`.
- **Usage** : à documenter dans `USER_DOC.md` (comment y accéder).

### MariaDB
- **Définition** : SGBD relationnel (base de données SQL), fork libre et compatible de MySQL.
- **Usage** : conteneur `mariadb` **sans nginx**, écoute sur 3306, données dans `/var/lib/mysql` (volume). Lancé en avant-plan (`mysqld` / `mariadbd`).

### Base de données / SGBD / SQL
- **Définition** : une base stocke des données structurées en tables ; le SGBD est le logiciel qui la gère ; SQL est le langage pour l'interroger.
- **Usage** : l'entrypoint crée la base WordPress et son utilisateur (`CREATE DATABASE`, `CREATE USER`, `GRANT`) au premier démarrage seulement.

### Utilisateur root (MariaDB)
- **Définition** : le super-utilisateur du SGBD.
- **Usage** : mot de passe dans `secrets/db_root_password.txt`. WordPress utilise un **utilisateur dédié** (`db_password.txt`), jamais root.

### `bind-address`
- **Définition** : l'adresse IP sur laquelle MariaDB accepte les connexions.
- **Usage** : par défaut `127.0.0.1` → WordPress ne peut pas se connecter. Mettre `0.0.0.0` dans `conf/`.

### Idempotence (initialisation)
- **Définition** : une opération qu'on peut rejouer sans changer le résultat.
- **Usage** : tes entrypoints doivent détecter si la base / WordPress existe déjà (fichiers dans le volume) et **ne pas réinstaller** à chaque redémarrage.

---

## 11. Bonus

### Redis (cache)
- **Définition** : base de données clé-valeur **en mémoire**, utilisée comme cache.
- **Usage** : conteneur dédié, branché à WordPress (plugin *Redis Object Cache*) pour éviter de requêter MariaDB à chaque page.

### FTP (File Transfer Protocol) / serveur FTP
- **Définition** : protocole de transfert de fichiers (port 21 + ports passifs). Serveur courant : `vsftpd`.
- **Usage** : conteneur pointant sur **le volume WordPress**, pour déposer/modifier les fichiers du site.

### Site statique
- **Définition** : site composé de fichiers fixes (HTML/CSS/JS), sans code exécuté côté serveur.
- **Usage** : bonus **dans n'importe quel langage sauf PHP** (vitrine, CV).

### Adminer
- **Définition** : interface web de gestion de base de données, en un seul fichier PHP.
- **Usage** : conteneur pour administrer MariaDB depuis le navigateur.

### Service au choix
- **Définition** : n'importe quel service utile (monitoring type cAdvisor, Uptime Kuma, etc.).
- **Usage** : à **justifier en défense**. Les bonus peuvent ouvrir d'autres ports. Évalués **seulement si le mandatory est parfait**.

---

## 12. Rendu & évaluation

### Arborescence imposée
- **Définition** : la structure attendue du dépôt.
- **Usage** :
  ```
  Makefile
  secrets/      credentials.txt, db_password.txt, db_root_password.txt
  srcs/         docker-compose.yml, .env, requirements/
  srcs/requirements/<service>/   Dockerfile, .dockerignore, conf/, tools/
  ```
  `conf/` = fichiers de configuration (nginx.conf, www.conf, 50-server.cnf) ; `tools/` = scripts (entrypoints).

### `README.md`
- **Définition** : documentation d'entrée du dépôt.
- **Usage** : **en anglais**, première ligne en italique (*This project has been created as part of the 42 curriculum by <login>*), sections **Description**, **Instructions**, **Resources** (dont l'usage de l'IA), **Project description** avec les 4 comparaisons : VM vs Docker, Secrets vs Env Vars, Docker Network vs Host Network, Docker Volumes vs Bind Mounts.

### `USER_DOC.md`
- **Définition** : documentation utilisateur / administrateur.
- **Usage** : services fournis, démarrer/arrêter, accéder au site et à l'admin, gérer les credentials, vérifier que tout tourne.

### `DEV_DOC.md`
- **Définition** : documentation développeur.
- **Usage** : installer l'environnement de zéro, build/lancer via Makefile + Compose, commandes de gestion des conteneurs/volumes, où sont stockées les données et comment elles persistent.

### Markdown (`.md`)
- **Définition** : format de texte léger avec balisage simple (`#`, `**`, listes…).
- **Usage** : format imposé des trois documents.

### Peer-evaluation / défense / modification en direct
- **Définition** : correction par un·e pair ; une petite modification du projet peut t'être demandée sur le moment.
- **Usage** : prépare-toi à changer un port, un nom, une variable en quelques minutes — d'où l'importance de comprendre chaque ligne. Voir [`13_DEFENSE.md`](../../en/docker/13_DEFENSE.md).

---

## 🧰 Commandes utiles (aide-mémoire)

| Commande | Rôle |
|---|---|
| `docker compose -f srcs/docker-compose.yml up -d --build` | Builder et lancer la stack en arrière-plan |
| `docker compose -f srcs/docker-compose.yml down` | Arrêter et supprimer conteneurs + réseau (volumes conservés) |
| `docker compose ... down -v` | Idem **+ suppression des volumes** |
| `docker ps` / `docker ps -a` | Conteneurs en cours / tous |
| `docker images` | Images locales (vérifier les noms) |
| `docker logs -f <c>` | Suivre les logs d'un conteneur |
| `docker exec -it <c> sh` | Ouvrir un shell dans un conteneur en cours |
| `docker inspect <c>` | Toute la config d'un conteneur (réseau, montages, restart…) |
| `docker network ls` / `inspect` | Réseaux et conteneurs attachés |
| `docker volume ls` / `inspect` | Volumes et leur `device` sur l'hôte |
| `docker system prune -a --volumes` | Tout nettoyer (⚠️ destructif) |
| `openssl s_client -connect login.42.fr:443 -tls1_2` | Tester la version TLS |
| `docker exec -it mariadb mariadb -u root -p` | Ouvrir la console SQL |
