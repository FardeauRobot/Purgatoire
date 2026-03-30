/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   image_utils_2.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/14 13:53:34 by tibras            #+#    #+#             */
/*   Updated: 2026/02/14 17:46:16 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "so_long.h"
#include "so_long/imgs.h"

char	*ft_create_path_char(int i, int j, char *l_or_r, int k)
{
	char	*chars[3];
	char	*moves[4];
	char	path[PATH_SIZE];
	char	frame[2];

	chars[0] = PINK_PATH;
	chars[1] = HOWLET_PATH;
	chars[2] = BLUE_PATH;
	moves[0] = IDLE_PATH;
	moves[1] = MOVING_PATH;
	moves[2] = JUMP_PATH;
	moves[3] = ATTACK_PATH;
	ft_bzero(path, PATH_SIZE);
	frame[0] = '0' + k;
	frame[1] = '\0';
	if (ft_path_append(path, IMG_PATH) || ft_path_append(path, chars[i])
		|| ft_path_append(path, moves[j]) || ft_path_append(path, l_or_r)
		|| ft_path_append(path, frame) || ft_path_append(path, XPM))
		return (NULL);
	return (ft_strdup(path));
}

char	*ft_create_path_assets(char *asset, char *frame)
{
	char	path[PATH_SIZE];

	ft_bzero(path, PATH_SIZE);
	if (ft_path_append(path, asset) || ft_path_append(path, frame)
		|| ft_path_append(path, XPM))
		return (NULL);
	return (ft_strdup(path));
}

void	ft_map_loader(t_game *game)
{
	int		i;
	char	*path;
	char	frame[2];

	i = 0;
	frame[1] = '\0';
	while (i < NB_FRAMES_ANIM_ASSETS)
	{
		frame[0] = i + '0';
		path = ft_create_path_assets(WALL_XPM, frame);
		if (!ft_xpm_img(game, path, &game->walls[i]))
			ft_img_load_error(game, &path, ERRN_LOAD_ASSETS, ERRS_LOAD_WALL);
		free(path);
		path = ft_create_path_assets(COLLECTIBLE_XPM, frame);
		if (!ft_xpm_img(game, path, &game->collectible[i]))
			ft_img_load_error(game, &path, ERRN_LOAD_ASSETS, ERRS_LOAD_COLL);
		free(path);
		path = ft_create_path_assets(EXIT_XPM, frame);
		if (!ft_xpm_img(game, path, &game->exit_f[i]))
			ft_img_load_error(game, &path, ERRN_LOAD_ASSETS, ERRS_LOAD_EXIT);
		free(path);
		i++;
	}
	if (!ft_xpm_img(game, GROUND_XPM, &game->assets[GROUND]))
		error_exit(game, "Error loading ground\n", ERRN_LOAD_ASSETS);
}

static void	ft_load_one_char_frame(t_game *game, int i, int j, int k)
{
	char	*path_l;
	char	*path_r;

	path_l = ft_create_path_char(i, j, "l_", k);
	path_r = ft_create_path_char(i, j, "r_", k);
	if (!path_l || !path_r)
		error_exit(game, "Error allocating imgs\n", ERRN_MALLOC);
	if (!ft_xpm_img(game, path_l, &game->characters[i][j][O_LEFT][k])
		|| !ft_xpm_img(game, path_r, &game->characters[i][j][O_RIGHT][k]))
	{
		ft_free_paths(path_l, path_r);
		error_exit(game, "Error loading imgs\n", ERRN_OPEN);
	}
	ft_free_paths(path_l, path_r);
}

void	ft_game_loader(t_game *game)
{
	int		i;
	int		j;
	int		k;

	ft_map_loader(game);
	i = 0;
	while (i++ < NB_CHARS)
	{
		j = 0;
		while (j++ < NB_MOVES)
		{
			k = 0;
			while (k++ < NB_FRAMES_ANIM_CHAR)
				ft_load_one_char_frame(game, i - 1, j - 1, k - 1);
		}
	}
}
