/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 16:22:21 by tibras            #+#    #+#             */
/*   Updated: 2025/11/29 18:21:06 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap_utils.h"

int ft_init_str(s_node **stack, char *str)
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
		new = ft_lstnew(ft_atoi(arr[i]));
		if (!new)
		{
			ft_free_array(arr);
			ft_lstclear(stack, free);
			return (-1);
		}
		ft_lstadd_back(stack, new);
		i++;
	}
	ft_free_array(arr);
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