/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 08:37:28 by tibras            #+#    #+#             */
/*   Updated: 2026/03/11 17:05:39 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	ft_img_init(t_cub *data, t_img *img, int width, int height)
{
	img->height = height;
	img->width = width;
	img->img = mlx_new_image(data->mlx, img->width, img->height);
	if (!img->img)
		ft_exit(data, ERRN_LOAD, ERR_MSG_LOADING, ERR_FAIL_MLX);
	img->addr = mlx_get_data_addr(img->img, &img->bpp,
			&img->line_len, &img->endian);
	if (!img->addr)
		ft_exit(data, ERRN_LOAD, ERR_MSG_LOADING, ERR_FAIL_MLX);
}

void	ft_pixel_draw(t_tile *tile, int x, int y, int color)
{
	t_img	*img;
	char	*dst;

	img = &tile->tile_img;
	if (!img || !img->addr)
		return;
	if (x < 0 || y < 0)
		return;
	if (x >= img->width || y >= img->height)
		return;
	dst = img->addr + (y * img->line_len + x * (img->bpp / 8));
	*(unsigned int *)dst = color;
}