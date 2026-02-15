/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_matrix_len.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/25 22:12:30 by tibras            #+#    #+#             */
/*   Updated: 2026/01/26 12:44:41 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_strings.h"

size_t	ft_matrix_len(char **matrix)
{
	int i;

	if (!matrix || !*matrix)
		return (0);
	i = 0;	
	while (matrix[i])
		i++;
	return (i);
}
