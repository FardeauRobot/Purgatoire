/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memswap.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/25 22:15:00 by tibras            #+#    #+#             */
/*   Updated: 2026/01/26 13:23:39 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_memory.h"

void	ft_memswap(void *a, void *b, size_t size)
{
	unsigned char *uns_a;
	unsigned char *uns_b;
	unsigned char tmp;
	size_t i;

	if (!a || !b)
		return ;
	uns_a = (unsigned char *)a;
	uns_b = (unsigned char *)b;
	i = 0;
	while (i < size)
	{
		tmp = uns_b[i];
		uns_b[i] = uns_a[i];
		uns_a[i] = tmp;
		i++;
	}
}
