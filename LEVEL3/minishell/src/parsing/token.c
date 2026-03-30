/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/13 14:14:33 by tibras            #+#    #+#             */
/*   Updated: 2026/02/28 11:22:22 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// CREATION DU TOKEN A PARTIR DU BUFFER
t_token	*ft_token_create(t_minishell *minishell, char *buffer)
{
	t_token	*new_token;

	new_token = ft_calloc_gc(1, sizeof(t_token), &minishell->gc_line);
	if (!new_token)
		return (NULL);
	new_token->str = ft_strdup_gc(buffer, &minishell->gc_line);
	if (!new_token->str)
		return (NULL);
	new_token->type = DEFAULT;
	ft_bzero(buffer, ft_strlen(buffer));
	return (new_token);
}

// AJOUT DU TOKEN A LA LISTE CHAINEE *head_token (generic ft_lstadd_back)
int	ft_token_add(t_minishell *minishell, t_token *to_add)
{
	if (!minishell || !to_add)
		return (GENERAL_ERROR);
	ft_lstadd_back((t_list **)&minishell->head_token, (t_list *)to_add);
	return (SUCCESS);
}

int	ft_find_token(t_types type, t_token *head)
{
	while (head)
	{
		if (head->type == type)
			return (1);
		head = (t_token *)head->node.next;
	}
	return (0);
}
