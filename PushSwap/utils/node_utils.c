/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   node_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/03 15:50:06 by tibras            #+#    #+#             */
/*   Updated: 2025/12/13 14:05:54 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap_utils.h"

// TROUVE LE NOEUD AVEC VALUE MIN
t_node	*ft_lstmin(t_list *lst)
{
	t_node	*min;
	t_node	*n_tmp;

	min = NULL;
	while (lst)
	{
		n_tmp = lst->content;
		if (!min || n_tmp->value < min->value)
			min = n_tmp;
		lst = lst->next;
	}
	return (min);
}

// TROUVE LE NOEUD AVEC VALUE MAX
t_node	*ft_lstmax(t_list *lst)
{
	t_node	*max;
	t_node	*n_tmp;

	max = NULL;
	while (lst)
	{
		n_tmp = lst->content;
		if (!max || n_tmp->value > max->value)
			max = n_tmp;
		lst = lst->next;
	}
	return (max);
}

// CREER UN NOUVEAU NOEUD
t_node	*ft_node_init(long nb)
{
	t_node	*new;	

	new = malloc(sizeof(t_node));
	if (!new)
		return (NULL);
	new->value = (int)nb;
	new->cost = 0;
	new->target = NULL;
	return (new);
}

