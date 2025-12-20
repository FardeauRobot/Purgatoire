/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   diverse.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/04 17:29:13 by tibras            #+#    #+#             */
/*   Updated: 2025/12/20 13:51:04 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap_utils.h"

t_node	*ft_get_content(t_list *lst)
{
	if (lst)
		return (lst->content);
	return (NULL);
}

t_list	*ft_get_target(t_node *node)
{
	if (node)
		return (node->target);
	return (NULL);
}

t_node	*ft_get_node(t_list *lst)
{
	if (lst)
		return (lst->content);
	return (NULL);
}

void	ft_lstprint_nbr(t_list *lst)
{
	t_node	*tmp;

	if (!lst)
		return ;
	while (lst)
	{
		tmp = (t_node *)lst->content;
		if (tmp)
			ft_printf("%d\t||\n", tmp->value);
		lst = lst->next;
	}
}

void	ft_lstprint_both(t_list *la, t_list *lb)
{
	ft_printf("///// STACK A ///// \n\n");
	ft_lstprint_nbr(la);
	ft_printf("\n\n");
	ft_printf("///// STACK B ///// \n\n");
	ft_lstprint_nbr(lb);
	ft_printf("\n\n");
}
