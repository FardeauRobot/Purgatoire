/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/02 13:01:49 by tibras            #+#    #+#             */
/*   Updated: 2026/03/07 19:09:05 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"


void	ft_philo_clean(t_philo *philo)
{
	int i;

	pthread_mutex_destroy(&philo->m_print);
	pthread_mutex_destroy(&philo->m_ended);
	pthread_mutex_destroy(&philo->m_full);
	i = -1;
	while (++i < philo->nb_philo)
	{
		pthread_mutex_destroy(&philo->guests[i].m_status);
		pthread_mutex_destroy(&philo->guests[i].m_last_meal);
		pthread_mutex_destroy(&philo->m_fork[i]);
	}
	if (philo->m_fork)
		free(philo->m_fork);
}

int	ft_error(char *context, char *detail, int error)
{
	ft_putstr_fd("philo: ", STDERR_FILENO);
	if (context && !detail)
		ft_putendl_fd(context, STDERR_FILENO);
	else 
		ft_putstr_fd(context, STDERR_FILENO);
	if (detail)
		ft_putendl_fd(detail, STDERR_FILENO);
	return (error);
}

void	ft_exit(t_philo *philo, char *context, char *detail, int error)
{
	ft_philo_clean(philo);
	ft_error(context, detail, error);
	exit(EXIT_FAILURE);
}