/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memmove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/08 16:46:32 by tibras            #+#    #+#             */
/*   Updated: 2025/11/27 18:34:57 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static void	*ft_cust_cpy(unsigned char *tmpd, unsigned char *tmps, size_t len)
{
	int	i;

	i = 0;
	if (tmpd < tmps)
	{
		while (i < (int)len)
		{
			tmpd[i] = tmps[i];
			i++;
		}
	}
	else
	{
		i = len;
		while (i > 0)
		{
			i--;
			tmpd[i] = tmps[i];
		}
	}
	return (tmpd);
}

void	*ft_memmove(void *dst, const void *src, size_t len)
{
	unsigned char	*tmpd;
	unsigned char	*tmps;

	if (!dst && !src)
		return (NULL);
	tmpd = (unsigned char *)dst;
	tmps = (unsigned char *)src;
	tmpd = ft_cust_cpy(tmpd, tmps, len);
	return (dst);
}
