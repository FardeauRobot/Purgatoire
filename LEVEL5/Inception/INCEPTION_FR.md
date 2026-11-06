# Inception

> **Résumé :** ce document est un exercice d'administration système.
>
> Version : 5.3

*Traduction française non officielle du sujet `INCEPTION.pdf`. En cas de doute, c'est le PDF anglais qui fait foi.*

## Sommaire

1. Préambule
2. Introduction
3. Consignes générales
4. Instructions relatives à l'IA
5. Partie obligatoire
6. Exigences pour le README
7. Prérequis pour la validation
8. Partie bonus
9. Rendu et évaluation par les pairs

---

## Chapitre I — Préambule

*(Page vide dans le sujet original.)*

---

## Chapitre II — Introduction

Ce projet a pour but d'élargir tes connaissances en administration système à travers l'utilisation de Docker.
Tu vas virtualiser plusieurs images Docker, en les créant dans ta nouvelle machine virtuelle personnelle.

---

## Chapitre III — Consignes générales

- Ce projet doit être réalisé sur une **machine virtuelle**.
- Tous les fichiers nécessaires à la configuration de ton projet doivent être placés dans un dossier **`srcs`**.
- Un **`Makefile`** est également requis et doit se trouver à la racine de ton dépôt. Il doit mettre en place toute ton application (c'est-à-dire qu'il doit construire les images Docker à l'aide de `docker-compose.yml`).
- Ce sujet demande de mettre en pratique des notions que, selon ton parcours, tu n'as peut-être pas encore apprises. On te conseille donc de ne pas hésiter à lire beaucoup de documentation sur l'utilisation de Docker, ainsi que tout ce qui pourra t'être utile pour mener à bien ce projet.

---

## Chapitre IV — Instructions relatives à l'IA

### ● Contexte

Au cours de ton apprentissage, l'IA peut t'aider dans de nombreuses tâches. Prends le temps d'explorer les différentes capacités des outils d'IA et la façon dont ils peuvent soutenir ton travail. Aborde-les toutefois toujours avec prudence et évalue les résultats de manière critique. Qu'il s'agisse de code, de documentation, d'idées ou d'explications techniques, tu ne peux jamais être totalement sûr que ta question était bien formulée ni que le contenu généré est exact. Tes pairs sont une ressource précieuse pour t'aider à éviter les erreurs et les angles morts.

### ● Message principal

- ☛ Utilise l'IA pour réduire les tâches répétitives ou fastidieuses.
- ☛ Développe des compétences en *prompting* — liées au code ou non — qui serviront ta future carrière.
- ☛ Apprends comment fonctionnent les systèmes d'IA pour mieux anticiper et éviter les risques, biais et problèmes éthiques courants.
- ☛ Continue de développer tes compétences techniques et tes *power skills* en travaillant avec tes pairs.
- ☛ N'utilise que du contenu généré par l'IA que tu comprends entièrement et dont tu peux assumer la responsabilité.

### ● Règles pour l'apprenant

- Tu dois prendre le temps d'explorer les outils d'IA et de comprendre leur fonctionnement, afin de les utiliser de manière éthique et de réduire les biais potentiels.
- Tu dois réfléchir à ton problème avant de *prompter* — cela t'aide à écrire des prompts plus clairs, plus détaillés et plus pertinents, avec un vocabulaire précis.
- Tu dois prendre l'habitude de vérifier, relire, remettre en question et tester systématiquement tout ce qui est généré par l'IA.
- Tu dois toujours chercher la relecture d'un pair — ne te repose pas uniquement sur ta propre validation.

### ● Objectifs de la phase

- Développer des compétences en *prompting*, à la fois générales et spécifiques à un domaine.
- Augmenter ta productivité grâce à une utilisation efficace des outils d'IA.
- Continuer à renforcer la pensée computationnelle, la résolution de problèmes, l'adaptabilité et la collaboration.

### ● Commentaires et exemples

- Tu rencontreras régulièrement des situations — examens, évaluations, etc. — où tu devras démontrer une réelle compréhension. Sois prêt, continue à développer tes compétences techniques et relationnelles.
- Expliquer ton raisonnement et débattre avec tes pairs révèle souvent des lacunes dans ta compréhension. Fais de l'apprentissage entre pairs une priorité.
- Les outils d'IA manquent souvent de ton contexte spécifique et ont tendance à fournir des réponses génériques. Tes pairs, qui partagent ton environnement, peuvent t'apporter des éclairages plus pertinents et plus précis.
- Là où l'IA a tendance à générer la réponse la plus probable, tes pairs peuvent t'offrir d'autres points de vue et des nuances précieuses. Appuie-toi sur eux comme point de contrôle qualité.

✓ **Bonne pratique :**
> Je demande à l'IA : « Comment tester une fonction de tri ? » Elle me donne quelques idées. Je les essaie et je passe en revue les résultats avec un pair. Nous affinons l'approche ensemble.

✗ **Mauvaise pratique :**
> Je demande à l'IA d'écrire une fonction entière, je la copie-colle dans mon projet. Pendant l'évaluation, je suis incapable d'expliquer ce qu'elle fait ni pourquoi. Je perds en crédibilité — et j'échoue au projet.

✓ **Bonne pratique :**
> J'utilise l'IA pour m'aider à concevoir un parseur. Ensuite, je parcours la logique avec un pair. Nous repérons deux bugs et le réécrivons ensemble — mieux, plus propre, et entièrement compris.

✗ **Mauvaise pratique :**
> Je laisse Copilot générer le code d'une partie clé de mon projet. Ça compile, mais je suis incapable d'expliquer comment il gère les pipes. Pendant l'évaluation, je n'arrive pas à me justifier et j'échoue au projet.

---

## Chapitre V — Partie obligatoire

Ce projet consiste à te faire mettre en place une petite infrastructure composée de différents services, en respectant des règles précises. L'ensemble du projet doit être réalisé dans une machine virtuelle. Tu dois utiliser **docker compose**.

Chaque image Docker doit porter **le même nom que le service** correspondant.
Chaque service doit tourner dans un **conteneur dédié**.
Pour des raisons de performance, les conteneurs doivent être construits à partir de **l'avant-dernière version stable d'Alpine ou de Debian**. Le choix t'appartient.
Tu dois aussi écrire **tes propres Dockerfiles**, un par service. Les Dockerfiles doivent être appelés dans ton `docker-compose.yml` par ton `Makefile`.
Cela signifie que tu dois construire toi-même les images Docker de ton projet. Il est donc **interdit de récupérer des images Docker toutes faites**, ainsi que d'utiliser des services comme DockerHub (Alpine/Debian étant exclues de cette règle).

Tu dois ensuite mettre en place :

- Un conteneur Docker contenant **NGINX avec TLSv1.2 ou TLSv1.3 uniquement**.
- Un conteneur Docker contenant **WordPress + php-fpm** (il doit être installé et configuré) uniquement, **sans nginx**.
- Un conteneur Docker contenant **MariaDB** uniquement, **sans nginx**.
- Un **volume** contenant ta base de données WordPress.
- Un **second volume** contenant les fichiers de ton site WordPress.
- Tu dois utiliser des **volumes nommés Docker** pour ces deux stockages persistants. Les *bind mounts* ne sont pas autorisés pour ces volumes.
- Les deux volumes nommés doivent stocker leurs données dans **`/home/login/data`** sur la machine hôte. Remplace « login » par ton nom d'utilisateur 42.
- Un **docker-network** qui établit la connexion entre tes conteneurs.

Tes conteneurs doivent **redémarrer en cas de crash**.

> ℹ️ Un conteneur Docker n'est pas une machine virtuelle. Il n'est donc pas recommandé d'utiliser des bidouilles basées sur `tail -f` ou des méthodes similaires pour le faire tourner. Renseigne-toi sur le fonctionnement des *daemons* et demande-toi si c'est une bonne idée de les utiliser ou non.

> ⚠️ Bien entendu, l'utilisation de `network: host`, de `--link` ou de `links:` est interdite.
> La ligne `network` doit être présente dans ton fichier `docker-compose.yml`.
> Tes conteneurs ne doivent pas être lancés avec une commande exécutant une boucle infinie. Cela s'applique donc aussi à toute commande utilisée comme *entrypoint* ou utilisée dans des scripts d'*entrypoint*. Voici quelques bidouilles interdites : `tail -f`, `bash`, `sleep infinity`, `while true`.

> ℹ️ Renseigne-toi sur le **PID 1** et sur les bonnes pratiques d'écriture des Dockerfiles.

- Dans ta base de données WordPress, il doit y avoir **deux utilisateurs**, dont l'un est l'administrateur. Le nom d'utilisateur de l'administrateur **ne peut pas contenir** `admin`/`Admin` ni `administrator`/`Administrator` (ex. : `admin`, `administrator`, `Administrator`, `admin-123`, etc.).

> ℹ️ Tes volumes seront accessibles dans le dossier `/home/login/data` de la machine hôte utilisant Docker. Bien entendu, tu dois remplacer `login` par le tien.

Pour simplifier les choses, tu dois configurer ton nom de domaine pour qu'il pointe vers ton adresse IP locale.
Ce nom de domaine doit être **`login.42.fr`**. Encore une fois, tu dois utiliser ton propre login.
Par exemple, si ton login est `wil`, `wil.42.fr` redirigera vers l'adresse IP pointant vers le site de wil.

> ⚠️ Le tag **`latest`** est interdit.
> **Aucun mot de passe** ne doit être présent dans tes Dockerfiles.
> L'utilisation de **variables d'environnement** est obligatoire.
> Il est également obligatoire d'utiliser un fichier **`.env`** pour stocker les variables d'environnement. Il est fortement recommandé d'utiliser les **Docker secrets** pour stocker toute information confidentielle. Tout identifiant, clé d'API ou mot de passe trouvé dans ton dépôt Git (en dehors de secrets correctement configurés) entraînera **l'échec du projet**.
> Ton conteneur NGINX doit être **l'unique point d'entrée** de ton infrastructure, via le **port 443 uniquement**, en utilisant le protocole TLSv1.2 ou TLSv1.3.

Voici un exemple de schéma du résultat attendu :

```
                                                    WWW (Internet)
                                                          │
                                                        [443]
┌─ Ordinateur HÔTE ───────────────────────────────────────┼──────┐
│                                                         │      │
│  ┌─ Réseau Docker ──────────────────────────────────────┼───┐  │
│  │                                                      │   │  │
│  │  ┌───────────┐ [3306] ┌───────────────┐ [9000] ┌─────┴─┐ │  │
│  │  │ Conteneur │◄──────►│   Conteneur   │◄──────►│Conten.│ │  │
│  │  │    DB     │        │ WordPress+PHP │        │ NGINX │ │  │
│  │  └─────┬─────┘        └───────┬───────┘        └───┬───┘ │  │
│  └────────┼──────────────────────┼────────────────────┼─────┘  │
│           │                      │                    │        │
│     ( Volume DB )       ( Volume WordPress ) ◄────────┘        │
│                                                                │
└────────────────────────────────────────────────────────────────┘

Légende : ┌──┐ image Docker   ( ) volume   ◄──► lien réseau   [n] port
```

*(Le volume WordPress est partagé entre le conteneur WordPress+PHP et le conteneur NGINX.)*

Voici un exemple de l'arborescence attendue :

```
$> ls -alR
total XX
drwxrwxr-x 3 wil wil 4096 avril 42 20:42 .
drwxrwxrwt 17 wil wil 4096 avril 42 20:42 ..
-rw-rw-r-- 1 wil wil XXXX avril 42 20:42 Makefile
drwxrwxr-x 3 wil wil 4096 avril 42 20:42 secrets
drwxrwxr-x 3 wil wil 4096 avril 42 20:42 srcs

./secrets:
total XX
drwxrwxr-x 2 wil wil 4096 avril 42 20:42 .
drwxrwxr-x 6 wil wil 4096 avril 42 20:42 ..
-rw-r--r-- 1 wil wil XXXX avril 42 20:42 credentials.txt
-rw-r--r-- 1 wil wil XXXX avril 42 20:42 db_password.txt
-rw-r--r-- 1 wil wil XXXX avril 42 20:42 db_root_password.txt

./srcs:
total XX
drwxrwxr-x 3 wil wil 4096 avril 42 20:42 .
drwxrwxr-x 3 wil wil 4096 avril 42 20:42 ..
-rw-rw-r-- 1 wil wil XXXX avril 42 20:42 docker-compose.yml
-rw-rw-r-- 1 wil wil XXXX avril 42 20:42 .env
drwxrwxr-x 5 wil wil 4096 avril 42 20:42 requirements

./srcs/requirements:
total XX
drwxrwxr-x 5 wil wil 4096 avril 42 20:42 .
drwxrwxr-x 3 wil wil 4096 avril 42 20:42 ..
drwxrwxr-x 4 wil wil 4096 avril 42 20:42 bonus
drwxrwxr-x 4 wil wil 4096 avril 42 20:42 mariadb
drwxrwxr-x 4 wil wil 4096 avril 42 20:42 nginx
drwxrwxr-x 4 wil wil 4096 avril 42 20:42 tools
drwxrwxr-x 4 wil wil 4096 avril 42 20:42 wordpress

./srcs/requirements/mariadb:
total XX
drwxrwxr-x 4 wil wil 4096 avril 42 20:45 .
drwxrwxr-x 5 wil wil 4096 avril 42 20:42 ..
drwxrwxr-x 2 wil wil 4096 avril 42 20:42 conf
-rw-rw-r-- 1 wil wil XXXX avril 42 20:42 Dockerfile
-rw-rw-r-- 1 wil wil XXXX avril 42 20:42 .dockerignore
drwxrwxr-x 2 wil wil 4096 avril 42 20:42 tools
[...]
./srcs/requirements/nginx:
total XX
drwxrwxr-x 4 wil wil 4096 avril 42 20:42 .
drwxrwxr-x 5 wil wil 4096 avril 42 20:42 ..
drwxrwxr-x 2 wil wil 4096 avril 42 20:42 conf
-rw-rw-r-- 1 wil wil XXXX avril 42 20:42 Dockerfile
-rw-rw-r-- 1 wil wil XXXX avril 42 20:42 .dockerignore
drwxrwxr-x 2 wil wil 4096 avril 42 20:42 tools
[...]

$> cat srcs/.env
DOMAIN_NAME=wil.42.fr
# MYSQL SETUP
MYSQL_USER=XXXXXXXXXXXX
[...]
$>
```

> ⚠️ Pour des raisons évidentes de sécurité, tous les identifiants, clés d'API, mots de passe, etc. doivent être enregistrés **localement**, de différentes manières / dans différents fichiers, et **ignorés par git**. Des identifiants stockés publiquement te mèneront directement à l'échec du projet.

> ℹ️ Tu peux stocker tes variables (comme un nom de domaine) dans un fichier de variables d'environnement comme `.env`.

---

## Chapitre VI — Exigences pour le README

Un fichier **`README.md`** doit être fourni à la racine de ton dépôt Git. Son but est de permettre à toute personne ne connaissant pas le projet (pairs, staff, recruteurs, etc.) de comprendre rapidement de quoi il s'agit, comment le lancer, et où trouver plus d'informations sur le sujet.

Le `README.md` doit contenir au minimum :

- **La toute première ligne** doit être en *italique* et indiquer : *This project has been created as part of the 42 curriculum by \<login1\>[, \<login2\>[, \<login3\>[...]]].*
- Une section **« Description »** qui présente clairement le projet, y compris son objectif et un bref aperçu.
- Une section **« Instructions »** contenant toutes les informations utiles sur la compilation, l'installation et/ou l'exécution.
- Une section **« Resources »** listant les références classiques liées au sujet (documentation, articles, tutoriels, etc.), ainsi qu'une description de la façon dont l'IA a été utilisée — en précisant pour quelles tâches et quelles parties du projet.

➠ Des sections supplémentaires peuvent être exigées selon le projet (ex. : exemples d'utilisation, liste de fonctionnalités, choix techniques, etc.).

Tout ajout requis est explicitement listé ci-dessous.

- Une section **description du projet** doit également expliquer l'utilisation de Docker et les sources incluses dans le projet. Elle doit indiquer les principaux choix de conception, ainsi qu'une comparaison entre :
  - Machines virtuelles vs Docker
  - Secrets vs variables d'environnement
  - Réseau Docker vs réseau de l'hôte (*host network*)
  - Volumes Docker vs *bind mounts*

> ℹ️ Ton README doit être rédigé **en anglais**.

---

## Chapitre VII — Prérequis pour la validation

En plus des exigences existantes, les fichiers de documentation suivants doivent être présents à la racine de ton dépôt. Ils doivent être rédigés au format Markdown (`.md`).

- **`USER_DOC.md` — Documentation utilisateur.** Ce fichier doit expliquer, en termes clairs et simples, comment un utilisateur final ou un administrateur peut :
  - Comprendre quels services sont fournis par la stack.
  - Démarrer et arrêter le projet.
  - Accéder au site web et au panneau d'administration.
  - Trouver et gérer les identifiants.
  - Vérifier que les services fonctionnent correctement.

- **`DEV_DOC.md` — Documentation développeur.** Ce fichier doit décrire comment un développeur peut :
  - Mettre en place l'environnement à partir de zéro (prérequis, fichiers de configuration, secrets).
  - Construire et lancer le projet à l'aide du Makefile et de Docker Compose.
  - Utiliser les commandes pertinentes pour gérer les conteneurs et les volumes.
  - Identifier où les données du projet sont stockées et comment elles persistent.

---

## Chapitre VIII — Partie bonus

Pour ce projet, la partie bonus se veut simple.

Un Dockerfile doit être écrit pour chaque service supplémentaire. Ainsi, chaque service tournera dans son propre conteneur et aura, si nécessaire, son volume dédié.

Liste des bonus :

- Mettre en place un cache **redis** pour ton site WordPress afin de gérer correctement le cache.
- Mettre en place un conteneur de **serveur FTP** pointant vers le volume de ton site WordPress.
- Créer un **site statique simple** dans le langage de ton choix, sauf PHP (oui, PHP est exclu). Par exemple, un site vitrine ou un site présentant ton CV.
- Mettre en place **Adminer**.
- Mettre en place **un service de ton choix** que tu juges utile. Pendant la soutenance, tu devras justifier ce choix.

> ℹ️ Pour réaliser la partie bonus, tu as la possibilité de mettre en place des services supplémentaires. Dans ce cas, tu peux ouvrir d'autres ports selon tes besoins.

> ⚠️ La partie bonus ne sera évaluée que si la partie obligatoire est **parfaitement** réalisée. « Parfaitement » signifie que la partie obligatoire a été entièrement réalisée et fonctionne sans aucun dysfonctionnement. Si tu n'as pas rempli TOUTES les exigences obligatoires, ta partie bonus ne sera pas évaluée du tout.

---

## Chapitre IX — Rendu et évaluation par les pairs

Rends ton travail sur ton dépôt Git comme d'habitude. Seul le travail présent dans ton dépôt sera évalué pendant la soutenance. N'hésite pas à vérifier deux fois les noms de tes dossiers et fichiers pour t'assurer qu'ils sont corrects.

Pendant l'évaluation, une **brève modification du projet** peut occasionnellement être demandée. Il peut s'agir d'un petit changement de comportement, de quelques lignes de code à écrire ou réécrire, ou d'une fonctionnalité facile à ajouter.

Même si cette étape ne s'applique pas forcément à tous les projets, tu dois y être préparé si elle est mentionnée dans les consignes d'évaluation.

Cette étape sert à vérifier ta compréhension réelle d'une partie précise du projet. La modification peut être réalisée dans l'environnement de développement de ton choix (ex. : ton environnement habituel), et elle doit être faisable en quelques minutes — sauf si une durée précise est définie dans le cadre de l'évaluation.
On peut par exemple te demander de faire une petite mise à jour d'une fonction ou d'un script, de modifier un affichage, ou d'adapter une structure de données pour stocker une nouvelle information, etc.

Les détails (périmètre, cible, etc.) seront précisés dans les consignes d'évaluation et peuvent varier d'une évaluation à l'autre pour un même projet.
