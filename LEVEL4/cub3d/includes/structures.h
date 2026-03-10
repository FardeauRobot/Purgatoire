/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   structures.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/02 00:00:00 by tibras            #+#    #+#             */
/*   Updated: 2026/03/10 23:01:41 by fardeau          ###   ########.fr       */
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
	EAST
}	t_orientation;


/* ============== STRUCTURES =============================== */

/*
** t_data - main project context
** Centralises all runtime data so it can be passed to functions cleanly.
** Add your fields here as the project grows.
*/
typedef struct s_img
{
	void	*img;
	char	*addr;
	int		bpp;
	int		line_len;
	int		endian;
}	t_img;

typedef struct s_textures
{
	char *north;
	t_img *wall_n;
	char *south;
	t_img *wall_s;
	char *east;
	t_img *wall_e;
	char *west;
	t_img *wall_w;
	char *floor;
	char *ceiling;
}	t_textures;

typedef struct s_img_database
{
	/* data */
	t_img	tile_wall;
	t_img	tile_empty;
	t_img	wall_north;
	t_img	wall_south;
	t_img	wall_west;
	t_img	wall_east;
}	t_img_database;

typedef struct s_player
{
	double			pos[2];
	t_orientation	orient;
}	t_player;


typedef struct s_cub
{
	void		    *mlx;
	void		    *win;
	int			    fd_map;
	char		    **file;
	int			    index_map_start;
	char	    	**map;
	t_player		player;
	t_textures		textures;
	t_img_database	img_data;
	t_list		    *gc_global;
	t_list		    *gc_tmp;
}	t_cub;

/*
** Add project-specific structures here, e.g.:
** typedef struct s_token { ... } t_token;
** typedef struct s_cmd   { ... } t_cmd;
*/

#endif
