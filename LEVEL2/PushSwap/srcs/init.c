/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/11 15:14:30 by tibras            #+#    #+#             */
/*   Updated: 2025/12/20 14:17:09 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap_srcs.h"

/*
 */

int	*ft_stack_to_arr(t_list *stack, int size)
{
	int		*arr;
	t_node	*node;
	int		i;

	arr = malloc(sizeof(int) * size);
	if (!arr)
		return (NULL);
	i = 0;
	while (stack)
	{
		node = ft_get_content(stack);
		arr[i] = node->value;
		i++;
		stack = stack->next;
	}
	return (arr);
}

void	ft_init_push(t_list **stack_a, t_list **stack_b, int median, int *s_len)
{
	t_node	*n_b;

	n_b = ft_get_content(*stack_b);
	if (n_b)
		if (ft_get_content(*stack_b)->value < median)
			ft_rb(stack_b, 1);
	(*s_len)--;
	ft_pb(stack_a, stack_b, 1);
}

void	ft_dispatch(t_list **stack_a, t_list **stack_b, int *lis, int lis_len)
{
	t_node	*n_current;
	int		s_len;
	int		min;
	int		max;
	long	median;

	min = ft_get_content(ft_lstmin(*stack_a))->value;
	max = ft_get_content(ft_lstmax(*stack_a))->value;
	median = (min + max) / 2;
	s_len = ft_lstsize(*stack_a);
	while (s_len > lis_len)
	{
		n_current = ft_get_content(*stack_a);
		if (ft_is_in_lis(lis, lis_len, n_current->value))
			ft_ra(stack_a, 1);
		else
			ft_init_push(stack_a, stack_b, median, &s_len);
	}
}

void	ft_init(t_list **stack_a, t_list **stack_b)
{
	int	stack_len;
	int	*lis;
	int	lis_len;

	lis_len = 0;
	lis = NULL;
	stack_len = ft_lstsize(*stack_a);
	ft_save_lis(stack_a, &lis_len, &lis);
	if (!lis)
		exit(ft_error_stacks(stack_a, stack_b));
	ft_dispatch(stack_a, stack_b, lis, lis_len);
	free(lis);
}
