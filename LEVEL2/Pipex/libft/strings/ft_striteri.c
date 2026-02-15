/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_striteri.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/10 19:16:16 by tibras            #+#    #+#             */
/*   Updated: 2025/11/18 14:20:25 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_striteri(char *s, void (*f)(unsigned int, char *))
{
	unsigned int	i;

	i = 0;
	while (s[i])
	{
		(*f)(i, &s[i]);
		i++;
	}
}

// void	ft_print(unsigned int index, char *str)
// {
// 	if (str[index] >= 'a' && str[index] <= 'z')
// 	{
// 		str[index] += 1;
// 		// str[index] = 'a';
// 	}
// }
// int main ()
// {
// 	char test[250] = "Coucou leeeeeeees salopes";
// 	ft_striteri(test, ft_print);
// 	printf("%s\n", test);
// }