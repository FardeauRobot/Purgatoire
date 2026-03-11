/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   game.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 21:54:39 by fardeau           #+#    #+#             */
/*   Updated: 2026/03/11 17:18:52 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

// FUNCTION USED TO HANDLE KEYBOARD INPUT DURING THE GAME LOOP
int	ft_keys_handle(int keycode, void *data)
{
	t_cub *cub;

	cub = (t_cub *)data;
	if (keycode == KEY_ESC)
	{
		if (cub->mlx)
			mlx_loop_end(cub->mlx);
	}
	if (keycode == KEY_W)
		cub->player.pos[0]--;
	if (keycode == KEY_S)
		cub->player.pos[0]++;
	if (keycode == KEY_A)
		cub->player.pos[1]--;
	if (keycode == KEY_D)
		cub->player.pos[1]++;
	return (SUCCESS);
}

// FUNCTION USED TO INITIALIZE MLX, THE MINIMAP AND THE PLAYER MARKER
void	ft_game_init(t_cub *data)
{
	ft_mlx_init(data);
	data->map.minimap.map = &data->map;
	data->map.data = data;
	ft_minimap_init(&data->map);
	ft_char_init(data);
	// ft_map_init(data->map.minimap);
	// ft_minimap_init(data->map.minimap);
	ft_printf("COUCOU INIT\n");
}

// FUNCTION USED TO START THE GAME LOOP AFTER PARSING IS DONE
void	ft_game(t_cub *data)
{
	ft_game_init(data);
	mlx_key_hook(data->win, ft_keys_handle, data);
	mlx_loop_hook(data->mlx, ft_map_render, data);
	mlx_loop(data->mlx);
	ft_data_clean(data);
}