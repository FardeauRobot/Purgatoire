/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   image_utils_1.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/08 15:54:36 by tibras            #+#    #+#             */
/*   Updated: 2026/02/14 13:44:54 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "so_long.h"
#include "so_long/imgs.h"

static long	ft_now_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000L + tv.tv_usec / 1000L);
}

void	ft_print_assets(t_game *game)
{
	size_t	x;
	size_t	y;

	y = 0;
	while (y < game->map_height)
	{
		x = 0;
		while (x < game-> map_width)
		{
			if (game->map[y][x] == '1')
				ft_put_img(game, &game->walls[game->frame_assets], x, y);
			else if (game->map[y][x] == 'C')
				ft_put_img(game, &game->collectible[game->frame_assets], x, y);
			else if (game->map[y][x] == 'E')
				ft_put_img(game, &game->exit_f[game->frame_assets], x, y);
			x++;
		}
		y++;
	}
}

void	ft_put_assets(t_game *game, long time)
{
	char	*nb_print;

	if (time - game->last_frame_assets_ms >= FRAME_MS_ASSETS)
	{
		game->frame_assets = (game->frame_assets + 1) % NB_FRAMES_ANIM_ASSETS;
		game->last_frame_assets_ms = time;
		ft_print_assets(game);
		nb_print = ft_itoa(game->move_count);
		mlx_string_put(game->mlx, game->win, IMG_SIZE / 3, IMG_SIZE, 0x000000,
			"NB MOVES =");
		if (!nb_print)
			error_exit(game, "Error malloc print score\n", ERRN_MALLOC);
		mlx_string_put(game->mlx, game->win, IMG_SIZE + 30, IMG_SIZE, 0x000000,
			nb_print);
		free(nb_print);
	}
}

int	ft_dynamic_render(t_game *game, int act)
{
	long	time;

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
	if (act)
		ft_put_assets(game, time);
	ft_put_img(game, &game->characters[game->active_char]
	[game->move][game->orient][game->frame], game->player_pos[X],
		game->player_pos[Y]);
	return (0);
}

void	ft_render_static(t_game *game)
{
	size_t	i;
	size_t	j;

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
		i++;
	}
}
