/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_get_max.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/12 16:22:32 by tibras            #+#    #+#             */
/*   Updated: 2025/12/20 18:04:45 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_get_max(int *arr, int arr_len)
{
	int		i;
	int		max;

	i = 0;
	max = 0;
	while (i < arr_len)
	{
		if (arr[i] > max)
			max = arr[i];
		i++;
	}
	return (max);
}
