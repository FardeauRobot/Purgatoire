/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 15:20:20 by tibras            #+#    #+#             */
/*   Updated: 2026/03/09 11:13:02 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	ft_status_change(t_guest *guest, t_state status)
{
	pthread_mutex_lock(&guest->m_status);
	guest->status = status;
	pthread_mutex_unlock(&guest->m_status);
	ft_action_print(guest);
}

void	ft_meal_update(t_guest *guest)
{
	pthread_mutex_lock(&guest->m_last_meal);
	guest->time_last_meal = ft_get_time(MILLISECONDS);
	pthread_mutex_unlock(&guest->m_last_meal);
}

long long	ft_meal_read(t_guest *guest)
{
	long long	tmp;

	pthread_mutex_lock(&guest->m_last_meal);
	tmp = guest->time_last_meal;
	pthread_mutex_unlock(&guest->m_last_meal);
	return (tmp);
}

int	ft_dead_check(t_philo *philo)
{
	pthread_mutex_lock(&philo->m_is_dead);
	if (philo->is_dead == 1)
	{
		pthread_mutex_unlock(&philo->m_is_dead);
		return (1);
	}
	pthread_mutex_unlock(&philo->m_is_dead);
	pthread_mutex_lock(&philo->m_full);
	if (philo->full == philo->nb_philo)
	{
		pthread_mutex_unlock(&philo->m_full);
		return (1);
	}
	pthread_mutex_unlock(&philo->m_full);
	return (0);
}
