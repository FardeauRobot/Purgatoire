/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   output.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 11:55:12 by fardeau           #+#    #+#             */
/*   Updated: 2026/03/11 18:53:44 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

// TODO : A DELETE A LA FIN
//  FOR THE TEST

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
	printf("[CUB3D MAIN STRUCT]\n");
	printf("[FD_MAP = %d]\n", data->fd_map);
	printf("[INDEX START = %d]\n", data->map.index_map_start);
	printf("[MAP SIZE = %d x %d]\n", data->map.width, data->map.height);
	printf("[PLAYER START = [%.2f|%.2f]]\n", data->player.pos_y, data->player.pos_x);
	ft_textures_print(&data->textures);
	printf("[MAP]\n");
	ft_matrix_print(data->map.map);
}