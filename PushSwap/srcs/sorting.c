/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sorting.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/04 18:47:40 by tibras            #+#    #+#             */
/*   Updated: 2025/12/13 18:40:49 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap_srcs.h"

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
		if (n_current->above == n_target->above)
		{
			n_current->cost = (n_current->to_top > n_target->to_top)
				? n_current->to_top : n_target->to_top;
		}
		else
			n_current->cost = n_current->to_top + n_target->to_top;
		l_current = l_current->next;
	}
}

void	ft_affect_cost(t_list **stack_a, t_list **stack_b)
{
	ft_get_cost(stack_a);
	ft_get_cost(stack_b);	
	ft_compute_cost(stack_b);
}

//  cherche la target dans la stack_a pour t_node
t_node	*ft_find_smallest(t_list *stack_a)
{
	t_node *small;
	t_node *node;
	t_list *current;

	small = stack_a->content;
	current = stack_a->next;
	while (current)
	{
		node = current->content;
		if (node->value < small->value)
			small = current->content;
		current = current->next;
	}
	return (small);
}

// Affecte a l'ensemble de la stack_b le noeud target
void	ft_affect_target(t_list **stack_a, t_list **stack_b)
{
	t_list	*b_current;
	t_list	*a_current;
	t_list	*l_target;
	t_node	*a_node;
	t_node	*b_node;
	t_node	*n_target;

	b_current = *stack_b;
	while (b_current)
	{
		b_node = ft_get_content(b_current);
		l_target = NULL;
		n_target = NULL;
		a_current = *stack_a;
		while (a_current)
		{
			a_node = ft_get_content(a_current);
			if (a_node->value > b_node->value
				&& (!n_target || a_node->value < n_target->value))
			{
				l_target = a_current;
				n_target = a_node;
			}
			a_current = a_current->next;
		}
		if (!l_target)
		{
			a_current = *stack_a;
			l_target = a_current;
			n_target = ft_get_content(a_current);
			while (a_current)
			{
				a_node = ft_get_content(a_current);
				if (a_node->value < n_target->value)
				{
					l_target = a_current;
					n_target = a_node;
				}
				a_current = a_current->next;
			}
		}
		b_node->target = l_target;
		b_current = b_current->next;
	}
}

int	ft_sorting(t_list **stack_a, t_list **stack_b)
{
	t_list	*l_to_move;
	t_list	*l_min;
	t_node	*n_min;

	l_to_move = NULL;
	ft_init(stack_a, stack_b); // INITIALISE LA STACK B
	while (*stack_b)
	{
		ft_lstindex(stack_a);
		ft_lstindex(stack_b);
		ft_affect_target(stack_a, stack_b);
		ft_affect_cost(stack_a, stack_b);
		l_to_move = ft_find_cheapest(*stack_b); // A FAIRE
		ft_move(l_to_move, stack_a, stack_b);
	}
	ft_lstindex(stack_a);
	l_min = *stack_a;
	n_min = ft_node_min(*stack_a);
	while (ft_get_content(l_min) != n_min)
		l_min = l_min->next;
	if (l_min->index <= ft_lstsize(*stack_a) / 2)
	{
		while (*stack_a != l_min)
			ft_ra(stack_a, 1);
	}
	else
	{
		while (*stack_a != l_min)
			ft_rra(stack_a, 1);
	}
	ft_clear_all(stack_a, stack_b);
	return (0);
}