/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strmapi.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/10 19:01:19 by tibras            #+#    #+#             */
/*   Updated: 2025/11/21 11:42:59 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strmapi(char const *s, char (*f)(unsigned int, char))
{
	size_t	size;
	char	*tmp;
	size_t	i;

	i = 0;
	size = ft_strlen((char *)s);
	tmp = malloc(sizeof(char) * size + 1);
	if (!tmp)
		return (NULL);
	while (s[i])
	{
		tmp[i] = f(i, s[i]);
		i++;
	}
	tmp[i] = '\0';
	return (tmp);
}

// char	ft_change(unsigned int index, char c)
// {
// 	if (index %= 2)
// 		return (c + 1);
// 	return (c);
// }

// #include <stdio.h>

// int main ()
// {
// 	const char *test = "";
// 	char *res;

// 	res = ft_strmapi(test, ft_change);
// 	printf("Adresse test = %p Value = %s \n", test, test);
// 	printf("Adresse res = %p = %s\n", res, res);
// 	free(res);
// }