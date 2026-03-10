/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   game.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 21:54:39 by fardeau           #+#    #+#             */
/*   Updated: 2026/03/10 22:48:46 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

int	ft_keys_handle(int keycode, t_cub *data)
{
	if (keycode == KEY_ESC)
	{
		if (data->mlx)
			mlx_loop_end(data->mlx);
	}
	return (SUCCESS);
}

int	ft_print_map(t_cub *data)
{
	(void)data;
	return (SUCCESS);
}

/*
** HANDLES ALL THE GAME LOGIC AFTER PARSING
*/
void	ft_game(t_cub *data)
{
	ft_mlx_init(data);
	mlx_key_hook(data->win, ft_keys_handle, data);
	mlx_loop_hook(data->mlx, ft_print_map, data);
	mlx_loop(data->mlx);
	ft_data_clean(data);
}