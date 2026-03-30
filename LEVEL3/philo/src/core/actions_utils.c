/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ations_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/20 17:06:42 by tibras            #+#    #+#             */
/*   Updated: 2026/03/20 17:13:50 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	ft_fork_giveback(t_guest *guest)
{
	pthread_mutex_unlock(guest->forks[LEFT]);
	pthread_mutex_unlock(guest->forks[RIGHT]);
}

int	ft_fork_handler(t_guest *guest, t_fork side)
{
	if (ft_dead_check(guest->data))
	{
		if (side == RIGHT && guest->t_id % 2)
			pthread_mutex_unlock(guest->forks[LEFT]);
		else if (side == LEFT && !(guest->t_id % 2))
			pthread_mutex_unlock(guest->forks[RIGHT]);
		return (1);
	}
	pthread_mutex_lock(guest->forks[side]);
	ft_status_change(guest, FORK);
	return (ft_dead_check(guest->data));
}
