/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   imgs.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/31 11:39:29 by fardeau           #+#    #+#             */
/*   Updated: 2026/01/08 18:15:02 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IMGS_H
# define IMGS_H


// PATHS TO IMGS
# define IMG_PATH "imgs/"
# define PATH_SIZE 500

# define HOWLET_PATH "howlet/"
# define PINK_PATH "pink/"
# define BLUE_PATH "blue/"

# define IDLE_PATH "idle_"
# define MOVING_PATH "move_"
# define PLACE_PATH "place_"

# define RIGHT "r_"
# define LEFT "l_"

# define XPM ".xpm"

typedef struct s_img
{
	void	*img;
	int		width;
	int		height;
}	t_img;

// RECALL THE STRUCT 
typedef struct s_game	t_game;
typedef struct s_menu	t_menu;

int	ft_put_img(t_game *game, t_img *img, int x, int y);
void	ft_xpm_img(t_game *game, char *path, t_img *img);
void	ft_menu_loader(t_game *game, t_menu *menu);

#endif