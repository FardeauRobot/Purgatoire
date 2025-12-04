/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_calloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/10 17:11:19 by tibras            #+#    #+#             */
/*   Updated: 2025/11/18 13:40:37 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_calloc(size_t count, size_t size)
{
	void	*tmp;
	size_t	range;
	size_t	i;

	if (size != 0 && count > (size_t) - 1 / size)
		return (NULL);
	i = 0;
	range = size * count;
	tmp = malloc(range);
	if (!tmp)
		return (NULL);
	ft_bzero(tmp, range);
	return (tmp);
}
