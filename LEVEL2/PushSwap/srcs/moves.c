

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   moves.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/13 17:30:14 by tibras            #+#    #+#             */
/*   Updated: 2025/12/18 13:03:22 by tibras           ###   ########.fr       */
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

// void	ft_move(t_list *to_place, t_list **stack_a, t_list **stack_b)
// {
// 	t_node	*n_to_place;
// 	t_node	*n_target;
// 	t_list	*l_target;

// 	n_to_place = ft_get_content(to_place);
// 	l_target = ft_get_target(n_to_place);
// 	n_target = ft_get_content(l_target);
// 	if (n_to_place->above && n_target->above)
// 	{
// 		while (l_target->index > to_place->index)
// 			ft_ra(stack_a, 1);
// 		while (to_place->index > l_target->index)
// 			ft_rb(stack_b, 1);
// 		while (to_place->index != 0)
// 			ft_rr(stack_a, stack_b, 1);
// 	}
// 	else if (n_to_place->above && !n_target->above)
// 	{
// 		while (to_place->index != 0)
// 			ft_rb(stack_b, 1);
// 		while (l_target->index != 0)
// 			ft_rra(stack_a, 1);
// 	}
// 	else if (!n_to_place->above && n_target->above)
// 	{
// 		while (to_place->index != 0)
// 			ft_rrb(stack_b, 1);
// 		while (l_target->index != 0)
// 			ft_ra(stack_a, 1);
// 	}
// 	else if (!n_to_place->above && !n_target->above)
// 	{
// 		while (l_target->index < to_place->index)
// 			ft_rra(stack_a, 1);
// 		while (to_place->index < l_target->index)
// 			ft_rrb(stack_b, 1);
// 		while (to_place->index != 0)
// 			ft_rrr(stack_a, stack_b, 1);
// 	}
// 	ft_pa(stack_b, stack_a, 1);
// 	return ;
// }

/* Helper function to rotate both stacks simultaneously */
static void	ft_rotate_both(t_list **stack_a, t_list **stack_b, int *moves_a,
		int *moves_b, int reverse)
{
	while (*moves_a > 0 && *moves_b > 0)
	{
		if (reverse)
			ft_rrr(stack_a, stack_b, 1);
		else
			ft_rr(stack_a, stack_b, 1);
		(*moves_a)--;
		(*moves_b)--;
	}
}

/* Helper function to finish rotating a single stack */
static void	ft_finish_rotate(t_list **stack, int *moves, int reverse,
		char stack_name)
{
	void	(*rotate_func)(t_list **, int);

	if (reverse)
		rotate_func = (stack_name == 'a') ? ft_rra : ft_rrb;
	else
		rotate_func = (stack_name == 'a') ? ft_ra : ft_rb;
	while (*moves > 0)
	{
		rotate_func(stack, 1);
		(*moves)--;
	}
}

void	ft_move(t_list *to_place, t_list **stack_a, t_list **stack_b)
{
	t_node *n_to_place;
	t_node *n_target;
	int moves_a;
	int moves_b;

	n_to_place = ft_get_content(to_place);
	n_target = ft_get_content(ft_get_target(n_to_place));
	moves_a = n_target->to_top;
	moves_b = n_to_place->to_top;

	if (n_to_place->above == n_target->above)
	{
		ft_rotate_both(stack_a, stack_b, &moves_a, &moves_b,
			!n_to_place->above);
		ft_finish_rotate(stack_a, &moves_a, !n_target->above, 'a');
		ft_finish_rotate(stack_b, &moves_b, !n_to_place->above, 'b');
	}
	else
	{
		ft_finish_rotate(stack_b, &moves_b, !n_to_place->above, 'b');
		ft_finish_rotate(stack_a, &moves_a, !n_target->above, 'a');
	}
	ft_pa(stack_b, stack_a, 1);
}