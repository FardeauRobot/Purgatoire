/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 16:22:21 by tibras            #+#    #+#             */
/*   Updated: 2026/01/14 18:10:02 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap_srcs.h"

void	ft_init_str(t_list **stack, char *str)
{
	char	**arr;
	t_node	*n_new;
	t_list	*l_new;
	long	nb;
	int		i;

	arr = ft_split_charset((const char *)str, SPACE);
	if (!arr)
		exit(ft_error_parse(arr, stack));
	i = 0;
	while (arr[i])
	{
		nb = ft_atol(arr[i]);
		if (ft_overint(nb) || !ft_is_number(arr[i]))
			exit(ft_error_parse(arr, stack));
		n_new = ft_node_init((int)nb);
		l_new = ft_lstnew(n_new);
		if (!n_new || !l_new)
			exit(ft_error_init(stack, arr, n_new, l_new));
		ft_lstadd_back(stack, l_new);
		i++;
	}
	ft_free_array(arr);
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
		ft_init_str(stack, argv[i]);
		i++;
	}
	return (0);
}

"19 29 1"		1			5
	argv 2		argv3		argv4