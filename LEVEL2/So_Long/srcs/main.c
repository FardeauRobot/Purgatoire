/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 12:10:57 by tibras            #+#    #+#             */
/*   Updated: 2026/01/12 18:07:28 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "so_long_srcs.h"

/*
void	ft_destroy_menu_img(t_game *game)
{
	ft_clear_imgs(game, &game->menu);
}

int	ft_end_menu(t_game *game)
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
		ft_put_img(game, &game->menu.characters[0][IDLE][LEFT][0],
			MENU_WIDTH / 2, MENU_HEIGHT / 2);
	else if (keycode == KEY_D)
		mlx_clear_window(game->mlx, game->win);
	else if (keycode == KEY_SPACE)
		ft_launch_game(game);
	return (0);
}
*/

int	ft_handle_keys(int keycode, t_game *game)
{
	ft_handle_keys_game(keycode, game);
	return (0);
}

void	ft_run_game(t_game *game)
{
	ft_game_loader(game);
	mlx_key_hook(game->win, ft_handle_keys, game);
	mlx_hook(game->win, 17, 0, ft_end_game, game);
	mlx_loop(game->mlx);
	ft_clear_game(game);
}

void	ft_game_init(t_game *game)
{
	game->mlx = mlx_init();
	if (!game->mlx)
		error_exit(game, "Couldn´t affect mlx\n", ERRN_MLX);
	game->win = mlx_new_window(game->mlx, MENU_WIDTH, MENU_HEIGHT, "SO_LONG");
	if (!game->win)
		error_exit(game, "Couldn´t affect win\n", ERRN_MLX);
}

int	main(int argc, char **argv)
{
	t_game	game;

	if (argc != 2)
		error_exit(NULL, ERRS_ARGS, ERRN_ARGS);
	ft_bzero(&game, sizeof(t_game));
	ft_parsing(&game, argv[1]);
	ft_game_init(&game);
	ft_run_game(&game);
	return (EXIT_SUCCESS);
}
