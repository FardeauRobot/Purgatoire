/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/11 15:14:30 by tibras            #+#    #+#             */
/*   Updated: 2025/12/18 18:32:57 by tibras           ###   ########.fr       */
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

void	ft_dispatch(t_list **stack_a, t_list **stack_b, int *lis, int lis_len)
{
	t_node	*n_current;
	t_node	*n_b;
	int		s_len;
	int		i;

	i = 0;
	s_len = ft_lstsize(*stack_a);
	while (i < s_len)
	{
		n_current = ft_get_content(*stack_a);
		if (ft_is_in_lis(lis, lis_len, n_current->value))
		{
			i++;
			ft_rra(stack_a, 1);
		}
		else
		{
			n_b = ft_get_content(*stack_b);
			if (n_b)
				if (n_current->value < n_b->value)
					ft_rb(stack_b, 1);
			ft_pb(stack_a, stack_b, 1);
		}
	}
}

void	ft_init(t_list **stack_a, t_list **stack_b)
{
	int	stack_len;
	int	*lis;
	int	lis_len;

	lis_len = 0;
	stack_len = ft_lstsize(*stack_a);
	lis = ft_save_lis(stack_a, &lis_len);
	ft_dispatch(stack_a, stack_b, lis, lis_len);
	free(lis);
}
