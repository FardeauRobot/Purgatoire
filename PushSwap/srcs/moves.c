/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   moves.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/13 17:30:14 by tibras            #+#    #+#             */
/*   Updated: 2025/12/15 18:34:17 by tibras           ###   ########.fr       */
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
	t_node	*n_to_place;
	t_node	*n_target;
	t_list	*l_target;

	n_to_place = ft_get_content(to_place);
	l_target = ft_get_target(n_to_place);
	n_target = ft_get_content(l_target);
	if (n_to_place->above && n_target->above)
	{
		while (l_target->index > to_place->index)
			ft_ra(stack_a, 1);
		while (to_place->index > l_target->index)
			ft_rb(stack_b, 1);
		while (to_place->index != 0)
			ft_rr(stack_a, stack_b, 1);
	}
	else if (n_to_place->above && !n_target->above)
	{
		while (to_place->index != 0)
			ft_rb(stack_b, 1);
		while (l_target->index != 0)
		{
			ft_rra(stack_a, 1);
			// ft_printf("INDEX = %d\n", l_target->index);
		}
	}
	else if (!n_to_place->above && n_target->above)
	{
		while (to_place->index != 0)
			ft_rrb(stack_b, 1);
		while (l_target->index != 0)
			ft_ra(stack_a, 1);
	}
	else if (!n_to_place->above && !n_target->above)
	{
		while (l_target->index < to_place->index)
			ft_rra(stack_a, 1);
		while (to_place->index < l_target->index)
			ft_rrb(stack_b, 1);
		while (to_place->index != 0)
			ft_rrr(stack_a, stack_b, 1);
	}
	ft_pa(stack_b, stack_a, 1);
	return;
}
