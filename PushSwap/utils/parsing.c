/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 16:22:21 by tibras            #+#    #+#             */
/*   Updated: 2025/12/02 13:48:53 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap_utils.h"

int	ft_init_str(s_node **stack, char *str)
{
	char	**arr;
	t_node	*new;
	long	nb;
	int		i;

	arr = ft_split((const *)str, ' ');
	if (!arr)
		return (-1);
	i = 0;
	while (arr[i])
	{
		nb = ft_atol(arr[i]);
		if (ft_overint(nb))
			return (ft_full_free(arr, stack));
		new = ft_lstnew((int)nb);
		if (!new)
			return (ft_full_free(arr, stack));
		ft_lstadd_back(stack, new);
		i++;
	}
	ft_free_array(arr);
	ft_lstiter();
	return (0);
}

int ft_parsing(s_node **stack, int argc, char **argv)
{
	if (argc == 2)
		return (ft_init_str(stack, argv[1]));
	else if (argc > 2)
		return (ft_init_each(stack, argc, argv));
	else
		return (1);
}