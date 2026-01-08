/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   image_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/08 15:54:36 by tibras            #+#    #+#             */
/*   Updated: 2026/01/08 18:19:19 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "imgs.h"
#include "../so_long.h"

int	ft_put_img(t_game *game, t_img *img, int x, int y)
{
	if (!game || !game->mlx || !game->win || !img || !img->img)
		return (1);
	mlx_put_image_to_window(game->mlx, game->win, img->img, x, y);
	return (0);
}

void	ft_xpm_img(t_game *game, char *path, t_img *img)
{
	if (!game || !img)
		return;
	img->img = mlx_xpm_file_to_image(game->mlx, path, &img->width, &img->height);
}

int ft_path_append(char *dst, char *src)
{
	size_t res;

	res = ft_strlcat(dst, src, PATH_SIZE);
	return (res >= PATH_SIZE);
}

char *ft_create_path(int i, int j)
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
		|| ft_path_append(path, IDLE_PATH) || ft_path_append(path, "l_")
		|| ft_path_append(path, frame) || ft_path_append(path, XPM))
		return (NULL);
	return (ft_strdup(path));
}

void	ft_menu_loader(t_game *game, t_menu *menu)
{
	int i;
	int j;
	char *path;

	i = 0;
	while (i < NB_CHARS)
	{
		j = 0;
		while (j < NB_FRAMES_ANIM_CHAR)
		{
			path = ft_create_path(i, j);
			ft_xpm_img(game, path, &menu->characters[i][0][j]);
			free(path);
			j++;
		}
		i++;
	}
}
