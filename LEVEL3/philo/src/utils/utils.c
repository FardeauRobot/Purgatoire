/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/07 15:20:20 by tibras            #+#    #+#             */
/*   Updated: 2026/03/07 16:55:00 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	ft_status_change(t_guest *guest, t_state status)
{
	pthread_mutex_lock(&guest->m_status);
	guest->status = status;
	pthread_mutex_unlock(&guest->m_status);
}