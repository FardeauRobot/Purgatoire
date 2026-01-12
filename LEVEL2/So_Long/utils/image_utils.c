/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   image_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/08 15:54:36 by tibras            #+#    #+#             */
/*   Updated: 2026/01/12 18:07:33 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../so_long.h"
#include "imgs.h"

int	ft_put_img(t_game *game, t_img *img, int x, int y)
{
	if (!game || !game->mlx || !game->win || !img || !img->img)
		return (1);
	mlx_put_image_to_window(game->mlx, game->win, img->img, x, y);
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

int	ft_path_append(char *dst, char *src)
{
	size_t	res;

	res = ft_strlcat(dst, src, PATH_SIZE);
	return (res >= PATH_SIZE);
}

char	*ft_create_path(int i, int j, char *l_or_r)
{
	char	*character;
	char	path[PATH_SIZE];
	char	frame[2];

	ft_bzero(path, PATH_SIZE);
	if (i == 0)
		character = PINK_PATH;
	else if (i == 1)
		character = HOWLET_PATH;
	else if (i == 2)
		character = BLUE_PATH;
	else
		return (NULL);
	frame[0] = '0' + j;
	frame[1] = '\0';
	if (ft_path_append(path, IMG_PATH) || ft_path_append(path, character)
		|| ft_path_append(path, IDLE_PATH) || ft_path_append(path, l_or_r)
		|| ft_path_append(path, frame) || ft_path_append(path, XPM))
		return (NULL);
	ft_printf("PATH = %s\n", path);
	return (ft_strdup(path));
}

void	ft_clear_imgs(t_game *game)
{
	int	i;
	int	j;
	int	k;

	i = 0;
	while (i < NB_CHARS)
	{
		j = 0;
		while (j < NB_MOVES)
		{
			k = 0;
			while (k < NB_FRAMES_ANIM_CHAR)
			{
				ft_destroy_img(game, &game->characters[i][j][LEFT][k]);
				ft_destroy_img(game, &game->characters[i][j][RIGHT][k]);
				k++;
			}
			j++;
		}
		i++;
	}
}

void	ft_game_loader(t_game *game)
{
	int		i;
	int		j;
	int		k;
	char	*path_l;
	char	*path_r;

	i = 0;
	while (i < NB_CHARS)
	{
		j = 0;
		while (j < NB_MOVES)
		{
			k = 0;
			while (k < NB_FRAMES_ANIM_CHAR)
			{
				path_l = ft_create_path(i, j, "l_");
				path_r = ft_create_path(i, j, "r_");
				if (!path_l || !path_r)
					error_exit(game, "Error allocating imgs\n", ERRN_MALLOC);
				if (ft_xpm_img(game, path_l, &game->characters[i][j][LEFT][k]) == FAILURE
					|| ft_xpm_img(game, path_r, &game->characters[i][j][RIGHT][k]) == FAILURE)
				{
					if (path_l)
						free(path_l);
					if (path_r)
						free(path_r);
					error_exit(game, "Error loading imgs\n", ERRN_OPEN);
				}
				free(path_l);
				free(path_r);
				k++;
			}
			j++;
		}
		i++;
	}
}
