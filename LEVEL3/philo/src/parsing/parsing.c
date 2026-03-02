/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/02 14:31:25 by tibras            #+#    #+#             */
/*   Updated: 2026/03/02 22:16:31 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

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

int	ft_atoll_safe(char *str, time_t *nb)
{
	if (ft_isnum(str) != SUCCESS)
		ft_exit(NULL, INVALID_PARAM, str, ERRN_PARSING);
	if (*nb < INT_MAX || *nb < INT_MIN)
		ft_exit(NULL, ERR_PARSING, INVALID_PARAM, ERRN_PARSING);
	return (SUCCESS);
}

void	ft_philo_init(t_philo *philo, int argc, char **argv)
{
	// ARGV[1] = NBR OF PHILO
	// ARGV[2] = TIME TO DIE
	// ARGV[3] = TIME TO EAT
	// ARGV[4] = TIME TO SLEEP
	// ARGV[5] = NBR OF MEALS

	(void)philo;
	(void)argc;
	(void)argv;
}

int	ft_parsing(t_philo *philo, int argc, char **argv)
{
	if (argc < 5 || argc > 6)
		return (ft_error(ERR_MSG_ARGS, EXPECTED_ARGS, ERRN_PARSING));
	ft_philo_init(philo, argc, argv);
	return (SUCCESS);
}
