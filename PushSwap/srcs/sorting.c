/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sorting.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/04 18:47:40 by tibras            #+#    #+#             */
/*   Updated: 2025/12/12 18:51:28 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap_srcs.h"

void	ft_affect_cost(t_list **stack_a, t_list**stack_b)
{
	t_list *b_current;

	b_current = *stack_b;
	while (b_current)
	{
		ft_compute_cost(b_current->content, *stack_a);
		b_current = b_current->next;
	}
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
	t_node	*target;

	b_current = *stack_b;
	while (b_current)
	{
		b_node = b_current->content;
		target = ft_find_smallest(*stack_a);
		a_current = *stack_a;
		while (a_current)
		{
			a_node = a_current->content;
			if (a_node->value > b_node->value && a_node->value < target->value)
					target = a_node;
			a_current = a_current->next;
		}
		b_current = b_current->next;
	}

}

int	ft_sorting(t_list **stack_a, t_list **stack_b)
{
	ft_init(stack_a, stack_b); // INITIALISE LA STACK B
	while (!ft_is_sorted(*stack_a, *stack_b))
	{
		ft_affect_target(stack_a, stack_b);
		ft_affect_cost(stack_a, stack_b);
		ft_move(stack_a, stack_b);
	}
	ft_lstprint_both(*stack_a, *stack_b);
	ft_clear_all(stack_a, stack_b);
	return (0);
}