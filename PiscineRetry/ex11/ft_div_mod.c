/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_div_mod.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 12:55:42 by tibras            #+#    #+#             */
/*   Updated: 2025/11/05 17:43:25 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

void	ft_div_mod(int a, int b, int *div, int *mod)
{
	*div = a / b;
	*mod = a % b;
}

// #include <stdio.h>
// int main ()
// {
// 	int a = 2;
// 	int b = 10;
// 	int div = 0;
// 	int mod = 0;

// 	ft_div_mod(a, b, &div, &mod);
// 	printf("%d || %d\n", div, mod);
// }