/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/13 20:42:27 by alamjada          #+#    #+#             */
/*   Updated: 2026/02/27 22:12:08 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	ft_env_delone(t_env **head_env, char *target_name)
{
	t_env	*head;
	t_env	*prev;

	prev = NULL;
	head = *head_env;
	while (head)
	{
		if (ft_strcmp(head->name, target_name) == 0 && ft_strcmp(head->name,
				(*head_env)->name) == 0)
		{
			*head_env = (t_env *)head->node.next;
			head->node.next = NULL;
			return ;
		}
		else if (ft_strcmp(head->name, target_name) == 0)
		{
			prev->node.next = head->node.next;
			return ;
		}
		prev = head;
		head = (t_env *)head->node.next;
	}
}

void	ft_unset(t_minishell *minishell, char **args)
{
	int	i;

	if (!args || !*args || !args[1])
		return ;
	i = 1;
	while (args[i])
	{
		if (ft_env_format_check(args[i]) == SUCCESS)
			ft_env_delone((t_env **)&minishell->head_env, args[i]);
		else if (args[i][0] == '-')
			ft_error(minishell, ERR_SYNTAX, ERR_UNSET_PFX, ERR_INVALID_OPT);
		else
			ft_error(minishell, GENERAL_ERROR,
				ft_strjoin_gc(ERR_UNSET_PFX, args[i], &minishell->gc_tmp),
				ERR_INVALID_ID);
		i++;
	}
}
