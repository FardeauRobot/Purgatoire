/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   convert.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamjada <alamjada@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/27 10:56:15 by tibras            #+#    #+#             */
/*   Updated: 2026/02/28 13:17:19 by alamjada         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	ft_ignore(void *ptr1, void *ptr2)
{
	(void)ptr1;
	(void)ptr2;
	print_header();
}

char	**ft_envp_build(t_minishell *minishell)
{
	int		size;
	char	**envp;
	int		i;
	t_env	*current;

	size = ft_lstsize((t_list *)minishell->head_env);
	envp = ft_gc_malloc((size + 1) * sizeof(char *), &minishell->gc_line);
	if (!envp)
	{
		ft_error(minishell, MALLOC_FAIL, ERR_MALLOC_ENV, NULL);
		return (NULL);
	}
	current = minishell->head_env;
	i = 0;
	while (i < size)
	{
		envp[i] = current->str;
		current = (t_env *)current->node.next;
		i++;
	}
	envp[i] = NULL;
	return (envp);
}
