/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/15 17:03:10 by fardeau           #+#    #+#             */
/*   Updated: 2026/02/15 17:35:47 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

void ft_state_detect (t_pipex *pipex, char c)
{
	if (c == '\"')
	{
		if (pipex->state == DEFAULT || pipex->state == WAITING)
			pipex->state = IN_DQUOTE;
		if (pipex->state == IN_DQUOTE)
			pipex->state = DEFAULT;
	}
	else if (c == '\'')
	{
		if (pipex->state == DEFAULT || pipex->state == WAITING)
			pipex->state = IN_QUOTE;
		if (pipex->state == IN_QUOTE)
			pipex->state = DEFAULT;
	}
	else if (!ft_ischarset(c, " \t\n") && pipex->state == WAITING)
		pipex->state = DEFAULT;
}

int	ft_state_interpret(t_pipex *pipex, char *str, int index, char *buffer)
{
	return (0);
}

int	ft_cmd_init(t_pipex *pipex, t_cmd *cmd, char *str)
{
	int i;
	size_t	str_len;
	char	*buffer;
	t_cmd_args *new;

	i = 0;
	str_len = ft_strlen(str);
	buffer = ft_calloc_gc(str_len + 1, sizeof(char), &pipex->gc);
	if (!buffer)
		return (ft_error(ERRN_MALLOC, "Error malloc buffer parsing\n"));
	while (str[i])
	{
		ft_state_detect(pipex, str[i]);
		if (ft_state_interpret(pipex, str, i, buffer))
			return (ERRN_INIT);
		i++;
	}
	buffer[i] = '\0';
	if (pipex->state == IN_QUOTE || pipex->state == IN_DQUOTE)
		return (ft_error(ERRN_INIT, "Syntax error: unclosed quotes\n"));
	return (0);
}