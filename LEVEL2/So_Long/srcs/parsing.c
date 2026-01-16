/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/08 18:21:27 by tibras            #+#    #+#             */
/*   Updated: 2026/01/16 17:20:09 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "so_long_srcs.h"

int	ft_char_check(char *str)
{
	int		i;
	char	*accepted;

	accepted = "01CEP\n";
	i = 0;
	while (str[i])
	{
		if (!ft_ischarset(str[i], accepted))
			return (FAILURE);
		i++;
	}
	return (SUCCESS);
}

int	ft_format_check(char *filepath)
{
	int	len_path;

	len_path = ft_strlen(filepath);
	if (ft_strncmp(&filepath[len_path - 4], ".ber", 4))
		return (FAILURE);
	return (SUCCESS);
}

void	ft_get_height(t_game *game, int fd)
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
	get_next_line(-1);
	// if (game->map_height * IMG_SIZE > game->display_height || game->map_width * IMG_SIZE > game->display_width)
	// 		error_measure_map(game, fd, line, ERRN_DISPLAY_SIZE);
}

int	ft_check_walls(t_game *game, char *row_map, size_t row)
{
	size_t	i;

	i = 0;
	if (row == 0 || row == game->map_height - 1)
	{
		while (row_map[i] && row_map[i] != '\n')
		{
			if (row_map[i] != '1')
				return (FAILURE);
			i++;
		}
	}
	else
		if (row_map[0] != '1' || row_map[game->map_width - 1] != '1')
			return (FAILURE);
	return (SUCCESS);
}

// ft_check_components(&player)
void	ft_find_assets(t_game *game, char *line, int nb_line)
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

void	ft_fill_map(t_game *game, char **map, int fd)
{
	size_t	i;

	if (fd == -1)
		error_exit (game, ERRS_OPEN_MAP, ERRN_OPEN_MAP);
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

void	ft_init_game(t_game *game)
{
	// mlx_get_screen_size(game->mlx, &game->display_width, &game->display_height);
	game->collectibles = 0;
	game->exit = 0;
	game->nb_player = 0;
	game->frame = 0;
	game->active_char = 0;
	game->move_count = 0;
	game->last_frame_ms = 0;
	game->last_move_ms = 0;
	game->frame_assets = 0;
	game->last_frame_assets_ms = 0;
	game->move = IDLE;
	game->orient = O_LEFT;
	game->map = NULL;
}

char **ft_create_map_ff(t_game *game)
{
	char **map_cpy;
	size_t	i;

	i = 0;
	map_cpy = malloc(sizeof(char *) * game->map_height);
	if (!map_cpy)
		error_exit(game, "Error malloc flood-fill\n", ERRN_MALLOC);
	ft_bzero(map_cpy, game->map_height * sizeof(char *));
	while (i < game->map_height)
	{
		map_cpy[i] = ft_strdup(game->map[i]);
		if (!map_cpy[i])
		{
			ft_clear_map(map_cpy, i);
			error_exit(game, "Error malloc flood-fill\n", ERRN_MALLOC);
		}
		i++;
	}
	return (map_cpy);
}

int ft_flood_fill(char **map, int y, int x, int *gold_count)
{
	int	exit_found;

	exit_found = 0;
	if (map[y][x] == '1' || map[y][x] == 'F')
		return (FAILURE);
	if (map[y][x] == 'C')
		(*gold_count)--;
	if (map[y][x] == 'E')
		exit_found = 1;
	map[y][x] = 'F';
	if (ft_flood_fill(map, y - 1, x, gold_count))
		exit_found = 1;
	if (ft_flood_fill(map, y + 1, x, gold_count))
		exit_found = 1;
	if (ft_flood_fill(map, y, x + 1, gold_count))
		exit_found = 1;
	if (ft_flood_fill(map, y, x - 1, gold_count))
		exit_found = 1;
	return (SUCCESS);
}

void	ft_check_exit(t_game *game)
{
	char **map_ff;
	int		gold_count;

	gold_count = game->collectibles;
	map_ff = ft_create_map_ff(game);
	if (!map_ff)
		error_exit(game, "Error malloc flood-fill\n", ERRN_MALLOC);
	if (!ft_flood_fill(map_ff, game->player_pos[1], game->player_pos[0], &gold_count) || gold_count != 0)
	{
		ft_clear_map(map_ff, game->map_height);
		error_exit(game, ERRS_MAP_EXIT, ERRN_MAP_EXIT);
	}
	ft_clear_map(map_ff, game->map_height);
	
}

void	ft_check_assets(t_game *game)
{
	if (game->collectibles < 1)
		error_exit(game, ERRS_MAP_COLLECT, ERRN_MAP_COLLECT);
	if (game->exit != 1)
		error_exit(game, ERRS_MAP_EXIT, ERRN_MAP_EXIT);
	if (game->nb_player != 1)
	{
		if (game->nb_player > 1)
			error_exit(game, "Too much players detected\n", ERRN_MAP_PLAYER);
		error_exit(game, ERRS_MAP_PLAYER, ERRN_MAP_PLAYER);
	}
}

void	ft_parsing(t_game *game, char *path_map)
{
	int	fd;

	if (!ft_format_check(path_map))
		error_exit (game, ERRS_BER, ERRN_BER);
	fd = open(path_map, O_RDONLY);
	if (fd == -1)
		error_exit (game, ERRS_OPEN, ERRN_OPEN);
	ft_init_game(game);
	ft_get_height(game, fd);
	game->map = malloc (sizeof(char *) * game->map_height);
	if (!game->map)
		error_exit(game, ERRS_MALLOC_MAP, ERRN_MALLOC);
	fd = open(path_map, O_RDONLY);
	ft_fill_map(game, game->map, fd);
	ft_check_assets(game);
	ft_check_exit(game);
}
