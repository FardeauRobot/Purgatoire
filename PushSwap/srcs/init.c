/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/11 15:14:30 by tibras            #+#    #+#             */
/*   Updated: 2025/12/11 19:44:42 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap_srcs.h"

// CHECK SI n_index appartient au tableau LIS
static int	ft_belongs(t_list *current, int *lis_arr, int lis_len)
{
	int		index;
	int		i;

	if (!current)
		return (0);
	index = current->index;
	i = 0;
	while (i < lis_len)
	{
		if (lis_arr[i] == index)
			return (1);
		i++;
	}
	return (0);
}

static void	ft_dispatch(int *lis_arr, int lis_len, t_list **stack_a, t_list **stack_b)
{
	int		a_len;
	int		i;

	a_len = ft_lstsize(*stack_a);
	i = 0;
	while (i < a_len)
	{
		if (ft_belongs(*stack_a, lis_arr, lis_len))
			ft_ra(stack_a, 1);
		else
			ft_pb(stack_a, stack_b, 1);
		i++;
	}
}

static int	*ft_extract_values(t_list *stack, int len)
{
	int		*values;
	int		i;
	t_node	*node;

	values = malloc(sizeof(int) * len);
	if (!values)
		return (NULL);
	i = 0;
	while (stack && i < len)
	{
		node = stack->content;
		values[i] = node->value;
		stack = stack->next;
		i++;
	}
	return (values);
}

static void	ft_compute_lis(int *values, int *dp, int *prev, int len)
{
}

static int	ft_find_lis_end(int *dp, int len)
{
}

static int	*ft_find_lis(t_list **stack, int *lis_len)
{
}

void	ft_init(t_list **stack_a, t_list **stack_b)
{
	int		*lis_arr;
	int		lis_len;

	ft_lstindex(stack_a);
	lis_arr = ft_find_lis(stack_a, &lis_len);
	if (!lis_arr)
		exit(ft_error_stacks(stack_a, stack_b));
	ft_dispatch(lis_arr, lis_len, stack_a, stack_b);
	free(lis_arr);
}