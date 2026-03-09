/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   start.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 14:39:15 by tibras            #+#    #+#             */
/*   Updated: 2026/03/09 11:29:46 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	ft_meal(t_philo *philo)
{
	if (ft_dead_check(philo))
		return (1);
	usleep(200);
	return (0);
}

static void	*ft_routine(void *ptr)
{
	int		i;
	t_guest	*guest;

	guest = (t_guest *)ptr;
	i = -1;
	if (guest->data->nb_philo == 1)
	{
		pthread_mutex_lock(guest->forks[LEFT]);
		ft_status_change(guest, FORK);
		while (!ft_dead_check(guest->data))
			usleep(200);
		pthread_mutex_unlock(guest->forks[LEFT]);
		return (NULL);
	}
	if (guest->t_id % 2 == 0)
		usleep(guest->data->time_to_eat * 5);
	while (!ft_meal(guest->data))
	{
		if (ft_eat(guest) || ft_sleep(guest) || ft_think(guest))
			break ;
	}
	return (NULL);
}

static void	ft_guests_arr(t_philo *philo)
{
	int	i;

	ft_bzero(&philo->guests, MAX_GUESTS * sizeof(t_guest));
	i = -1;
	while (++i < philo->nb_philo)
	{
		philo->guests[i].t_id = i + 1;
		philo->guests[i].data = philo;
		philo->guests[i].status = STANDARD;
		philo->guests[i].time_last_meal = philo->start_time;
		if (pthread_mutex_init(&philo->guests[i].m_last_meal, NULL)
			|| pthread_mutex_init(&philo->guests[i].m_status, NULL))
			ft_exit(philo, ERR_MUTEX, ERR_INIT, ERRN_THREADS);
		philo->guests[i].forks[LEFT] = &philo->m_fork[i];
		if (i == 0)
			philo->guests[i].forks[RIGHT] = &philo->m_fork[philo->nb_philo - 1];
		else
			philo->guests[i].forks[RIGHT] = &philo->m_fork[i - 1];
		philo->guests[i].print = &philo->m_print;
		if (pthread_create(&philo->guests[i].thread, NULL,
				ft_routine, &philo->guests[i]))
			ft_exit(philo, ERR_THREAD, ERR_INIT, ERRN_THREADS);
	}
}

void	ft_start(t_philo *philo)
{
	int			i;
	pthread_t	reaper;

	ft_guests_arr(philo);
	if (pthread_create(&reaper, NULL, ft_reaper, philo))
		ft_exit(philo, ERR_THREAD, ERR_INIT, ERRN_THREADS);
	i = -1;
	while (++i < philo->nb_philo)
		pthread_join(philo->guests[i].thread, NULL);
	pthread_join(reaper, NULL);
}
