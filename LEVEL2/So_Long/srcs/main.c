/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 12:10:57 by tibras            #+#    #+#             */
/*   Updated: 2026/01/06 19:01:30 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "so_long_srcs.h"

void	ft_destroy_menu_img(t_game *game)
{
	if (game->menu.character1[0].img)
	{
		mlx_destroy_image(game->mlx, game->menu.character1[0].img);
		game->menu.character1[0].img = NULL;
	}
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
	{
		ft_destroy_menu_img(game);
		game->menu.character1[0].img = mlx_xpm_file_to_image(game->mlx, IMG_PATH"Pink_Idle_0_64x64.xpm", &game->menu.character1[0].width, &game->menu.character1[0].height);
		if (game->menu.character1[0].img)
		{
			PUT_IMG(game, game->menu.character1[0].img, MENU_CENTER_OFF_X(100), MENU_CENTER_Y);
			PUT_IMG(game, game->menu.character1[0].img, MENU_CENTER_X, MENU_CENTER_Y);
			PUT_IMG(game, game->menu.character1[0].img, MENU_CENTER_OFF_X(-100), MENU_CENTER_Y);
		}
	}
	else if (keycode == KEY_D)
		mlx_clear_window(game->mlx, game->win);
	return (0);
}

void	ft_run_menu(t_game *vars)
{
	// vars->test.img = mlx_xpm_file_to_image(vars->mlx, IMG_PATH"Pink_Idle_0_64x64.xpm", &vars->test.img_width, &vars->test.img_height);
	// mlx_put_image_to_window(vars->mlx, vars->win, vars->test.img, 400, 400);
	mlx_key_hook(vars->win, ft_handle_keys_menu, vars);
	mlx_hook(vars->win, 17, 0, ft_end_menu, vars);
	mlx_loop(vars->mlx);
	// Cleanup after loop exits to avoid using freed X11 display inside mlx_loop
	ft_destroy_menu_img(vars);
	if (vars->win)
	{
		mlx_destroy_window(vars->mlx, vars->win);
		vars->win = NULL;
		ft_printf("FIN DE GAME\n");
	}
	if (vars->mlx)
	{
		mlx_destroy_display(vars->mlx);
		free(vars->mlx);
		vars->mlx = NULL;
		ft_printf("FIN DE GAME 2\n");
	}
}


#include <stdio.h>
int	main(void)
{
	t_game	game;

	ft_bzero(&game, sizeof(t_game));
	game.mlx = mlx_init();
	if (!game.mlx)
		return (1);
	game.win = mlx_new_window(game.mlx, MENU_WIDTH, MENU_HEIGHT, "Hello World");
	if (!game.win)
		return (ft_end_menu(&game));
	ft_run_menu(&game);
	return (0);
}