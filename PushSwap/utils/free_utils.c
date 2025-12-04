/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/03 15:45:25 by tibras            #+#    #+#             */
/*   Updated: 2025/12/04 12:13:50 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap_utils.h"

int	ft_full_free(char **arr, t_list **stack, void (*del)(void *))
{
	ft_lstclear(stack, del);
	ft_free_array(arr);
	return (1);
}

// FREE TOUS LES ELEMENTS D'UN NOEUD
void	ft_node_del(void *content)
{
	t_node	*node;

	node = content;	
	if(!node)
		return;
	free(node->str);
	free(node);
}
