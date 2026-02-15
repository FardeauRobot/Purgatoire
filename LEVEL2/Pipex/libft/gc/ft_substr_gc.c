/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_substr_gc.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 10:00:00 by tibras            #+#    #+#             */
/*   Updated: 2026/01/27 10:39:08 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_substr_gc(char const *s, unsigned int start, size_t len, t_list **gc_head)
{
	char	*sub;

	sub = ft_substr(s, start, len);
	if (!sub)
		return (NULL);
	if (ft_gc_add_node(gc_head, sub))
	{
		free(sub);
		return (NULL);
	}
	return (sub);
}
