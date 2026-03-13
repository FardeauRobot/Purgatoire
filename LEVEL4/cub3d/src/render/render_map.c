/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_map.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 22:44:44 by fardeau           #+#    #+#             */
/*   Updated: 2026/03/13 12:32:38 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

// FUNCTION USED TO DRAW ONE MINIMAP TILE AT THE GIVEN MAP POSITION
void	ft_tile_draw(t_tile *tile, int map_x, int map_y)
{
	mlx_put_image_to_window(tile->minimap->map->data->mlx,
		tile->minimap->map->data->win,
		tile->tile_img.img,
		tile->minimap->offset_x + (map_x * TILE_SIZE),
		tile->minimap->offset_y + (map_y * TILE_SIZE));
}

// FUNCTION USED TO DRAW THE WHOLE MINIMAP FROM THE MAP GRID
void	ft_minimap_draw(t_minimap *minimap)
{
	
	int	y;
	int	x;

	y = -1;
	while (++y < minimap->map->height)
	{
		x = -1;
		while (++x < minimap->map->width)
		{
			if (ft_ischarset(minimap->map->map[y][x], "0NSEW"))
				ft_tile_draw(&minimap->tiles[EMPTY], x, y);
			else if (minimap->map->map[y][x] == '1')
				ft_tile_draw(&minimap->tiles[WALL], x, y);
		}
	}
}

// FUNCTION USED TO DRAW THE PLAYER MARKER AT THE RIGHT MINIMAP POSITION
void	ft_char_draw(t_player *player)
{
	int	screen_x;
	int	screen_y;

	screen_x = player->data->map.minimap.offset_x
		+ (int)(player->pos_x * TILE_SIZE) - (player->char_img.width / 2);
	screen_y = player->data->map.minimap.offset_y
		+ (int)(player->pos_y * TILE_SIZE) - (player->char_img.height / 2);
	mlx_put_image_to_window(player->data->mlx,
		player->data->win,
		player->char_img.img,
		screen_x,
		screen_y);
}

void	ft_orient_draw(t_player *player)
{
	int	screen_x;
	int	screen_y;

	screen_x = player->data->map.minimap.offset_x
		+ (int)((player->pos_x + player->dir_x) * TILE_SIZE)
		- (player->test_view.width / 2);
	screen_y = player->data->map.minimap.offset_y
		+ (int)((player->pos_y + player->dir_y) * TILE_SIZE)
		- (player->test_view.height / 2);
	mlx_put_image_to_window(player->data->mlx,
		player->data->win,
		player->test_view.img,
		screen_x,
		screen_y);
}

// FUNCTION USED TO RENDER THE MINIMAP AND THE PLAYER MARKER EACH FRAME
int	ft_map_render(void *cub)
{
	t_cub	*data;

	data = (t_cub *)cub;
	ft_minimap_draw(&data->map.minimap);
	ft_char_draw(&data->player);
	ft_orient_draw(&data->player);
	return (SUCCESS);
}