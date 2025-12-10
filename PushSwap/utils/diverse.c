/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   diverse.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/04 17:29:13 by tibras            #+#    #+#             */
/*   Updated: 2025/12/10 18:32:42 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap_utils.h"

int	ft_print_error(int value)
{
	ft_putstr_fd("Error\n", 2);
	return (value);
}

void	ft_lstprint_both(t_list *la, t_list *lb)
{
	ft_printf("STACK A");
	ft_lstprint_nbr(la);
	ft_printf("STACK B");
	ft_lstprint_nbr(lb);
}