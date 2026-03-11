/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 08:37:28 by tibras            #+#    #+#             */
/*   Updated: 2026/03/11 11:25:27 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	ft_pixel_draw(t_tile *tile, int x, int y, int color)
{
	t_img	*img;
	void	*win;

	img = &tile->tile_img;
	win = (void *)tile->minimap->map->data->win;
	if (!img || !img->addr)
		return;
	if (x < 0 || y < 0)
		return;
	if (x >= img->width || y >= img->height)
		return;
	mlx_pixel_put(img, win, x, y, color);
}