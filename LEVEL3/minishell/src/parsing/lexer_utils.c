/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamjada <alamjada@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/21 19:00:37 by alamjada          #+#    #+#             */
/*   Updated: 2026/02/21 19:03:43 by alamjada         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ft_state_inter_space_normal(t_minishell *minishell, char *buffer)
{
	int	len_buffer;

	len_buffer = ft_strlen(buffer);
	if (len_buffer > 0)
		if (ft_token_add(minishell, ft_token_create(minishell, buffer)))
			return (ft_error(minishell, MALLOC_FAIL, ERR_MALLOC_INTERP, NULL));
	minishell->state = WAITING;
	return (SUCCESS);
}

int	ft_state_inter_ope_normal(t_minishell *minishell, char *buffer, char *line,
		int index)
{
	int	len_buffer;

	len_buffer = ft_strlen(buffer);
	if (len_buffer > 0 && (buffer[0] != line[index] || len_buffer >= 2))
		if (ft_token_add(minishell, ft_token_create(minishell, buffer)))
			return (ft_error(minishell, MALLOC_FAIL, ERR_MALLOC_INTERP, NULL));
	if (ft_buffer_add(buffer, line[index]))
		return (BUFFER_FAIL);
	return (SUCCESS);
}

int	ft_state_inter_not_waiting(t_minishell *minishell, char *buffer, char *line,
		int index)
{
	if (!ft_ischarset(line[index], OPERATORS) && ft_ischarset(buffer[0],
			OPERATORS))
	{
		if (ft_token_add(minishell, ft_token_create(minishell, buffer)))
			return (ft_error(minishell, MALLOC_FAIL, ERR_MALLOC_INTERP, NULL));
	}
	if (ft_buffer_add(buffer, line[index]))
		return (BUFFER_FAIL);
	return (SUCCESS);
}

// A SECURISER VIA LA TAILLE MAX
int	ft_buffer_add(char *buffer, char c)
{
	int	len;

	len = ft_strlen(buffer);
	if (len < BUFFER_SIZE)
	{
		buffer[len] = c;
		buffer[len + 1] = '\0';
		return (0);
	}
	else
		return (ft_error(NULL, BUFFER_FAIL, ERR_BUF_SIZE, NULL));
}
