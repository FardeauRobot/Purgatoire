/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   game.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 21:54:39 by fardeau           #+#    #+#             */
/*   Updated: 2026/03/11 12:07:09 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

int	ft_keys_handle(int keycode, void *data)
{
	t_cub *cub;

	cub = (t_cub *)data;
	if (keycode == KEY_ESC)
	{
		if (cub->mlx)
			mlx_loop_end(cub->mlx);
	}
	return (SUCCESS);
}

void	ft_game_init(t_cub *data)
{
	ft_mlx_init(data);
	data->map.minimap.map = &data->map;
	data->map.data = data;
	ft_minimap_init(&data->map);
	// ft_map_init(data->map.minimap);
	// ft_minimap_init(data->map.minimap);
	ft_printf("COUCOU INIT\n");
}

/*
** HANDLES ALL THE GAME LOGIC AFTER PARSING
*/
void	ft_game(t_cub *data)
{
	ft_game_init(data);
	mlx_key_hook(data->win, ft_keys_handle, data);
	mlx_loop_hook(data->mlx, ft_map_render, data);
	mlx_loop(data->mlx);
	ft_data_clean(data);
}