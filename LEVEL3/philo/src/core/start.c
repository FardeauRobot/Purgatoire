/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   start.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 14:39:15 by tibras            #+#    #+#             */
/*   Updated: 2026/03/06 10:34:06 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	ft_meal(t_philo *philo)
{
	pthread_mutex_lock(&philo->m_is_dead);
	if (philo->is_dead == DEAD)
	{
		pthread_mutex_unlock(&philo->m_is_dead);
		return (1);
	}
	pthread_mutex_unlock(&philo->m_is_dead);
	pthread_mutex_lock(&philo->m_full);
	if (philo->full == philo->nb_philo)
	{
		pthread_mutex_unlock(&philo->m_full);
		return (1);
	}
	pthread_mutex_unlock(&philo->m_full);
	usleep(200);
	return (0);
}

void	*ft_routine(void *ptr)
{
	int i;
	t_guest *guest;

	guest = (t_guest *)ptr;
	i = -1;
	if (guest->t_id % 2)
		usleep(3000);
	while (!ft_meal(guest->data))
	{
		ft_eat(guest);
		ft_sleep(guest);
		ft_think(guest);
	}
	return (NULL);
}

void	ft_guests_arr(t_philo *philo)
{
	int i;

	ft_bzero(&philo->guests, MAX_GUESTS * sizeof(t_guest));
	i = -1;
	while (++i < philo->nb_philo)
	{
		philo->guests[i].t_id = i + 1;
		philo->guests[i].data = philo;
		philo->guests[i].status = STANDARD;
		philo->guests[i].forks[LEFT] = &philo->m_fork[i];
		if (i == 0)
			philo->guests[i].forks[RIGHT] = &philo->m_fork[philo->nb_philo - 1];
		else
			philo->guests[i].forks[RIGHT] = &philo->m_fork[i - 1];
		philo->guests[i].print = &philo->m_print;
		if (pthread_create(&philo->guests[i].thread, NULL, ft_routine, &philo->guests[i]))
			ft_exit(philo, ERR_THREAD, ERR_INIT, ERRN_THREADS);
	}
}

void	ft_start(t_philo *philo)
{
	int	i;

	ft_guests_arr(philo);
	i = -1;
	while (++i < philo->nb_philo)
		pthread_join(philo->guests[i].thread, NULL);
}