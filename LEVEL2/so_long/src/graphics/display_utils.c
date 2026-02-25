/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   display_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/14 13:45:30 by tibras            #+#    #+#             */
/*   Updated: 2026/02/14 17:37:51 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "so_long.h"
#include "so_long/imgs.h"

int	ft_put_img(t_game *game, t_img *img, int x, int y)
{
	if (!game || !game->mlx || !game->win || !img || !img->img)
		error_exit(game, "Error put_img", 25);
	mlx_put_image_to_window(game->mlx, game->win, img->img, x * IMG_SIZE,
		y * IMG_SIZE);
	return (0);
}

int	ft_xpm_img(t_game *game, char *path, t_img *img)
{
	if (!game || !img)
		return (FAILURE);
	img->img = mlx_xpm_file_to_image(game->mlx, path, &img->width,
			&img->height);
	if (!img->img)
		return (FAILURE);
	return (SUCCESS);
}

void	ft_destroy_img(t_game *game, t_img *img)
{
	if (img->img)
		mlx_destroy_image(game->mlx, img->img);
	img->img = NULL;
}

void	ft_clear_assets(t_game *game)
{
	int	i;

	i = 0;
	while (i < NB_ASSETS)
	{
		ft_destroy_img(game, &game->assets[i]);
		i++;
	}
	i = 0;
	while (i < NB_FRAMES_ANIM_ASSETS)
	{
		ft_destroy_img(game, &game->walls[i]);
		ft_destroy_img(game, &game->collectible[i]);
		ft_destroy_img(game, &game->exit_f[i]);
		i++;
	}
}

void	ft_clear_imgs(t_game *game)
{
	int	i;
	int	j;
	int	k;

	ft_clear_assets(game);
	i = 0;
	while (i < NB_CHARS)
	{
		j = 0;
		while (j < NB_MOVES)
		{
			k = 0;
			while (k < NB_FRAMES_ANIM_CHAR)
			{
				ft_destroy_img(game, &game->characters[i][j][O_LEFT][k]);
				ft_destroy_img(game, &game->characters[i][j][O_RIGHT][k]);
				k++;
			}
			j++;
		}
		i++;
	}
}
