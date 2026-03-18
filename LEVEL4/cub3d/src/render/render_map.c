/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_map.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 22:44:44 by fardeau           #+#    #+#             */
/*   Updated: 2026/03/18 15:54:43 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

// FUNCTION USED TO DRAW ONE MINIMAP TILE AT THE GIVEN MAP POSITION
void	ft_tile_draw(t_minimap *minimap, t_tile *tile, int map_x, int map_y)
{
	mlx_put_image_to_window(minimap->p_structs->p_cub->mlx,
		minimap->p_structs->p_cub->win,
		tile->tile_img.img,
		minimap->offset_x + (map_x * TILE_SIZE),
		minimap->offset_y + (map_y * TILE_SIZE));
}

// FUNCTION USED TO DRAW THE WHOLE MINIMAP FROM THE MAP GRID
void	ft_minimap_draw(t_minimap *minimap)
{
	int	y;
	int	x;
	int	len;

	y = -1;
	while (++y < minimap->p_structs->p_map->height)
	{
		x = -1;
		len = ft_strlen(minimap->p_structs->p_map->map[y]);
		while (++x < len)
		{
			if (ft_ischarset(minimap->p_structs->p_map->map[y][x], "0NSEW"))
				ft_tile_draw(minimap, &minimap->tiles[EMPTY], x, y);
			else if (minimap->p_structs->p_map->map[y][x] == '1')
				ft_tile_draw(minimap, &minimap->tiles[WALL], x, y);
		}
	}
}

// FUNCTION USED TO DRAW THE PLAYER MARKER AT THE RIGHT MINIMAP POSITION
void	ft_char_draw(t_player *player)
{
	int	screen_x;
	int	screen_y;

	screen_x = player->p_structs->p_minimap->offset_x
		+ (int)(player->pos_x * TILE_SIZE)
		- (player->char_img.width / 2);
	screen_y = player->p_structs->p_minimap->offset_y
		+ (int)(player->pos_y * TILE_SIZE)
		- (player->char_img.width / 2);
	mlx_put_image_to_window(player->p_structs->p_cub->mlx,
		player->p_structs->p_cub->win,
		player->char_img.img,
		screen_x,
		screen_y);
}

void	ft_orient_draw(t_player *player)
{
	int	screen_x;
	int	screen_y;

	screen_x = player->p_structs->p_minimap->offset_x
		+ (int)((player->pos_x + player->dir_x) * TILE_SIZE)
		- (player->test_view.width / 2);
	screen_y = player->p_structs->p_minimap->offset_y
		+ (int)((player->pos_y + player->dir_y) * TILE_SIZE)
		- (player->test_view.height / 2);
	mlx_put_image_to_window(player->p_structs->p_cub->mlx,
		player->p_structs->p_cub->win,
		player->test_view.img,
		screen_x,
		screen_y);
}

void	ft_background_draw(t_cub *data)
{
	ft_img_fill(data->display.img, EMPTY_TILE_COL);
}

void	ft_img_draw(t_cub *data)
{
	if (data->map.minimap.display_map == OFF)
	{
		mlx_clear_window(data->mlx, data->win);
		return ;
	}
	ft_background_draw(data);
	mlx_put_image_to_window(data->mlx, data->win, data->display.img, data->screen_width, data->screen_height);
	ft_minimap_draw(&data->map.minimap);
	ft_char_draw(&data->player);
	ft_orient_draw(&data->player);
}

// FUNCTION USED TO RENDER THE MINIMAP AND THE PLAYER MARKER EACH FRAME
int	ft_map_render(void *cub)
{
	t_cub	*data;

	data = (t_cub *)cub;
	ft_img_draw(data);
	return (SUCCESS);
}