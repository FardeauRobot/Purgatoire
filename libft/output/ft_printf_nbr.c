/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_nbr.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 18:53:55 by tibras            #+#    #+#             */
/*   Updated: 2025/12/29 14:03:44 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	ft_cputstr(char *str)
{
	int	i;

	i = 0;
	if (!str)
		return (0);
	while (str[i])
		i++;
	write(1, str, i);
	return (i);
}

static int	ft_lllen(unsigned long long nb, int l_base)
{
	int					len;
	unsigned long long	ul_base;

	len = 1;
	ul_base = (unsigned long long)l_base;
	while (nb >= ul_base)
	{
		nb /= ul_base;
		len++;
	}
	return (len);
}

static char	*ft_fillll(unsigned long long nb, char *base, int l_base, int r_len)
{
	char	*res;

	res = malloc(r_len + 1);
	if (!res)
		return (NULL);
	res[r_len] = '\0';
	while (r_len > 0)
	{
		r_len--;
		res[r_len] = base[nb % l_base];
		nb /= l_base;
	}
	return (res);
}

static char	*ft_cust_lltoa(unsigned long long nb, int l_base, int mod)
{
	char	*base;
	char	*res;
	int		r_len;

	if (l_base == 16 && !mod)
		base = BASE_UPPER;
	else if (l_base == 16 && mod)
		base = BASE_LOWER;
	else
		base = BASE_10;
	r_len = ft_lllen(nb, l_base);
	res = ft_fillll(nb, base, l_base, r_len);
	if (!res)
		return (NULL);
	return (res);
}

int	ft_print_nbr(long long nb, int l_base, int mod)
{
	char				*res;
	int					size;
	unsigned long long	unb;

	size = 0;
	if (nb < 0 && mod != 2)
	{
		size += ft_cputstr("-");
		unb = (unsigned long long)(nb * -1);
	}
	else
		unb = (unsigned long long)nb;
	if (mod == 2)
	{
		if (!nb)
			return (ft_cputstr("(nil)"));
		size += ft_cputstr("0x");
	}
	res = ft_cust_lltoa(unb, l_base, mod);
	if (!res)
		return (-1);
	size += ft_cputstr(res);
	free(res);
	return (size);
}
