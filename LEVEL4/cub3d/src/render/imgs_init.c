/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   imgs_init.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 08:49:47 by tibras            #+#    #+#             */
/*   Updated: 2026/03/11 17:49:27 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

// FUNCTION USED TO RETURN THE DISPLAY COLOR ASSOCIATED WITH A TILE TYPE
static int	ft_tile_color_get(t_etile content)
{
	if (content == WALL)
		return (WALL_TILE_COL);
	return (EMPTY_TILE_COL);
}

// FUNCTION USED TO DRAW ONE MINIMAP TILE AT THE GIVEN MAP POSITION
void	ft_tile_draw(t_tile *tile, int map_x, int map_y)
{
	mlx_put_image_to_window(tile->minimap->map->data->mlx,
		tile->minimap->map->data->win,
		tile->tile_img.img,
		tile->minimap->offset_x + (map_x * TILE_SIZE),
		tile->minimap->offset_y + (map_y * TILE_SIZE));
}

// FUNCTION USED TO INITIALIZE ONE TILE IMAGE WITH THE RIGHT COLOR
void ft_tiles_init(t_tile *tile, t_minimap *minimap, t_etile content)
{
	tile->minimap = minimap;
	ft_img_init(minimap->map->data, &tile->tile_img, TILE_SIZE, TILE_SIZE);
	tile->tile_img.color = ft_tile_color_get(content);
	ft_img_fill(&tile->tile_img, tile->tile_img.color);
}

// FUNCTION USED TO INITIALIZE ALL IMAGES NEEDED BY THE MINIMAP
void	ft_minimap_init(t_map *map)
{
	map->minimap.map = map;
	ft_tiles_init(&map->minimap.tiles[EMPTY], &map->minimap, EMPTY);
	ft_tiles_init(&map->minimap.tiles[WALL], &map->minimap, WALL);
}

// FUNCTION USED TO INITIALIZE THE PLAYER MARKER IMAGE FOR THE MINIMAP
void	ft_char_init(t_cub *data)
{
	t_player	*character;

	character = &data->player;
	character->data = data;
	ft_img_init(data, &character->char_img, CHAR_SIZE, CHAR_SIZE);
	character->char_img.color = CHAR_COL;
	ft_img_fill(&character->char_img, character->char_img.color);
}

