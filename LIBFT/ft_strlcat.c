/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/10 19:32:19 by tibras            #+#    #+#             */
/*   Updated: 2025/11/12 13:16:35 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcat(char *dst, const char *src, size_t siz)
{	
	size_t	d_len;
	size_t	s_len;
	size_t	i;

	d_len = ft_strlen(dst);
	s_len = ft_strlen(src);
	i = 0;
	while (src[i] && i + d_len + 1 < siz)
	{
		dst[i + d_len] = src[i];
		i++;
	}
	if (siz > d_len)
	{
		dst[i + d_len] = '\0';
		return (d_len + s_len);
	}
	else
		return (siz + s_len);
}

// #include <stdio.h>
// #include <bsd/string.h>
// int main ()
// {
// 	char dest1[20] = "Coucou les";
// 	char dest2[20] = "Coucou les";
// 	char src1[20] = "COPAINS";
// 	char src2[20] = "COPAINS";

// 	size_t res1;	
// 	size_t res2;	
// 	size_t	size = 17;

// 	res1 = ft_strlcat(dest1, src1, size);
// 	res2 = strlcat(dest2, src2, size);
// 	printf("DIY = %s || Taille = %zu\n", dest1, res1);
// 	printf("BIN = %s || Taille = %zu\n", dest2, res2);
// }