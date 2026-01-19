/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   so_long_srcs.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 12:11:38 by tibras            #+#    #+#             */
/*   Updated: 2026/01/19 12:48:54 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SO_LONG_SRCS_H
# define SO_LONG_SRCS_H

# include "../so_long.h"

// STANDARD DEFINES FOR READABILITY
# define NB_FRAMES_ANIM_CHAR 4
# define NB_FRAMES_ANIM_ASSETS 4

typedef enum e_chars
{
	PINK,
	OWLET,
	DUDE,
	NB_CHARS
}				t_chars;

typedef enum e_directions
{
	UP,
	DOWN,
	LEFT,
	RIGHT,
	NEUTRAL,
	NB_DIR
}	t_directions;

typedef enum e_moves
{
	IDLE,
	WALK,
	JUMP,
	ATTACK,
	NB_MOVES
}				t_moves;


typedef enum e_orientation
{
	O_LEFT,
	O_RIGHT,
	NB_ORIENTATION
}				t_orientation;

typedef enum e_coordinates
{
	X,
	Y,
}	t_coordinates;

typedef enum e_assets
{
	WALL,
	GROUND,
	COLLECTIBLE,
	EXIT,
	NB_ASSETS,
}	t_assets;

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
	int			display_height;
	int			display_width;
	size_t		map_height;
	size_t		map_width;
	int			active_char;
	int			player_pos[2];
	int			move;
	int			move_count;
	int			collected;
	int			collectibles;
	int			exit;
	int			nb_player;
	int			frame;
	int			frame_assets;
	int			orient;
	long		last_frame_ms;
	long		last_frame_assets_ms;
	long		last_move_ms;
	t_img		walls[NB_FRAMES_ANIM_ASSETS];
	t_img		collectible[NB_FRAMES_ANIM_ASSETS];
	t_img		exit_f[NB_FRAMES_ANIM_ASSETS];
	t_img		assets[NB_ASSETS];
	t_img		characters[NB_CHARS][NB_MOVES]
	[NB_ORIENTATION][NB_FRAMES_ANIM_CHAR];
}				t_game;

////// PARSING.C //////
void			ft_parsing(t_game *game, char *path_map);
//------------------//

////// GAME.C //////
int				ft_handle_keys(int keycode, t_game *game);
int				ft_end_game(t_game *game);
void			ft_launch_game(t_game *game);
//------------------//

////// MAIN.C //////
// int			ft_end_menu(t_game *game);
//------------------//

#endif