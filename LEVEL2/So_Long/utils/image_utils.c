/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   image_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/08 15:54:36 by tibras            #+#    #+#             */
/*   Updated: 2026/01/14 17:57:52 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../so_long.h"
#include "imgs.h"

static long ft_now_ms(void)
{
	struct timeval tv;

	gettimeofday(&tv, NULL);
	// printf("%lx\n", tv.tv_sec);
	return (tv.tv_sec * 1000L + tv.tv_usec / 1000L);
}

void	ft_put_assets(t_game *game, long time)
{
	size_t x;
	size_t y;

	y = 0;
	if (time - game->last_frame_assets_ms >= FRAME_MS_ASSETS)
	{
		game->frame_assets = (game->frame_assets + 1) % NB_FRAMES_ANIM_ASSETS;
		game->last_frame_assets_ms = time;
	}
	while (y < game->map_height)
	{
		x = 0;
		while (x < game-> map_width)
		{
			if (game->map[y][x] == '1')
				ft_put_img(game,&game->walls[game->frame_assets], x ,y);
			else if (game->map[y][x] == 'C') 
				ft_put_img(game,&game->collectible[game->frame_assets], x ,y);
			x++;
		}
		y++;
	}
}

int	ft_dynamic_render(t_game *game)
{
	long time;

	time = ft_now_ms();
	if (game->move != IDLE)
	{
		if (game->last_move_ms == 0)
			game->last_move_ms = time;
		if (time == game->last_move_ms + FRAME_MS_CHAR * NB_FRAMES_ANIM_CHAR)
		{
			game->move = IDLE;
			game->last_move_ms = 0;
		}
	}
	if (time - game->last_frame_ms >= FRAME_MS_CHAR)
	{
		game->frame = (game->frame + 1) % NB_FRAMES_ANIM_CHAR;
		game->last_frame_ms = time;
	}
	ft_put_img(game, &game->characters[game->active_char][game->move][game->orient][game->frame], game->player_pos[X], game->player_pos[Y]);
	ft_put_assets(game, time);
	return (0);
}

int	ft_put_img(t_game *game, t_img *img, int x, int y)
{
	if (!game || !game->mlx || !game->win || !img || !img->img)
		error_exit(game, "Error put_img", 25);
	mlx_put_image_to_window(game->mlx, game->win, img->img, x * IMG_SIZE, y * IMG_SIZE);
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

char	*ft_create_path_char(int i, int j,  char *l_or_r, int k)
{
	char	*character;
	char	*move;
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
	if (j == 0)
		move = IDLE_PATH;
	if (j == 1)
		move = MOVING_PATH;
	frame[0] = '0' + k;
	frame[1] = '\0';
	if (ft_path_append(path, IMG_PATH) || ft_path_append(path, character)
		|| ft_path_append(path, move) || ft_path_append(path, l_or_r)
		|| ft_path_append(path, frame) || ft_path_append(path, XPM))
		return (NULL);
	ft_printf("PATH = %s\n", path);
	return (ft_strdup(path));
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
// char	*ft_create_path_walls(void)
// {
// 	char	*path;
// 	char	path[PATH_SIZE];
// 	char	frame[2];

// 	ft_bzero(path, PATH_SIZE);
// 	else
// 		return (NULL);
// 	if (j == 0)
// 		move = IDLE_PATH;
// 	if (j == 1)
// 		move = MOVING_PATH;

// 	frame[0] = '0' + k;
// 	frame[1] = '\0';
// 	if (ft_path_append(path, IMG_PATH) || ft_path_append(path, character)
// 		|| ft_path_append(path, move) || ft_path_append(path, l_or_r)
// 		|| ft_path_append(path, frame) || ft_path_append(path, XPM))
// 		return (NULL);
// 	ft_printf("PATH = %s\n", path);
// 	return (ft_strdup(path));
// }

void	ft_render_static(t_game *game)
{
	size_t i;
	size_t j;

	i = 0;
	while (i < game->map_height)
	{
		j = 0;
		while (j < game->map_width)
		{
			if (game->map[i][j] == '0')
				ft_put_img(game, &game->assets[GROUND], j, i);
			j++;
		}
		ft_printf("%s\n", game->map[i]);
		i++;
	}
	ft_printf("GAME HEIGHT = %d || GAME WIDTH = %d\n", game->map_height, game->map_height);
}


char	*ft_create_path_assets(char *asset, char *frame)
{
	char path[PATH_SIZE];

	ft_bzero(path, PATH_SIZE);
	if (ft_path_append(path, asset) || ft_path_append(path, frame)
		|| ft_path_append(path, XPM))
		return (NULL);
	ft_printf("PATH = %s\n", path);
	return (ft_strdup(path));
}
void	ft_map_loader(t_game *game)
{
	int i;
	char *path;
	char frame[2];

	i = 0;
	frame[1] = '\0';
	while (i < NB_FRAMES_ANIM_ASSETS)
	{
		frame[0] = i + '0';
		path = ft_create_path_assets(WALL_XPM, frame);
		if (!path)
			error_exit(game, ERRS_MALLOC, ERRN_MALLOC);
		if (!ft_xpm_img(game, path, &game->walls[i]))
			error_exit(game, "Error loading walls\n", ERRN_LOAD_ASSETS);
		free(path);
		path = ft_create_path_assets(COLLECTIBLE_XPM, frame);
		if (!path)
			error_exit(game, ERRS_MALLOC, ERRN_MALLOC);
		if (!ft_xpm_img(game, path, &game->collectible[i]))
			error_exit(game, "Error loading collectibles\n", ERRN_LOAD_ASSETS);
		free(path);
		i++;
	}
	if (!ft_xpm_img(game, GROUND_XPM, &game->assets[GROUND]))
		error_exit(game, "Error loading ground\n", ERRN_LOAD_ASSETS);
	if (!ft_xpm_img(game, EXIT_XPM, &game->assets[EXIT]))
		error_exit(game, "Error loading exit\n", ERRN_LOAD_ASSETS);
	
}
void	ft_game_loader(t_game *game)
{
	int		i;
	int		j;
	int		k;
	char	*path_l;
	char	*path_r;

	ft_map_loader(game);
	i = 0;
	while (i < NB_CHARS)
	{
		j = 0;
		while (j < NB_MOVES)
		{
			k = 0;
			while (k < NB_FRAMES_ANIM_CHAR)
			{
				path_l = ft_create_path_char(i, j,"l_", k);
				path_r = ft_create_path_char(i, j,"r_", k);
				if (!path_l || !path_r)
					error_exit(game, "Error allocating imgs\n", ERRN_MALLOC);
				if (!ft_xpm_img(game, path_l, &game->characters[i][j][O_LEFT][k])
					|| !ft_xpm_img(game, path_r, &game->characters[i][j][O_RIGHT][k]))
				{
					ft_free_paths(path_l, path_r);
					error_exit(game, "Error loading imgs\n", ERRN_OPEN);
				}
				ft_free_paths(path_l, path_r);
				k++;
			}
			j++;
		}
		i++;
	}
}
