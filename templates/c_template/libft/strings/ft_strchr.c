/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/08 14:38:44 by tibras            #+#    #+#             */
/*   Updated: 2025/11/21 15:37:52 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strchr(const char *s, int c)
{
	int				i;
	unsigned char	uc;

	i = 0;
	uc = (unsigned char)c;
	while (s[i])
	{
		if (s[i] == uc)
			return ((char *)s + i);
		i++;
	}
	if (!uc)
		return ((char *)s + i);
	return (NULL);
}

// #include <bsd/string.h>
// #include <stdio.h>

// int main ()
// {
// 	char *test = "teste";
// 	int c = 1024;
// 	char *res = strchr(test, c);
// 	char *res2 = ft_strchr(test, c);
// 	printf("%s\n", res);
// 	printf("%s\n", res2);
// }