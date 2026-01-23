/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/23 17:11:41 by tibras            #+#    #+#             */
/*   Updated: 2026/01/23 17:55:20 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex_srcs.h"

void	*ft_free_dbl_arr(void **arr, long size)
{
	long i;

	if (!arr)
		return (NULL);
	i = 0;
	if (size == -1)
	{
		while (arr[i])
			free(arr[i++]);
	}
	else
	{
		while (i++ < size)
			free(arr[i - 1]);
	}
	free(arr);
	return (NULL);
}