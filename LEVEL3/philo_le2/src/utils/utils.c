/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 15:20:20 by tibras            #+#    #+#             */
/*   Updated: 2026/03/07 21:58:25 by tibras           ###   ########.fr       */
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

int		ft_end_check(t_philo *philo)
{
	pthread_mutex_lock(&philo->m_ended);
	if (philo->ended == 1)
	{
		pthread_mutex_unlock(&philo->m_ended);
		return (STANDARD_ERROR);
	}
	pthread_mutex_unlock(&philo->m_ended);
	return (SUCCESS);
}