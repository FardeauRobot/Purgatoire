/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   game.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/11 16:47:35 by fardeau           #+#    #+#             */
/*   Updated: 2026/01/13 19:00:45 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "so_long_srcs.h"

int ft_end_game(t_game *game)
{
	if (game->mlx)
		mlx_loop_end(game->mlx);
	return (0);
}

int	ft_check_move(t_game *game, int x, int y)
{
	if (game->map[y][x] == '1')
		return (FAILURE);
	else if (game->map[y][x] == 'C')
	{
		game->collected++;
		game->map[y][x] = '0';
	}
	else if (game->map[y][x] == 'E' && game->collected != game->collectibles)
		ft_printf("You didn´t gather all the treasures : Collected = %d\n", game->collected);
	else if (game->map[y][x] == 'E' && game->collected >= game->collectibles)
		error_exit(game, "YOU COLLECTED ALL THE TREASURES\n", WIN);
	return (SUCCESS);
}

int	ft_affect_pos(t_game *game, int direction, int pos_x, int pos_y )
{
	if (direction == UP)
		pos_y--;
	if (direction == DOWN)
		pos_y++;
	if (direction == LEFT)
		pos_x--;
	if (direction == RIGHT)
		pos_x++;
	if (ft_check_move(game, pos_x, pos_y) == FAILURE)
		return (FAILURE);
	return (SUCCESS);
}

void	ft_move(t_game *game, int direction)
{
	int pos_x;
	int pos_y;

	pos_x = game->player_pos[X];
	pos_y = game->player_pos[Y];
	if (ft_affect_pos(game, direction, pos_x, pos_y) == SUCCESS)
	{
		if (direction == UP)
			game->player_pos[Y]--;
		if (direction == DOWN)
			game->player_pos[Y]++;
		if (direction == LEFT)
			game->player_pos[X]--;
		if (direction == RIGHT)
			game->player_pos[X]++;
		if (game->map[pos_y][pos_x] != 'E')
			ft_put_img(game, &game->assets[GROUND], pos_x, pos_y);
		else 
			ft_put_img(game, &game->assets[EXIT], pos_x, pos_y);
	}
}

int	ft_handle_keys(int keycode, t_game *game)
{
	if (keycode == KEY_ESC)
		ft_end_game(game);
	else if (keycode == KEY_A)
		ft_move(game, LEFT);
	else if (keycode == KEY_D)
		ft_move(game, RIGHT);
	else if (keycode == KEY_W)
		ft_move(game, UP);
	else if (keycode == KEY_S)
		ft_move(game, DOWN);
	else if (keycode == KEY_E)
	{
		game->active_char++; 
		game->active_char = game->active_char % NB_CHARS;
	}
	else if (keycode == KEY_UP)
		mlx_clear_window(game->mlx, game->win);
	return (0);
}

void	ft_launch_game(t_game *game)
{
	mlx_clear_window(game->mlx, game->win);
}
