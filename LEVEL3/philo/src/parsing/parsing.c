/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/02 14:31:25 by tibras            #+#    #+#             */
/*   Updated: 2026/03/20 17:18:01 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static int	ft_forks_init(t_philo *philo)
{
	int	i;

	i = -1;
	while (++i < philo->nb_philo)
		if (pthread_mutex_init(&philo->m_fork[i], NULL))
			break ;
	return (i);
}

static int	ft_mutex_init(t_philo *philo)
{
	if (pthread_mutex_init(&philo->m_print, NULL)
		|| pthread_mutex_init(&philo->m_is_dead, NULL)
		|| pthread_mutex_init(&philo->m_lock_eat, NULL)
		|| pthread_mutex_init(&philo->m_full, NULL))
		return (STANDARD_ERROR);
	return (SUCCESS);
}

static int	ft_philo_fill(t_philo *philo, long long tmp, int i)
{
	if ((i == 2 || i == 3 || i == 4) && tmp < 60)
		return (ft_error(ERR_PARSING, ERR_MIN_TIME, ERRN_PARSING));
	if (i == 1)
	{
		if (tmp > MAX_GUESTS)
			return (ft_error(ERR_PARSING, ERR_MAX_CAP, ERRN_PARSING));
		philo->nb_philo = (int)tmp;
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
			return (ft_error(ERR_PARSING, ERR_MAX_MEALS, ERRN_PARSING));
		philo->needed_meals = (int)tmp;
	}
	else
		philo->needed_meals = 0;
	return (SUCCESS);
}

int	ft_parsing(t_philo *philo, int argc, char **argv)
{
	int			i;
	long long	tmp;

	if (argc < 5 || argc > 6)
		return (ft_error(ERR_MSG_ARGS, EXPECTED_ARGS, ERRN_PARSING));
	i = 0;
	if (ft_mutex_init(philo) != SUCCESS)
		return (ft_error(ERR_MUTEX, ERR_INIT, ERRN_MUTEX));
	while (argv[++i])
	{
		tmp = 0;
		if (ft_atoll_safe(argv[i], &tmp) == -1)
			return (STANDARD_ERROR);
		if (ft_philo_fill(philo, tmp, i) != SUCCESS)
			return (STANDARD_ERROR);
	}
	ft_forks_init(philo);
	philo->start_time = ft_get_time(MILLISECONDS);
	return (SUCCESS);
}
