/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_calloc_gc.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 10:00:00 by tibras            #+#    #+#             */
/*   Updated: 2026/02/05 12:30:23 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_calloc_gc(size_t count, size_t size, t_list **gc_head)
{
	void	*ptr;
	size_t	total_size;

	if (size != 0 && count > (size_t) - 1 / size)
		return (NULL);
	total_size = count * size;
	ptr = malloc(total_size);
	if (!ptr)
		return (NULL);
	if (ft_gc_add_node(gc_head, ptr) == 1)
	{
		free(ptr);
		return (NULL);
	}
	ft_bzero(ptr, total_size);
	return (ptr);
}
