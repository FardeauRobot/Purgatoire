/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   output.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/02 18:58:49 by fardeau           #+#    #+#             */
/*   Updated: 2026/03/06 09:16:15 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"


void	ft_putstr_fd(char *str, int fd)
{
	int i;

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
	long long tsp;

	tsp = ft_get_time(MILLISECONDS);
	tsp -= guest->data->start_time;
	pthread_mutex_lock(guest->print);
	if (guest->status == EATING)
		printf(GREEN"%lld %d %s"RESET, tsp, guest->t_id, STR_EAT);
	if (guest->status == SLEEPING)
		printf(CYAN"%lld %d %s"RESET, tsp, guest->t_id, STR_SLEEPING);
	if (guest->status == FORK)
		printf(YELLOW"%lld %d %s"RESET, tsp, guest->t_id, STR_FORK);
	if (guest->status == THINKING)
		printf(BLUE"%lld %d %s"RESET, tsp, guest->t_id, STR_THINKING);
	if (guest->status == DEAD)
		printf(RED"%lld %d %s"RESET, tsp, guest->t_id, STR_DEAD);
	pthread_mutex_unlock(guest->print);
}

// TODO : A DELETE
void	ft_print_main_philo(t_philo *philo)
{
	printf("[NB OF GUESTS = %d]\n", philo->nb_philo);
	printf("[NB OF FORKS = %d]\n", philo->nb_forks);
	printf("[TIME TO DIE = %ld]\n",(long)philo->time_to_die);
	printf("[TIME TO EAT = %ld]\n",(long)philo->time_to_eat);
	printf("[TIME TO SLEEP = %ld]\n",(long)philo->time_to_sleep);
	printf("[NB OF MEALS = %d]\n", philo->needed_meals);
}