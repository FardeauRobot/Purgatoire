/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   structures.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/02 00:00:00 by tibras            #+#    #+#             */
/*   Updated: 2026/03/08 20:33:48 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CUB3D_STRUCTURES_H
# define CUB3D_STRUCTURES_H

# include "defines.h"
# include "libft.h"

/* ============== STRUCTURES =============================== */

/*
** t_data - main project context
** Centralises all runtime data so it can be passed to functions cleanly.
** Add your fields here as the project grows.
*/
typedef struct s_img
{
	int     width;
	int     height;
	void	*img;
}	t_img;

typedef struct s_textures
{
	char *north;
	char *south;
	char *east;
	char *west;
	char *floor;
	char *ceiling;
}	t_textures;

typedef struct s_cub
{
	int			fd_map;
	char		**file;
	int			index_map_start;
	char	    **map;
	int			start_pos[2];
	t_textures	textures;
	t_list		*gc_global;
	t_list		*gc_tmp;
}	t_cub;

/*
** Add project-specific structures here, e.g.:
** typedef struct s_token { ... } t_token;
** typedef struct s_cmd   { ... } t_cmd;
*/

#endif
