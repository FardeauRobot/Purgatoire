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
** FT_ERROR - PRINTS A FORMATTED ERROR MESSAGE TO STDERR
** FORMAT: "ERROR\n<CONTEXT>: <DETAIL>\n"
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

/*
** FT_DATA_CLEAN - FREES ALL RESOURCES (GNL BUFFER, FD, GARBAGE COLLECTORS)
*/
void	ft_data_clean(t_cub *data)
{
	get_next_line(-1);
	close(data->fd_map);
	ft_gc_free_all(&data->gc_global);
	ft_gc_free_all(&data->gc_tmp);
}

/*
** FT_EXIT - PRINTS ERROR, FREES RESOURCES, AND EXITS WITH ERROR CODE
*/
void	ft_exit(t_cub *data, int error, char *context, char *detail)
{

	ft_error(context, detail, error);
	ft_data_clean(data);
	exit(error);
}

/*
** CHECK_ARGS - VALIDATES ARGUMENT COUNT
** RETURNS SUCCESS IF ARGC == EXPECTED, PRINTS USAGE AND RETURNS FAILURE OTHERWISE
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
