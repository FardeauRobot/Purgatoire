/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 00:00:00 by author            #+#    #+#             */
/*   Updated: 2026/03/02 08:35:38 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/*
** print_error - prints a formatted error message to stderr
** Format: "project: <context>: <detail>\n"
*/
void	print_error(char *context, char *detail)
{
	ft_putstr_fd("philo: ", 2);
	if (context)
	{
		ft_putstr_fd(context, 2);
		ft_putstr_fd(": ", 2);
	}
	if (detail)
		ft_putstr_fd(detail, 2);
	ft_putstr_fd("\n", 2);
}

/*
** exit_error - prints error, frees resources, and exits with FAILURE
*/
void	exit_error(t_data *data, char *context, char *detail)
{
	print_error(context, detail);
	// ADD FREE

	exit(EXIT_FAILURE);
}

/*
** check_args - validates argument count
** Returns SUCCESS if argc == expected, prints usage and returns FAILURE otherwise
*/
int	check_args(int argc, int expected, char *usage)
{
	if (argc != expected)
	{
		ft_putstr_fd("Usage: ", 2);
		ft_putstr_fd(usage, 2);
		ft_putstr_fd("\n", 2);
		return (FAILURE);
	}
	return (SUCCESS);
}
