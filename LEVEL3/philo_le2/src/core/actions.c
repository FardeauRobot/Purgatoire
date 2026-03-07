/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/04 14:37:28 by tibras            #+#    #+#             */
/*   Updated: 2026/03/07 22:54:33 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	ft_fork_appropriation(t_guest *guest)
{
	if (ft_end_check(guest->data) != SUCCESS)
		return (STANDARD_ERROR);
	if (guest->status == THINKING)
	{
		if (guest->t_id % 2)
		{
			pthread_mutex_lock(guest->forks[LEFT]);
			guest->held_fork[LEFT] = 1;
		}
		else
		{
			pthread_mutex_lock(guest->forks[RIGHT]);
			guest->held_fork[RIGHT] = 1;
		}
		ft_status_change(guest, FORK);
	}
	else if (guest->status == FORK)
	{
		if (guest->t_id % 2)
		{
			pthread_mutex_lock(guest->forks[RIGHT]);
			guest->held_fork[RIGHT] = 1;
		}
		else
		{
			pthread_mutex_lock(guest->forks[LEFT]);
			guest->held_fork[LEFT] = 1;
		}
		ft_status_change(guest, FORK);
	}
	if (guest->status == EATING)
	{
		pthread_mutex_unlock(guest->forks[LEFT]);
		pthread_mutex_unlock(guest->forks[RIGHT]);
		guest->held_fork[LEFT] = 0;
		guest->held_fork[RIGHT] = 0;
	}
	return (SUCCESS);
}


int	ft_fork_handler(t_guest *guest)
{
	int i;

	i = -1;
	if (ft_end_check(guest->data) != SUCCESS)
	{
		if (guest->held_fork[LEFT] == 1)
			pthread_mutex_unlock(guest->forks[LEFT]);
		if (guest->held_fork[RIGHT] == 1)
			pthread_mutex_unlock(guest->forks[RIGHT]);
		return (STANDARD_ERROR);
	}
	ft_fork_appropriation(guest);
	ft_fork_appropriation(guest);
	ft_update_last_meal(guest);
	ft_status_change(guest, EATING);
	ft_precise_sleep(guest);
	ft_fork_appropriation(guest);
	return (SUCCESS);
}

int	ft_eat(t_guest *guest)
{
	if (ft_end_check(guest->data) != SUCCESS)
		return (STANDARD_ERROR);
	if (ft_fork_handler(guest) != SUCCESS)
		return (STANDARD_ERROR);
	return (SUCCESS);
}

int	ft_sleep(t_guest *guest)
{
	if (ft_end_check(guest->data) != SUCCESS)
		return (STANDARD_ERROR);
	ft_status_change(guest, SLEEPING);
	ft_precise_sleep(guest);
	return (SUCCESS);
}

int	ft_think(t_guest *guest)
{
    long long think_time;

    if (ft_end_check(guest->data) != SUCCESS)
        return (STANDARD_ERROR);
    ft_status_change(guest, THINKING);
    if (guest->data->nb_philo % 2 == 1)
    {
        think_time = (guest->data->time_to_eat * 2)
            - guest->data->time_to_sleep;
        if (think_time > 0)
            usleep(think_time * 100);
    }
    return (SUCCESS);
}
