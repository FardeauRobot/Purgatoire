/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 12:10:57 by tibras            #+#    #+#             */
/*   Updated: 2025/12/29 14:12:38 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "so_long_srcs.h"

int ft_end(t_vars *vars)
{
	mlx_destroy_window(vars->mlx, vars->win);
	mlx_destroy_display(vars->mlx);
	free(vars->mlx);
	exit (1);
}

int	ft_handle_keys(int keycode, t_vars *vars)
{
	if (keycode == KEY_ESC)
		return (ft_end(vars));
	return (0);
}

#include <stdio.h>

int	main(void)
{
	t_vars	vars;

	vars.mlx = mlx_init();
	if (!vars.mlx)
		return (1);
	vars.win = mlx_new_window(vars.mlx, 600, 600, "Hello World");
	mlx_key_hook(vars.win, ft_handle_keys, &vars);
	mlx_loop(vars.mlx);
	return (0);
}