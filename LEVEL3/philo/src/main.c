/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/02 13:03:00 by tibras            #+#    #+#             */
/*   Updated: 2026/03/03 15:15:45 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	main(int argc, char **argv)
{
	t_philo	philo;

	ft_bzero(&philo, sizeof(t_philo));
	if (ft_parsing(&philo, argc, argv) != SUCCESS)
		return (ERRN_PARSING);
	ft_print_main_philo(&philo);
	ft_start(&philo);
	return (EXIT_SUCCESS);
}
