/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 11:59:48 by fardeau           #+#    #+#             */
/*   Updated: 2026/03/11 19:29:41 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

// UTILITARY FILE IN CHARGE OF PARSING

/*
** FT_FORMAT_CHECK - VALIDATES THAT THE FILE EXTENSION IS .CUB
** RETURNS SUCCESS IF VALID, FAILURE OTHERWISE
*/

void	ft_player_set(t_player *player, int x, int y, char orient)
{
	player->pos_y = (double)y + 0.5;
	player->pos_x = (double)x + 0.5;
	if (orient == 'N')
	{
		player->dir_x = 0;
		player->dir_y = -1;
		player->orient = NORTH;
	}
	if (orient == 'S')
	{
		player->dir_x = 0;
		player->dir_y = 1;
		player->orient = SOUTH;
	}
	if (orient == 'E')
	{
		player->dir_x = 1;
		player->dir_y = 0;
		player->orient = EAST;
	}
	if (orient == 'W')
	{
		player->dir_x = -1;
		player->dir_y = 0;
		player->orient = WEST;
	}
}

int	ft_format_check(char *filepath)
{
	int	len_path;

	len_path = ft_strlen(filepath);
	if (ft_strncmp(&filepath[len_path - 4], ".cub", 4))
		return (FAILURE);
	return (SUCCESS);
}