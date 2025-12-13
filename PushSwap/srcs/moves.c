/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   moves.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/13 17:30:14 by tibras            #+#    #+#             */
/*   Updated: 2025/12/13 19:03:31 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap_moves.h"

/*

A PRENDRE EN COMPTE : 
	Cout
	Mediane
	
	if (above_a && above_b)
	{
		tant que index_a > index_b
			ft_ra;
		tant que index_b > index_a
			ft_rb;
		tant que (index->b)
			ft_rr;
	}
	if (!above_a && !above_b)
	{
		tant que index_a > index_b
			ft_rra;
		tant que index_b > index_a
			ft_rrb;
		tant que (index->b)
			ft_rrr;
	}
	else if (!above_a && above_b)
	{
		while(index->a)
			ft_rra;
		while (index->b)
			ft_rb;
	}
	else if (above_a && !above_b)
	{
		while(index->a)
			ft_ra;
		while (index->b)
			ft_rrb;
	}
*/


void	ft_move(t_list *to_place, t_list **stack_a, t_list **stack_b)
{
	int		stack_len;
	t_node	*n_to_place;
	t_node	*n_target;
	t_list	*l_target;


	stack_len = ft_lstsize(*stack_b);
	n_to_place = ft_get_content(to_place);
	l_target = ft_get_target(n_to_place);
	n_target = ft_get_content(l_target);
	while (*stack_b != to_place)
	{
		if (n_target->above && n_to_place->above)
		{
			while (l_target->index > to_place->index)
			{
				ft_ra(stack_a, 1);
				l_target = ft_get_target(n_to_place);
				n_target = ft_get_content(l_target);
			}
			while (l_target->index < to_place->index)
			{
				ft_rb(stack_b, 1);
				l_target = ft_get_target(n_to_place);
				n_target = ft_get_content(l_target);
			}
			while (to_place->index)
			{
				ft_rr(stack_a, stack_b, 1);
				l_target = ft_get_target(n_to_place);
				n_target = ft_get_content(l_target);
			}
		}
		if (!n_target->above && !n_to_place->above)
		{
			while (l_target->index > to_place->index)
				ft_rra(stack_a, 1);
			while (l_target->index < to_place->index)
				ft_rra(stack_b, 1);
			while (to_place->index)
				ft_rrr(stack_a, stack_b, 1);
		}
	}
	ft_pa(stack_b, stack_a, 1);
	ft_lstprint_both(*stack_a, *stack_b);
}

// t_list	*to_place;
// t_node	*to_place_content = ft_get_content(to_place);
// t_list	*target = ft_get_target(to_place_content);