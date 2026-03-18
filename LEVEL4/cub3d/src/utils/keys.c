/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   keys.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/15 18:47:32 by fardeau           #+#    #+#             */
/*   Updated: 2026/03/18 15:38:48 by tibras           ###   ########.fr       */
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

int	ft_move(t_player *player)
{
	t_map *map;
	double	move_x;
	double	move_y;

	move_x = 0;
	move_y = 0;
	map = player->p_structs->p_map;
	if (player->moving & UP)
	{
		move_y += player->dir_y;
		move_x += player->dir_x;
	}
	if (player->moving & DOWN)
	{
		move_y -= player->dir_y;
		move_x -= player->dir_x;
	}
	if (player->moving & LEFT)
	{
		move_y -= player->dir_x;
		move_x += player->dir_y;
	}
	if (player->moving & RIGHT)
	{
		move_y += player->dir_x;
		move_x -= player->dir_y;
	}
	if (map->map[(int)(player->pos_y + move_y * CHAR_SPEED)]
			[(int)(player->pos_x + move_x * CHAR_SPEED)] == '1')
		return (ERRN_WALL);
	player->pos_x += move_x * CHAR_SPEED;
	player->pos_y += move_y * CHAR_SPEED;
	return (SUCCESS);
}

int	ft_press_keys(int keycode, void *cub)
{
	t_cub *data;

	data = (t_cub *)cub;
	if (keycode == KEY_LEFT)
		data->player.rotating = LEFT;
	if (keycode == KEY_RIGHT)
		data->player.rotating = RIGHT;
	if (keycode == KEY_W)
		data->player.moving |= UP;
	if (keycode == KEY_S)
		data->player.moving |= DOWN;
	if (keycode == KEY_A)
		data->player.moving |= LEFT;
	if (keycode == KEY_D)
		data->player.moving |= RIGHT;
	return (SUCCESS);
}

int	ft_release_keys(int keycode, void *cub)
{
	t_cub *data;

	data = (t_cub *)cub;
	if (keycode == KEY_ESC)
	{
		if (data->mlx)
			mlx_loop_end(data->mlx);
	}
	if (keycode == KEY_W)
		data->player.moving &= ~UP;
	if (keycode == KEY_S)
		data->player.moving &= ~DOWN;
	if (keycode == KEY_A)
		data->player.moving &= ~LEFT;
	if (keycode == KEY_D)
		data->player.moving &= ~RIGHT;
	if (keycode == KEY_RIGHT || keycode == KEY_LEFT)
		data->player.rotating = NONE;
	if (keycode == KEY_TAB)
	{
		if (data->map.minimap.display_map == ON)
			data->map.minimap.display_map = OFF;
		else if (data->map.minimap.display_map == OFF)
			data->map.minimap.display_map = ON;
		ft_map_render(cub);

	}
	return (SUCCESS);
}