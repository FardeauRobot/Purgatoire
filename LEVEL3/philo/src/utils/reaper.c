/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   reaper.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 15:20:22 by tibras            #+#    #+#             */
/*   Updated: 2026/03/09 11:15:09 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	ft_death_declare(t_philo *philo)
{
	pthread_mutex_lock(&philo->m_is_dead);
	philo->is_dead = 1;
	pthread_mutex_unlock(&philo->m_is_dead);
}

void	*ft_reaper(void *ptr)
{
	t_philo		*philo;
	long long	time_gap;
	int			i;

	philo = (t_philo *)ptr;
	while (1)
	{
		if (ft_dead_check(philo))
			return (NULL);
		i = -1;
		while (++i < philo->nb_philo)
		{
			time_gap = ft_get_time(MILLISECONDS)
				- ft_meal_read(&philo->guests[i]);
			if (time_gap > philo->time_to_die)
			{
				ft_death_declare(philo);
				ft_status_change(&philo->guests[i], DEAD);
				return (NULL);
			}
		}
		usleep(200);
	}
	return (NULL);
}
