/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strcheck.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/03 16:12:45 by tibras            #+#    #+#             */
/*   Updated: 2025/12/20 18:04:56 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_strcheck(const char *s, int (*check)(int))
{
	int	i;

	if (!s || !check)
		return (0);
	i = 0;
	while (s[i])
	{
		if (!check(s[i]))
			return (0);
		i++;
	}
	return (1);
}
