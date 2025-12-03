/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strrchr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/09 16:12:58 by tibras            #+#    #+#             */
/*   Updated: 2025/11/21 11:43:22 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strrchr(const char *s, int c)
{
	int				len;
	unsigned char	uc;

	len = (int)ft_strlen((char *)s);
	uc = (unsigned char)c;
	if (!uc)
		return ((char *)s + len);
	else if (s[0])
		len--;
	while (len >= 0)
	{
		if (s[len] == uc)
			return ((char *)s + len);
		len--;
	}
	return (NULL);
}

// #include <stdio.h>
// #include <string.h>
// int main ()
// {
// 	const char s[] = "abbbbbbbb";
// 	// char *empty = malloc(1);
// 	// if (!empty)
// 	// 	return (1);
// 	// const char s2[] = "ltripouiel";
// 	// const char s3[] = "";
// 	int c = 'a';
// 	char *res1;
// 	char *res2;

// 	res1 = ft_strrchr(s, c);
// 	res2 = strrchr(s, c);
// 	printf("DIY = %s\n", res1);
// 	printf("BIN = %s\n", res2);
// 	// free(empty);
// }