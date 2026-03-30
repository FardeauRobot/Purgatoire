/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamjada <alamjada@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/11 13:04:05 by tibras            #+#    #+#             */
/*   Updated: 2026/02/28 13:15:45 by alamjada         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	ft_expand_separator(t_minishell *minishell, char *usable_str)
{
	if (ft_buffer_add(usable_str, '$') != 0)
		return (ft_error(minishell, BUFFER_FAIL, ERR_BUF_SIZE, NULL));
	return (SUCCESS);
}

static int	ft_expand_error_val(t_minishell *minishell, char *usable_str,
	int *start)
{
	char	*err_value;

	err_value = ft_itoa_gc(minishell->cached_status, &minishell->gc_line);
	if (!err_value)
		return (ft_error(minishell, MALLOC_FAIL, ERR_MALLOC_EXPAND, NULL));
	if (ft_strlcat(usable_str, err_value, BUFFER_SIZE) > BUFFER_SIZE)
		return (ft_error(minishell, BUFFER_FAIL, ERR_BUF_SIZE, NULL));
	(*start)++;
	return (SUCCESS);
}

static int	ft_expand(char *str, int *start, char *usable_str,
		t_minishell *minishell)
{
	char	buffer[BUFFER_SIZE];
	t_env	*path_env;

	ft_bzero(buffer, BUFFER_SIZE);
	if (minishell->state == NORMAL && (str[*start + 1] == '\''
			|| str[*start + 1] == '\"'))
		return ((*start)++, SUCCESS);
	if (ft_ischarset(str[*start + 1], SEPARATORS) || !str[*start + 1])
		return ((*start)++, ft_expand_separator(minishell, usable_str));
	(*start)++;
	if (str[*start] == '?')
		return (ft_expand_error_val(minishell, usable_str, start));
	while (str[*start] && ft_isalnum(str[*start]))
	{
		if (ft_buffer_add(buffer, str[*start]) != 0)
			return (BUFFER_FAIL);
		(*start)++;
	}
	path_env = ft_env_find(minishell->head_env, buffer);
	if (path_env && path_env->content && ft_strlcat(usable_str,
			path_env->content, BUFFER_SIZE) > BUFFER_SIZE)
		return (ft_error(minishell, BUFFER_FAIL, ERR_BUF_SIZE, NULL));
	return (SUCCESS);
}

void	ft_quotes_utils(t_token *token, t_minishell *minishell,
		char *usable_str)
{
	int	i;

	i = 0;
	while (token->str[i])
	{
		ft_state_detect(token->str[i], minishell);
		if (token->str[i] == '$' && minishell->state != IN_QUOTE)
		{
			if (minishell->state == NORMAL)
				token->should_split = 1;
			if (ft_expand(token->str, &i, usable_str, minishell))
				return ;
			continue ;
		}
		else if ((token->str[i] != '\'' || minishell->state == IN_DQUOTE)
			&& (token->str[i] != '\"' || minishell->state == IN_QUOTE))
		{
			if (ft_buffer_add(usable_str, token->str[i]))
			{
				ft_error(NULL, 0, ERR_BUF_SMALL, NULL);
				return ;
			}
		}
		i++;
	}
}
