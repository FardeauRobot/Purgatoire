/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/02 13:01:49 by tibras            #+#    #+#             */
/*   Updated: 2026/03/09 11:16:30 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

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

void	ft_philo_clean(t_philo *philo)
{
	int	i;

	pthread_mutex_destroy(&philo->m_lock_eat);
	pthread_mutex_destroy(&philo->m_print);
	pthread_mutex_destroy(&philo->m_is_dead);
	pthread_mutex_destroy(&philo->m_full);
	i = -1;
	while (++i < philo->nb_forks)
		pthread_mutex_destroy(&philo->m_fork[i]);
	free(philo->m_fork);
}

void	ft_exit(t_philo *philo, char *context, char *detail, int error)
{
	if (philo)
		ft_philo_clean(philo);
	ft_error(context, detail, error);
	exit(EXIT_FAILURE);
}
