/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   imgs.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/31 11:39:29 by fardeau           #+#    #+#             */
/*   Updated: 2026/01/15 14:14:00 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IMGS_H
# define IMGS_H

#include "../so_long.h"

# define FRAME_MS_CHAR 150
# define FRAME_MS_ASSETS 400 

# define IMG_SIZE 64

// PATHS TO IMGS
# define IMG_PATH "imgs/"

# define HOWLET_PATH "howlet/"
# define PINK_PATH "pink/"
# define BLUE_PATH "blue/"

# define IDLE_PATH "idle_"
# define MOVING_PATH "walk_"

# define RIGHT_P "r_"
# define LEFT_P "l_"

# define XPM ".xpm"

// DEFINE ASSETS PATHS
# define WALL_XPM "imgs/assets/wall_"
# define COLLECTIBLE_XPM "imgs/assets/collectible_"
# define GROUND_XPM "imgs/assets/floor.xpm"
# define EXIT_XPM "imgs/assets/exit.xpm"

// RECALL THE STRUCT 
typedef struct s_img	t_img;
typedef struct s_game	t_game;

int		ft_put_img(t_game *game, t_img *img, int x, int y);
int		ft_xpm_img(t_game *game, char *path, t_img *img);
void	ft_destroy_img(t_game *game, t_img *img);
void	ft_game_loader(t_game *game);

#endif