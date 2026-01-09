/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   so_long_srcs.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 12:11:38 by tibras            #+#    #+#             */
/*   Updated: 2026/01/09 23:20:22 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SO_LONG_SRCS_H
# define SO_LONG_SRCS_H

# include "../so_long.h"

// STANDARD DEFINES FOR READABILITY
# define NB_FRAMES_ANIM_CHAR 1

typedef enum e_chars{ PINK, HOWLET, NB_CHARS}	t_chars;
typedef enum e_moves{ IDLE, NB_MOVES}	t_moves;

typedef struct s_img {
	void	*img;
	int		width;
	int		height;
}	t_img;

typedef struct s_menu {
	t_img	characters[NB_CHARS][NB_MOVES][NB_FRAMES_ANIM_CHAR];
	t_img	background;
	t_img	frame1;
	t_img	frame2;
}	t_menu;

// STRUCT FOR THE GAME
typedef struct s_game {
	void	*mlx;
	void	*win;
	char	**map;
	size_t		map_height;
	size_t		map_width;
	int		collectibles;
	int		exit;
	int		player;
	int		framerate;
	t_menu	menu;
}	t_game;

void ft_parsing(t_game *game, char *path_map);

#endif