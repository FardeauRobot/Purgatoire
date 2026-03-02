/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcpy.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/12 11:47:31 by tibras            #+#    #+#             */
/*   Updated: 2025/11/21 11:42:44 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcpy(char *dst, const char *src, size_t siz)
{
	size_t	i;
	size_t	s_len;

	i = 0;
	s_len = ft_strlen((char *)src);
	if (siz == 0)
		return (s_len);
	while (src[i] && i < siz - 1)
	{
		dst[i] = src[i];
		i++;
	}
	dst[i] = '\0';
	return (s_len);
}

// #include <stdio.h>
// #include <bsd/string.h>

// int main (int argc, char **argv)
// {
// 	char buf1[256];
// 	char buf2[256];
// 	size_t	s_diy;
// 	size_t	s_bin;
// 	size_t size = (size_t)atoi(argv[2]);

// 	ft_memset(buf1, '\0', sizeof(buf1));
// 	ft_memset(buf2, '\0', sizeof(buf2));
// 	s_diy = ft_strlcpy(buf1, argv[1], size);
// 	s_bin = strlcpy(buf2, argv[1], size);

// 	printf("DIY = %zu || %s\n", s_diy, buf1);
// 	printf("BIN = %zu || %s\n", s_bin, buf2);
// }