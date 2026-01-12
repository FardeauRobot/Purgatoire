/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   game.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/11 16:47:35 by fardeau           #+#    #+#             */
/*   Updated: 2026/01/12 17:05:00 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "so_long_srcs.h"

int	ft_end_game(t_game *game)
{
	ft_clear_imgs(game);
	if (game->mlx)
		mlx_loop_end(game->mlx);
	return (0);
}

int	ft_handle_keys_game(int keycode, t_game *game)
{
	if (keycode == KEY_ESC)
		ft_end_game(game);
	else if (keycode == KEY_A)
		ft_put_img(game, &game->characters[0][IDLE][LEFT][0], 100, 100);
	else if (keycode == KEY_D)
		mlx_clear_window(game->mlx, game->win);
	return (0);
}

void	ft_launch_game(t_game *game)
{
	mlx_clear_window(game->mlx, game->win);
}
