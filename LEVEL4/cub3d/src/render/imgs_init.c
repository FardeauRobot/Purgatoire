/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   imgs_init.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 08:49:47 by tibras            #+#    #+#             */
/*   Updated: 2026/03/11 11:24:56 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	ft_tile_init(t_tile *tile)
{
	int x;
	int y;

	y = -1;
	while (++y < TILE_SIZE)
	{
		x = -1;
		while (++x < TILE_SIZE)
		{
			ft_pixel_draw(tile, x + TILE_SIZE, y + TILE_SIZE, tile->color);
		}
	}
}

void	ft_tile_draw(t_tile *tile, int map_x, int map_y)
{
	ft_tile_init(tile);
	printf("Fin print rectangle\n");
	mlx_put_image_to_window(tile->minimap->map->data->mlx,
		tile->minimap->map->data->win,
		tile->tile_img.img,
		map_x,
		map_y);
}

void	ft_minimap_draw(t_minimap *minimap)
{
	int y;
	int x;

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
