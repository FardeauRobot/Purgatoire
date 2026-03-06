/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   reaper.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 15:20:22 by tibras            #+#    #+#             */
/*   Updated: 2026/03/06 08:46:51 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	ft_exec_finished(t_philo *philo) 
{
	int i;

	i = -1;
	while (++i < philo->m_fork[i])
		pthread_join(philo->guests[i].thread, NULL);
}