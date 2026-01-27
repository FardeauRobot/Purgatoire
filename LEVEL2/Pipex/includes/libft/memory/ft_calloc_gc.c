/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_calloc_gc.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 10:00:00 by tibras            #+#    #+#             */
/*   Updated: 2026/01/27 09:50:53 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libft.h"

void	*ft_calloc_gc(size_t count, size_t size, t_list **gc_head)
{
	void	*tmp;
	size_t	range;

	if (size != 0 && count > (size_t) - 1 / size)
		return (NULL);
	range = size * count;
	tmp = ft_gc_malloc(range, gc_head);
	if (!tmp)
		return (NULL);
	ft_bzero(tmp, range);
	return (tmp);
}
