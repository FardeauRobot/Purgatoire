/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sorting.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/04 18:47:40 by tibras            #+#    #+#             */
/*   Updated: 2025/12/11 19:18:23 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap_srcs.h"

int	ft_sorting(t_list **stack_a, t_list **stack_b)
{
	// while (!ft_is_sorted(*stack_a) && stack_b)
	if (ft_is_sorted(*stack_a))
	{
		ft_lstclear(stack_a, free);
		exit (0);
	}
	// ft_lstprint_nbr(*stack_a);
	// ft_lstprint_nbr(*stack_b);
	ft_lstprint_both(*stack_a, *stack_b);
	ft_clear_all(stack_a, stack_b);
	return (0);
}