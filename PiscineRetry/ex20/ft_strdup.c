/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strdup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 13:41:30 by tibras            #+#    #+#             */
/*   Updated: 2025/11/05 18:19:07 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>

size_t	ft_strlen(char *str)
{
	size_t	i;

	i = 0;
	while (str[i])
		i++;
	return (i);
}

char	*ft_strdup(char *src)
{
	char	*dst;
	int		size;
	int		i;

	if (!src)
		return (NULL);
	i = 0;
	size = ft_strlen(src);
	dst = malloc(sizeof(char) * size + 1);
	if (!dst)
		return (NULL);
	while (src[i])
	{
		dst[i] = src[i];
		i++;
	}
	dst[i] = '\0';
	return (dst);
}

// #include <stdio.h>

// int main ()
// {
// 	char *test;
// 	char *dst;

// 	test = "";
// 	dst = ft_strdup(test);
// 	printf("SRC = %s || DST = %s\n", test, dst);
// 	free(dst);
// }