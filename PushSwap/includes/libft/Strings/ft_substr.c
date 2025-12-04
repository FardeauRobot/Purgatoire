/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_substr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/10 16:31:44 by tibras            #+#    #+#             */
/*   Updated: 2025/11/21 11:43:41 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	char		*tmp;
	size_t		s_len;
	size_t		t_len;

	if (!s)
		return (NULL);
	s_len = ft_strlen((char *)s);
	if ((size_t)start >= s_len)
	{
		tmp = malloc(1);
		if (!tmp)
			return (NULL);
		tmp[0] = '\0';
		return (tmp);
	}
	t_len = s_len - (size_t)start;
	if (t_len > len)
		t_len = len;
	tmp = malloc(sizeof(char) * t_len + 1);
	if (!tmp)
		return (NULL);
	ft_strlcpy(tmp, s + start, t_len + 1);
	return (tmp);
}

// #include <stdio.h>
// int	main()
// {
// 	char *res;
// 	char *test1 = "tripouille";
// 	int start = 100;
// 	size_t len = 1;
// 	res = ft_substr(test1, start, len);
// 	printf("%s\n", res);
// }