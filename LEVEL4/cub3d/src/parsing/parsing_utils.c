/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 11:59:48 by fardeau           #+#    #+#             */
/*   Updated: 2026/03/11 11:07:47 by tibras           ###   ########.fr       */
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
	player->pos[0] = (double)y;
	player->pos[1] = (double)x;
	if (orient == 'N')
		player->orient = NORTH;
	if (orient == 'S')
		player->orient = SOUTH;
	if (orient == 'E')
		player->orient = EAST;
	if (orient == 'W')
		player->orient = WEST;
}

int	ft_format_check(char *filepath)
{
	int	len_path;

	len_path = ft_strlen(filepath);
	if (ft_strncmp(&filepath[len_path - 4], ".cub", 4))
		return (FAILURE);
	return (SUCCESS);
}