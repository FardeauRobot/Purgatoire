/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_map.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 22:44:44 by fardeau           #+#    #+#             */
/*   Updated: 2026/03/11 17:13:48 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	ft_char_draw(t_player *player)
{
	int	screen_x;
	int	screen_y;

	screen_x = player->data->map.minimap.offset_x
		+ (int)(player->pos[1] * TILE_SIZE) - (player->char_img.width / 2);
	screen_y = player->data->map.minimap.offset_y
		+ (int)(player->pos[0] * TILE_SIZE) - (player->char_img.height / 2);
	mlx_put_image_to_window(player->data->mlx,
		player->data->win,
		player->char_img.img,
		screen_x,
		screen_y);
}

// FILE USED TO RENDER THE MINIMAP
int	ft_char_pos_render(void *cub)
{
	t_cub *data;

	data = (t_cub*)cub;
	ft_char_draw(&data->player);
	return (SUCCESS);
}

int	ft_map_render(void *cub)
{
	t_cub *data;

	data = (t_cub *)cub;
	ft_minimap_draw(&data->map.minimap);
	ft_char_draw(&data->player);
	return (SUCCESS);
}