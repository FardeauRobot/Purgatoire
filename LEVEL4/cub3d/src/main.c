/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 00:00:00 by author            #+#    #+#             */
/*   Updated: 2026/03/15 16:37:16 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

// THIS IS THE MAIN

// INITALISATION DES STRUCTURES
void	ft_p_structs_init(t_cub *data)
{
	data->p_structs.p_cub = data;
	data->p_structs.p_map = &data->map;
	data->p_structs.p_minimap = &data->map.minimap;
	data->p_structs.p_player = &data->player;
	data->p_structs.p_textures = &data->textures;

	data->map.p_structs = &data->p_structs;
	data->map.minimap.p_structs = &data->p_structs;
	data->player.p_structs = &data->p_structs;
}

// FUNCTION USED TO INITIALIZE THE MAIN STRUCTURE AND START THE PROGRAM
int	main(int argc, char **argv)
{
	t_cub data;

	ft_bzero(&data, sizeof(t_cub));
	ft_p_structs_init(&data);
	ft_parsing(&data, argv, argc);
	ft_cub_print(&data);
	ft_game(&data);
	return (EXIT_SUCCESS);
}
