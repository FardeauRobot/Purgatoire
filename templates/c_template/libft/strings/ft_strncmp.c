/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strncmp.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/18 13:59:13 by tibras            #+#    #+#             */
/*   Updated: 2025/11/18 13:59:40 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_strncmp(const char *s1, const char *s2, size_t n)
{
	size_t			i;
	unsigned char	*tmp1;
	unsigned char	*tmp2;

	i = 0;
	tmp1 = (unsigned char *)s1;
	tmp2 = (unsigned char *)s2;
	while ((tmp1[i] || tmp2[i]) && i < n)
	{
		if (tmp1[i] != tmp2[i])
			return (tmp1[i] - tmp2[i]);
		i++;
	}
	return (0);
}

// int main ()
// {
// 	const char *s1 = "abc";
// 	const char *s2 = "abc";
// 	size_t size =3;

// 	int res1 = ft_strncmp(s1, s2, size);
// 	int res2 = strncmp(s1, s2, size);
// 	printf("DIY = %d || B-IN = %d\n", res1, res2);
// }