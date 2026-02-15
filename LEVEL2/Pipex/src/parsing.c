/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/15 18:00:00 by fardeau           #+#    #+#             */
/*   Updated: 2026/02/15 19:02:21 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/pipex.h"

// AJOUTE UN CARACTERE AU BUFFER (meme logique que ft_buffer_add du lexer)
void	ft_buffer_add(char *buffer, char c)
{
	int	i;

	i = 0;
	while (buffer[i])
		i++;
	buffer[i] = c;
	buffer[i + 1] = '\0';
}

// AFFECTE L'ETAT (meme logique que ft_state_detect du lexer)
void	ft_state_detect(char c, t_pipex *pipex)
{
	// SI GUILLEMETS
	if (c == '"')
	{
		if (pipex->state == NORMAL || pipex->state == WAITING)
			pipex->state = IN_DQUOTE;
		else if (pipex->state == IN_DQUOTE)
			pipex->state = NORMAL;
	}
	// SI APOSTROPHE
	else if (c == '\'')
	{
		if (pipex->state == NORMAL || pipex->state == WAITING)
			pipex->state = IN_QUOTE;
		else if (pipex->state == IN_QUOTE)
			pipex->state = NORMAL;
	}
	// SI EN PAUSE ET TROUVE UN AUTRE
	else if (!ft_ischarset(c, SPACES) && pipex->state == WAITING)
		pipex->state = NORMAL;
}

// COMPTE LE NOMBRE DE MOTS AVEC LA MACHINE A ETATS
static int	ft_words_count(t_pipex *pipex, char *line)
{
	int		count;
	int		i;
	int		in_word;

	count = 0;
	i = 0;
	in_word = 0;
	pipex->state = NORMAL;
	while (line[i])
	{
		ft_state_detect(line[i], pipex);
		if (ft_ischarset(line[i], SPACES) && pipex->state == NORMAL)
		{
			if (in_word)
				count++;
			in_word = 0;
			pipex->state = WAITING;
		}
		else if (pipex->state != WAITING)
			in_word = 1;
		i++;
	}
	if (in_word)
		count++;
	if (pipex->state == IN_QUOTE || pipex->state == IN_DQUOTE)
		return (-1);
	return (count);
}

// INTERPRETE LE CARACTERE EN FONCTION DE L'ETAT ET REMPLIT ARGS
static void	ft_state_interpret(t_pipex *pipex, char *line, char *buffer,
		char **args)
{
	int	i;
	int	index;

	i = 0;
	index = 0;
	pipex->state = NORMAL;
	while (line[i])
	{
		ft_state_detect(line[i], pipex);
		// SI ESPACES EN ETAT NORMAL => ON FLUSH LE BUFFER
		if (ft_ischarset(line[i], SPACES) && pipex->state == NORMAL)
		{
			if (ft_strlen(buffer) > 0)
			{
				args[index++] = ft_strdup_gc(buffer, &pipex->gc);
				ft_bzero(buffer, ft_strlen(buffer));
			}
			pipex->state = WAITING;
		}
		// SI STATE != WAITING => ON AJOUTE AU BUFFER (saute les quotes)
		else if (pipex->state != WAITING)
		{
			if (line[i] != '\'' && line[i] != '"')
				ft_buffer_add(buffer, line[i]);
		}
		i++;
	}
	if (ft_strlen(buffer) > 0)
		args[index++] = ft_strdup_gc(buffer, &pipex->gc);
	args[index] = NULL;
}

static void	ft_cmd_parse(t_pipex *pipex, t_cmd *cmd, char *arg)
{
	char	*buffer;
	int		line_len;
	int		word_count;

	line_len = ft_strlen(arg);
	word_count = ft_words_count(pipex, arg);
	if (word_count < 0)
	{
		ft_putstr_fd("pipex: syntax error: unclosed quote\n", STDERR_FILENO);
		return ;
	}
	if (word_count == 0)
	{
		ft_putstr_fd("pipex: command not found: ", STDERR_FILENO);
		ft_putendl_fd(arg, STDERR_FILENO);
		return ;
	}
	// ALLOUE LE BUFFER (meme logique que ft_token_lst_create)
	buffer = ft_calloc_gc(line_len + 1, sizeof(char), &pipex->gc);
	cmd->args = ft_calloc_gc(word_count + 1, sizeof(char *), &pipex->gc);
	if (!cmd->args || !buffer)
		return ;
	// ON INTERPRETE CHAQUE CHAR AVEC LA MACHINE A ETATS
	ft_state_interpret(pipex, arg, buffer, cmd->args);
	if (!cmd->args[0])
	{
		ft_putstr_fd("pipex: command not found: ", STDERR_FILENO);
		ft_putendl_fd(arg, STDERR_FILENO);
		return ;
	}
	cmd->path = ft_path_resolve(pipex, cmd->args[0]);
	if (!cmd->path)
	{
		ft_putstr_fd("pipex: command not found: ", STDERR_FILENO);
		ft_putendl_fd(cmd->args[0], STDERR_FILENO);
	}
}

// OUVRE LE FICHIER D'ENTREE (meme logique que ft_open de cntrl.c)
static void	ft_infile_open(t_pipex *pipex, char *filename)
{
	pipex->cmd1.infd = open(filename, O_RDONLY);
	if (pipex->cmd1.infd == -1)
		perror(filename);
}

// OUVRE LE FICHIER DE SORTIE (meme logique que ft_open de cntrl.c)
static void	ft_outfile_open(t_pipex *pipex, char *filename)
{
	pipex->cmd2.outfd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (pipex->cmd2.outfd == -1)
		perror(filename);
}

// PARSE TOUS LES ARGUMENTS : file1 cmd1 cmd2 file2
void	ft_args_parse(t_pipex *pipex, char **argv)
{
	ft_infile_open(pipex, argv[1]);
	ft_cmd_parse(pipex, &pipex->cmd1, argv[2]);
	ft_cmd_parse(pipex, &pipex->cmd2, argv[3]);
	ft_outfile_open(pipex, argv[4]);
}
