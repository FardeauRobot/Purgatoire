/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_recursive_factorial.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 13:04:30 by tibras            #+#    #+#             */
/*   Updated: 2025/11/05 18:14:53 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

int	ft_recursive_factorial(int nb)
{
	if (nb < 0)
		return (0);
	else if (nb == 1 || nb == 0)
		return (1);
	else
		return (nb * ft_recursive_factorial(nb - 1));
}
// #include <stdio.h>

// int main ()
// {
// 	int res;

// 	res = ft_recursive_factorial(10);
// 	printf("Res = %d\n", res);
// 	res = ft_recursive_factorial(0);
// 	printf("Res = %d\n", res);
// 	res = ft_recursive_factorial(-4);
// 	printf("Res = %d\n", res);
// }