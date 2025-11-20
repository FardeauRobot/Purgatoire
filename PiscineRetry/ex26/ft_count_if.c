/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_count_if.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/05 15:53:51 by tibras            #+#    #+#             */
/*   Updated: 2025/11/05 18:21:21 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

int	ft_count_if(char **tab, int (*f)(char*))
{
	int	i;
	int	sum;

	i = 0;
	sum = 0;
	while (tab[i])
	{
		if (f(tab[i]) == 1)
			sum++;
		i++;
	}
	return (sum);
}

// int main (void)
// {
// 	char tab[][];
// 	int res;

// 	tab[0] = "COUCOU";
// 	tab[1] = "COOU";
// 	tab[2] = "COUUUUUCOU";
// 	tab[3] = "";
// 	tab[4] = "OUCOU";
// 	tab[5] = "SOUCOU";
// 	res = ft_count_if(tab, &ft_size);
// 	printf("RES = %d\n", res);
// }