/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 18:30:19 by tibras            #+#    #+#             */
/*   Updated: 2025/12/20 13:50:35 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap_utils.h"

int	ft_is_number(char *nbr)
{	
	int	i;

	i = 0;
	if (nbr[0] == '-' || nbr[0] == '+')
	{
		if (!nbr[1] || !ft_isdigit(nbr[1]))
			return (0);
		i++;
	}
	while (nbr[i])
	{
		if (!ft_isdigit(nbr[i]))
			return (0);
		i++;
	}
	return (1);
}

t_list	*ft_lstmax(t_list *stack)
{
	t_node	*node;
	t_node	*n_max;
	t_list	*l_max;

	l_max = stack;
	while (stack)
	{
		node = ft_get_content(stack);
		n_max = ft_get_content(l_max);
		if (node->value > n_max->value)
			l_max = stack;
		stack = stack->next;
	}
	return (l_max);
}

t_list	*ft_lstmin(t_list *stack)
{
	t_node	*node;
	t_node	*min_node;
	t_list	*min;

	min = stack;
	while (stack)
	{
		node = ft_get_content(stack);
		min_node = ft_get_content(min);
		if (node->value < min_node->value)
			min = stack;
		stack = stack->next;
	}
	return (min);
}
