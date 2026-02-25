/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/09 17:35:57 by tibras            #+#    #+#             */
/*   Updated: 2026/02/24 11:56:55 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "so_long_utils.h"

void	ft_free_paths(char *path_l, char *path_r)
{
	if (path_l)
		free(path_l);
	if (path_r)
		free(path_r);
}

void	ft_clear_map(char **map, ssize_t height)
{
	ssize_t	i;

	i = 0;
	while (i < height)
	{
		if (map[i])
			free(map[i]);
		i++;
	}
	if (map)
		free(map);
}

int	ft_clear_game(t_game *game)
{
	char	*tmp;

	tmp = get_next_line(-1);
	if (tmp)
		free(tmp);
	ft_end_game(game);
	ft_clear_imgs(game);
	if (game->map)
		ft_clear_map(game->map, game->map_height);
	if (game->win)
		mlx_destroy_window(game->mlx, game->win);
	if (game->mlx)
	{
		mlx_destroy_display(game->mlx);
		free(game->mlx);
		game->mlx = NULL;
	}
	game = NULL;
	return (0);
}

void	error_measure_map(t_game *game, int fd, char *line, int err_nb)
{
	free(line);
	close (fd);
	if (err_nb == ERRN_MAP_CHARS)
		error_exit(game, ERRS_MAP_CHARS, err_nb);
	else if (err_nb == ERRN_MAP_SHAPE)
		error_exit(game, ERRS_MAP_SHAPE, err_nb);
	else if (err_nb == ERRN_MAP_WALLS)
		error_exit(game, ERRS_MAP_WALLS, err_nb);
}

int	ft_path_append(char *dst, char *src)
{
	size_t	res;

	res = ft_strlcat(dst, src, PATH_SIZE);
	return (res >= PATH_SIZE);
}
