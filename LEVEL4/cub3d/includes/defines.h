/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   defines.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/02 00:00:00 by tibras            #+#    #+#             */
/*   Updated: 2026/03/11 18:48:45 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CUB3D_DEFINES_H
# define CUB3D_DEFINES_H

# include "structures.h"

/* ============== RETURN CODES ============================= */
# define SUCCESS 0
# define FAILURE 1

/* ============== GAME MACROS ============================= */

# define	WIN_WIDTH	1920
# define	WIN_HEIGHT 1080
# define	WIN_NAME 	"cub3d"

# define	TILE_SIZE 32
# define	CHAR_SIZE TILE_SIZE / 4

# define	EMPTY_TILE_COL 0xFFFFFF
# define	WALL_TILE_COL 0xC0C0C0
# define	CHAR_COL 0xFF0000

# define	CHAR_SPEED 0.4

/* ============== CORE ============================= */
// GAME.C
void	ft_game(t_cub *data);

/* ============== PARSING ============================= */
// PARSING.C
int	ft_parsing(t_cub *data, char **argv, int argc);

// PARSING_TEXTURES.C
int	ft_file_store(t_cub *data);
int	ft_textures_parsing(t_cub *data);

/* ============== RENDER ============================= */
// RENDER_MAP.C
void	ft_minimap_draw(t_minimap *minimap);
void	ft_char_draw(t_player *player);
int	ft_map_render(void *cub);

// RENDER_UTILS.C
void	ft_img_pixel_put(t_img *img, int x, int y, int color);
void	ft_img_fill(t_img *img, int color);

// IMGS_INIT.C
void	ft_img_init(t_cub *data, t_img *img, int width, int height);
void	ft_minimap_init(t_map *map);
void	ft_char_init(t_cub *data);

// RENDER_MAP.C
void	ft_tile_draw(t_tile *tile, int map_x, int map_y);

/* ============== UTILS ============================= */
// OUTPUT.C
void	ft_matrix_print(char **matrix);
void	ft_cub_print(t_cub *data);

// PARSING_UTILS.C
void	ft_player_set(t_player *player, int x, int y, char orient);
int	ft_format_check(char *filepath);

// ERROR.C
void	ft_data_clean(t_cub *data);

// ERROR.C
void	ft_mlx_init(t_cub *data);

#endif
