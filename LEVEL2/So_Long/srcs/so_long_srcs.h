/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   so_long_srcs.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 12:11:38 by tibras            #+#    #+#             */
/*   Updated: 2026/01/12 17:05:45 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SO_LONG_SRCS_H
# define SO_LONG_SRCS_H

# include "../so_long.h"

// STANDARD DEFINES FOR READABILITY
# define NB_FRAMES_ANIM_CHAR 1

typedef enum e_chars
{
	PINK,
	HOWLET,
	NB_CHARS
}				t_chars;
typedef enum e_moves
{
	IDLE,
	NB_MOVES
}				t_moves;
typedef enum e_orientation
{
	LEFT,
	RIGHT,
	NB_ORIENTATION
}				t_orientation;

typedef struct s_img
{
	void		*img;
	int			width;
	int			height;
}				t_img;

// STRUCT FOR THE GAME
typedef struct s_game
{
	void		*mlx;
	void		*win;
	char		**map;
	size_t		map_height;
	size_t		map_width;
	int			collectibles;
	int			exit;
	int			player;
	int			framerate;
	t_img		characters[NB_CHARS][NB_MOVES]
	[NB_ORIENTATION][NB_FRAMES_ANIM_CHAR];
}				t_game;

////// PARSING.C //////
void			ft_parsing(t_game *game, char *path_map);
//------------------//

////// GAME.C //////
int				ft_end_game(t_game *game);
int				ft_handle_keys_game(int keycode, t_game *game);
void			ft_launch_game(t_game *game);
//------------------//

////// MAIN.C //////
// int			ft_end_menu(t_game *game);
//------------------//

#endif