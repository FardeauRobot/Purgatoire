/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   loading_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 22:09:34 by fardeau           #+#    #+#             */
/*   Updated: 2026/03/18 15:37:52 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

// FILE IN CHARGE OF THE INITIALIZATION OF THE GAME

void	ft_mlx_init(t_cub *data)
{
	data->mlx = mlx_init();
	if (!data->mlx)
		ft_exit(data, ERRN_LOAD, ERR_MSG_LOADING, ERR_MSG_MLX);
	if (mlx_get_screen_size(data->mlx, &data->screen_width, &data->screen_height))
		ft_exit(data, ERRN_LOAD, ERR_MSG_LOADING, ERR_MSG_MLX);
	data->screen_height *= DISP_SCALE;
	data->screen_width *= DISP_SCALE;
	data->win = mlx_new_window(data->mlx, data->screen_width, data->screen_height, WIN_NAME);
	if (!data->win)
		ft_exit(data, ERRN_LOAD, ERR_MSG_LOADING, ERR_MSG_MLX);
	data->display.img = mlx_new_image(data->mlx, data->screen_width, data->screen_height);
	if (!data->display.img)
		ft_exit(data, ERRN_LOAD, ERR_MSG_LOADING, ERR_FAIL_MLX);
	data->display.addr = mlx_get_data_addr(data->display.img, &data->display.bpp, &data->display.line_len, &data->display.endian);
	if (!data->display.addr)
		ft_exit(data, ERRN_LOAD, ERR_MSG_LOADING, ERR_FAIL_MLX);
}