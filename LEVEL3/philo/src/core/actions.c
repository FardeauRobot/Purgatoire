/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/04 14:37:28 by tibras            #+#    #+#             */
/*   Updated: 2026/03/09 10:17:23 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	ft_fork_handler(t_guest *guest, t_fork side)
{
	if (side != BOTH)
	{
		if (ft_dead_check(guest->data))
		{
			if (guest->t_id % 2 && guest->status == FORK)
				pthread_mutex_unlock(guest->forks[LEFT]);
			else if (guest->status == FORK)
				pthread_mutex_unlock(guest->forks[RIGHT]);
			return (1);
		}
		pthread_mutex_lock(guest->forks[side]);
		ft_status_change(guest, FORK);
		ft_action_print(guest);
		return (0);
	}
	pthread_mutex_unlock(guest->forks[LEFT]);
	pthread_mutex_unlock(guest->forks[RIGHT]);
	return (0);
}

static int	ft_take_forks(t_guest *guest)
{
	if (guest->t_id % 2)
	{
		if (ft_fork_handler(guest, LEFT))
			return (1);
		if (ft_fork_handler(guest, RIGHT))
			return (1);
	}
	else
	{
		if (ft_fork_handler(guest, RIGHT))
			return (1);
		if (ft_fork_handler(guest, LEFT))
			return (1);
	}
	return (0);
}

int	ft_eat(t_guest *guest)
{
	if (ft_dead_check(guest->data))
		return (1);
	if (ft_take_forks(guest))
		return (1);
	ft_status_change(guest, EATING);
	guest->nb_meals++;
	if (guest->nb_meals == guest->data->needed_meals && guest->nb_meals != 0)
	{
		pthread_mutex_lock(&guest->data->m_full);
		guest->data->full++;
		pthread_mutex_unlock(&guest->data->m_full);
	}
	ft_meal_update(guest);
	ft_precise_sleep(guest);
	ft_fork_handler(guest, BOTH);
	return (SUCCESS);
}

int	ft_sleep(t_guest *guest)
{
	if (ft_dead_check(guest->data))
		return (1);
	ft_status_change(guest, SLEEPING);
	ft_precise_sleep(guest);
	return (SUCCESS);
}

int	ft_think(t_guest *guest)
{
	long long	think_time;

	if (ft_dead_check(guest->data))
		return (1);
	ft_status_change(guest, THINKING);
	if (guest->data->nb_philo % 2)
		think_time = guest->data->time_to_eat * 2
			- guest->data->time_to_sleep;
	else
		think_time = guest->data->time_to_eat
			- guest->data->time_to_sleep;
	if (think_time < 0)
		think_time = 0;
	if (think_time > 0)
		ft_precise_sleep(guest);
	return (SUCCESS);
}