/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/11 14:44:59 by tibras            #+#    #+#             */
/*   Updated: 2025/12/11 15:02:14 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap_utils.h"

int	ft_error_parse(char **arr, t_list **stack)
{
	ft_putstr_fd("Error\n", 2);
	if (arr)
		ft_free_array(arr);
	if (*stack)
		ft_lstclear(stack, free);
	return (1);
}

int	ft_error_stacks(t_list **s_a, t_list **s_b)
{
	ft_putstr_fd("Error\n", 2);
	ft_clear_all(s_a, s_b);
	return (1);
}