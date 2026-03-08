/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 00:00:00 by author            #+#    #+#             */
/*   Updated: 2026/03/08 19:10:15 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

/*
** print_error - prints a formatted error message to stderr
** Format: "project: <context>: <detail>\n"
*/
int	ft_error(char *context, char *detail, int error)
{
	ft_putstr_fd("Error\n", 2);
	if (context)
	{
		ft_putstr_fd(context, 2);
		ft_putstr_fd(": ", 2);
	}
	if (detail)
		ft_putstr_fd(detail, 2);
	ft_putstr_fd("\n", 2);
	return (error);
}

void	ft_data_clean(t_cub *data)
{
	get_next_line(-1);
	close(data->fd_map);
	ft_gc_free_all(&data->gc_global);
	ft_gc_free_all(&data->gc_tmp);
}

/*
** exit_error - prints error, frees resources, and exits with FAILURE
*/
void	ft_exit(t_cub *data, int error, char *context, char *detail)
{

	ft_error(context, detail, error);
	ft_data_clean(data);
	exit(error);
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
