/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   reaper.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 15:20:22 by tibras            #+#    #+#             */
/*   Updated: 2026/03/06 15:54:44 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	*ft_reaper(void *ptr)
{
	t_philo *philo;
	long long time_tmp;
	int i;

	philo = (t_philo *)ptr;
	while (1)
	{
		i = -1;
		while(++i < philo->nb_philo)
		{
			pthread_mutex_lock(&philo->guests[i].m_last_meal);
			time_tmp = ft_get_time(MILLISECONDS) - philo->guests[i].time_last_meal;
			if (time_tmp > philo->time_to_die || philo->guests[i].status == DEAD)
			{
				pthread_mutex_lock(&philo->m_is_dead);
				philo->is_dead = DEAD;
				ft_status_change(&philo->guests[i], DEAD);
				pthread_mutex_unlock(&philo->m_is_dead);
				pthread_mutex_unlock(&philo->guests[i].m_last_meal);
				return (NULL);
			}
			pthread_mutex_unlock(&philo->guests[i].m_last_meal);
		}
	}
}
