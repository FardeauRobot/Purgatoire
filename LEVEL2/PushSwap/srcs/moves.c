/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   moves.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/13 17:30:14 by tibras            #+#    #+#             */
/*   Updated: 2025/12/19 17:43:33 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap_moves.h"

/*
*/

static void	ft_rotate_both(t_list **stack_a, t_list **stack_b,
	int *moves, int reverse)
{
	while (*moves > 0)
	{
		if (reverse)
			ft_rrr(stack_a, stack_b, 1);
		else
			ft_rr(stack_a, stack_b, 1);
		(*moves)--;
	}
}

static void	ft_finish_rotate(t_list **stack, int *moves, int reverse,
		char stack_name)
{
	void	(*ft_move)(t_list **, int);

	if (reverse)
	{
		if (stack_name == 'a')
			ft_move = ft_rra;
		else
			ft_move = ft_rrb;
	}
	else
	{
		if (stack_name == 'a')
			ft_move = ft_ra;
		else
			ft_move = ft_rb;
	}
	while (*moves > 0)
	{
		ft_move(stack, 1);
		(*moves)--;
	}
}

void	ft_move(t_list *to_place, t_list **stack_a, t_list **stack_b)
{
	t_node	*n_to_place;
	t_node	*n_target;
	int		moves_a;
	int		moves_b;
	int		moves_both;

	n_to_place = ft_get_content(to_place);
	n_target = ft_get_content(ft_get_target(n_to_place));
	moves_a = n_target->to_top;
	moves_b = n_to_place->to_top;
	moves_both = ft_min_int(n_target->to_top, moves_b);
	if (n_to_place->above == n_target->above)
	{
		moves_a -= moves_both;
		moves_b -= moves_both;
		ft_rotate_both(stack_a, stack_b, &moves_both, !n_to_place->above);
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
