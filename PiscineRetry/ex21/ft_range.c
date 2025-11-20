/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_range.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 13:46:34 by tibras            #+#    #+#             */
/*   Updated: 2025/11/05 18:19:38 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>

int	*ft_range(int min, int max)
{
	int	range;
	int	i;
	int	*tab;

	if (min >= max)
		return (NULL);
	range = max - min;
	tab = malloc(sizeof(int) * range);
	if (!tab)
		return (NULL);
	i = 0;
	while (i < range)
	{
		tab[i] = min + i;
		i++;
	}
	return (tab);
}

// #include <stdio.h>

// int main ()
// {
// 	int min;
// 	int max;
// 	int i;
// 	int range;
// 	int *tab;

// 	i = 0;
// 	min = -10;
// 	max = -20;
// 	range = max - min;
// 	tab = ft_range(min, max);
// 	while (i < range)
// 	{
// 		printf("Arr[%d] = %d\n", i, tab[i]);
// 		i++;
// 	}
// free(tab);
// }