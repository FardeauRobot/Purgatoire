/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_free_matrix.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/25 22:12:10 by tibras            #+#    #+#             */
/*   Updated: 2026/01/26 13:29:19 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_memory.h"

void	ft_free_matrix(char ***matrix)
{
	char	**arr;
	size_t	i;

	if (!matrix || !*matrix)
		return ;
	arr = *matrix;
	i = 0;
	while (arr[i])
	{
		free(arr[i]);
		i++;
	}
	free(arr);
	*matrix = NULL;
}
