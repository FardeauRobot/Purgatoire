/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sorting.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/04 18:47:40 by tibras            #+#    #+#             */
/*   Updated: 2025/12/13 14:29:32 by tibras           ###   ########.fr       */
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
		if (n_cheapest->value > n_current->value)
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
			n_current->to_top = index;
		else
			n_current->to_top = l_len - index;
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
		n_current = l_current->content;
		l_target = n_current->target;
		n_target = l_target->content;
		n_current->cost += n_target->cost;
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
	t_node	*b_node;
	t_list	*a_current;
	t_node	*a_node;
	t_node	*n_target;

	b_current = *stack_b;
	while (b_current)
	{
		b_node = b_current->content;
		n_target = ft_find_smallest(*stack_a);
		a_current = *stack_a;
		while (a_current)
		{
			a_node = a_current->content;
			if (a_node->value > b_node->value && a_node->value < n_target->value)
					b_node->target = a_current;
			a_current = a_current->next;
		}
		b_current = b_current->next;
	}

}

int	ft_sorting(t_list **stack_a, t_list **stack_b)
{
	t_list	*l_to_move;

	l_to_move = NULL;
	ft_init(stack_a, stack_b); // INITIALISE LA STACK B
	// while (*stack_b)
	// {
	ft_affect_target(stack_a, stack_b);
	ft_affect_cost(stack_a, stack_b);
	l_to_move = ft_find_cheapest(*stack_b); // A FAIRE
		// ft_move(stack_a, stack_b);
	// }
	ft_lstprint_both(*stack_a, *stack_b);
	ft_clear_all(stack_a, stack_b);
	return (0);
}