/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/04 14:37:28 by tibras            #+#    #+#             */
/*   Updated: 2026/03/06 10:31:29 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	ft_fork_handler(t_guest *guest, t_fork side)
{
	if (side != BOTH)
	{
		pthread_mutex_lock(guest->forks[side]);
		guest->status = FORK;
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
	ft_fork_handler(guest, LEFT);
	usleep(100);
	ft_fork_handler(guest, RIGHT);
	guest->status = EATING;
	guest->nb_meals++;
	if (guest->nb_meals == guest->data->needed_meals && guest->nb_meals > 0)
	{
		pthread_mutex_lock(&guest->data->m_full);
		guest->data->full++;
		pthread_mutex_unlock(&guest->data->m_full);
	}
	ft_action_print(guest);
	ft_fork_handler(guest, BOTH);
	return (SUCCESS);
}

int	ft_sleep(t_guest *guest)
{
	guest->status = SLEEPING;
	ft_precise_sleep(guest);
	ft_action_print(guest);
	return (SUCCESS);
}

int	ft_think(t_guest *guest)
{
	guest->status = THINKING;
	ft_action_print(guest);
	return (SUCCESS);
}