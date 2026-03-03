/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/02 13:03:00 by tibras            #+#    #+#             */
/*   Updated: 2026/03/03 20:08:13 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	main(int argc, char **argv)
{
	t_philo	philo;

	ft_bzero(&philo, sizeof(t_philo));
	if (ft_parsing(&philo, argc, argv) != SUCCESS)
		return (ERRN_PARSING);
	ft_start(&philo);
	return (EXIT_SUCCESS);
}
