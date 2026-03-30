/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_list.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/23 12:52:37 by tibras            #+#    #+#             */
/*   Updated: 2026/02/27 20:47:18 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_cmd	*ft_cmd_new(t_minishell *minishell)
{
	t_cmd	*new;

	new = ft_calloc_gc(1, sizeof(t_cmd), &minishell->gc_line);
	if (!new)
		return (NULL);
	new->args = NULL;
	new->infd = STDIN_FILENO;
	new->outfd = STDOUT_FILENO;
	return (new);
}

int	ft_cmd_add(t_minishell *minishell, t_cmd *to_add)
{
	if (!minishell || !to_add)
		return (GENERAL_ERROR);
	ft_lstadd_back((t_list **)&minishell->head_cmd, (t_list *)to_add);
	return (SUCCESS);
}
