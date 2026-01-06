/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main2.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/06 18:30:00 by fardeau           #+#    #+#             */
/*   Updated: 2026/01/06 18:12:36 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "so_long_srcs.h"

static void ft_destroy_menu_img(t_game *game)
{
    if (game->test.img)
    {
        mlx_destroy_image(game->mlx, game->test.img);
        game->test.img = NULL;
    }
}

int ft_end_menu(t_game *game)
{
    if (game->mlx)
        mlx_loop_end(game->mlx);
    ft_destroy_menu_img(game);
    if (game->win)
    {
        mlx_destroy_window(game->mlx, game->win);
        game->win = NULL;
    }
    if (game->mlx)
    {
        mlx_destroy_display(game->mlx);
        free(game->mlx);
        game->mlx = NULL;
    }
    return (0);
}

int ft_handle_keys_menu(int keycode, t_game *game)
{
    if (keycode == KEY_ESC)
        return (ft_end_menu(game));
    else if (keycode == KEY_A)
    {
        ft_destroy_menu_img(game);
        game->test.img = mlx_xpm_file_to_image(game->mlx,
                IMG_PATH"Pink_Idle_0_64x64.xpm", &game->test.img_width,
                &game->test.img_height);
        if (game->test.img)
            mlx_put_image_to_window(game->mlx, game->win,
                    game->test.img, 400, 400);
    }
    else if (keycode == KEY_D)
        mlx_clear_window(game->mlx, game->win);
    return (0);
}

void ft_run_menu(t_game *game)
{
    mlx_key_hook(game->win, ft_handle_keys_menu, game);
    mlx_hook(game->win, 17, 0, ft_end_menu, game);
    mlx_loop(game->mlx);
}

int main(void)
{
    t_game game;

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
