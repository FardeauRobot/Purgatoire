/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   structures.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/02 00:00:00 by tibras            #+#    #+#             */
/*   Updated: 2026/03/18 13:24:52 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CUB3D_STRUCTURES_H
# define CUB3D_STRUCTURES_H

# include "defines.h"
# include "libft.h"

/* ============== ENUM =============================== */

typedef enum e_orientation
{
	NORTH,
	SOUTH,
	WEST,
	EAST,
}	t_orientation;

typedef enum e_etile
{
	EMPTY,
	WALL
}	t_etile;

typedef enum e_switch
{
	ON,
	OFF
}	t_switch;

/* ============== STRUCTURES =============================== */

/*
** t_data - main project context
** Centralises all runtime data so it can be passed to functions cleanly.
** Add your fields here as the project grows.
*/

// RECALL FOR STRUCTURES USED BELOW THEIR CHILDREN
typedef struct s_map t_map;
typedef struct s_cub t_cub;
typedef struct s_player t_player;
typedef struct s_textures t_textures;
typedef struct s_minimap t_minimap;
typedef struct s_p_structs t_p_structs;

typedef struct s_p_structs
{
	t_cub		*p_cub;
	t_map 		*p_map;
	t_minimap 	*p_minimap;
	t_player 	*p_player;
	t_textures	*p_textures;
}	t_p_structs;

// STRUCTURE USED TO STORE THE DIFFERENT INFOS ABOUT AN IMG
typedef struct s_img
{
	void	*img;
	char	*addr;
	int		bpp;
	int		line_len;
	int		endian;
	int		width;
	int		height;
	int		color;
}	t_img;


// STRUCTURE USED TO STORE ALL THE DIFFERENT TEXTURES INFO
typedef struct s_textures
{
	t_img	empty_tile_map;
	t_img	wall_tile_map;
	char *north;
	t_img wall_n;
	char *south;
	t_img wall_s;
	char *east;
	t_img wall_e;
	char *west;
	t_img wall_w;
	char *floor;
	char *ceiling;
}	t_textures;

// STRUCTURE USED TO PRINT TILES OF THE MINIMAP
typedef struct s_tile
{
	t_img	tile_img;
	int y;
	int x;
}	t_tile;

// STRUCTURE USED TO STORE ALL THE MINIMAP INFOS -> NESTED IN MAP
typedef struct s_minimap
{
	t_p_structs	*p_structs;
	int		offset_x;
	int		offset_y;
	int		display_map;
	t_tile	tiles[2];
}	t_minimap;

// STRUCTURE USED TO STORE ALL THE MAP INFOS
typedef struct s_map
{
	t_p_structs *p_structs;
	int	    index_map_start;
	char 	**map;
	int 	height;
	int 	width;
	t_minimap		minimap;
}	t_map;

// STRUCTURE USED TO STORE ALL THE PLAYER INFOS
typedef struct s_player
{
	t_p_structs *p_structs;
	t_img			char_img;
	t_img			test_view;
	double			pos_x;
	double			pos_y;
	double			dir_x;
	double			dir_y;
	double			camera_plane_x;
	double			camera_plane_y;
	int				rotating;
	int				moving;
	t_orientation	orient;
}	t_player;

// STRUCTURE USED AS THE MAIN ACCESS POINT
typedef struct s_cub
{
	t_p_structs p_structs;
	void		    *mlx;
	void		    *win;
	t_img			display;
	int				screen_width;
	int				screen_height;
	char		    **file;
	int			    fd_map;
	t_map			map;
	t_minimap		minimap;
	t_player		player;
	t_textures		textures;
	t_list		    *gc_global;
	t_list		    *gc_tmp;
}	t_cub;

#endif
