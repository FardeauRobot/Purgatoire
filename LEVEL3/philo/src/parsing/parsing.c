/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/02 14:31:25 by tibras            #+#    #+#             */
/*   Updated: 2026/03/02 14:43:27 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	ft_parsing(t_philo *philo, int argc, char **argv)
{
	if (argc < 4 || argc > 5)
		return (ft_error(ERR_MSG_ARGS, EXPECTED_ARGS, ERR_ARGS));
	(void)philo;
	(void)argv;
	return (SUCCESS);
}
