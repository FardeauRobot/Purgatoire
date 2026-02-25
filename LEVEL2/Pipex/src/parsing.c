/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/15 18:00:00 by fardeau           #+#    #+#             */
/*   Updated: 2026/02/23 10:46:12 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/pipex.h"

void	ft_buffer_add(char *buffer, char c)
{
	int	i;

	i = 0;
	while (buffer[i])
		i++;
	buffer[i] = c;
	buffer[i + 1] = '\0';
}

void	ft_state_detect(char c, t_pipex *pipex)
{
	if (c == '"')
	{
		if (pipex->state == NORMAL || pipex->state == WAITING)
			pipex->state = IN_DQUOTE;
		else if (pipex->state == IN_DQUOTE)
			pipex->state = NORMAL;
	}
	else if (c == '\'')
	{
		if (pipex->state == NORMAL || pipex->state == WAITING)
			pipex->state = IN_QUOTE;
		else if (pipex->state == IN_QUOTE)
			pipex->state = NORMAL;
	}
	else if (!ft_ischarset(c, SPACES) && pipex->state == WAITING)
		pipex->state = NORMAL;
}

static void	ft_words_count(t_pipex *pipex, char *line, int *count)
{
	int		i;
	int		in_word;

	i = -1;
	in_word = 0;
	pipex->state = NORMAL;
	while (line[++i])
	{
		ft_state_detect(line[i], pipex);
		if (ft_ischarset(line[i], SPACES) && pipex->state == NORMAL)
		{
			if (in_word)
				(*count)++;
			in_word = 0;
			pipex->state = WAITING;
		}
		else if (pipex->state != WAITING)
			in_word = 1;
	}
	if (in_word)
		(*count)++;
	if (pipex->state == IN_QUOTE || pipex->state == IN_DQUOTE)
		(*count) = -1;
}

static void	ft_state_interpret(t_pipex *pipex, char *line, char *buffer,
		char **args)
{
	int	i;
	int	idx;

	i = -1;
	idx = 0;
	pipex->state = NORMAL;
	while (line[++i])
	{
		ft_state_detect(line[i], pipex);
		if (ft_ischarset(line[i], SPACES) && pipex->state == NORMAL)
		{
			if (ft_strlen(buffer) > 0)
			{
				args[idx++] = ft_strdup_gc(buffer, &pipex->gc);
				ft_bzero(buffer, ft_strlen(buffer));
			}
			pipex->state = WAITING;
		}
		else if (pipex->state != WAITING
			&& line[i] != '\'' && line[i] != '"')
			ft_buffer_add(buffer, line[i]);
	}
	if (ft_strlen(buffer) > 0)
		args[idx++] = ft_strdup_gc(buffer, &pipex->gc);
	args[idx] = NULL;
}

void	ft_cmd_parse(t_pipex *pipex, t_cmd *cmd, char *arg)
{
	char	*buffer;
	int		word_count;

	word_count = 0;
	ft_words_count(pipex, arg, &word_count);
	if (word_count < 0)
	{
		ft_putstr_fd("pipex: syntax error: unclosed quote\n", STDERR_FILENO);
		return ;
	}
	if (word_count == 0)
		return (ft_cmd_not_found(arg));
	buffer = ft_calloc_gc(ft_strlen(arg) + 1, sizeof(char), &pipex->gc);
	cmd->args = ft_calloc_gc(word_count + 1, sizeof(char *), &pipex->gc);
	if (!cmd->args || !buffer)
		return ;
	ft_state_interpret(pipex, arg, buffer, cmd->args);
	if (!cmd->args[0])
		return (ft_cmd_not_found(arg));
	cmd->path = ft_path_resolve(pipex, cmd->args[0]);
	if (!cmd->path)
		ft_cmd_not_found(cmd->args[0]);
}
