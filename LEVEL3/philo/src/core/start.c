/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   start.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 14:39:15 by tibras            #+#    #+#             */
/*   Updated: 2026/03/03 15:20:18 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	*ft_routine(void *ptr)
{
	int i;

	i = -1;
	while (++i < 3)
		printf("ID%p\n", ptr);
	return (NULL);
}

void	ft_guests_arr(t_philo *philo)
{
	int i;

	ft_bzero(&philo->guests, 250 * sizeof(t_guest));
	i = -1;
	while (++i < philo->nb_philo)
	{
		if (pthread_create(&philo->guests[i].thread, NULL, ft_routine, &philo->guests[i]))
			ft_exit(philo, ERR_THREAD, ERR_THREAD_INIT, ERR_THREADS);
	}

}

void	ft_start(t_philo *philo)
{
	ft_guests_arr(philo);
}