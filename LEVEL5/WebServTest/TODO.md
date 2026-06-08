Penser a partir des classes. On est plus en C. Regarde le main, trouve comment changer de vision afin de psasser de l'imperatif a l'oop. 
Prendre en compte le fait de developper les classes avant de coder. Tu vas devoir tout changer sinon. 
Decomposer en sous probleme, creeer des classes pour resoudre ce sous probleme et faire remonter le tout.

Classes / Struct s réutilisables donc c'est cool.

Commencer par le parsing du config. A partir de la, loader tous les sockets qui permettront d'avoir une connexion a partir du .conf
Choix du container : Vector / Map / Queue ?

Fonctions utilisees :	socket -> Créer un fd au niveau de la page des fd (géré par le kernel)
			setsockopt -> Associe les options au socket_fd (
			bind
			listen
			accept

A partir de la -> On run poll



1 - Créer le serveur

Ouvrir un socket (ou pluseieurs si plusieurs ports)
Initialiser le socket (ou pluseieurs si plusieurs ports)

2 - Créer le client

Créer également un socket de connection avec les memes options
Initialiser la connection avec connect

3 - Mettre le serveur en mode écoute



4 : Envoyer un premier ping avec le client

5 : Réponse du serveur qui confirme qu'il ecoute

6 : Requete du client vers le serveur avec ce qu'il veut recuperer .
