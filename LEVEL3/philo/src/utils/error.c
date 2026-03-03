/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/02 13:01:49 by tibras            #+#    #+#             */
/*   Updated: 2026/03/03 09:32:22 by tibras           ###   ########.fr       */
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

void	ft_free_philo (t_philo *philo)
{
	int i;
	
	i = -1;
	while (++i < philo->nb_philo)
	{
		if (philo->guests[i])
			free(philo->guests[i]);
	}
}

int	ft_error(char *context, char *detail, int error)
{
	ft_putstr_fd("philo: ", 2);
	if (context && !detail)
		ft_putendl_fd(context, 2);
	else 
		ft_putstr_fd(context, 2);
	if (detail)
		ft_putendl_fd(detail, 2);
	return (error);
}

void	ft_exit(t_philo *philo, char *context, char *detail, int error)
{
	if (philo)
		ft_free_philo(philo);
	ft_error(context, detail, error);
	exit(EXIT_FAILURE);
}