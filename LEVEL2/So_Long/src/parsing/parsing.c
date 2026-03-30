/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/08 18:21:27 by tibras            #+#    #+#             */
/*   Updated: 2026/02/24 10:23:19 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "so_long_srcs.h"

static void	ft_get_height(t_game *game, int fd)
{
	char	*line;
	size_t	line_len;

	line = get_next_line(fd);
	if (!line)
		error_exit(game, ERRS_EMPTY, ERRN_EMPTY);
	game->map_height = 0;
	game->map_width = ft_strlen(line) - 1;
	while (line)
	{
		if (!ft_char_check(line))
			error_measure_map(game, fd, line, ERRN_MAP_CHARS);
		line_len = ft_strlen(line);
		if (line[line_len - 1] == '\n')
			line_len--;
		game->map_height++;
		if (game->map_width != line_len)
			error_measure_map(game, fd, line, ERRN_MAP_SHAPE);
		free(line);
		line = get_next_line(fd);
	}
}

static void	ft_find_assets(t_game *game, char *line, int nb_line)
{
	int	i;

	i = 0;
	while (line[i])
	{
		if (line[i] == 'C')
			game->collectibles++;
		if (line[i] == 'E')
			game->exit++;
		if (line[i] == 'P')
		{
			if (game->nb_player == 0)
			{
				game->player_pos[0] = i;
				game->player_pos[1] = nb_line;
			}
			game->nb_player++;
		}
		i++;
	}
}

static void	ft_fill_map(t_game *game, char **map, int fd)
{
	size_t	i;

	i = 0;
	while (i < game->map_height)
	{
		map[i] = get_next_line(fd);
		if (!map[i])
			error_exit(game, "Failed allocation for the map\n", ERRN_MALLOC);
		ft_find_assets(game, map[i], i);
		if (ft_check_walls(game, map[i], i) == FAILURE)
			error_exit(game, ERRS_MAP_WALLS, ERRN_MAP_WALLS);
		i++;
	}
	close(fd);
}

void	ft_parsing(t_game *game, char *path_map)
{
	int	fd;

	if (!ft_format_check(path_map))
		error_exit (game, ERRS_BER, ERRN_BER);
	fd = open(path_map, O_RDONLY);
	if (fd == -1)
		error_exit (game, ERRS_OPEN, ERRN_OPEN);
	ft_bzero(game, sizeof(t_game));
	ft_get_height(game, fd);
	close(fd);
	game->map = malloc(sizeof(char *) * game->map_height);
	if (!game->map)
		error_exit(game, ERRS_MALLOC_MAP, ERRN_MALLOC);
	ft_bzero(game->map, game->map_height * sizeof(char *));
	fd = open(path_map, O_RDONLY);
	if (fd == -1)
		error_exit (game, ERRS_OPEN, ERRN_OPEN);
	ft_fill_map(game, game->map, fd);
	ft_check_assets(game);
	ft_check_exit(game);
}
