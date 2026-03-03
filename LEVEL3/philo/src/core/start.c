/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   start.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 14:39:15 by tibras            #+#    #+#             */
/*   Updated: 2026/03/03 20:10:02 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	*ft_routine(void *ptr)
{
	int i;
	t_guest *guest;

	guest = (t_guest *)ptr;
	i = -1;
	while (++i < 3)
	{
		if (i == 0)
			guest->status = EATING;
		if (i == 1)
			guest->status = DEAD;
		if (i == 2)
			guest->status = SLEEPING;
		ft_action_print(guest->t_id, guest->status);
	}
	return (NULL);
}

void	ft_guests_arr(t_philo *philo)
{
	int i;

	ft_bzero(&philo->guests, MAX_GUESTS * sizeof(t_guest));
	i = -1;
	while (++i <= philo->nb_philo)
	{
		if (pthread_create(&philo->guests[i].thread, NULL, ft_routine, &philo->guests[i]))
			ft_exit(philo, ERR_THREAD, ERR_THREAD_INIT, ERR_THREADS);
		philo->guests[i].t_id = i + 1;
	}

}

void	ft_start(t_philo *philo)
{
	ft_guests_arr(philo);
}