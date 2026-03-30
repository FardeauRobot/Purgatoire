/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/04 14:37:28 by tibras            #+#    #+#             */
/*   Updated: 2026/03/20 17:07:53 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static int	ft_take_forks(t_guest *guest)
{
	if (guest->t_id % 2)
	{
		if (ft_fork_handler(guest, LEFT))
			return (STANDARD_ERROR);
		if (ft_fork_handler(guest, RIGHT))
			return (STANDARD_ERROR);
	}
	else
	{
		if (ft_fork_handler(guest, RIGHT))
			return (STANDARD_ERROR);
		if (ft_fork_handler(guest, LEFT))
			return (STANDARD_ERROR);
	}
	return (ft_dead_check(guest->data));
}

int	ft_eat(t_guest *guest)
{
	if (ft_take_forks(guest))
		return (1);
	ft_status_change(guest, EATING);
	ft_meal_update(guest);
	guest->nb_meals++;
	if (guest->nb_meals == guest->data->needed_meals && guest->nb_meals != 0)
	{
		pthread_mutex_lock(&guest->data->m_full);
		guest->data->full++;
		pthread_mutex_unlock(&guest->data->m_full);
	}
	ft_precise_sleep(guest);
	ft_fork_giveback(guest);
	return (ft_dead_check(guest->data));
}

int	ft_sleep(t_guest *guest)
{
	ft_status_change(guest, SLEEPING);
	ft_precise_sleep(guest);
	return (ft_dead_check(guest->data));
}

int	ft_think(t_guest *guest)
{
	ft_status_change(guest, THINKING);
	ft_precise_sleep(guest);
	return (ft_dead_check(guest->data));
}
