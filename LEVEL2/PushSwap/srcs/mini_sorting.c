/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mini_sorting.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/20 17:36:42 by tibras            #+#    #+#             */
/*   Updated: 2026/01/14 11:54:26 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap_srcs.h"


void	ft_mini_sorting(t_list **stack_a, t_list **stack_b)
{
	t_list	*min;
	t_list	*max;

	while (ft_lstsize(*stack_a) > 3)
		ft_pb(stack_a, stack_b, 1);
	min = ft_lstmin(*stack_a);
	max = ft_lstmax(*stack_a);
	if (max == *stack_a)
		ft_ra(stack_a, 1);
	else if (max != *stack_a && max != ft_lstlast(*stack_a))
		ft_rra(stack_a, 1);
	if (ft_get_content(*stack_a)->value > ft_get_content((*stack_a)->next)->value)
		ft_sa(stack_a, 1);
	ft_sorting(stack_a, stack_b);
	return ;
}