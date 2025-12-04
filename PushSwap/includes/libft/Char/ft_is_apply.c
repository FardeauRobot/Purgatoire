/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_is_apply.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/04 17:33:20 by tibras            #+#    #+#             */
/*   Updated: 2025/12/04 17:57:50 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_is_only(const char *s, int (*is)(int))
{
	int				i;
	unsigned char	uc;

	if (!s || !is)
		return (0);
	i = 0;
	while (s[i])
	{
		uc = (unsigned char)s[i];
		if (is(uc))
			return (1);
		i++;
	}
	return (0);
}
