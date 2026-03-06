/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/04 14:37:28 by tibras            #+#    #+#             */
/*   Updated: 2026/03/06 13:09:39 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	ft_status_change(t_guest *guest, t_state status)
{
	pthread_mutex_lock(&guest->m_status);
	guest->status = status;
	pthread_mutex_unlock(&guest->m_status);
}

void	ft_fork_handler(t_guest *guest, t_fork side)
{
	if (side != BOTH)
	{
		pthread_mutex_lock(guest->forks[side]);
		ft_status_change(guest, FORK);
		ft_action_print(guest);
	}
	else
	{
		pthread_mutex_unlock(guest->forks[LEFT]);
		pthread_mutex_unlock(guest->forks[RIGHT]);
	}
}

int	ft_eat(t_guest *guest)
{
	if (guest->t_id % 2)
	{
		ft_fork_handler(guest, LEFT);
		ft_fork_handler(guest, RIGHT);
	}
	else
	{
		ft_fork_handler(guest, RIGHT);
		ft_fork_handler(guest, LEFT);
	}
	ft_status_change(guest, EATING);
	guest->nb_meals++;
	pthread_mutex_lock(&guest->m_last_meal);
	guest->time_last_meal = ft_get_time(MILLISECONDS) - guest->data->start_time;
	pthread_mutex_unlock(&guest->m_last_meal);
	if (guest->nb_meals == guest->data->needed_meals && guest->nb_meals > 0)
	{
		pthread_mutex_lock(&guest->data->m_full);
		guest->data->full++;
		pthread_mutex_unlock(&guest->data->m_full);
	}
	ft_action_print(guest);
	ft_precise_sleep(guest);
	ft_fork_handler(guest, BOTH);
	return (SUCCESS);
}

int	ft_sleep(t_guest *guest)
{
	ft_status_change(guest, SLEEPING);
	ft_action_print(guest);
	ft_precise_sleep(guest);
	return (SUCCESS);
}

int	ft_think(t_guest *guest)
{
	ft_status_change(guest, THINKING);
	ft_action_print(guest);
	return (SUCCESS);
}