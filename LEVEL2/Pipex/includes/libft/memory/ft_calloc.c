/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_calloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/10 17:11:19 by tibras            #+#    #+#             */
/*   Updated: 2026/01/25 22:04:33 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_calloc(size_t count, size_t size)
{
	void	*tmp;
	size_t	range;

	if (size != 0 && count > (size_t) - 1 / size)
		return (NULL);
	range = size * count;
	tmp = malloc(range);
	if (!tmp)
		return (NULL);
	ft_bzero(tmp, range);
	return (tmp);
}
