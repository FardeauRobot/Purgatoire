/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 00:00:00 by author            #+#    #+#             */
/*   Updated: 2026/03/11 17:26:26 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

// THIS IS THE MAIN


void ft_tiles_init(t_tile *tile, t_minimap *minimap, t_etile content)
{
	tile->minimap = minimap;
	ft_img_init(minimap->map->data, &tile->tile_img, TILE_SIZE, TILE_SIZE);
	if (content == EMPTY)
		tile->tile_img.color = EMPTY_TILE_COL;
	if (content == WALL)
		tile->tile_img.color = WALL_TILE_COL;
	ft_tile_init(tile);
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
