/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   push.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/11 11:59:32 by tibras            #+#    #+#             */
/*   Updated: 2025/12/19 15:22:43 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap_moves.h"

void	ft_push(t_list **stack_src, t_list **stack_dest)
{
	t_list	*tmp;
	t_node	*node;

	if (!*stack_src)
		return ;
	tmp = *stack_src;
	*stack_src = (*stack_src)->next;
	tmp->next = *stack_dest;
	node = tmp->content;
	node->target = NULL;
	*stack_dest = tmp;
}

void	ft_pa(t_list **stack_b, t_list **stack_a, int check)
{
	ft_push(stack_b, stack_a);
	if (check)
		ft_printf("pa\n");
}

void	ft_pb(t_list **stack_a, t_list **stack_b, int check)
{
	ft_push(stack_a, stack_b);
	if (check)
		ft_printf("pb\n");
}
