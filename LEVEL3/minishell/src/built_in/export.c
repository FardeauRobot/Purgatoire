/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/12 20:27:53 by fardeau           #+#    #+#             */
/*   Updated: 2026/02/27 18:37:17 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ft_env_format_check(char *str)
{
	int	i;

	if (str[0] != '_' && !ft_isalpha(str[0]))
		return (GENERAL_ERROR);
	i = 1;
	while (str[i] && str[i] != '=')
	{
		if (!ft_isalnum(str[i]) && str[i] != '_')
			return (GENERAL_ERROR);
		i++;
	}
	return (SUCCESS);
}

static int	ft_create_or_update_env(t_minishell *minishell, char *str)
{
	t_env	*new;
	t_env	*old;

	new = ft_env_new(minishell, str);
	if (!new)
		return (ft_error(minishell, MALLOC_FAIL, ERR_MALLOC_EXPORT, NULL));
	old = ft_env_find(minishell->head_env, new->name);
	if (!old)
	{
		if (ft_env_add(minishell, new) == GENERAL_ERROR)
			return (ft_error(minishell, GENERAL_ERROR, ERR_ADD_EXPORT, NULL));
	}
	else
		old->content = new->content;
	return (SUCCESS);
}

int	ft_export(t_minishell *minishell, int fd, char **args)
{
	int	i;

	if (!args || !*args || !args[1])
		return (ft_export_null(minishell, fd), SUCCESS);
	i = 1;
	while (args[i])
	{
		if (args[i] && (ft_is_only(args[i],
					ft_isdigit) || (args[i][0] == '=' && !args[i][1])
					|| ft_env_format_check(args[i]) != SUCCESS))
			ft_error(minishell, GENERAL_ERROR,
				ft_strjoin_gc(ERR_EXPORT_PFX, args[i], &minishell->gc_tmp),
				ERR_INVALID_ID);
		else
			ft_create_or_update_env(minishell, args[i]);
		i++;
	}
	return (minishell->exit_status);
}
