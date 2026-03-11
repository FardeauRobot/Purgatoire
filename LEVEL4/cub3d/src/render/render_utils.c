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

// FUNCTION USED TO WRITE ONE PIXEL AT THE GIVEN POSITION INSIDE AN IMAGE
void	ft_img_pixel_put(t_img *img, int x, int y, int color)
{
	char	*dst;

	if (!img || !img->addr)
		return;
	if (x < 0 || y < 0)
		return;
	if (x >= img->width || y >= img->height)
		return;
	dst = img->addr + (y * img->line_len + x * (img->bpp / 8));
	*(unsigned int *)dst = color;
}

// FUNCTION USED TO FILL A WHOLE IMAGE WITH A SINGLE COLOR
void	ft_img_fill(t_img *img, int color)
{
	int	x;
	int	y;

	y = -1;
	while (++y < img->height)
	{
		x = -1;
		while (++x < img->width)
			ft_img_pixel_put(img, x, y, color);
	}
}