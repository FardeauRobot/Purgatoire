/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lis_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/20 13:52:24 by tibras            #+#    #+#             */
/*   Updated: 2025/12/20 17:10:23 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap_utils.h"

int	ft_is_in_lis(int *lis, int lis_len, int value)
{
	int	i;

	i = 0;
	while (i < lis_len)
	{
		if (value == lis[i])
			return (1);
		i++;
	}
	return (0);
}

void	ft_max_lis(int lis[], int len, int *max_len, int *max_index)
{
	int	i;

	i = 0;
	while (i < len)
	{
		if (lis[i] > *max_len)
		{
			*max_len = lis[i];
			*max_index = i;
		}
		i++;
	}
}
