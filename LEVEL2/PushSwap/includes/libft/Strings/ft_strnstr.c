/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strnstr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/10 16:40:39 by tibras            #+#    #+#             */
/*   Updated: 2025/11/21 11:43:10 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strnstr(const char *haystack, const char *needle, size_t len)
{
	size_t	i;
	size_t	j;
	size_t	n_len;

	if (!*needle)
		return ((char *)haystack);
	i = 0;
	n_len = ft_strlen((char *)needle);
	while (haystack[i] && i < len)
	{
		j = 0;
		while (haystack[i + j] == needle[j] && needle[j] && i + j < len)
			j++;
		if (j == n_len)
			return ((char *)haystack + i);
		i++;
	}
	return (NULL);
}

// #include <stdio.h>
// #include <bsd/string.h>

// int main ()
// {
// 	const char *hay = "aaabcabcd";
// 	const char *needle = "cd";
// 	size_t len = 8;

// 	char *res1 = ft_strnstr(hay, needle, len);
// 	char *res2 = strnstr(hay, needle, len);
// 	printf("%s\n", res1);
// 	printf("%s\n", res2);
// 	if (res1 != res2)
// 		printf("Error\n");
// 	else
// 		printf("Oeeeee\n");
// }