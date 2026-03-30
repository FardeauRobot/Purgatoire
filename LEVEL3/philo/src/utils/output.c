/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   output.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/02 18:58:49 by fardeau           #+#    #+#             */
/*   Updated: 2026/03/09 14:00:21 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	ft_putstr_fd(char *str, int fd)
{
	int	i;

	i = 0;
	while (str[i])
		i++;
	if (i > 0)
		write(fd, str, i);
}

void	ft_putendl_fd(char *str, int fd)
{
	ft_putstr_fd(str, fd);
	write(fd, "\n", 1);
}

void	ft_action_print(t_guest *guest)
{
	long long	tsp;

	pthread_mutex_lock(guest->print);
	tsp = ft_get_time(MILLISECONDS);
	tsp -= guest->data->start_time;
	if (guest->status == DEAD)
		printf(RED"%lld %d %s"RESET, tsp, guest->t_id, STR_DEAD);
	if (ft_dead_check(guest->data))
	{
		pthread_mutex_unlock(guest->print);
		return ;
	}
	if (guest->status == EATING)
		printf(GREEN"%lld %d %s"RESET, tsp, guest->t_id, STR_EAT);
	if (guest->status == SLEEPING)
		printf(BLUE"%lld %d %s"RESET, tsp, guest->t_id, STR_SLEEPING);
	if (guest->status == FORK)
		printf(YELLOW"%lld %d %s"RESET, tsp, guest->t_id, STR_FORK);
	if (guest->status == THINKING)
		printf(CYAN"%lld %d %s"RESET, tsp, guest->t_id, STR_THINKING);
	pthread_mutex_unlock(guest->print);
}
