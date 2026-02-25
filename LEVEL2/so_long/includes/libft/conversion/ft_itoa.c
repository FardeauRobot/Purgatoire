/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 13:42:04 by tibras            #+#    #+#             */
/*   Updated: 2025/11/18 14:18:45 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	ft_get_size(int nb)
{
	int		size;
	long	tmp;

	tmp = (long)nb;
	size = 0;
	if (nb == 0)
		return (1);
	if (tmp < 0)
	{
		size++;
		tmp *= 1;
	}
	while (tmp)
	{
		tmp /= 10;
		size++;
	}
	return (size);
}

char	*ft_itoa(int n)
{
	int		size;
	int		limit;
	char	*res;
	long	tmp;

	size = ft_get_size(n);
	res = malloc(sizeof(char) * size + 1);
	if (!res)
		return (NULL);
	limit = 0;
	tmp = n;
	if (tmp < 0)
	{
		res[0] = '-';
		tmp *= -1;
		limit++;
	}
	res[size] = '\0';
	while (size-- > limit)
	{
		res[size] = tmp % 10 + '0';
		tmp /= 10;
	}
	return (res);
}

// #include <limits.h>
// #include <stdio.h>

// int main ()
// {
// 	char *str;
// 	int nb = INT_MIN;

// 	str = ft_itoa(nb);
// 	printf("STR = %s\n", str);
// 	free(str);
// }