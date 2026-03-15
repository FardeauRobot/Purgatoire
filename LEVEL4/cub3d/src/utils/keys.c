/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   keys.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/15 18:47:32 by fardeau           #+#    #+#             */
/*   Updated: 2026/03/15 19:16:52 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

int	ft_press_keys(int keycode, void *cub)
{
	t_cub *data;

	data = (t_cub *)cub;
	if (keycode == KEY_LEFT)
		data->player.rotating = LEFT;
	if (keycode == KEY_RIGHT)
		data->player.rotating = RIGHT;
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
		data->player.pos_y -= CHAR_SPEED;
	if (keycode == KEY_S)
		data->player.pos_y += CHAR_SPEED;
	if (keycode == KEY_A)
		data->player.pos_x -= CHAR_SPEED;
	if (keycode == KEY_D)
		data->player.pos_x += CHAR_SPEED;
	if (keycode == KEY_RIGHT || keycode == KEY_LEFT)
		data->player.rotating = NONE;
	return (SUCCESS);
}