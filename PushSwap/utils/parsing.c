/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 16:22:21 by tibras            #+#    #+#             */
/*   Updated: 2025/11/29 18:10:03 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap_utils.h"

int ft_init_str(s_node **stack, char *str)
{
	char	**arr;
	t_node	*new;

	arr = ft_split((const *)str, ' ');
	while (arr[i])
	{
		node = ft_lstnew(ft_atoi(arr[i]));
		if (!node)
		{
			ft_lstclear(stack, free);
			return (-1);
		}
		ft_lstadd_back(stack, new);
		i++;
	}
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