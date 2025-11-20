/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_sqrt.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 13:07:35 by tibras            #+#    #+#             */
/*   Updated: 2025/11/05 18:15:16 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

int	ft_sqrt(int nb)
{
	int	n;

	if (nb < 0)
		return (0);
	n = 0;
	while (n <= 46340)
	{
		if (n * n == nb)
			return (n);
		n++;
	}
	return (0);
}

// #include <stdio.h>

// int main ()
// {
// 	int res;

// 	res = ft_sqrt(9);
// 	printf("RES = %d\n", res);
// 	res = ft_sqrt(0);
// 	printf("RES = %d\n", res);
// 	res = ft_sqrt(-2147395600);
// 	printf("RES = %d\n", res);
// }