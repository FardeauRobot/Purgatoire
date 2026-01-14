/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mini_sorting.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/20 17:36:42 by tibras            #+#    #+#             */
/*   Updated: 2026/01/14 12:11:08 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap_srcs.h"

void	ft_mini_sorting(t_list **s_a, t_list **stack_b)
{
	t_list	*min;
	t_list	*max;

	while (ft_lstsize(*s_a) > 3)
		ft_pb(s_a, stack_b, 1);
	min = ft_lstmin(*s_a);
	max = ft_lstmax(*s_a);
	if (max == *s_a)
		ft_ra(s_a, 1);
	else if (max != *s_a && max != ft_lstlast(*s_a))
		ft_rra(s_a, 1);
	if (ft_get_content(*s_a)->value > ft_get_content((*s_a)->next)->value)
		ft_sa(s_a, 1);
	ft_sorting(s_a, stack_b);
	return ;
}
