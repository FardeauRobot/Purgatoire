/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   reverse_rotate.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/04 17:14:48 by tibras            #+#    #+#             */
/*   Updated: 2025/12/11 12:12:40 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap_srcs.h"

void	ft_reverse_rotate(t_list **lst)
{
	t_list	*first;
	t_list	*last;

	if (!lst || !*lst || !(*lst)->next)
		return ;
	first = *lst;
	last = ft_lstlast(*lst);
	last->previous->next = NULL;
	last->next = first;
	*lst = last;
}

void	ft_rra(t_list **lst, int check)
{
	ft_reverse_rotate(lst);
	if (check)
		ft_printf("rra\n");
}

void	ft_rrb(t_list **lst, int check)
{
	ft_reverse_rotate(lst);
	if (check)
		ft_printf("rrb\n");
}

void	ft_rrr(t_list **la, t_list **lb, int check)
{
	ft_rra(la, 0);
	ft_rra(lb, 0);
	if (check)
		ft_printf("rrr\n");
}
