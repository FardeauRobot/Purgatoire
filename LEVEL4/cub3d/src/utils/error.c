/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 00:00:00 by author            #+#    #+#             */
/*   Updated: 2026/03/18 13:29:17 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

// FILE IN CHARGE OF ERRORS

/*
** FT_ERROR - PRINTS A FORMATTED ERROR MESSAGE TO STDERR
** FORMAT: "ERROR\n<CONTEXT>: <DETAIL>\n"
*/
int	ft_error(char *context, char *detail, int error)
{
	ft_putstr_fd("Error\n", 2);
	if (context)
	{
		ft_putstr_fd(context, 2);
		ft_putstr_fd(": ", 2);
	}
	if (detail)
		ft_putstr_fd(detail, 2);
	ft_putstr_fd("\n", 2);
	return (error);
}

void	ft_destroy_imgs(t_cub *data)
{
	if (data->mlx)
	{
		if (data->display.img)
			mlx_destroy_image(data->mlx, data->display.img);
		if (data->map.minimap.tiles[0].tile_img.img)
			mlx_destroy_image(data->mlx, data->map.minimap.tiles[0].tile_img.img);
		if (data->map.minimap.tiles[1].tile_img.img)
			mlx_destroy_image(data->mlx, data->map.minimap.tiles[1].tile_img.img);
		if (data->player.char_img.img)
			mlx_destroy_image(data->mlx, data->player.char_img.img);
		if (data->player.test_view.img)
			mlx_destroy_image(data->mlx, data->player.test_view.img);
	}
}

/*
** FT_DATA_CLEAN - FREES ALL RESOURCES (GNL BUFFER, FD, GARBAGE COLLECTORS)
*/
void	ft_data_clean(t_cub *data)
{
	close(data->fd_map);
	get_next_line(-1);
	ft_destroy_imgs(data);
	ft_gc_free_all(&data->gc_global);
	ft_gc_free_all(&data->gc_tmp);
	if (data->win)
		mlx_destroy_window(data->mlx, data->win);
	if (data->mlx)
	{
		mlx_destroy_display(data->mlx);
		free(data->mlx);
	}
}

/*
** FT_EXIT - PRINTS ERROR, FREES RESOURCES, AND EXITS WITH ERROR CODE
*/
void	ft_exit(t_cub *data, int error, char *context, char *detail)
{
	ft_error(context, detail, error);
	ft_data_clean(data);
	exit(error);
}
