/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strmapi_gc.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 10:00:00 by tibras            #+#    #+#             */
/*   Updated: 2026/01/27 10:39:08 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strmapi_gc(char const *s, char (*f)(unsigned int, char), t_list **gc_head)
{
	char	*mapi;

	mapi = ft_strmapi(s, f);
	if (!mapi)
		return (NULL);
	if (ft_gc_add_node(gc_head, mapi))
	{
		free(mapi);
		return (NULL);
	}
	return (mapi);
}
