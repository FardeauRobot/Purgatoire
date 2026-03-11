/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   imgs_init.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 08:49:47 by tibras            #+#    #+#             */
/*   Updated: 2026/03/11 17:32:47 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

static void	ft_char_img_fill(t_player *character)
{
	int			x;
	int			y;
	char		*dst;
	t_img		*img;

	img = &character->char_img;
	y = -1;
	while (++y < img->height)
	{
		x = -1;
		while (++x < img->width)
		{
			dst = img->addr + (y * img->line_len + x * (img->bpp / 8));
			*(unsigned int *)dst = character->color;
		}
	}
}

void	ft_tile_init(t_tile *tile)
{
	int x;
	int y;

	y = -1;
	while (++y < TILE_SIZE)
	{
		x = -1;
		while (++x < TILE_SIZE)
			ft_pixel_draw(tile, x, y, tile->tile_img.color);
	}
}

void	ft_tile_draw(t_tile *tile, int map_x, int map_y)
{
	mlx_put_image_to_window(tile->minimap->map->data->mlx,
		tile->minimap->map->data->win,
		tile->tile_img.img,
		tile->minimap->offset_x + (map_x * TILE_SIZE),
		tile->minimap->offset_y + (map_y * TILE_SIZE));
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

// FUNCTION USED TO PRINT THE CHAR POSITION ON THE MINIMAP
void	ft_char_init(t_cub *data)
{
	t_player *character;

	character = &data->player;
	character->data = data;
	character->color = CHAR_COL;
	ft_img_init(data, &character->char_img, CHAR_SIZE, CHAR_SIZE);
	ft_char_img_fill(character);
}

