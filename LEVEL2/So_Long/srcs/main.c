/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 12:10:57 by tibras            #+#    #+#             */
/*   Updated: 2026/01/14 16:58:57 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "so_long_srcs.h"

void	ft_run_game(t_game *game)
{
	ft_game_loader(game);
	ft_render_static(game);
	mlx_key_hook(game->win, ft_handle_keys, game);
	mlx_loop_hook(game->mlx, ft_dynamic_render, game);
	mlx_hook(game->win, 17, 0, ft_end_game, game);
	mlx_loop(game->mlx);
	ft_clear_game(game);
}

void	ft_game_init(t_game *game)
{
	game->mlx = mlx_init();
	if (!game->mlx)
		error_exit(game, "Couldn´t affect mlx\n", ERRN_MLX);
	game->win = mlx_new_window(game->mlx, game->map_width * IMG_SIZE,
		game->map_height * IMG_SIZE, "SO_LONG");
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
