/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rotate.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/04 16:30:33 by tibras            #+#    #+#             */
/*   Updated: 2025/12/18 17:15:07 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap_srcs.h"

void ft_rotate(t_list **lst)
{
	t_list	*first;
	t_list	*last;

	if (!lst || !*lst || !(*lst)->next)
		return ;
	first = *lst;
	*lst = first->next;
	last = ft_lstlast(*lst);
	last->next = first;
	first->next = NULL;
	ft_lstindex(lst);
}

void ft_ra(t_list **lst, int check)
{
	ft_rotate(lst);
	if (check)
		ft_printf("ra\n");
}

void	ft_rb(t_list **lst, int check)
{
	ft_rotate(lst);
	if (check)
		ft_printf("rb\n");
}

void ft_rr(t_list **la, t_list **lb, int check)
{
	ft_ra(la, 0);
	ft_rb(lb, 0);
	if (check)
		ft_printf("rr\n");
}