/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/04 14:47:51 by tibras            #+#    #+#             */
/*   Updated: 2026/03/09 10:45:12 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

long long	ft_think_time(t_guest *guest)
{
	long long	tt;

	if (guest->data->nb_philo % 2)
		tt = guest->data->time_to_eat * 2
			- guest->data->time_to_sleep;
	else
		tt = guest->data->time_to_eat
			- guest->data->time_to_sleep;
	if (tt < 0)
		tt = 0;
	return (tt);
}

long long	ft_get_time(t_scales scale)
{
	struct timeval	tv;

	if (gettimeofday(&tv, NULL))
		return (-1);
	if (scale == SECONDS)
		return (tv.tv_sec + tv.tv_usec / 1000000);
	else if (scale == MILLISECONDS)
		return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
	else if (scale == MICROSECONDS)
		return (tv.tv_sec * 1000000 + tv.tv_usec);
	return (-1);
}

void	ft_precise_sleep(t_guest *guest)
{
	long long	start;

	start = ft_get_time(MILLISECONDS);
	while (!ft_meal(guest->data))
	{
		if (guest->status == SLEEPING
			&& (ft_get_time(MILLISECONDS) - start)
			>= guest->data->time_to_sleep)
			break ;
		if (guest->status == EATING
			&& (ft_get_time(MILLISECONDS) - start)
			>= guest->data->time_to_eat)
			break ;
		if (guest->status == THINKING
			&& (ft_get_time(MILLISECONDS) - start)
			>= ft_think_time(guest))
			break ;
		if (guest->status == ALONE
			&& (ft_get_time(MILLISECONDS) - start)
			>= guest->data->time_to_die)
			break ;
	}
}
