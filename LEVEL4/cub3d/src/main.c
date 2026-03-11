/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 00:00:00 by author            #+#    #+#             */
/*   Updated: 2026/03/11 12:07:38 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

// THIS IS THE MAIN


void ft_tiles_init(t_tile *tile, t_minimap *minimap, t_etile content)
{
	tile->minimap = minimap;
	tile->tile_img.height = TILE_SIZE;
	tile->tile_img.width = TILE_SIZE;
	printf("COUCOU\n");
	tile->tile_img.img = mlx_new_image(minimap->map->data->mlx,
			tile->tile_img.width, tile->tile_img.height);
	if (!tile->tile_img.img)
		ft_exit(minimap->map->data, ERRN_LOAD, ERR_MSG_LOADING, ERR_FAIL_MLX);
	tile->tile_img.addr = 
		mlx_get_data_addr(tile->tile_img.img,
			&tile->tile_img.bpp,
			&tile->tile_img.line_len,
			&tile->tile_img.endian);
	if (!tile->tile_img.addr)
		ft_exit(minimap->map->data, ERRN_LOAD, ERR_MSG_LOADING, ERR_FAIL_MLX);
	if (content == EMPTY)
		tile->color = EMPTY_TILE_COL;
	if (content == WALL)
		tile->color = WALL_TILE_COL;
}
void	ft_minimap_init(t_map *map)
{
	map->minimap.map = map;
	ft_tiles_init(&map->minimap.tiles[EMPTY], &map->minimap, EMPTY);
	ft_tiles_init(&map->minimap.tiles[WALL], &map->minimap, WALL);
}


/*
** MAIN - ENTRY POINT, INITS DATA, PARSES, PRINTS AND CLEANS UP
*/
int	main(int argc, char **argv)
{
	t_cub data;

	ft_bzero(&data, sizeof(t_cub));
	// ft_cub_init(&data);
	ft_parsing(&data, argv, argc);
	ft_cub_print(&data);
	ft_game(&data);
	return (EXIT_SUCCESS);
}
