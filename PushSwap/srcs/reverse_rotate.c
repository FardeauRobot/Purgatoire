/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   reverse_rotate.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/04 17:14:48 by tibras            #+#    #+#             */
/*   Updated: 2025/12/16 13:52:51 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap_srcs.h"

void	ft_reverse_rotate(t_list **lst)
{
	t_list	*first;
	t_list	*last;
	t_list	*second_last;

	if (!(*lst)->next)
	{
		t_node *test = ft_get_content(*lst);
		ft_printf(" LST INDEX = %d || VALUE = %d\n", (*lst)->index, test->value);
		return ;
	}
	first = *lst;
	last = ft_lstlast(*lst);
	second_last = *lst;
	while (second_last->next->next)
		second_last = second_last->next;
	second_last->next = NULL;
	last->next = first;
	*lst = last;
	ft_lstindex(lst);
	// ft_printf("INDEX = %d\n", (*lst)->index);
	// ft_lstindex(lst);
	// t_node *n_first = ft_get_content(first);
	// ft_printf("INDEX = %d, VALUE = %d\n", first->index, n_first->value);
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
	ft_rrb(lb, 0);
	if (check)
		ft_printf("rrr\n");
}
