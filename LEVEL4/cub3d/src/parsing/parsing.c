/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 11:03:04 by fardeau           #+#    #+#             */
/*   Updated: 2026/03/15 16:41:27 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

// FILE IN CHARGE OF THE PARSING BEFORE EXECUTION OF THE GAME

static int	ft_map_width(char **map)
{
	int	i;
	int	len;
	int	max;

	i = 0;
	max = 0;
	while (map[i])
	{
		len = ft_strlen(map[i]);
		if (len > max)
			max = len;
		i++;
	}
	return (max);
}

// FILL THE MAP BASED ON THE FILE STORED IN T_CUB FILE
void	ft_map_fill(t_cub *data)
{
	int	i;

	i = 0;
	while (data->file[i + data->map.index_map_start])
		i++;
	data->map.map = ft_calloc_gc(i + 1, sizeof(char *), &data->gc_global);
	if (!data->map.map)
		ft_exit(data, ERRN_MALLOC, ERR_MSG_PARSING, ERR_MSG_MALLOC);
	i = -1;
	while (data->file[++i + data->map.index_map_start])
		data->map.map[i] = data->file[i + data->map.index_map_start];
	data->map.height = i;
	data->map.width = ft_map_width(data->map.map);
}

// CHECKS FOR EMPTY CELLS OR NO WALLS ON THE T_DATA MAP
static int	ft_cell_check(char **map, int y, int x)
{
	if (y == 0 || !map[y + 1])
		return (FAILURE);
	if (x == 0)
		return (FAILURE);
	if (x >= (int)ft_strlen(map[y + 1]) || x >= (int)ft_strlen(map[y - 1]))
		return (FAILURE);
	if (ft_isspace(map[y - 1][x]) || ft_isspace(map[y + 1][x]))
		return (FAILURE);
	if (ft_isspace(map[y][x - 1]) || ft_isspace(map[y][x + 1]))
		return (FAILURE);
	return (SUCCESS);
}

// GOES THROUGH ALL THE MAPS AND CHECK FOR INVALID CHARS / MAP NOT CLOSED BY WALLS
// ALSO INITALIZES INFOS ABOUT PLAYER (POS AND ORIENTATION)
void	ft_map_check(t_cub *data)
{
	int		x;
	int		y;
	char	**map;

	map = data->map.map;
	y = -1;
	while (map[++y])
	{
		x = -1;
		while (map[y][++x] && map[y][x] != '\n')
		{
			if (ft_ischarset(map[y][x], "01NSEW"))
			{
				if (ft_ischarset(map[y][x], "NSEW"))
				{
					if (data->player.pos_y != 0 || data->player.pos_x != 0)
						ft_exit(data, ERRN_PARSING, ERR_MSG_PARSING, ERR_MSG_PLAYER_COUNT);
					ft_player_set(&data->player, x, y, map[y][x]);
				}
				if (map[y][x] != '1' && ft_cell_check(map, y, x) != SUCCESS)
					ft_exit(data, ERRN_PARSING, ERR_MSG_PARSING, ERR_MSG_WALLS);
			}
			else if (!ft_isspace(map[y][x]))
				ft_exit(data, ERRN_PARSING, ERR_MSG_PARSING, ERR_MSG_INVALID_CHAR);
		}
	}
}

// TODO : HOW DO WE HANDLE THE CASE WHERE A TEXTURE INFO IS AFTER THE MAP

/*
** FT_PARSING - MAIN PARSING ENTRY POINT
** VALIDATES ARGS, FILE FORMAT, STORES FILE, PARSES TEXTURES AND MAP
*/
int	ft_parsing(t_cub *data, char **argv, int argc)
{
	if (argc != 2)
		ft_exit(data, ERRN_PARSING, ERR_MSG_ARGS, ERR_MSG_ARGC);

	// CHECK FORMAT
	if (ft_format_check(argv[1]) != SUCCESS)
		ft_exit(data, ERRN_PARSING, ERR_MSG_OPEN, ERR_MSG_FORMAT);

	// CHECK OPEN
	data->fd_map = open(argv[1], O_RDONLY);
	if (data->fd_map == -1)
		ft_exit(data, ERRN_OPEN, ERR_MSG_OPEN, argv[1]);

	// STORE THE FILE
	if (ft_file_store(data) != SUCCESS)
		ft_exit(data, ERRN_PARSING, NULL, NULL);

	// CHECK TEXTURES
	if (ft_textures_parsing(data) != SUCCESS)
		ft_exit(data, ERRN_PARSING, ERR_MSG_PARSING, ERR_MSG_TEXTURES);

	// FILL MAP
	ft_map_fill(data);

	// CHECK MAP
	ft_map_check(data);

	// RELEASE TMP MALLOCS (SPLITS / GNL / FILE)
	ft_gc_free_all(&data->gc_tmp);
	return (SUCCESS);
}
