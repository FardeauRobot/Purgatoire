/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   output.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/02 18:58:49 by fardeau           #+#    #+#             */
/*   Updated: 2026/03/03 09:27:06 by tibras           ###   ########.fr       */
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