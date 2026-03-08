/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/26 19:16:33 by tibras            #+#    #+#             */
/*   Updated: 2025/12/29 17:17:31 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int			ft_print_nbr(long long nb, int l_base, int mod);

static int	ft_printf_strlen(char *str)
{
	int	i;

	i = 0;
	while (str[i])
		i++;
	return (i);
}

int	ft_cputchar(int c)
{
	write(1, &c, 1);
	return (1);
}

int	ft_cputstr(char *str)
{
	size_t	len;
	char	*error;

	error = "(null)";
	if (!str)
	{
		len = ft_printf_strlen(error);
		write(1, error, len);
		return (len);
	}
	len = ft_printf_strlen(str);
	write(1, str, len);
	return (len);
}

int	ft_check_print(va_list args, char c)
{
	int	nb_written;

	nb_written = 0;
	if (c == 'c')
		nb_written = ft_cputchar(va_arg(args, int));
	else if (c == 's')
		nb_written = ft_cputstr(va_arg(args, char *));
	else if (c == 'u')
		nb_written = ft_print_nbr(va_arg(args, unsigned int), 10, 0);
	else if (c == 'd' || c == 'i')
		nb_written = ft_print_nbr(va_arg(args, int), 10, 0);
	else if (c == 'x')
		nb_written = ft_print_nbr(va_arg(args, unsigned int), 16, 1);
	else if (c == 'X')
		nb_written = ft_print_nbr(va_arg(args, unsigned int), 16, 0);
	else if (c == 'p')
		nb_written = ft_print_nbr(va_arg(args, long long), 16, 2);
	else if (c == '%')
		nb_written = ft_cputstr("%");
	return (nb_written);
}

int	ft_printf(const char *s, ...)
{
	int		nb_written;
	int		add;
	int		i;
	va_list	args;

	i = 0;
	nb_written = 0;
	va_start(args, s);
	while (s[i])
	{
		if (s[i] == '%' && s[i + 1])
		{
			add = ft_check_print(args, s[i + 1]);
			if (add == -1)
				return (-1);
			nb_written += add;
			i++;
		}
		else
			nb_written += ft_cputchar(s[i]);
		i++;
	}
	va_end(args);
	return (nb_written);
}
