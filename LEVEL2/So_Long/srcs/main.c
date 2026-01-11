/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 12:10:57 by tibras            #+#    #+#             */
/*   Updated: 2026/01/11 18:04:33 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "so_long_srcs.h"

void	ft_destroy_menu_img(t_game *game)
{
	ft_clear_imgs(game, &game->menu);
}
int ft_end_menu(t_game *game)
{
	if (game->mlx)
		mlx_loop_end(game->mlx);
	return (0);
}

int	ft_handle_keys_menu(int keycode, t_game *game)
{
	if (keycode == KEY_ESC)
		ft_end_menu(game);
	else if (keycode == KEY_A)
		ft_put_img(game, &game->menu.characters[0][IDLE][0], MENU_WIDTH / 2 - 16, MENU_HEIGHT / 2 - 16);
	else if (keycode == KEY_D)
		mlx_clear_window(game->mlx, game->win);
	else if (keycode == KEY_SPACE)
		ft_launch_game(game);
	return (0);
}

int ft_handle_keys(int keycode, t_game *game)
{
	if (game->display == MENU)
		ft_handle_keys_menu(keycode, game);
	else if (game->display == GAME)
		ft_handle_keys_game(keycode, game);
	return (0);
}


void	ft_run_menu(t_game *vars)
{
	ft_menu_loader(vars, &vars->menu);
	mlx_key_hook(vars->win, ft_handle_keys, vars);
	mlx_hook(vars->win, 17, 0, ft_end_menu, vars);
	mlx_loop(vars->mlx);
	ft_destroy_menu_img(vars);
	if (vars->map)
		ft_clear_map(vars->map, vars->map_height);
	if (vars->win)
	{
		mlx_destroy_window(vars->mlx, vars->win);
		vars->win = NULL;
	}
	if (vars->mlx)
	{
		mlx_destroy_display(vars->mlx);
		free(vars->mlx);
		vars->mlx = NULL;
		ft_printf("FIN DE GAME \n");
	}
}

void ft_game_init(t_game *game)
{
	game->mlx = mlx_init();
	if (!game->mlx)
		error_exit(game, "Couldn´t affect mlx\n");
	game->win = mlx_new_window(game->mlx, MENU_WIDTH, MENU_HEIGHT, "SO_LONG");
	if (!game->win)
		error_exit(game, "Couldn´t affect win\n");
	// game->display = MENU;
}

#include <stdio.h>
int	main(int argc, char **argv)
{
	t_game	game;

	if (argc != 2)
		error_exit(NULL, "Wrong number of arguments || Prototyped ./so_long path_map\n");
	ft_bzero(&game, sizeof(t_game));
	ft_parsing(&game, argv[1]);
	ft_game_init(&game);
	ft_run_menu(&game);
	return (0);
}