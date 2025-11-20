/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strcmp.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 13:18:21 by tibras            #+#    #+#             */
/*   Updated: 2025/11/05 18:16:02 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

int	ft_strcmp(char *s1, char *s2)
{
	int	i;

	i = 0;
	while (s1[i] || s2[i])
	{
		if (s1[i] != s2[i])
			return (s1[i] - s2[i]);
		i++;
	}
	return (0);
}

// #include <stdio.h>
// #include <string.h>

// int main (void)
// {
// 	char *test1;
// 	char *test2;
// 	int res1;
// 	int res2;

// 	test1 = "TEST";
// 	test2 = "TeST";
// 	res1 = ft_strcmp(test1, test2);
// 	res2 = strcmp(test1, test2);
// 	printf("DIY = %d || B-IN = %d\n", res1, res2);
// }