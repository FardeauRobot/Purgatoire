/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   imgs.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/31 11:39:29 by fardeau           #+#    #+#             */
/*   Updated: 2026/01/09 23:17:31 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IMGS_H
# define IMGS_H

#include "../so_long.h"

// PATHS TO IMGS
# define IMG_PATH "imgs/"

# define HOWLET_PATH "howlet/"
# define PINK_PATH "pink/"
# define BLUE_PATH "blue/"

# define IDLE_PATH "idle_"
# define MOVING_PATH "move_"
# define PLACE_PATH "place_"

# define RIGHT "r_"
# define LEFT "l_"

# define FRAMERATE 60

# define XPM ".xpm"
// RECALL THE STRUCT 
typedef struct s_img	t_img;
typedef struct s_game	t_game;
typedef struct s_menu	t_menu;

int		ft_put_img(t_game *game, t_img *img, int x, int y);
int		ft_xpm_img(t_game *game, char *path, t_img *img);
void	ft_menu_loader(t_game *game, t_menu *menu);

#endif