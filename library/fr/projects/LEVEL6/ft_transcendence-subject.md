# ft_transcendence — Sujet

> 🇬🇧 [English version](../../../en/projects/LEVEL6/ft_transcendence-subject.md)

> *« Surprise. »*
>
> Ce projet consiste à te lancer dans des tâches que tu n'as jamais faites.
> Souviens-toi du début de ton parcours en informatique.
> Regarde-toi maintenant : c'est ton moment de briller !

Traduction de la réécriture du sujet officiel (**version 21.1**), tirée de `LEVEL6/Transcendence/TRANSCENDENCE.pdf`. Le fond est fidèle ; seules la mise en page et la langue changent. **Le PDF (en anglais) fait foi.** Les noms de technologies et les termes techniques usuels (framework, backend, WebSockets, RAG…) restent en anglais.

---

## En bref

| | |
|---|---|
| **Équipe** | 4 à 5 personnes |
| **Exigé** | Partie obligatoire + **14 points de modules** (Majeur = 2 pts, Mineur = 1 pt) |
| **Bonus** | Modules au-delà des 14 pts, **5 points de bonus maximum** |
| **Stack** | Application web : frontend + backend + base de données, conteneurisée, lancée en une seule commande |
| **Navigateur** | Dernière version stable de Google Chrome, zéro warning ni erreur dans la console |
| **Rejet immédiat** | Exigences générales non respectées, Politique de confidentialité ou CGU absentes ou bidon |

**Sommaire**

