/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   so_long.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 12:12:07 by tibras            #+#    #+#             */
/*   Updated: 2026/01/08 18:22:41 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SO_LONG_H
# define SO_LONG_H

# include "libft.h"
# include "mlx.h"
# include "utils/keys.h"

// STANDARD DEFINES FOR READABILITY
# define TRUE 1
# define FALSE 0

# define EXIT_FAILURE 1
# define EXIT_SUCCESS 0

# define NB_FRAMES_ANIM_CHAR 1

typedef enum e_chars{ PINK, HOWLET, NB_CHARS}	t_chars;
typedef enum e_moves{ IDLE, NB_MOVES}	t_moves;

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
	int		map_height;
	int		map_width;
	int		framerate;
	t_menu	menu;
}	t_game;

#endif