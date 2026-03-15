/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   game.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 21:54:39 by fardeau           #+#    #+#             */
/*   Updated: 2026/03/15 19:13:08 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	ft_rotate(t_player *player)
{
	double	old_dir_x;
	double	rot;

	if (player->rotating == NONE)
		return;
	if (player->rotating == LEFT)
		rot = -ROT_SPEED;
	if (player->rotating == RIGHT)
		rot = ROT_SPEED;
	old_dir_x = player->dir_x;
	player->dir_x = old_dir_x * cos(rot) - player->dir_y * sin(rot);
	player->dir_y = old_dir_x * sin(rot) + player->dir_y * cos(rot);
}

// FUNCTION USED TO INITIALIZE MLX, THE MINIMAP AND THE PLAYER MARKER
void	ft_move_update(void *cub)
{
	t_cub	*data;

	data = (t_cub *)cub;
	if (data->player.rotating != NONE )
		ft_rotate(&data->player);
}

int	ft_game_loop(void *cub)
{
	ft_move_update(cub);
	ft_map_render(cub);
	return (SUCCESS);
}

void	ft_game_init(t_cub *data)
{
	ft_mlx_init(data);
	ft_minimap_init(&data->map);
	ft_char_init(data);
}

// FUNCTION USED TO START THE GAME LOOP AFTER PARSING IS DONE
void	ft_game(t_cub *data)
{
	ft_game_init(data);
	// mlx_key_hook(data->win, ft_keys_handle, data);
	mlx_loop_hook(data->mlx, ft_game_loop, data);
	mlx_hook(data->win, 2, 1L << 0, ft_press_keys, data);
	mlx_hook(data->win, 3, 1L << 1, ft_release_keys, data);
	mlx_loop(data->mlx);
	ft_data_clean(data);
}