1. [Instructions sur l'IA](#i--instructions-sur-lia)
2. [Préambule](#ii--préambule)
3. [Partie obligatoire](#iii--partie-obligatoire)
4. [Modules](#iv--modules) — avec un [tableau complet des points](#catalogue-des-modules-en-un-coup-dœil)
5. [Idées et exemples de projets](#v--idées-et-exemples-de-projets)
6. [Exigences du README](#vi--exigences-du-readme)
7. [Partie bonus](#vii--partie-bonus)
8. [Rendu et évaluation par les pairs](#viii--rendu-et-évaluation-par-les-pairs)

---

## I — Instructions sur l'IA

### Contexte

Au fil de ton apprentissage, l'IA peut t'aider sur beaucoup de tâches différentes. Prends le temps d'explorer ce que savent faire les outils d'IA et comment ils peuvent soutenir ton travail. Mais aborde-les toujours avec prudence et évalue les résultats de façon critique. Qu'il s'agisse de code, de documentation, d'idées ou d'explications techniques, tu ne peux jamais être complètement sûr que ta question était bien posée ni que le contenu généré est exact. Tes pairs sont une ressource précieuse pour éviter les erreurs et les angles morts.

### Message principal

- Utilise l'IA pour réduire les tâches répétitives ou fastidieuses.
- Développe des compétences de prompting — liées au code ou non — qui serviront ta future carrière.
- Apprends comment fonctionnent les systèmes d'IA pour mieux anticiper et éviter les risques, biais et problèmes éthiques courants.
- Continue à développer tes compétences techniques et humaines en travaillant avec tes pairs.
- N'utilise que du contenu généré par IA que tu comprends entièrement et dont tu peux assumer la responsabilité.

### Règles pour l'apprenant

- Prends le temps d'explorer les outils d'IA et de comprendre leur fonctionnement, pour les utiliser de façon éthique et réduire les biais potentiels.
- Réfléchis à ton problème avant de prompter — cela t'aide à écrire des prompts plus clairs, plus détaillés et plus pertinents, avec un vocabulaire précis.
- Prends l'habitude de vérifier, relire, questionner et tester systématiquement tout ce que l'IA génère.
- Cherche toujours une relecture par un pair — ne te repose pas uniquement sur ta propre validation.

### Objectifs de la phase

- Développer des compétences de prompting générales et spécifiques à un domaine.
- Augmenter ta productivité grâce à un usage efficace des outils d'IA.
- Continuer à renforcer la pensée computationnelle, la résolution de problèmes, l'adaptabilité et la collaboration.

### Commentaires et exemples

- Tu rencontreras régulièrement des situations — examens, évaluations, etc. — où tu devras démontrer une vraie compréhension. Sois prêt, continue à développer tes compétences techniques et relationnelles.
- Expliquer ton raisonnement et débattre avec tes pairs révèle souvent des lacunes dans ta compréhension. Fais de l'apprentissage entre pairs une priorité.
- Les outils d'IA manquent souvent de ton contexte précis et ont tendance à donner des réponses génériques. Tes pairs, qui partagent ton environnement, peuvent apporter des éclairages plus pertinents et plus justes.
- Là où l'IA a tendance à générer la réponse la plus probable, tes pairs peuvent apporter d'autres perspectives et des nuances précieuses. Appuie-toi sur eux comme point de contrôle qualité.

> [!success] Bonne pratique
> Je demande à l'IA : « Comment tester une fonction de tri ? » Elle me donne quelques idées. Je les essaie et je passe les résultats en revue avec un pair. On affine l'approche ensemble.

> [!failure] Mauvaise pratique
> Je demande à l'IA d'écrire une fonction entière et je la copie-colle dans mon projet. En évaluation, je suis incapable d'expliquer ce qu'elle fait ni pourquoi. Je perds ma crédibilité — et j'échoue au projet.

> [!success] Bonne pratique
> J'utilise l'IA pour m'aider à concevoir un parser. Ensuite je déroule la logique avec un pair. On repère deux bugs et on le réécrit ensemble — mieux, plus propre, et entièrement compris.

> [!failure] Mauvaise pratique
> Je laisse Copilot générer le code d'une partie clé de mon projet. Ça compile, mais je suis incapable d'expliquer comment il gère les pipes. En évaluation, je ne sais pas le justifier et j'échoue au projet.

---

## II — Préambule

Tout d'abord, félicitations d'être arrivé jusqu'ici ! Tu entres dans le dernier projet de ton Tronc Commun (Common Core), et oui, ce ne sera pas facile.

Transcendence est un **projet de groupe (4 à 5 personnes)**, pensé pour renforcer ta créativité, ta confiance en toi, ton adaptabilité aux nouvelles technologies et ton travail d'équipe. Vous allez créer en équipe une vraie application web, qui peut partir dans de nombreuses directions selon les modules et les choix que vous ferez. Prenez le temps d'y réfléchir ensemble avant de commencer.

Le projet est divisé en deux parties :

- **La partie obligatoire** — le socle fixe du projet, auquel chaque membre de l'équipe doit contribuer.
- **Un ensemble de modules** — que vous choisissez, et qui comptent dans la note finale.

> [!warning]
> C'est un long projet de groupe. De mauvais choix au départ et un manque de coordination coûteront beaucoup de temps. La gestion du projet et de l'équipe aura un fort impact sur vos résultats. **Tous les membres de l'équipe doivent participer activement et contribuer à la fois à la partie obligatoire et aux modules.**

### II.1 Organisation de l'équipe et gestion de projet

Comme c'est un projet de groupe, une bonne organisation de l'équipe est cruciale. Vous devez établir des rôles et des responsabilités clairs dès le départ.

#### II.1.1 Rôles obligatoires dans l'équipe

Votre équipe doit attribuer les rôles suivants (une même personne peut cumuler plusieurs rôles si l'équipe compte 4 membres) :

- **Product Owner (PO)** — définit la vision du produit, priorise les fonctionnalités et s'assure que le projet répond aux besoins des utilisateurs.
  - Tient à jour le backlog produit.
  - Prend les décisions sur les fonctionnalités et les priorités.
  - Valide le travail terminé.
  - Communique avec les parties prenantes (évaluateurs, pairs).
- **Project Manager (PM) / Scrum Master** — facilite la coordination de l'équipe et lève les obstacles.
  - Organise les réunions d'équipe et les sessions de planification.
  - Suit l'avancement et les échéances.
  - Garantit la communication dans l'équipe.
  - Gère les risques et les blocages.
- **Technical Lead / Architecte** — supervise les décisions techniques et l'architecture.
  - Définit l'architecture technique.
  - Décide de la stack technologique.
  - Garantit la qualité du code et les bonnes pratiques.
  - Relit les modifications de code critiques.
- **Développeurs (tous les membres)** — implémentent les fonctionnalités et les modules.
  - Écrivent le code des fonctionnalités qui leur sont attribuées.
  - Participent aux revues de code.
  - Testent leurs implémentations.
  - Documentent leur travail.

> [!note] Taille de l'équipe
> - **4 personnes :** certains membres auront plusieurs rôles (par ex. PM + Développeur, PO + Développeur).
> - **5 personnes :** les rôles peuvent être plus spécialisés, avec un PO, un PM et un Tech Lead dédiés, plus 2 Développeurs.
>
> **Tous les rôles doivent être clairement documentés dans votre `README.md`.**

#### II.1.2 Pratiques de gestion de projet recommandées

Sans être obligatoires, nous vous recommandons fortement de mettre en place quelques pratiques de gestion de projet de base pour réussir :

- **Communication régulière :** réunissez-vous régulièrement (chaque semaine ou toutes les deux semaines) pour faire le point sur l'avancement et les blocages.
- **Organisation des tâches :** utilisez des outils simples comme GitHub Issues, Trello, ou même un document partagé pour savoir qui fait quoi.
- **Découpage du travail :** divisez le projet en tâches plus petites et gérables.
- **Revues de code :** essayez de faire relire chaque modification importante par au moins un autre membre.
- **Documentation :** gardez une trace des décisions importantes et du fonctionnement des choses.
- **Canal de communication :** utilisez Discord, Slack ou équivalent pour échanger rapidement.

> [!tip]
> Ces pratiques sont des recommandations pour vous aider à organiser votre travail. Trouvez ce qui marche le mieux pour votre équipe ! L'important, c'est que tout le monde contribue et que le travail soit bien coordonné.

Pendant l'évaluation, l'équipe devra expliquer :

- Comment les rôles ont été répartis.
- Comment le travail a été organisé et divisé.
- Comment vous avez communiqué et vous êtes coordonnés.
- Comment chaque membre a contribué au projet.

> [!important]
> Tous les membres de l'équipe doivent être capables d'expliquer le projet et leurs contributions.

---

## III — Partie obligatoire

Le contenu du projet est libre. Oui, vous devez apporter vos propres idées et décider ensemble de l'application à construire.

C'est un cran au-dessus de ce que vous avez fait jusqu'ici dans le Tronc Commun. Il faut penser le projet dans son ensemble, pas seulement les fonctionnalités que vous allez implémenter.

Bien sûr, vous ne serez pas totalement libres — il y a des contraintes — mais l'idée vous appartient.

### III.1 Qu'est-ce qu'on fait ?

D'abord, vous devrez écrire un `README.md` complet. Les exigences détaillées sont décrites dans [Exigences du README](#vi--exigences-du-readme).

**Exemples de projets** — votre projet peut prendre de nombreuses formes. Voici quelques exemples valides :

- Un jeu de Pong multijoueur avec un système de tournoi
- Une plateforme collaborative avec des fonctionnalités temps réel
- Un réseau social avec des interactions entre utilisateurs
- Un jeu en ligne (échecs, morpion, etc.) avec matchmaking
- Une application de gestion de projet
- Toute autre application web créative qui respecte les exigences

> [!tip]
> L'essentiel est de créer quelque chose d'engageant qui démontre vos compétences techniques et votre créativité.

### III.2 Exigences générales

Construire un projet complet est compliqué, et beaucoup de choses peuvent mal tourner. Pour vous aider, voici une liste d'exigences générales à respecter. **Si vous ne les respectez pas, votre projet sera rejeté.**

- Le projet doit être une **application web**, avec un **frontend, un backend et une base de données**.
- **Git** doit être utilisé avec des messages de commit clairs et explicites. Le dépôt doit montrer :
  - Des commits de tous les membres de l'équipe.
  - Des messages de commit clairs décrivant les changements.
  - Une bonne répartition du travail dans l'équipe.
- Le déploiement doit utiliser une **solution de conteneurisation** (Docker, Podman ou équivalent) et **se lancer en une seule commande**.
- Votre site doit être compatible avec la **dernière version stable de Google Chrome**.
- **Aucun warning ni aucune erreur** ne doit apparaître dans la console du navigateur.
- Le projet doit inclure des pages **Politique de confidentialité** (Privacy Policy) et **Conditions d'utilisation** (Terms of Service) accessibles, au contenu pertinent.

> [!danger] Politique de confidentialité et Conditions d'utilisation
> Ces pages seront vérifiées pendant l'évaluation. Elles doivent :
> - Être facilement accessibles depuis l'application (par ex. liens dans le footer).
> - Avoir un contenu pertinent et adapté à votre projet.
> - Ne pas être des pages vides ou de remplissage.
>
> **Des pages Politique de confidentialité / Conditions d'utilisation absentes ou insuffisantes entraîneront le rejet du projet.**

> [!danger] Support multi-utilisateurs (obligatoire)
> Votre site doit supporter plusieurs utilisateurs simultanément. C'est une exigence centrale du projet. Les utilisateurs doivent pouvoir interagir avec l'application en même temps, sans conflit ni problème de performance. Cela inclut :
> - Plusieurs utilisateurs peuvent être connectés et actifs en même temps.
> - Les actions concurrentes de différents utilisateurs sont correctement gérées.
> - Les mises à jour en temps réel sont répercutées chez tous les utilisateurs connectés, quand c'est pertinent.
> - Aucune corruption de données ni race condition lors d'actions simultanées.

### III.3 Exigences techniques

Cette section, comme la précédente, est obligatoire. Vous pourrez ensuite choisir vos modules au chapitre suivant.

- Un frontend **clair, responsive et accessible sur tous les appareils**.
- Utilisez un **framework CSS ou une solution de style** de votre choix (par ex. Tailwind CSS, Bootstrap, Material-UI, Styled Components, etc.).
- Stockez les secrets (clés d'API, variables d'environnement, etc.) dans un **fichier `.env` local ignoré par Git**, et fournissez un fichier **`.env.example`**.
- La base de données doit avoir un **schéma clair et des relations bien définies**.
- Votre application doit avoir un **système de gestion des utilisateurs de base**. Les utilisateurs doivent pouvoir s'inscrire et se connecter de façon sécurisée :
  - Au minimum : authentification par email et mot de passe avec une sécurité correcte (mots de passe hachés, salés, etc.).
  - D'autres méthodes d'authentification (OAuth, 2FA, etc.) peuvent être implémentées via des modules.
- Tous les formulaires et toutes les saisies utilisateur doivent être **correctement validés côté frontend et côté backend**.
- **Toute connexion au backend** — depuis un navigateur, un script, une API externe, etc. — **doit utiliser HTTPS**. Les connexions internes au backend (par ex. entre le serveur web et la base de données, entre logiciels dans vos conteneurs) peuvent ne pas être chiffrées.

> [!info] Qu'est-ce qu'un framework ?
> Pour ce projet, un framework est défini comme un outil complet qui fournit :
> - Une architecture structurée et des conventions pour organiser le code.
> - Des fonctionnalités intégrées pour les tâches courantes (routing, gestion d'état, etc.).
> - Un écosystème complet d'outils et de bibliothèques.
>
> Exemples :
> - **Frameworks frontend :** React, Vue, Angular, Svelte, Next.js (ce sont des frameworks).
> - **Frameworks backend :** Express, Fastify, NestJS, Django, Flask, Ruby on Rails.
> - **Pas des frameworks :** jQuery (bibliothèque), Lodash (bibliothèque utilitaire), Axios (client HTTP).
>
> *Note :* React est considéré comme un framework dans ce contexte, en raison de son écosystème et de ses patterns d'architecture, même si c'est techniquement une bibliothèque.

---

## IV — Modules

Vous devez obtenir **14 points au total** pour valider votre projet. Chaque module **Majeur** vaut **2 points**, chaque module **Mineur** vaut **1 point**.

Les catégories suivantes sont disponibles. Vous pouvez choisir plusieurs modules dans n'importe quelle catégorie :

| | |
|---|---|
| Web | Jeu et expérience utilisateur |
| Accessibilité et internationalisation | Devops |
| Gestion des utilisateurs | Données et analytique |
| Intelligence artificielle | Blockchain |
| Cybersécurité | Modules au choix |

Nous vous recommandons fortement de ne choisir vos modules qu'une fois vos idées claires et une bonne compréhension de ce que vous voulez construire.

Par ailleurs, viser **plus de 14 points** au total peut être une bonne idée, surtout si certains modules ne sont pas validés en évaluation.

> [!warning] Dépendances entre modules et évaluation
> - Certains modules nécessitent d'en avoir implémenté d'autres avant (signalé par des notes d'info).
> - Les **modules de jeu** (AI Opponent, Tournoi, Personnalisation du jeu, Mode spectateur, Multijoueur 3+, Ajouter un autre jeu) exigent qu'au moins un jeu soit implémenté d'abord.
> - Le module **Statistiques de jeu** exige qu'un jeu soit implémenté.
> - Les fonctionnalités de **chat avancé** exigent le chat de base du module « Interaction entre utilisateurs ».
> - **Le SSR est incompatible avec le backend blockchain ICP.**
> - Planifiez vos modules avec soin pour qu'ils fonctionnent ensemble de façon cohérente !
> - **Pendant l'évaluation :** on vous demandera de démontrer chaque module revendiqué. Seuls les modules pleinement fonctionnels et correctement implémentés comptent dans votre score final. **Module non fonctionnel ou incomplet = 0 point.**

### Catalogue des modules en un coup d'œil

*Absent du PDF — un tableau récapitulatif construit à partir des sections ci-dessous.*

| Catégorie | Module | Type | Pts | Prérequis |
|---|---|---|:-:|---|
| Web | Framework frontend **et** backend | Majeur | 2 | |
| Web | Framework frontend seul | Mineur | 1 | |
| Web | Framework backend seul | Mineur | 1 | |
| Web | Fonctionnalités temps réel (WebSockets…) | Majeur | 2 | |
| Web | Interaction entre utilisateurs (chat, profil, amis) | Majeur | 2 | |
| Web | API publique (clé d'API, rate limiting, doc, ≥ 5 endpoints) | Majeur | 2 | |
| Web | ORM | Mineur | 1 | |
| Web | Système de notifications | Mineur | 1 | |
| Web | Fonctionnalités collaboratives temps réel | Mineur | 1 | |
| Web | Server-Side Rendering (SSR) | Mineur | 1 | ✗ ICP |
| Web | Progressive Web App (PWA) | Mineur | 1 | |
| Web | Design system maison (≥ 10 composants) | Mineur | 1 | |
| Web | Recherche avancée | Mineur | 1 | |
| Web | Upload et gestion de fichiers | Mineur | 1 | |
| Accessibilité & i18n | Conformité WCAG 2.1 AA | Majeur | 2 | |
| Accessibilité & i18n | Plusieurs langues (≥ 3) | Mineur | 1 | |
| Accessibilité & i18n | Support droite-à-gauche (RTL) | Mineur | 1 | |
| Accessibilité & i18n | Navigateurs supplémentaires (≥ 2) | Mineur | 1 | |
| Gestion des utilisateurs | Gestion des utilisateurs & authentification standard | Majeur | 2 | |
| Gestion des utilisateurs | Statistiques de jeu & historique des matchs | Mineur | 1 | un jeu |
| Gestion des utilisateurs | Authentification distante OAuth 2.0 | Mineur | 1 | |
| Gestion des utilisateurs | Système de permissions avancé | Majeur | 2 | |
| Gestion des utilisateurs | Système d'organisations | Majeur | 2 | |
| Gestion des utilisateurs | 2FA | Mineur | 1 | |
| Gestion des utilisateurs | Tableau de bord d'activité utilisateur | Mineur | 1 | |
| IA | AI Opponent (adversaire IA) | Majeur | 2 | un jeu |
| IA | Système RAG | Majeur | 2 | |
| IA | Interface de système LLM | Majeur | 2 | |
| IA | Système de recommandation par ML | Majeur | 2 | |
| IA | Modération de contenu par IA | Mineur | 1 | |
| IA | Intégration voix / parole | Mineur | 1 | |
| IA | Analyse de sentiment | Mineur | 1 | |
| IA | Reconnaissance et tagging d'images | Mineur | 1 | |
| Cybersécurité | WAF/ModSecurity + HashiCorp Vault | Majeur | 2 | |
| Jeu & UX | Jeu web complet | Majeur | 2 | |
| Jeu & UX | Joueurs distants | Majeur | 2 | un jeu |
| Jeu & UX | Multijoueur (3 joueurs et +) | Majeur | 2 | un jeu |
| Jeu & UX | Autre jeu + historique + matchmaking | Majeur | 2 | un premier jeu |
| Jeu & UX | Graphismes 3D avancés (Three.js, Babylon.js) | Majeur | 2 | |
| Jeu & UX | Chat avancé | Mineur | 1 | chat de base |
| Jeu & UX | Système de tournoi | Mineur | 1 | un jeu |
| Jeu & UX | Personnalisation du jeu | Mineur | 1 | un jeu |
| Jeu & UX | Système de gamification | Mineur | 1 | |
| Jeu & UX | Mode spectateur | Mineur | 1 | un jeu |
| Devops | Gestion des logs avec ELK | Majeur | 2 | |
| Devops | Monitoring Prometheus + Grafana | Majeur | 2 | |
| Devops | Backend en microservices | Majeur | 2 | |
| Devops | Health check, page de statut, sauvegardes, reprise après sinistre | Mineur | 1 | |
| Données & analytique | Tableau de bord analytique avancé | Majeur | 2 | |
| Données & analytique | Export et import de données | Mineur | 1 | |
| Données & analytique | Conformité RGPD | Mineur | 1 | |
| Blockchain | Scores de tournoi sur la blockchain (Avalanche/Solidity) | Majeur | 2 | |
| Blockchain | Backend ICP | Mineur | 1 | ✗ SSR |
| Modules au choix | Module personnalisé | Majeur | 2 | justification dans le README |
| Modules au choix | Module personnalisé | Mineur | 1 | justification dans le README |

### IV.1 Web

- **Majeur — Utiliser un framework pour le frontend et pour le backend.**
  - Utiliser un framework frontend (React, Vue, Angular, Svelte, etc.).
  - Utiliser un framework backend (Express, NestJS, Django, Flask, Ruby on Rails, etc.).
  - Les frameworks full-stack (Next.js, Nuxt.js, SvelteKit) comptent pour les deux si vous utilisez à la fois leurs capacités frontend et backend.
- **Mineur — Utiliser un framework frontend** (React, Vue, Angular, Svelte, etc.).
- **Mineur — Utiliser un framework backend** (Express, Fastify, NestJS, Django, etc.).
- **Majeur — Implémenter des fonctionnalités temps réel avec WebSockets ou une technologie similaire.**
  - Mises à jour en temps réel entre les clients.
  - Gérer proprement les connexions/déconnexions.
  - Diffusion (broadcast) efficace des messages.
- **Majeur — Permettre aux utilisateurs d'interagir entre eux.** Le minimum requis :
  - Un système de chat de base (envoyer/recevoir des messages entre utilisateurs).
  - Un système de profil (voir les informations d'un utilisateur).
  - Un système d'amis (ajouter/retirer des amis, voir la liste d'amis).
- **Majeur — Une API publique** pour interagir avec la base de données, avec une clé d'API sécurisée, du rate limiting, une documentation et au moins 5 endpoints :
  - `GET /api/{something}`
  - `POST /api/{something}`
  - `PUT /api/{something}`
  - `DELETE /api/{something}`

  *(Le sujet demande au moins 5 endpoints mais ne liste que ces 4 méthodes.)*
- **Mineur — Utiliser un ORM pour la base de données.**
- **Mineur — Un système de notifications complet** pour toutes les actions de création, de modification et de suppression.
- **Mineur — Fonctionnalités collaboratives temps réel** (espaces de travail partagés, édition en direct, dessin collaboratif, etc.).
- **Mineur — Server-Side Rendering (SSR)** pour de meilleures performances et un meilleur SEO.
- **Mineur — Progressive Web App (PWA)** avec support hors ligne et installabilité.
- **Mineur — Design system maison** avec des composants réutilisables, incluant une vraie palette de couleurs, une typographie et des icônes (minimum : 10 composants réutilisables).
- **Mineur — Recherche avancée** avec filtres, tri et pagination.
- **Mineur — Système d'upload et de gestion de fichiers.**
  - Support de plusieurs types de fichiers (images, documents, etc.).
  - Validation côté client et côté serveur (type, taille, format).
  - Stockage sécurisé avec un contrôle d'accès adapté.
  - Aperçu des fichiers quand c'est pertinent.
  - Indicateurs de progression pendant l'upload.
  - Possibilité de supprimer les fichiers uploadés.

### IV.2 Accessibilité et internationalisation

- **Majeur — Conformité complète en accessibilité (WCAG 2.1 AA)**, avec support des lecteurs d'écran, navigation au clavier et technologies d'assistance.
- **Mineur — Support de plusieurs langues (au moins 3).**
  - Implémenter un système d'i18n (internationalisation).
  - Au moins 3 traductions complètes.
  - Un sélecteur de langue dans l'interface.
  - Tout texte visible par l'utilisateur doit être traduisible.
- **Mineur — Support des langues de droite à gauche (RTL).**
  - Support d'au moins une langue RTL (arabe, hébreu, etc.).
  - Mise en miroir complète de la mise en page (pas seulement le sens du texte).
  - Ajustements d'interface spécifiques au RTL quand nécessaire.
  - Bascule fluide entre LTR et RTL.
- **Mineur — Support de navigateurs supplémentaires.**
  - Compatibilité complète avec au moins 2 navigateurs supplémentaires (Firefox, Safari, Edge, etc.).
  - Tester et corriger toutes les fonctionnalités dans chaque navigateur.
  - Documenter les éventuelles limitations propres à un navigateur.
  - UI/UX cohérente sur tous les navigateurs supportés.

### IV.3 Gestion des utilisateurs

- **Majeur — Gestion des utilisateurs et authentification standard.**
  - Les utilisateurs peuvent modifier les informations de leur profil.
  - Les utilisateurs peuvent uploader un avatar (avec un avatar par défaut s'ils n'en fournissent pas).
  - Les utilisateurs peuvent ajouter d'autres utilisateurs en amis et voir leur statut en ligne.
  - Les utilisateurs ont une page de profil affichant leurs informations.
- **Mineur — Statistiques de jeu et historique des matchs** (nécessite un module de jeu).
  - Suivre les statistiques de jeu (victoires, défaites, classement, niveau, etc.).
  - Afficher l'historique des matchs (parties en 1v1, dates, résultats, adversaires).
  - Montrer les succès (achievements) et la progression.
  - Intégration d'un classement (leaderboard).

  > [!info]
  > Ce module nécessite d'avoir implémenté au moins un jeu (voir [Jeu et expérience utilisateur](#iv6-jeu-et-expérience-utilisateur)). Impossible de revendiquer ce module sans jeu fonctionnel.
- **Mineur — Authentification distante avec OAuth 2.0** (Google, GitHub, 42, etc.).
- **Majeur — Système de permissions avancé :**
  - Voir, modifier et supprimer des utilisateurs (CRUD).
  - Gestion des rôles (admin, user, guest, moderator, etc.).
  - Vues et actions différentes selon le rôle de l'utilisateur.
- **Majeur — Un système d'organisations :**
  - Créer, modifier et supprimer des organisations.
  - Ajouter des utilisateurs à une organisation.
  - Retirer des utilisateurs d'une organisation.
  - Consulter les organisations et permettre aux utilisateurs d'y effectuer des actions spécifiques (minimum : créer, lire, modifier).
- **Mineur — Un système 2FA (authentification à deux facteurs) complet** pour les utilisateurs.
- **Mineur — Tableau de bord d'analyse et d'insights sur l'activité des utilisateurs.**

### IV.4 Intelligence artificielle

- **Majeur — Ajouter un adversaire IA (AI Opponent) pour les jeux.**
  - L'IA doit être stimulante et capable de gagner de temps en temps.
  - L'IA doit simuler un comportement humain (pas un jeu parfait).
  - Si vous implémentez des options de personnalisation du jeu, l'IA doit pouvoir les utiliser.
  - Vous devez être capables d'expliquer votre implémentation de l'IA pendant l'évaluation.

  > [!info]
  > Ce module nécessite d'avoir implémenté au moins un jeu (voir [Jeu et expérience utilisateur](#iv6-jeu-et-expérience-utilisateur)). L'IA doit savoir jouer correctement à votre jeu.
- **Majeur — Implémenter un système RAG (Retrieval-Augmented Generation) complet.**
  - Interagir avec un grand jeu de données.
  - Les utilisateurs peuvent poser des questions et obtenir des réponses pertinentes.
  - Implémenter une vraie récupération de contexte et une vraie génération de réponse.
- **Majeur — Implémenter une interface complète vers un système LLM.**
  - Générer du texte et/ou des images à partir de la saisie de l'utilisateur.
  - Gérer correctement les réponses en streaming.
  - Implémenter la gestion d'erreurs et le rate limiting.
- **Majeur — Système de recommandation par machine learning.**
  - Recommandations personnalisées selon le comportement de l'utilisateur.
  - Filtrage collaboratif ou filtrage basé sur le contenu.
  - Amélioration continue des recommandations au fil du temps.
- **Mineur — Modération de contenu par IA** (modération, suppression, avertissement automatiques, etc.).
- **Mineur — Intégration voix/parole** pour l'accessibilité ou l'interaction.
- **Mineur — Analyse de sentiment** sur le contenu généré par les utilisateurs.
- **Mineur — Système de reconnaissance et de tagging d'images.**

### IV.5 Cybersécurité

- **Majeur — Implémenter un WAF/ModSecurity (durci) + HashiCorp Vault pour les secrets :**
  - Configurer un ModSecurity/WAF strict.
  - Gérer les secrets dans Vault (clés d'API, identifiants, variables d'environnement), chiffrés et isolés.

### IV.6 Jeu et expérience utilisateur

- **Majeur — Implémenter un jeu web complet où les utilisateurs s'affrontent.**
  - Le jeu peut être multijoueur en temps réel (par ex. Pong, échecs, morpion, jeux de cartes, etc.).
  - Les joueurs doivent pouvoir jouer des matchs en direct.
  - Le jeu doit avoir des règles claires et des conditions de victoire/défaite.
  - Le jeu peut être en 2D ou en 3D.
- **Majeur — Joueurs distants** — permettre à deux joueurs sur deux ordinateurs différents de jouer à la même partie en temps réel.
  - Gérer proprement la latence réseau et les déconnexions.
  - Offrir une expérience fluide pour le jeu à distance.
  - Implémenter une logique de reconnexion.
- **Majeur — Jeu multijoueur (plus de deux joueurs).**
  - Support de trois joueurs ou plus simultanément.
  - Mécaniques de jeu équitables pour tous les participants.
  - Synchronisation correcte entre tous les clients.

  > [!info]
  > Ce module nécessite d'avoir implémenté au moins un jeu. Vous étendez votre jeu pour supporter trois joueurs ou plus.
- **Majeur — Ajouter un autre jeu, avec historique utilisateur et matchmaking.**
  - Implémenter un second jeu distinct.
  - Suivre l'historique et les statistiques des utilisateurs pour ce jeu.
  - Implémenter un système de matchmaking.
  - Maintenir les performances et la réactivité.

  > [!info]
  > Ce module nécessite d'avoir déjà implémenté un premier jeu (voir le module « Implémenter un jeu web complet » ci-dessus). Impossible de revendiquer ce module sans premier jeu fonctionnel.
- **Majeur — Implémenter des graphismes 3D avancés** avec une bibliothèque comme Three.js ou Babylon.js.
  - Créer un environnement 3D immersif.
  - Implémenter des techniques de rendu avancées.
  - Garantir des performances et une interaction fluides.
- **Mineur — Fonctionnalités de chat avancées** (enrichit le chat de base du module « Interaction entre utilisateurs »).
  - Pouvoir bloquer des utilisateurs pour qu'ils ne puissent plus t'écrire.
  - Inviter des utilisateurs à jouer directement depuis le chat.
  - Notifications de partie/tournoi dans le chat.
  - Accès aux profils utilisateurs depuis l'interface du chat.
  - Persistance de l'historique du chat.
  - Indicateurs de saisie et accusés de lecture.

  > [!info]
  > Ce module enrichit le chat de base du module « Permettre aux utilisateurs d'interagir ». Impossible de le revendiquer sans avoir d'abord implémenté le chat de base.
- **Mineur — Implémenter un système de tournoi.**
  - Ordre des affrontements clair et système d'arbre (bracket).
  - Savoir qui joue contre qui.
  - Système de matchmaking pour les participants au tournoi.
  - Inscription et gestion des tournois.

  > [!info]
  > Ce module nécessite d'avoir implémenté au moins un jeu. Pas de tournoi sans jeu auquel jouer.
- **Mineur — Options de personnalisation du jeu.**
  - Power-ups, attaques ou capacités spéciales.
  - Différentes cartes ou différents thèmes.
  - Paramètres de jeu personnalisables.
  - Des options par défaut doivent être disponibles.

  > [!info]
  > Ce module nécessite d'avoir implémenté au moins un jeu. Vous ajoutez de la personnalisation à un jeu existant.
- **Mineur — Un système de gamification** pour récompenser les utilisateurs de leurs actions.
  - Implémenter au moins 3 éléments parmi : succès (achievements), badges, classements, système d'XP/niveaux, défis quotidiens, récompenses.
  - Le système doit être persistant (stocké en base de données).
  - Retour visuel pour les utilisateurs (notifications, barres de progression, etc.).
  - Règles et mécaniques de progression claires.

  > [!tip]
  > Même s'il s'agit d'un module Mineur (1 point), un système de gamification complet peut représenter beaucoup de travail. Privilégiez la qualité à la quantité — trois fonctionnalités bien faites valent mieux que six bâclées.
- **Mineur — Implémenter un mode spectateur pour les jeux.**
  - Permettre aux utilisateurs de regarder les parties en cours.
  - Mises à jour en temps réel pour les spectateurs.
  - Facultatif : un chat pour les spectateurs.

  > [!info]
  > Ce module nécessite d'avoir implémenté au moins un jeu. Les spectateurs ont besoin d'un jeu à regarder.

### IV.7 Devops

- **Majeur — Infrastructure de gestion des logs avec ELK** (Elasticsearch, Logstash, Kibana).
  - Elasticsearch pour stocker et indexer les logs.
  - Logstash pour collecter et transformer les logs.
  - Kibana pour la visualisation et les tableaux de bord.
  - Mettre en place des politiques de rétention et d'archivage des logs.
  - Sécuriser l'accès à tous les composants.
- **Majeur — Système de monitoring avec Prometheus et Grafana.**
  - Configurer Prometheus pour collecter les métriques.
  - Configurer les exporters et les intégrations.
  - Créer des tableaux de bord Grafana personnalisés.
  - Mettre en place des règles d'alerte.
  - Sécuriser l'accès à Grafana.
- **Majeur — Backend en microservices.**
  - Concevoir des services faiblement couplés avec des interfaces claires.
  - Utiliser des API REST ou des files de messages (message queues) pour communiquer.
  - Chaque service doit avoir une responsabilité unique.
- **Mineur — Système de health check et page de statut**, avec sauvegardes automatiques et procédures de reprise après sinistre (disaster recovery).

### IV.8 Données et analytique

- **Majeur — Tableau de bord analytique avancé avec visualisation de données.**
  - Graphiques interactifs (courbes, barres, camemberts, etc.).
  - Mises à jour des données en temps réel.
  - Fonction d'export (PDF, CSV, etc.).
  - Plages de dates et filtres personnalisables.
- **Mineur — Fonctions d'export et d'import de données.**
  - Exporter les données dans plusieurs formats (JSON, CSV, XML, etc.).
  - Importer des données avec validation.
  - Support des opérations en masse.
- **Mineur — Fonctionnalités de conformité au RGPD (GDPR).**
  - Permettre aux utilisateurs de demander leurs données.
  - Suppression des données avec confirmation.
  - Export des données utilisateur dans un format lisible.
  - Emails de confirmation pour les opérations sur les données.

### IV.9 Blockchain

- **Majeur — Stocker les scores de tournoi sur la blockchain.**
  - Utiliser Avalanche et des smart contracts Solidity sur une blockchain de test.
  - Implémenter des smart contracts pour enregistrer, gérer et récupérer les scores de tournoi.
  - Garantir l'intégrité et l'immutabilité des données.
- **Mineur — Utiliser ICP (Internet Computer Protocol)** pour un backend qui tourne sur une blockchain (**incompatible avec le SSR**).

### IV.10 Modules au choix

- **Majeur — Implémenter un module personnalisé qui ne figure pas dans la liste ci-dessus.**
  - Le module doit être conséquent et démontrer une complexité technique.
  - Vous devez fournir une vraie justification dans votre `README.md`, expliquant :
    - Pourquoi vous avez choisi ce module.
    - Quels défis techniques il relève.
    - Ce qu'il apporte à votre projet.
    - Pourquoi il mérite le statut de module Majeur (2 points).
  - Prendre des raccourcis ou implémenter des fonctionnalités triviales entraînera un rejet.
  - Soyez créatifs et sortez des sentiers battus.
  - Le module doit être pertinent dans le contexte de votre projet.
- **Mineur — Comme le module majeur, mais de portée plus réduite et moins complexe.**
  - Doit tout de même démontrer des compétences techniques et de la créativité.
  - Doit apporter une vraie valeur à votre projet.
  - Nécessite une justification dans le `README.md` (comme le Majeur, mais pour 1 point).

---

## V — Idées et exemples de projets

Pour vous aider à démarrer et nourrir votre créativité, ce chapitre propose des idées de projets concrètes et des exemples de façons d'atteindre les 14 points requis. Ce ne sont que des suggestions — n'hésitez pas à être créatifs et à trouver vos propres idées !

### V.1 Exemple : construire un jeu de Pong

Si vous choisissez de créer un Pong (comme dans la version d'origine du projet), voici comment atteindre 14 points :

| Catégorie | Modules | Points |
|---|---|:-:|
| Jeu et expérience utilisateur | Jeu web (2) + Joueurs distants (2) + Système de tournoi (1) + Personnalisation du jeu (1) | 6 |
| Gestion des utilisateurs | Gestion des utilisateurs standard (2) + OAuth (1) | 3 |
| Web | Frameworks, frontend + backend (2) + ORM (1) | 3 |
| Intelligence artificielle | AI Opponent (2) | 2 |
| **Total** | | **14** |

> [!tip]
> Ce n'est qu'un exemple. Vous pouvez combiner librement des modules de différentes catégories pour créer votre propre projet. L'essentiel est que vos modules fonctionnent ensemble de façon cohérente et apportent de la valeur à votre application.

### V.2 Projets de jeu

Ces projets misent sur le gameplay interactif et la compétition entre utilisateurs :

| Projet | Description | Modules suggérés | Potentiel |
|---|---|---|:-:|
| **Pong multijoueur** | Le Pong classique avec tournois, jeu à distance, adversaires IA et power-ups | Jeu web, Joueurs distants, Système de tournoi, AI Opponent, Personnalisation du jeu | 14+ |
| **Plateforme d'échecs en ligne** | Échecs en temps réel avec matchmaking, classement ELO, analyse de parties et mode spectateur | Jeu web, Joueurs distants, AI Opponent, Mode spectateur, Statistiques de jeu | 15+ |
| **Arène de jeux de cartes** | Jeux de cartes multijoueurs (poker, Uno, etc.) avec tournois et classements | Jeu web, Multijoueur 3+, Système de tournoi, Gamification | 14+ |
| **Mini-jeu Battle Royale** | Un battle royale simple dans le navigateur, avec plusieurs joueurs | Jeu web, Multijoueur 3+, Fonctionnalités temps réel, Personnalisation du jeu | 14+ |
| **Plateforme de quiz** | Quiz multijoueur en temps réel, avec catégories et tournois | Jeu web, Multijoueur 3+, Système de tournoi, Gamification, Tableau de bord analytique | 15+ |

### V.3 Projets sociaux et collaboratifs

Ces projets mettent l'accent sur l'interaction entre utilisateurs et la construction d'une communauté :

| Projet | Description | Modules suggérés | Potentiel |
|---|---|---|:-:|
| **Réseau social** | Profils, posts, commentaires, likes, amis, chat en temps réel et notifications | Interaction entre utilisateurs, Fonctionnalités temps réel, Système de notifications, Chat avancé, Upload de fichiers | 14+ |
| **Espace de travail collaboratif** | Édition de documents en temps réel, gestion de projet, chat d'équipe et partage de fichiers | Fonctionnalités collaboratives temps réel, Interaction entre utilisateurs, Système d'organisations, Upload de fichiers, Permissions avancées | 15+ |
| **Forum** | Forums de discussion avec catégories, fils, outils de modération et système de réputation | Interaction entre utilisateurs, Permissions avancées, Gamification, Modération de contenu par IA, Recherche avancée | 14+ |
| **Gestion d'événements** | Créer et gérer des événements, système de RSVP, intégration calendrier et notifications | Interaction entre utilisateurs, Système de notifications, Système d'organisations, API publique, Recherche avancée | 14+ |
| **Plateforme d'apprentissage (LMS)** | Cours, devoirs, quiz, suivi de progression et échanges élèves-enseignants | Interaction entre utilisateurs, Système d'organisations, Permissions avancées, Upload de fichiers, Tableau de bord analytique | 15+ |

### V.4 Projets créatifs et médias

Ces projets se concentrent sur la création et le partage de contenu :

| Projet | Description | Modules suggérés | Potentiel |
|---|---|---|:-:|
| **Streaming musical** | Uploader et écouter de la musique, playlists, recommandations et fonctionnalités sociales | Upload de fichiers, Interaction entre utilisateurs, Système de recommandation, Recherche avancée, Tableau de bord analytique | 15+ |
| **Partage de vidéos** | Uploader et regarder des vidéos, commentaires, likes, abonnements et recommandations | Upload de fichiers, Interaction entre utilisateurs, Système de recommandation, Modération de contenu par IA, Recherche avancée | 16+ |
| **Galerie d'art** | Partager des œuvres dans des galeries, avec commentaires, likes et profils d'artistes | Upload de fichiers, Interaction entre utilisateurs, Reconnaissance d'images, Recherche avancée, Design system maison | 14+ |
| **Plateforme de blog** | Écrire et publier des articles, avec commentaires, tags, catégories et engagement des lecteurs | Interaction entre utilisateurs, SSR, Recherche avancée, Analyse de sentiment, Plusieurs langues | 14+ |
| **Partage de recettes** | Partager des recettes, notes, commentaires, planification de repas et listes de courses | Interaction entre utilisateurs, Upload de fichiers, Recherche avancée, Système de recommandation, PWA | 14+ |

### V.5 Projets de productivité et outils

Ces projets aident les utilisateurs à organiser et gérer leur travail :

| Projet | Description | Modules suggérés | Potentiel |
|---|---|---|:-:|
| **Gestionnaire de tâches** | Projets, tâches, assignations, échéances, collaboration d'équipe et suivi de progression | Système d'organisations, Interaction entre utilisateurs, Fonctionnalités collaboratives temps réel, Système de notifications, Tableau de bord analytique | 15+ |
| **Plateforme de collaboration sur du code** | Partage de snippets, codage collaboratif, gestion de versions et discussions | Interaction entre utilisateurs, Fonctionnalités collaboratives temps réel, API publique, Recherche avancée, Design system maison | 14+ |
| **Système de réservation** | Réserver des ressources (salles, matériel, rendez-vous), calendrier et notifications | Interaction entre utilisateurs, Système d'organisations, Système de notifications, API publique, Recherche avancée | 14+ |
| **Marketplace** | Acheter et vendre des objets, avec notes des utilisateurs, messagerie, paiement intégré et recherche | Interaction entre utilisateurs, Upload de fichiers, Recherche avancée, Système de recommandation, API publique | 14+ |
| **Suivi sportif** | Enregistrer ses séances, suivre sa progression, défis, classements et fonctionnalités sociales | Interaction entre utilisateurs, Gamification, Tableau de bord analytique, PWA, Export/import de données | 14+ |

### V.6 Projets spécialisés

Ces projets visent des niches ou des secteurs précis :

| Projet | Description | Modules suggérés | Potentiel |
|---|---|---|:-:|
| **Simulateur de trading en temps réel** | Simulation de trading d'actions et de cryptomonnaies, avec données en temps réel et portefeuilles | Fonctionnalités temps réel, Interaction entre utilisateurs, Tableau de bord analytique, API publique, Graphismes 3D avancés | 15+ |
| **Apprentissage des langues** | Leçons, exercices, suivi de progression et pratique entre pairs | Interaction entre utilisateurs, Gamification, Plusieurs langues, Intégration voix, Tableau de bord analytique | 15+ |
| **Adoption d'animaux** | Parcourir les animaux, processus d'adoption, profils utilisateurs et messagerie | Interaction entre utilisateurs, Upload de fichiers, Recherche avancée, Système d'organisations, Système de notifications | 14+ |
| **Organisation de voyages** | Planifier des voyages, partager des itinéraires, recommandations et fonctionnalités sociales | Interaction entre utilisateurs, Fonctionnalités collaboratives temps réel, Système de recommandation, Plusieurs langues, Recherche avancée | 15+ |
| **Financement participatif** | Créer des campagnes, dons, actualités et engagement de la communauté | Interaction entre utilisateurs, Upload de fichiers, API publique, Tableau de bord analytique, Système de notifications | 14+ |

> [!tip] Choisir votre projet
> Ce ne sont que des idées pour vous inspirer. L'essentiel est de choisir un projet qui :
> - Intéresse votre équipe et motive tout le monde à y travailler.
> - Permet d'implémenter les modules requis (14 points minimum).
> - Démontre de la complexité technique et de la créativité.
> - Peut réalistement être terminé dans les délais du projet.
> - Repose sur des combinaisons de modules cohérentes qui fonctionnent bien ensemble.
>
> Discutez en équipe, passez en revue les modules disponibles, et choisissez avec soin !

---

## VI — Exigences du README

Un fichier `README.md` doit être fourni à la racine de votre dépôt Git. Son but est de permettre à quiconque ne connaît pas le projet (pairs, staff, recruteurs, etc.) de comprendre rapidement de quoi il s'agit, comment le lancer, et où trouver plus d'informations sur le sujet.

> [!warning]
> **Le README doit être rédigé en anglais.** Les noms de sections attendus sont donc les noms anglais, indiqués entre parenthèses ci-dessous.

Le `README.md` doit contenir au minimum :

- **La toute première ligne** doit être en italique et dire exactement :
  `*This project has been created as part of the 42 curriculum by <login1>[, <login2>[, <login3>[...]]].*`
- Une section **« Description »** qui présente clairement le projet, son objectif et un bref aperçu.
- Une section **« Instructions »** avec toutes les informations utiles sur la compilation, l'installation et/ou l'exécution.
- Une section **« Resources »** qui liste les références classiques sur le sujet (documentation, articles, tutoriels, etc.), ainsi qu'une description de **la façon dont l'IA a été utilisée** — pour quelles tâches et sur quelles parties du projet.

Des sections supplémentaires peuvent être exigées selon le projet (par ex. exemples d'utilisation, liste des fonctionnalités, choix techniques, etc.). Celles qui sont exigées sont listées ci-dessous.

- La section « Description » doit aussi contenir **un nom clair pour le projet et ses fonctionnalités clés**.
- La section « Instructions » doit mentionner **tous les prérequis** (logiciels, outils, versions, configuration comme la mise en place du `.env`, etc.) et des **instructions pas à pas** pour lancer le projet.

### Sections supplémentaires exigées pour ce projet

- **Informations sur l'équipe** (*Team Information*) — pour chaque membre cité en tête du `README.md` :
  - Rôle(s) attribué(s) : PO, PM, Tech Lead, Développeurs, etc.
  - Brève description de ses responsabilités.
- **Gestion de projet** (*Project Management*) :
  - Comment l'équipe a organisé le travail (répartition des tâches, réunions, etc.).
  - Outils de gestion de projet utilisés (GitHub Issues, Trello, etc.).
  - Canaux de communication utilisés (Discord, Slack, etc.).
- **Stack technique** (*Technical Stack*) :
  - Technologies et frameworks frontend utilisés.
  - Technologies et frameworks backend utilisés.
  - Système de base de données, et pourquoi il a été choisi.
  - Toute autre technologie ou bibliothèque importante.
  - Justification des choix techniques majeurs.
- **Schéma de la base de données** (*Database Schema*) :
  - Représentation visuelle ou description de la structure de la base.
  - Tables/collections et leurs relations.
  - Champs clés et types de données.
- **Liste des fonctionnalités** (*Features List*) :
  - Liste complète des fonctionnalités implémentées.
  - Quel(s) membre(s) a/ont travaillé sur chaque fonctionnalité.
  - Brève description du fonctionnement de chacune.
- **Modules** (*Modules*) :
  - Liste de tous les modules choisis (Majeurs et Mineurs).
  - Calcul des points (Majeur = 2 pts, Mineur = 1 pt).
  - Justification de chaque choix de module, en particulier pour les « Modules au choix ».
  - Comment chaque module a été implémenté.
  - Quel(s) membre(s) a/ont travaillé sur chaque module.
- **Contributions individuelles** (*Individual Contributions*) :
  - Détail de ce que chaque membre a apporté.
  - Fonctionnalités, modules ou composants précis implémentés par chaque personne.
  - Difficultés rencontrées et comment elles ont été surmontées.

Toute autre information utile ou pertinente est bienvenue (documentation d'utilisation, limitations connues, licence, crédits, etc.).

> [!important]
> Le `README.md` est une partie essentielle de l'évaluation. Il doit être :
> - Clair et bien organisé.
> - Complet, avec toutes les sections exigées.
> - Professionnel et agréable à lire.
> - Honnête sur les contributions et les difficultés.
>
> Un README pauvre ou incomplet peut pénaliser votre évaluation.
>
> **Votre README doit être rédigé en anglais.**

---

## VII — Partie bonus

La partie bonus ne sera prise en compte **que si tous les modules requis ont été implémentés**, c'est-à-dire le minimum obligatoire de 14 points.

Chaque module supplémentaire implémenté au-delà des 14 points requis peut être compté en bonus. Pour chaque module en plus :

- Il doit être pleinement fonctionnel.
- Il doit respecter la description des exigences du module.
- Il doit apporter une vraie valeur au projet.
- Il doit être correctement justifié dans le README.

Chaque module supplémentaire validé compte ainsi :

- **Modules Majeurs :** 2 points chacun
- **Modules Mineurs :** 1 point chacun

Vous pouvez obtenir **5 points de bonus au maximum** (par ex. 5 modules mineurs, ou 2 modules majeurs + 1 module mineur).

---

## VIII — Rendu et évaluation par les pairs

Rendez votre travail sur votre dépôt Git, comme d'habitude. Seul le travail présent dans votre dépôt sera évalué. Vérifiez bien les noms de vos fichiers.

Nous vous recommandons vivement de discuter de vos idées avec votre équipe et avec d'autres étudiants avant de commencer à travailler sur le projet.

Pendant l'évaluation, une **brève modification du projet** pourra parfois être demandée. Il peut s'agir d'un petit changement de comportement, de quelques lignes de code à écrire ou réécrire, ou d'une fonctionnalité facile à ajouter.

Cette étape ne s'applique pas forcément à tous les projets, mais vous devez y être prêts si elle figure dans les consignes d'évaluation.

Elle sert à vérifier votre compréhension réelle d'une partie précise du projet. La modification peut se faire dans l'environnement de développement de votre choix (par ex. votre setup habituel), et doit être faisable en quelques minutes — sauf si un temps précis est fixé dans le cadre de l'évaluation. On peut par exemple vous demander de faire une petite modification dans une fonction ou un script, de changer un affichage, ou d'adapter une structure de données pour stocker une nouvelle information.

Les détails (périmètre, cible, etc.) seront précisés dans les consignes d'évaluation et peuvent varier d'une évaluation à l'autre pour un même projet.
