/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 11:03:04 by fardeau           #+#    #+#             */
/*   Updated: 2026/03/08 20:53:24 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

int	ft_map_fill(t_cub *data)
{
	int i;

	i = 0; 
	while (data->file[i + data->index_map_start])
		i++;
	data->map = ft_calloc_gc(i, sizeof(char *), &data->gc_global);
	if (!data->map)
		return (ft_error(ERR_MSG_PARSING, ERR_MSG_MALLOC, ERRN_MALLOC));
	i = -1;
	while (data->file[++i + data->index_map_start])
		data->map[i] = data->file[i + data->index_map_start];
	return (SUCCESS);
}

static int	ft_cell_check(char **map, int y, int x)
{
    if (y == 0 || !map[y + 1])
        return (FAILURE);
    if (x == 0 || x >= (int)ft_strlen(map[y]) - 1)
        return (FAILURE);
    if (x >= (int)ft_strlen(map[y - 1]) || map[y - 1][x] == ' ')
        return (FAILURE);
    if (x >= (int)ft_strlen(map[y + 1]) || map[y + 1][x] == ' ')
        return (FAILURE);
    if (map[y][x - 1] == ' ' || map[y][x + 1] == ' ')
        return (FAILURE);
    return (SUCCESS);
}

int	ft_map_check(t_cub *data)
{
	int x;
	int y;

	y = -1;
	while (data->map[++y])
	{
		x = -1;
		while (data->map[y][++x] && data->map[y][x] != '\n')
		{
			if (ft_ischarset(data->map[y][x], "0NSEW"))
			{
				if (ft_ischarset(data->map[y][x], "NSEW"))
				{
					if (data->start_pos[0] != 0 || data->start_pos[1] != 0)
						return (ft_error(ERR_MSG_PARSING, ERR_MSG_PLAYER_COUNT, ERRN_PARSING));
					data->start_pos[0] = y;
					data->start_pos[1] = x;
				}
				if (ft_cell_check(data->map, y, x) != SUCCESS)
					return (ft_error(ERR_MSG_PARSING, ERR_MSG_WALLS, ERRN_PARSING));
			}
		}
	}
	return (SUCCESS);
}

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
	if (ft_map_fill(data) != SUCCESS)
		ft_exit(data, ERRN_PARSING, NULL, NULL);

	// CHECK MAP
	if (ft_map_check(data) != SUCCESS) 
		// TODO : CHANGE LE EXIT
		ft_exit(data, ERRN_PARSING, NULL, NULL);

	ft_gc_free_all(&data->gc_tmp);
	return (SUCCESS);
}
