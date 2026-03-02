/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtrim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/10 15:27:29 by tibras            #+#    #+#             */
/*   Updated: 2025/11/21 11:43:32 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <stdio.h>	

static int	ft_is_set(char c, char const *set)
{
	int	i;

	i = 0;
	while (set[i])
	{
		if (c == set[i])
			return (1);
		i++;
	}
	return (0);
}

char	*ft_strtrim(char const *s1, char const *set)
{
	char	*str;
	size_t	i;
	size_t	j;
	size_t	end;

	i = 0;
	j = ft_strlen((char *)s1) - 1;
	while (ft_is_set(s1[i], set))
		i++;
	while (ft_is_set(s1[j], set) && j > i)
		j--;
	end = j + 1;
	str = malloc(sizeof(char) * (end - i) + 1);
	if (!str)
		return (NULL);
	j = 0;
	while (j + i < end)
	{
		str[j] = s1[i + j];
		j++;
	}
	str[j] = '\0';
	return (str);
}

// int main ()
// {
// 	const char *s1 = "    Cec|&|i es|$| t un test    ";
// 	// const char *s1 = "";
// 	const char *sep = "| ";
// 	char *res;

// 	res = ft_strtrim(s1, sep);
// 	printf("%s\n", res);
// }