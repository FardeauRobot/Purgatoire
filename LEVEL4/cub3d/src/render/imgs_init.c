/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   imgs_init.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 08:49:47 by tibras            #+#    #+#             */
/*   Updated: 2026/03/18 15:36:51 by tibras           ###   ########.fr       */
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

// FUNCTION USED TO INITIALIZE AN MLX IMAGE AND ITS RAW PIXEL BUFFER
void	ft_img_init(t_cub *data, t_img *img, int width, int height)
{
	img->height = height;
	img->width = width;
	img->img = mlx_new_image(data->mlx, img->width, img->height);
	if (!img->img)
		ft_exit(data, ERRN_LOAD, ERR_MSG_LOADING, ERR_FAIL_MLX);
	img->addr = mlx_get_data_addr(img->img, &img->bpp,
			&img->line_len, &img->endian);
	if (!img->addr)
		ft_exit(data, ERRN_LOAD, ERR_MSG_LOADING, ERR_FAIL_MLX);
}

// FUNCTION USED TO INITIALIZE ONE TILE IMAGE WITH THE RIGHT COLOR
void ft_tiles_init(t_minimap *minimap, t_tile *tile, t_etile content)
{
	ft_img_init(minimap->p_structs->p_cub, &tile->tile_img, TILE_SIZE, TILE_SIZE);
	tile->tile_img.color = ft_tile_color_get(content);
	ft_img_fill(&tile->tile_img, tile->tile_img.color);
}

// FUNCTION USED TO INITIALIZE ALL IMAGES NEEDED BY THE MINIMAP
void	ft_minimap_init(t_map *map)
{
	map->minimap.display_map = OFF;
	map->minimap.offset_x = map->p_structs->p_cub->screen_width - map->width * TILE_SIZE;
	map->minimap.offset_y = map->p_structs->p_cub->screen_height / 4 - map->height * TILE_SIZE;
	ft_tiles_init(&map->minimap, &map->minimap.tiles[EMPTY], EMPTY);
	ft_tiles_init(&map->minimap, &map->minimap.tiles[WALL], WALL);
}

// FUNCTION USED TO INITIALIZE THE PLAYER MARKER IMAGE FOR THE MINIMAP
void	ft_char_init(t_cub *data)
{
	t_player	*character;

	character = &data->player;
	character->moving = NONE;
	character->rotating = NONE;
	character->char_img.color = CHAR_COL;
	ft_img_init(data, &character->char_img, CHAR_SIZE, CHAR_SIZE);
	ft_img_init(data, &character->test_view, CHAR_SIZE / 2, CHAR_SIZE / 2);
	ft_img_fill(&character->char_img, character->char_img.color);
	ft_img_fill(&character->test_view, character->char_img.color);
}

