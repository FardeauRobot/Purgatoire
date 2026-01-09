/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/08 18:21:27 by tibras            #+#    #+#             */
/*   Updated: 2026/01/09 20:34:56 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../so_long.h"

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
	int len_path;

	len_path = ft_strlen(filepath);
	if (ft_strncmp(&filepath[len_path - 4], ".ber", 4))
		return (FAILURE);
	return (SUCCESS);
}

int ft_get_height(t_game *game, int fd)
{
	char	*line;
	size_t	line_len;

	line = get_next_line(fd);
	if (!line)
		error_exit(game, "Empty file !\n");
	game->map_height = 0;
	game->map_width = ft_strlen(line) - 1;
	while (line)
	{
		if (!ft_char_check(line))
			error_measure_map(game, fd, line, "Incorrect characters in the map\n");
		line_len = ft_strlen(line);
		if (line[line_len - 1] == '\n')
			line_len--;
		game->map_height++;
		if (game->map_width != line_len)
			error_measure_map(game, fd, line, "Invalid shape of the map\n");
		free(line);
		line = get_next_line(fd);
	}
	get_next_line(-1);
	close (fd);
	return (0);
}

int	ft_check_walls(t_game *game, char *row_map, size_t row)
{
	size_t i;

	i = 0;
	if (row == 0 || row == game->map_height - 1)
	{
		while(row_map[i] && row_map[i] != '\n')
		{
			if (row_map[i] != '1')
				return (FAILURE);
			i++;
		}
	}
	else
		if (row_map[0] != '1' || row_map[game->map_width - 2] != '1')
			return (FAILURE);
	return (SUCCESS);
}

// ft_check_components(&player)

void	ft_fill_map(t_game *game, char **map, int fd)
{
	size_t	i;
	int		player;
	int		collectibles;
	int		exit;


	if (fd == -1)
		error_exit (game, "Can't open file to fill the map\n"); // ERREUR OPEN
	i = 0;
	player = 0;
	collectibles = 0;
	exit = 0;
	while (i < game->map_height)
	{
		map[i] = get_next_line(fd);
		if (!map[i] || ft_check_walls(game, map[i], i) == FAILURE)
			ft_clear_map(map, game->map_height);
		// ft_check_components(&player, &collectibles, &exit);
		i++;
	}
	close(fd);
}

int ft_parsing(t_game *game, char *path_map)
{
	int fd;

	if (!ft_format_check(path_map))
		error_exit (game, "File not ending with .ber\n"); // ERREUR OPEN
	fd = open(path_map, O_RDONLY);
	if (fd == -1)
		error_exit (game, "Can't open file to affect\n"); // ERREUR OPEN
	ft_get_height(game, fd);
	game->map = malloc (sizeof(char *) * game->map_height);
	if (!game->map)
		error_exit(game, "Fail malloc map[]\n"); // ERREUR MALLOC
	fd = open(path_map, O_RDONLY);
	ft_fill_map(game, game->map, fd);
	// ft_path_check(game->map);
	return (0);
}
