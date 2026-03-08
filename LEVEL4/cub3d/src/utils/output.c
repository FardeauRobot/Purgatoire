/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   output.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 11:55:12 by fardeau           #+#    #+#             */
/*   Updated: 2026/03/08 19:25:06 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

/*
** FT_MATRIX_PRINT - PRINTS ALL STRINGS OF A NULL-TERMINATED CHAR ** TO STDOUT
*/
void	ft_matrix_print(char **matrix)
{
	int i;

	i = -1;
	while (matrix[++i])
		ft_putstr_fd(matrix[i], STDOUT_FILENO);
	ft_putchar_fd('\n', STDOUT_FILENO);
}

/*
** FT_TEXTURES_PRINT - PRINTS ALL TEXTURE AND COLOR PATHS FOR DEBUG
*/
void	ft_textures_print(t_textures *textures)
{
	ft_printf("[NORTH = %s]\n", textures->north);
	ft_printf("[SOUTH = %s]\n", textures->south);
	ft_printf("[WEST = %s]\n", textures->west);
	ft_printf("[EAST = %s]\n", textures->east);
	ft_printf("[FLOOR = %s]\n", textures->floor);
	ft_printf("[CEILING = %s]\n", textures->ceiling);
}

/*
** FT_CUB_PRINT - PRINTS THE MAIN CUB3D STRUCT FIELDS FOR DEBUG
*/
void	ft_cub_print(t_cub *data)
{
	ft_printf("[CUB3D MAIN STRUCT]\n");
	ft_printf("[FD_MAP = %d]\n", data->fd_map);
	// ft_matrix_print(data->file);
	ft_textures_print(&data->textures);
	ft_printf("[INDEX START = %d]\n", data->index_map_start);
}