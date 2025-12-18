/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cost.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 15:58:12 by tibras            #+#    #+#             */
/*   Updated: 2025/12/18 18:07:16 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap_srcs.h"

void	ft_compute_cost(t_list **stack_b)
{
	t_list	*l_current;
	t_list	*l_target;
	t_node	*n_target;
	t_node	*n_current;

	l_current = *stack_b;
	while (l_current)
	{
		n_current = ft_get_node(l_current);
		l_target = ft_get_target(n_current);
		n_target = ft_get_content(l_target);
		n_current->cost = n_current->to_top;
		n_current->cost += n_target->to_top;
		l_current = l_current->next;
	}
}

void	ft_get_cost(t_list **stack)
{
	int		l_len;
	int		index;
	t_list	*l_current;
	t_node	*n_current;

	l_current = *stack;
	l_len = ft_lstsize(*stack);
	while (l_current)
	{
		n_current = l_current->content;
		index = l_current->index;
		if (l_current->index > l_len / 2)
		{
			n_current->above = 0;
			n_current->to_top = l_len - index;
		}
		else
		{
			n_current->above = 1;
			n_current->to_top = index;
		}
		l_current = l_current->next;
	}
}

t_list	*ft_find_cheapest(t_list *stack)
{
	t_node	*n_cheapest;
	t_node	*n_current;
	t_list	*l_cheapest;

	l_cheapest = stack;
	while (stack)
	{
		n_cheapest = l_cheapest->content;
		n_current = stack->content;
		if (n_cheapest->cost > n_current->cost)
			l_cheapest = stack;
		stack = stack->next;
	}
	return (l_cheapest);
}

void	ft_affect_cost(t_list **stack_a, t_list **stack_b)
{
	ft_lstindex(stack_a);
	ft_lstindex(stack_b);
	ft_check_above(stack_a);
	ft_check_above(stack_b);
	ft_affect_target(stack_a, stack_b);
	ft_get_cost(stack_a);
	ft_get_cost(stack_b);
	ft_compute_cost(stack_b);
}
