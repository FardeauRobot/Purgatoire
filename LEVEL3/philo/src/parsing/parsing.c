/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/02 14:31:25 by tibras            #+#    #+#             */
/*   Updated: 2026/03/03 14:46:04 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

// CHECK NUMS
int ft_isnum(char *str)
{
	int i;

	i = -1;
	while (str[++i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (STANDARD_ERROR);
	}
	return (SUCCESS);
}


// INIT NUMBERS
static int	ft_atoll_safe(char *str, long long *nb)
{
	int i;

	if (ft_isnum(str) != SUCCESS)
		ft_exit(NULL, INVALID_PARAM, str, ERRN_PARSING);
	i = -1;
	while (str[++i])
		*nb = *nb * 10 + str[i] - '0';
	if (*nb > MAX_TIME)
		ft_exit(NULL, ERR_TIME, str, ERRN_PARSING);
	if (*nb == 0)
		ft_exit(NULL, INVALID_PARAM, str, ERRN_PARSING);
	return (SUCCESS);
}

// ARGV[1] = NBR OF PHILO
// ARGV[2] = TIME TO DIE
// ARGV[3] = TIME TO EAT
// ARGV[4] = TIME TO SLEEP
// ARGV[5] = NBR OF MEALS
static void	ft_philo_fill(t_philo *philo, long long tmp, int i)
{
	if (i == 1)
	{
		if (tmp > MAX_GUESTS)
			ft_exit(NULL, ERR_GUESTS, ERR_MAX_CAP, ERRN_PARSING);
		philo->nb_philo = (int)tmp;
		philo->nb_forks = (int)tmp;
	}
	if (i == 2)
		philo->time_to_die = (time_t)tmp;
	if (i == 3)
		philo->time_to_eat = (time_t)tmp;
	if (i == 4)
		philo->time_to_sleep = (time_t)tmp;
	if (i == 5)
	{
		if (tmp > MAX_MEALS)
			ft_exit(NULL, ERR_MEALS, ERR_MAX_MEALS, ERRN_PARSING);
		philo->needed_meals = (int)tmp;
	}
}

int	ft_parsing(t_philo *philo, int argc, char **argv)
{
	int i;
	long long tmp;
	if (argc < 5 || argc > 6)
		return (ft_error(ERR_MSG_ARGS, EXPECTED_ARGS, ERRN_PARSING));
	i = 0;
	while (argv[++i])
	{
		tmp = 0;
		ft_atoll_safe(argv[i], &tmp);
		ft_philo_fill(philo, tmp, i);
	}
	return (SUCCESS);
}
