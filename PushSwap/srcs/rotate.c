/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rotate.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/04 16:30:33 by tibras            #+#    #+#             */
/*   Updated: 2025/12/04 17:26:13 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap_srcs.h"

/*
ra (rotate a): Shift up all elements of stack a by 1.
	The first element becomes the last one.
rb (rotate b): Shift up all elements of stack b by 1.
	The first element becomes the last one.
*/

int ft_rotate(t_list **lst)
{
	t_list	*first;
	t_list	*last;

	if (!lst || !*lst || !(*lst)->next)
		return (1);
	first = *lst;
	*lst = first->next;
	last = ft_lstlast(*lst);
	last->next = first;
	first->next = NULL;
	return (0);
}

int ft_ra(t_list **lst, int check)
{
	if(ft_rotate(lst))
		return (1);
	if (check)
		ft_printf("ra\n");
	return (0);
}

int	ft_rb(t_list **lst, int check)
{
	if(ft_rotate(lst))
		return (1);
	if (check)
		ft_printf("rb\n");
	return (0);
}

int ft_rr(t_list **la, t_list **lb, int check)
{
	if (!ft_ra(la, 0))
		return (1);
	else if (!ft_rb(lb, 0))
		return (1);
	if (check)
		ft_printf("rr\n");
	return (0);
}