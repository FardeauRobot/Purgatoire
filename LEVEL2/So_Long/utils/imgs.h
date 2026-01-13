/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   imgs.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/31 11:39:29 by fardeau           #+#    #+#             */
/*   Updated: 2026/01/13 16:20:59 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IMGS_H
# define IMGS_H

#include "../so_long.h"

# define SLOT1 (int[2]){100, 100}
# define SLOT2 (int[2]){200, 100}
# define SLOT3 (int[2]){300, 100}

// PATHS TO IMGS
# define IMG_PATH "imgs/"

# define HOWLET_PATH "howlet/"
# define PINK_PATH "pink/"
# define BLUE_PATH "blue/"

# define IDLE_PATH "idle_"
# define MOVING_PATH "move_"
# define PLACE_PATH "place_"

# define RIGHT_P "r_"
# define LEFT_P "l_"

# define FRAMERATE 60

# define XPM ".xpm"

# define WALL_XPM "imgs/assets/wall.xpm"
# define GROUND_XPM "imgs/assets/floor.xpm"
# define COLLECTIBLE_XPM "imgs/assets/collectible.xpm"
# define EXIT_XPM "imgs/assets/exit.xpm"

// RECALL THE STRUCT 
typedef struct s_img	t_img;
typedef struct s_game	t_game;

int		ft_put_img(t_game *game, t_img *img, int x, int y);
int		ft_xpm_img(t_game *game, char *path, t_img *img);
void	ft_game_loader(t_game *game);

#endif