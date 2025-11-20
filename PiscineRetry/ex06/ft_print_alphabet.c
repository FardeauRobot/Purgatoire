/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_print_alphabet.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 12:31:30 by tibras            #+#    #+#             */
/*   Updated: 2025/11/05 18:13:47 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #include <unistd.h>

// void ft_putchar(char c)
// {
// 	write(1, &c, 1);
// }

void	ft_putchar(char c);

void	ft_print_alphabet(void)
{
	int		i;
	char	c;

	i = 0;
	c = 'a';
	while (c + i <= 'z')
	{
		ft_putchar(c + i);
		i++;
	}
}

// int main ()
// {
// 	ft_print_alphabet();
// }