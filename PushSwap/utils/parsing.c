/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 16:22:21 by tibras            #+#    #+#             */
/*   Updated: 2025/12/11 14:35:24 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap_utils.h"

int	ft_init_str(t_list **stack, char *str)
{
	char	**arr;
	t_node	*n_new;
	t_list	*l_new;
	long	nb;
	int		i;

	arr = ft_split_charset((const char*)str, SPACE);
	if (!arr)
		return (-1);
	i = 0;
	while (arr[i])
	{
		nb = ft_atol(arr[i]);
		if (ft_overint(nb) || !ft_is_number(arr[i]))
			exit(ft_free_parse(arr, stack));
		n_new = ft_node_init((int)nb);
		if (!n_new)
			exit(ft_free_parse(arr, stack));
		l_new = ft_lstnew(n_new);
		ft_lstadd_back(stack, l_new);
		i++;
	}
	ft_free_array(arr);
	return (0);
}

int	ft_no_double(t_list *stack)
{
	t_list 	*l_current;
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

int	ft_is_sorted(t_list *lst)
{
	int		max;
	t_node	*current;

	max = INT_MIN;
	while (lst)
	{
		current = lst->content;
		if (max > current->value)
			return (0);
		max = current->value;
		lst = lst->next;
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
