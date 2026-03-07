/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/04 14:47:51 by tibras            #+#    #+#             */
/*   Updated: 2026/03/07 21:55:41 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void ft_update_last_meal(t_guest *guest)
{
	if (ft_end_check(guest->data))
		return ;
	pthread_mutex_lock(&guest->m_last_meal);
	guest->time_last_meal = ft_get_time(MILLISECONDS);
	pthread_mutex_unlock(&guest->m_last_meal);
	pthread_mutex_lock(&guest->m_nb_meal);
	guest->nb_meals++;
	pthread_mutex_unlock(&guest->m_nb_meal);
}

long long ft_get_time(t_scales scale)
{
	struct timeval tv;

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
	while (ft_end_check(guest->data) == SUCCESS)
	{
		if (guest->status == SLEEPING && (ft_get_time(MILLISECONDS) - start) >= guest->data->time_to_sleep)
			break;
		if (guest->status == EATING && (ft_get_time(MILLISECONDS) - start) >= guest->data->time_to_eat)
			break;
		if (guest->status == ALONE && (ft_get_time(MILLISECONDS) - start) >= guest->data->time_to_die) 
			break;
	}
}
