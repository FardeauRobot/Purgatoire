/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_print_numbers.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 12:44:17 by tibras            #+#    #+#             */
/*   Updated: 2025/11/05 17:01:03 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #include <unistd.h>

// void ft_putchar(char c)
// {
// 	write(1, &c, 1);
// }

void	ft_putchar(char c);

void	ft_print_numbers(void)
{
	int		i;
	char	c;

	i = 0;
	c = '0';
	while (c + i <= '9')
	{
		ft_putchar(c + i);
		i++;
	}
}
// int main ()
// {
// 	ft_print_numbers();
// }