/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   game.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/11 16:47:35 by fardeau           #+#    #+#             */
/*   Updated: 2026/01/11 18:15:26 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "so_long_srcs.h"

int ft_end_game(t_game *game)
{
	ft_clear_imgs(game, &game->menu);
	if (game->mlx)
		mlx_loop_end(game->mlx);
	return (0);
}

int	ft_handle_keys_game(int keycode, t_game *game)
{
	if (keycode == KEY_ESC)
		ft_end_game(game);
	else if (keycode == KEY_A)
		ft_put_img(game, &game->menu.characters[0][IDLE][0], 100, 100);
	else if (keycode == KEY_D)
		mlx_clear_window(game->mlx, game->win);
	return (0);
}

void ft_launch_game(t_game *game)
{
	// ft_clear_menu(game);
	game->display = GAME;
	// ft_load_game(game, game->ingame);
	mlx_clear_window(game->mlx, game->win);
}