/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 16:22:21 by tibras            #+#    #+#             */
/*   Updated: 2025/12/18 17:20:14 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap_srcs.h"

int	ft_init_str(t_list **stack, char *str)
{
	char	**arr;
	t_node	*n_new;
	t_list	*l_new;
	long	nb;
	int		i;

	arr = ft_split_charset((const char *)str, SPACE);
	if (!arr)
		return (-1);
	i = 0;
	while (arr[i])
	{
		nb = ft_atol(arr[i]);
		if (ft_overint(nb) || !ft_is_number(arr[i]))
			exit(ft_error_parse(arr, stack));
		n_new = ft_node_init((int)nb);
		if (!n_new)
			exit(ft_error_parse(arr, stack));
		l_new = ft_lstnew(n_new);
		ft_lstadd_back(stack, l_new);
		i++;
	}
	ft_free_array(arr);
	return (0);
}

int	ft_no_double(t_list *stack)
{
	t_list	*l_current;
	t_node	*n_compared;
	t_node	*n_current;

	while (stack->next)
	{
		l_current = stack->next;
		n_compared = stack->content;
		while (l_current)
		{
			n_current = l_current->content;
			if (n_current->value == n_compared->value)
				return (1);
			l_current = l_current->next;
		}
		stack = stack->next;
	}
	return (0);
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

int	ft_is_sorted(t_list **lst)
{
	int		max;
	t_list	*l_current;
	t_node	*n_current;

	l_current = *lst;
	max = INT_MIN;
	while (l_current)
	{
		n_current = ft_get_content(l_current);
		if (max > n_current->value)
			return (0);
		max = n_current->value;
		l_current = l_current->next;
	}
	return (1);
}

int	ft_parsing(t_list **stack, int argc, char **argv)
{
	int	i;

	i = 1;
	while (argv[i] && i < argc)
	{
		if (ft_init_str(stack, argv[i]))
			return (1);
		i++;
	}
	return (0);
}
