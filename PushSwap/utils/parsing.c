/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 16:22:21 by tibras            #+#    #+#             */
/*   Updated: 2025/12/03 15:54:23 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap_utils.h"

int	ft_init_str(t_list **stack, char *str)
{
	char	**arr;
	t_list	*new;
	long	nb;
	int		i;

	arr = ft_split((const char*)str, ' ');
	if (!arr)
		return (-1);
	i = 0;
	while (arr[i])
	{
		nb = ft_atol(arr[i]);
		if (ft_overint(nb))
		{
			ft_free_array(arr);
			return (ft_lstclear(stack, ft_node_del));
		}
		new = ft_lstnew((int)nb);
		if (!new)
			return (ft_full_free(arr, stack));
		ft_lstadd_back(stack, new);
		i++;
	}
	ft_free_array(arr);
	ft_lstprint_nbr(*stack);
	return (0);
}

int ft_parsing(t_list **stack, int argc, char **argv)
{
	if (argc == 2)
		return (ft_init_str(stack, argv[1]));
	else if (argc > 2)
		return (ft_init_each(stack, argc, argv));
	else
		return (1);
}