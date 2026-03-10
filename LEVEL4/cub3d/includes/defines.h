/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   defines.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/02 00:00:00 by tibras            #+#    #+#             */
/*   Updated: 2026/03/10 23:04:40 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CUB3D_DEFINES_H
# define CUB3D_DEFINES_H

# include "structures.h"

/* ============== RETURN CODES ============================= */
# define SUCCESS 0
# define FAILURE 1

/* ============== GAME MACROS ============================= */

# define WIN_WIDTH	1920
# define WIN_HEIGHT 1080
# define WIN_NAME 	"cub3d"

# define MAP_TILE_SIZE 32


// PARSING/PARSING.C
int	ft_parsing(t_cub *data, char **argv, int argc);

// PARSING/PARSING_TEXTURES.C
int	ft_file_store(t_cub *data);
int	ft_textures_parsing(t_cub *data);

// UTILS/OUTPUT.C
void	ft_matrix_print(char **matrix);
void	ft_cub_print(t_cub *data);

// UTILS/PARSING_UTILS.C
void	ft_player_init(t_player *player, int x, int y, char orient);
int	ft_format_check(char *filepath);

// UTILS/ERROR.C
void	ft_data_clean(t_cub *data);

// CORES/GAME.C
void	ft_game(t_cub *data);

// UTILS/ERROR.C
void	ft_mlx_init(t_cub *data);


#endif
