/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 16:22:21 by tibras            #+#    #+#             */
/*   Updated: 2025/12/09 16:54:37 by tibras           ###   ########.fr       */
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
			return (ft_full_free(arr, stack, ft_node_del));
		n_new = ft_node_init((int)nb);
		if (!n_new)
			return (ft_full_free(arr, stack, ft_node_del));
		l_new = ft_lstnew(n_new);
		ft_lstadd_back(stack, l_new);
		i++;
	}
	ft_free_array(arr);
	return (0);
}

int ft_parsing(t_list **stack, int argc, char **argv)
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