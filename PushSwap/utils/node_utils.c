/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   node_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/03 15:50:06 by tibras            #+#    #+#             */
/*   Updated: 2025/12/04 14:35:05 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap_utils.h"

void	ft_lstprint_nbr(t_list *lst)
{
	t_node	*tmp;

	if (!lst)
		return ;
	while (lst)
	{
		tmp = (t_node *)lst->content;
		if (tmp)
			ft_printf("%d\n", tmp->value);
		lst = lst->next;
	}
}

// CREER UN NOUVEAU NOEUD
t_node	*ft_node_init(long nb)
{
	t_node	*new;	

	new = malloc(sizeof(t_node));
	if (!new)
		return (NULL);
	new->value = (int)nb;
	new->index = -1;
	return (new);
}

