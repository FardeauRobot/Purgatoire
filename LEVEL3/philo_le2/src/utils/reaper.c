/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   reaper.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 15:20:22 by tibras            #+#    #+#             */
/*   Updated: 2026/03/07 22:40:44 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	*ft_reaper(void *ptr)
{
	t_philo *philo;
	long long time_tmp;
	int i;

	philo = (t_philo *)ptr;
	while (ft_end_check(philo) == SUCCESS)
	{
		i = -1;
		philo->full = 0;
		while(++i < philo->nb_philo)
		{
			pthread_mutex_lock(&philo->guests[i].m_last_meal);
			time_tmp = ft_get_time(MILLISECONDS) - philo->guests[i].time_last_meal;
			if (time_tmp > philo->time_to_die)
			{
				pthread_mutex_unlock(&philo->guests[i].m_last_meal);
				pthread_mutex_lock(&philo->m_ended);
				philo->ended = 1;
				pthread_mutex_unlock(&philo->m_ended);
				ft_status_change(&philo->guests[i], DEAD);
				break;
			}
			pthread_mutex_unlock(&philo->guests[i].m_last_meal);
			pthread_mutex_lock(&philo->guests[i].m_nb_meal);
			if (philo->guests[i].nb_meals == philo->needed_meals)
				philo->full++;
			pthread_mutex_unlock(&philo->guests[i].m_nb_meal);
		}
		if (philo->full == philo->nb_philo)
		{
			pthread_mutex_lock(&philo->m_ended);
			philo->ended = 1;
			pthread_mutex_unlock(&philo->m_ended);
			pthread_mutex_lock(&philo->m_print);
			printf(WHITE"%lld Everybody is totally full\n"RESET, ft_get_time(MILLISECONDS) - philo->start_time);
			pthread_mutex_unlock(&philo->m_print);
		}
	}
	return (NULL);
}
