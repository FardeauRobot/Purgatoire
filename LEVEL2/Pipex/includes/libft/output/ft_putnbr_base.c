/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putnbr_base.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/25 22:12:45 by tibras            #+#    #+#             */
/*   Updated: 2026/01/26 13:16:59 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_output.h"

void	ft_putnbr_base(int nbr, char *base)
{
	long	nb;
	size_t len_base;

	len_base = ft_strlen(base);
	if (len_base < 2)
		return ;
	nb = (long)nbr;
	if (nb < 0)
	{
		ft_putchar_fd('-', 1);
		nb *= -1;
	}
	if (nb >= (long)len_base)
		ft_putnbr_base(nb / len_base, base);
	ft_putchar_fd(base[nb % len_base], 1);
}
