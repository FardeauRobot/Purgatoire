/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strdup_gc.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 10:00:00 by tibras            #+#    #+#             */
/*   Updated: 2026/01/27 10:39:05 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strdup_gc(const char *str, t_list **gc_head)
{
	char	*dup;

	dup = ft_strdup(str);
	if (!dup)
		return (NULL);
	if (ft_gc_add_node(gc_head, dup))
	{
		free(dup);
		return (NULL);
	}
	return (dup);
}
