/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   diverse.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/04 17:29:13 by tibras            #+#    #+#             */
/*   Updated: 2025/12/11 19:00:54 by tibras           ###   ########.fr       */
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

void	ft_lstprint_both(t_list *la, t_list *lb)
{
	ft_printf("STACK A : \n\n");
	ft_lstprint_nbr(la);
	ft_printf("STACK B : \n\n");
	ft_lstprint_nbr(lb);
}