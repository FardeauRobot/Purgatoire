/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/04 14:47:51 by tibras            #+#    #+#             */
/*   Updated: 2026/03/19 13:11:25 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

long long	ft_think_time(t_guest *guest)
{
	long long	tt;

	tt = guest->data->time_to_eat - guest->data->time_to_sleep;
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
		if (ft_status_get(guest) == SLEEPING
			&& (ft_get_time(MILLISECONDS) - start)
			>= guest->data->time_to_sleep)
			break ;
		if (ft_status_get(guest) == EATING
			&& (ft_get_time(MILLISECONDS) - start)
			>= guest->data->time_to_eat)
			break ;
		if (ft_status_get(guest) == THINKING
			&& (ft_get_time(MILLISECONDS) - start)
			>= ft_think_time(guest))
			break ;
	}
}
