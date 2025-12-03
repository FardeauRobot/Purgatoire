/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/03 15:45:25 by tibras            #+#    #+#             */
/*   Updated: 2025/12/03 15:55:13 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap_utils.h"

// FREE TOUS LES ELEMENTS D'UN NOEUD
static void	ft_node_del(void *content)
{
	t_node	*node;

	node = content;	
	if(!node)
		return;
	free(node->str);
	free(node);
}
