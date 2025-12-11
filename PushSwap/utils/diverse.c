/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   diverse.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/04 17:29:13 by tibras            #+#    #+#             */
/*   Updated: 2025/12/11 14:30:58 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap_utils.h"

int	ft_print_error(void)
{
	ft_putstr_fd("Error\n", 2);
	return (1);
}

int	ft_free_parse(char **arr, t_list **stack)
{
	if (arr)
		ft_free_array(arr);
	if (*stack)
		ft_lstclear(stack, free);
	return (1);
}

int	ft_error(t_list **s_a, t_list **s_b)
{
	ft_print_error();
	ft_clear_all(s_a, s_b);
	return (1);
}

void	ft_lstprint_both(t_list *la, t_list *lb)
{
	ft_printf("STACK A");
	ft_lstprint_nbr(la);
	ft_printf("STACK B");
	ft_lstprint_nbr(lb);
}