/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/04 09:17:08 by tibras            #+#    #+#             */
/*   Updated: 2026/03/20 17:19:53 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	ft_strlen(char *str)
{
	int	i;

	i = 0;
	while (str[i])
		i++;
	return (i);
}

void	ft_bzero(void *ptr, int size)
{
	int		i;
	char	*str;

	i = -1;
	str = (char *)ptr;
	while (++i < size)
		str[i] = 0;
}

// CHECK NUMS
int	ft_isnum(char *str)
{
	int	i;

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
	int	i;

	if (ft_isnum(str) != SUCCESS || ft_strlen(str) > 7)
		return (ft_error(INVALID_PARAM, str, -1));
	i = -1;
	while (str[++i])
		*nb = *nb * 10 + str[i] - '0';
	if (*nb > MAX_TIME || *nb <= 0)
		return (ft_error(INVALID_PARAM, str, -1));
	if (*nb == 0)
		return (ft_error(INVALID_PARAM, str, -1));
	return (SUCCESS);
}
