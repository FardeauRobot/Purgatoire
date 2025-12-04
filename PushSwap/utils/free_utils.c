/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/03 15:45:25 by tibras            #+#    #+#             */
/*   Updated: 2025/12/04 14:38:20 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap_utils.h"

int	ft_full_free(char **arr, t_list **stack, void (*del)(void *))
{
	if (arr)
		ft_free_array(arr);
	if (stack && *stack)
		ft_lstclear(stack, del);
	return (1);
}

// FREE TOUS LES ELEMENTS D'UN NOEUD
void	ft_node_del(void *content)
{
	t_node	*node;

	node = (t_node *) content;	
	if(!node)
		return;
	if (node->str)
		free(node->str);
	free(node);
}
