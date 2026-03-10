/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   loading_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 22:09:34 by fardeau           #+#    #+#             */
/*   Updated: 2026/03/10 22:47:14 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

// FILE IN CHARGE OF THE INITIALIZATION OF THE GAME

void	ft_mlx_init(t_cub *data)
{
	data->mlx = mlx_init();
	if (!data->mlx)
		ft_exit(data, ERRN_LOAD, ERR_MSG_LOADING, ERR_MSG_MLX);
	data->win = mlx_new_window(data->mlx, WIN_WIDTH, WIN_HEIGHT, WIN_NAME);
	if (!data->win)
		ft_exit(data, ERRN_LOAD, ERR_MSG_LOADING, ERR_MSG_MLX);
}