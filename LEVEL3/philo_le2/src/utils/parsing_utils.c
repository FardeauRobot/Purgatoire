/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/04 09:17:08 by tibras            #+#    #+#             */
/*   Updated: 2026/03/04 12:40:22 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	ft_bzero(void *ptr, int size)
{
	int i;
	char *str;

	i = -1;
	str = (char *)ptr;
	while (++i < size)
		str[i] = 0;
}

// CHECK NUMS
int ft_isnum(char *str)
{
	int i;

	i = -1;
	while (str[++i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (STANDARD_ERROR);
	}
	return (SUCCESS);
}

// INIT NUMBERS
int	ft_atoll_safe(char *str, long long *nb)
{
	int i;

	if (ft_isnum(str) != SUCCESS)
		ft_exit(NULL, INVALID_PARAM, str, ERRN_PARSING);
	i = -1;
	while (str[++i])
		*nb = *nb * 10 + str[i] - '0';
	if (*nb > MAX_TIME)
		ft_exit(NULL, ERR_TIME, str, ERRN_PARSING);
	if (*nb == 0)
		ft_exit(NULL, INVALID_PARAM, str, ERRN_PARSING);
	return (SUCCESS);
}