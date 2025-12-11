/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   node_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/03 15:50:06 by tibras            #+#    #+#             */
/*   Updated: 2025/12/11 13:56:58 by tibras           ###   ########.fr       */
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
	new->index = 0;
	new->to_top = 0;
	new->above = 0;
	new->target = NULL;
	return (new);
}

