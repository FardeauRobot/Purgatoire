/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   start.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 14:39:15 by tibras            #+#    #+#             */
/*   Updated: 2026/03/07 22:45:35 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static void	*ft_routine_guests(void *ptr)
{
	int i;
	t_guest *guest;

	guest = (t_guest *)ptr;
	i = -1;
	if (guest->t_id % 2 == 1)
		usleep(3000);
	while (1)
	{
		if (ft_eat(guest) != SUCCESS)
			break;
		if (ft_sleep(guest)!= SUCCESS)
			break;
		if (ft_think(guest) != SUCCESS)
			break;
	}
	return (NULL);
}

void	ft_guests_arr(t_philo *philo)
{
	int i;

	ft_bzero(&philo->guests, philo->nb_philo * sizeof(t_guest));
	i = -1;
	while (++i < philo->nb_philo)
	{
		philo->guests[i].t_id = i + 1;
		philo->guests[i].data = philo;
		philo->guests[i].status = THINKING;
		philo->guests[i].time_last_meal = philo->start_time;
		philo->guests[i].print = &philo->m_print;

		// TODO : PROTEGER INIT MUTEX
		pthread_mutex_init(&philo->guests[i].m_last_meal, NULL);
		pthread_mutex_init(&philo->guests[i].m_status, NULL);

		philo->guests[i].forks[LEFT] = &philo->m_fork[i];
		if (i == 0)
			philo->guests[i].forks[RIGHT] = &philo->m_fork[philo->nb_philo - 1];
		else
			philo->guests[i].forks[RIGHT] = &philo->m_fork[i - 1];
		if (pthread_create(&philo->guests[i].thread, NULL, ft_routine_guests, &philo->guests[i]))
			ft_exit(philo, ERR_THREAD, ERR_INIT, ERRN_THREADS);

	}
}

void	ft_start(t_philo *philo)
{
	int		i;
	pthread_t	reaper;

	ft_print_main_philo(philo);
	ft_guests_arr(philo);
	if (pthread_create(&reaper, NULL, ft_reaper, philo))
		ft_exit(philo, ERR_THREAD, ERR_INIT, ERRN_THREADS);
	i = -1;
	while (++i < philo->nb_philo)
	{
		pthread_join(philo->guests[i].thread, NULL);
	}
	pthread_join(reaper, NULL);
}