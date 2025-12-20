/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putnbr_fd.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/10 18:46:57 by tibras            #+#    #+#             */
/*   Updated: 2025/11/18 13:56:17 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_putnbr_fd(int n, int fd)
{
	long	nb;

	nb = n;
	if (n < 0)
	{
		nb *= -1;
		ft_putchar_fd('-', fd);
	}
	if (nb > 9)
		ft_putnbr_fd(nb / 10, fd);
	ft_putchar_fd(nb % 10 + '0', fd);
}

// #include <limits.h>
// int main ()
// {
// 	int nb = INT_MIN;

// 	ft_putnbr_fd(nb, 1);
// }