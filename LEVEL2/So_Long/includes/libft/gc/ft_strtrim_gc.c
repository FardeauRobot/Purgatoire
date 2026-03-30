/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtrim_gc.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 10:00:00 by tibras            #+#    #+#             */
/*   Updated: 2026/01/27 10:39:08 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strtrim_gc(char const *s1, char const *set, t_list **gc_head)
{
	char	*trim;

	trim = ft_strtrim(s1, set);
	if (!trim)
		return (NULL);
	if (ft_gc_add_node(gc_head, trim))
	{
		free(trim);
		return (NULL);
	}
	return (trim);
}
