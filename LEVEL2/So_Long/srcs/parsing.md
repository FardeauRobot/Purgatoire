A PRENDRE EN COMPTE

5 CHARS DIFFERENTS
	0 VIDE
	1 WALL
	C COLLEC
	E EXIT
	P PLAYER

OK		Un trajet valide
NOPE	Map doit etre rectangulaire
NOPE	Map entourée de murs. Si pas le cas : Erreur
NOPE	Parse toutes les maps tant qu'elles respectent les criteres données


Besoin de GNL

Parsing : 
	Regarder si le fichier finit bien par .ber,
	Regarder si le fichier peut s'ouvrir,

# DEFINE OK_CHARS "01CEP"

ft_parsing (t_game *game, char *filepath)
{
	int fd;
	int m_len;
	int	i;
	char **map;

	int i;
	map = game->map;
	Check si filepath fin == ".ber"
		// ERREUR TERMINAISON FICHIER
	fd = open (filepath, O_RDONLY);
	si fd == -1
		// ERREUR ACCES FICHIER
	line = ft_get_next_line (fd);
	m_len = 0;
	while (line)
		REGARDER LA TAILLE DE LA LIGNE
		REGARDER LES CARACS A L'INTERIEUR
		VERIFIER LES MURS
		AUGMENTER I
		VIDER line
		free(line)
		if (TAILLE != m_len || !OK_CHARS || !WALLS)
		REGNL (fd)
	
}

map[][]
