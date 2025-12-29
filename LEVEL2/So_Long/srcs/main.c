/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 12:10:57 by tibras            #+#    #+#             */
/*   Updated: 2025/12/29 17:58:01 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "so_long_srcs.h"

int ft_end_menu(t_game *vars)
{
	mlx_loop_end(vars->mlx);
	return (1);
}

int	ft_handle_keys_menu(int keycode, t_game *vars)
{
	if (keycode == KEY_ESC)
		ft_end_menu(vars);
	else if (keycode == KEY_A)
		ft_printf("A\n");
	return (0);
}

void	ft_run_menu(t_game *vars)
{
	mlx_key_hook(vars->win, ft_handle_keys_menu, vars);
	mlx_loop(vars->mlx);
	if (vars->win)
		mlx_clear_window(vars->mlx, vars->win);
	if (vars->mlx)
	{
		mlx_destroy_display(vars->mlx);
		free(vars->mlx);
	}
}


#include <stdio.h>
int	main(void)
{
	t_game	game;

	game.mlx = mlx_init();
	if (!game.mlx)
		return (1);
	game.win = mlx_new_window(game.mlx, 600, 600, "Hello World");
	ft_run_menu(&game);
	// ft_run_game(&menu);
	return (0);
}