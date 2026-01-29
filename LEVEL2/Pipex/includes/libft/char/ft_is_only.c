/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_is_only.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/04 17:33:20 by tibras            #+#    #+#             */
/*   Updated: 2025/12/04 18:03:08 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_is_only(const char *s, int (*is)(int))
{
	int				i;
	size_t			count;
	unsigned char	uc;

	if (!s || !is)
		return (0);
	i = 0;
	count = 0;
	while (s[i])
	{
		uc = (unsigned char)s[i];
		if (is(uc))
			count++;
		i++;
	}
	if (count == ft_strlen(s))
		return (1);
	return (0);
}
