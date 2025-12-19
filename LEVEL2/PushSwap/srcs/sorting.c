/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sorting.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/04 18:47:40 by tibras            #+#    #+#             */
/*   Updated: 2025/12/19 15:28:52 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap_srcs.h"

t_list	*ft_find_smallest(t_list *stack_a)
{
	t_node	*n_small;
	t_node	*node;
	t_list	*l_small;

	l_small = stack_a;
	n_small = ft_get_content(stack_a);
	while (stack_a)
	{
		node = ft_get_content(stack_a);
		if (node->value < n_small->value)
		{
			l_small = stack_a;
			n_small = ft_get_content(l_small);
		}
		stack_a = stack_a->next;
	}
	return (l_small);
}

// // Affecte a l'ensemble de la stack_b le noeud target
// void	ft_affect_target(t_list **stack_a, t_list **stack_b)
// {
// 	t_node	*n_a;
// 	t_node	*n_b;
// 	t_node	*n_target;
// 	t_list	*l_a;
// 	t_list	*l_b;
// 	t_list	*l_target;

// 	l_b = *stack_b;
// 	while (l_b)
// 	{
// 		n_b = ft_get_content(l_b);
// 		l_target = NULL;
// 		n_target = NULL;
// 		l_a = *stack_a;
// 		while (l_a)
// 		{
// 			n_a = ft_get_content(l_a);
// 			if (n_a->value > n_b->value)
// 			{
// 				if (!l_target || n_a->value < n_target->value)
// 				{
// 					l_target = l_a;
// 					n_target = ft_get_content(l_target);
// 				}
// 			}
// 			l_a = l_a->next;
// 		}
// 		if (!l_target)
// 			l_target = ft_find_smallest(*stack_a);
// 		n_b->target = l_target;
// 		l_b = l_b->next;
// 	}
// }

static t_list	*ft_find_target(t_list *stack_a, int value)
{
	t_node	*n_a;
	t_node	*n_target;
	t_list	*l_target;

	l_target = NULL;
	n_target = NULL;
	while (stack_a)
	{
		n_a = ft_get_content(stack_a);
		if (n_a->value > value)
		{
			if (!l_target || n_a->value < n_target->value)
			{
				l_target = stack_a;
				n_target = n_a;
			}
		}
		stack_a = stack_a->next;
	}
	return (l_target);
}

void	ft_affect_target(t_list **stack_a, t_list **stack_b)
{
	t_node	*n_b;
	t_list	*l_target;

	while (*stack_b)
	{
		n_b = ft_get_content(*stack_b);
		l_target = ft_find_target(*stack_a, n_b->value);
		if (!l_target)
			l_target = ft_find_smallest(*stack_a);
		n_b->target = l_target;
		*stack_b = (*stack_b)->next;
	}
}

void	ft_check_above(t_list **stack)
{
	t_list	*l_current;
	t_node	*n_current;
	int		l_len;

	l_current = *stack;
	l_len = ft_lstsize(*stack);
	while (l_current)
	{
		n_current = ft_get_content(l_current);
		if (l_current->index < l_len / 2)
			n_current->above = 1;
		else
			n_current->above = 0;
		l_current = l_current->next;
	}
}

int	ft_sorting(t_list **stack_a, t_list **stack_b)
{
	t_list	*l_to_move;
	t_node	*min;

	l_to_move = NULL;
	ft_init(stack_a, stack_b);
	while (*stack_b)
	{
		ft_affect_cost(stack_a, stack_b);
		l_to_move = ft_find_cheapest(*stack_b);
		ft_move(l_to_move, stack_a, stack_b);
	}
	min = ft_node_min(*stack_a);
	while ((*stack_a)->content != min && min->above == 1)
		ft_ra(stack_a, 1);
	while ((*stack_a)->content != min && min->above == 0)
		ft_rra(stack_a, 1);
	return (0);
}
