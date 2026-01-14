/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/09 14:10:09 by tibras            #+#    #+#             */
/*   Updated: 2026/01/13 18:55:14 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "so_long_utils.h"

void	ft_printerror(char *msg)
{
	if (!msg)
		return ;
	write(2, msg, ft_strlen(msg));
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

void	ft_clear_game(t_game *game)
{
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
}

void	error_exit(t_game *game, char *msg, int err_nb)
{
	if (err_nb != 0)
		ft_printerror("Error\n");
	ft_printerror(msg);
	get_next_line(-1);
	if (game)
		ft_clear_game(game);
	exit(err_nb);
}