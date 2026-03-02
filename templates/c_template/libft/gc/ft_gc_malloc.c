/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_gc_malloc.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 10:00:00 by tibras            #+#    #+#             */
/*   Updated: 2026/02/05 12:43:36 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_gc.h"

void	*ft_gc_malloc(size_t size, t_list **gc_head)
{
	void	*ptr;

	ptr = malloc(size);
	if (!ptr)
		return (NULL);
	if (ft_gc_add_node(gc_head, ptr) == 1)
	{
		free(ptr);
		return (NULL);
	}
	return (ptr);
}
