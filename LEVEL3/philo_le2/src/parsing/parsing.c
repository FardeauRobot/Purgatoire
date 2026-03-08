/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/02 14:31:25 by tibras            #+#    #+#             */
/*   Updated: 2026/03/07 19:09:20 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"


static void	ft_forks_init(t_philo *philo)
{
	int i;

	philo->m_fork = malloc(sizeof(pthread_mutex_t) * philo->nb_forks);
	if (!philo->m_fork)
		ft_exit(philo, ERR_MUTEX, ERR_MSG_MALLOC, ERRN_MALLOC);
	i = -1;
	while (++i < philo->nb_forks)
	{
		if (pthread_mutex_init(&philo->m_fork[i], NULL))
			ft_exit(philo, ERR_MUTEX, ERR_MSG_MALLOC, ERRN_MALLOC);
	}
}

static int ft_mutex_init(t_philo *philo)
{
	if (pthread_mutex_init(&philo->m_print, NULL) ||
			pthread_mutex_init(&philo->m_ended, NULL) ||
			pthread_mutex_init(&philo->m_full, NULL))
		return (STANDARD_ERROR);
	return (SUCCESS);
}

static void	ft_philo_fill(t_philo *philo, long long tmp, int i)
{
	if (i == 1)
	{
		if (tmp > MAX_GUESTS)
			ft_exit(NULL, ERR_GUESTS, ERR_MAX_CAP, ERRN_PARSING);
		philo->nb_philo = (int)tmp;
		philo->nb_forks = (int)tmp;
	}
	if (i == 2)
		philo->time_to_die = (time_t)tmp;
	if (i == 3)
		philo->time_to_eat = (time_t)tmp;
	if (i == 4)
		philo->time_to_sleep = (time_t)tmp;
	if (i == 5)
	{
		if (tmp > MAX_MEALS)
			ft_exit(NULL, ERR_MEALS, ERR_MAX_MEALS, ERRN_PARSING);
		philo->needed_meals = (int)tmp;
	}
	else
		philo->needed_meals = 0;
}

int	ft_parsing(t_philo *philo, int argc, char **argv)
{
	int i;
	long long tmp;
	if (argc < 5 || argc > 6)
		return (ft_error(ERR_MSG_ARGS, EXPECTED_ARGS, ERRN_PARSING));
	i = 0;
	if (ft_mutex_init(philo) != SUCCESS)
		ft_exit(philo, ERR_MUTEX, ERR_INIT, ERRN_MUTEX);
	while (argv[++i])
	{
		tmp = 0;
		ft_atoll_safe(argv[i], &tmp);
		ft_philo_fill(philo, tmp, i);
	}
	ft_forks_init(philo);
	philo->start_time = ft_get_time(MILLISECONDS);
	return (SUCCESS);
}
