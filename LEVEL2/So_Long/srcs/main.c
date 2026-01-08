/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 12:10:57 by tibras            #+#    #+#             */
/*   Updated: 2026/01/08 18:20:55 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "so_long_srcs.h"
#include "../utils/imgs.h"

void	ft_destroy_menu_img(t_game *game)
{
	if (game->menu.characters[0][0][0].img)
	{
		mlx_destroy_image(game->mlx, game->menu.characters[0][0][0].img);
		game->menu.characters[0][0][0].img = NULL;
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
		ft_menu_loader(game, &game->menu);
		if (game->menu.characters[0][0][0].img && game->menu.characters[1][0][0].img)
		{
			ft_put_img(game, &game->menu.characters[0][0][0], 200, 200);
			ft_put_img(game, &game->menu.characters[1][0][0], 300, 200);
		}
		else
			ft_printf("YA PAS WESH\n");
	}
	else if (keycode == KEY_D)
		mlx_clear_window(game->mlx, game->win);
	return (0);
}

void	ft_run_menu(t_game *vars)
{
	mlx_key_hook(vars->win, ft_handle_keys_menu, vars);
	mlx_hook(vars->win, 17, 0, ft_end_menu, vars);
	mlx_loop(vars->mlx);
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
int	main(int argc, char **argv)
{
	t_game	game;

	if (argc != 2)
	{
		ft_printf("ERREUR ARGUMENTS %s\n", argv[1]);
		// ERREUR NBR ARGUMENTS = "Expects ./so_long mapname"
	}
	// ft_parsing(&game, argv[1]);
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