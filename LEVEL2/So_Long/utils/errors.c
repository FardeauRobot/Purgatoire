/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/09 14:10:09 by tibras            #+#    #+#             */
/*   Updated: 2026/01/09 19:38:24 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "so_long_utils.h"

void	ft_printerror(char *msg)
{
	write(2, msg, ft_strlen(msg));
}

void error_measure_map(t_game *game, int fd, char *line, char *msg)
{
	// (void)line;
	free(line);
	close (fd);
	error_exit(game, msg);
}

void error_exit(t_game *game, char *msg)
{
	ft_printerror("Error\n");
	if (msg)
		ft_printerror(msg);
	get_next_line(-1);
	if (game)
	{
		if (game->map)
			ft_clear_map(game->map, game->map_height);
		if (game->win)
			mlx_destroy_window(game->mlx, game->win);
		if (game->mlx)
			mlx_destroy_display(game->mlx);
	}
	exit (EXIT_FAILURE);
}