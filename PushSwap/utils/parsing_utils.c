/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 18:30:19 by tibras            #+#    #+#             */
/*   Updated: 2025/12/12 13:00:25 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap_utils.h"

int	ft_is_number(char *nbr)
{	
	int	i;

	i = 0;
	if (nbr[0] == '-' || nbr[0] == '+')
	{
		if (!nbr[1] || !ft_isdigit(nbr[1]))
			return (0);
		i++;
	}
	while (nbr[i])
	{
		if (!ft_isdigit(nbr[i]))
			return (0);
		i++;
	}
	return (1);
}
