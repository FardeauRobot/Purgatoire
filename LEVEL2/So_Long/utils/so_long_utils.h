/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   so_long_utils.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/06 18:00:21 by fardeau           #+#    #+#             */
/*   Updated: 2026/01/14 17:00:15 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SO_LONG_UTILS_H
# define SO_LONG_UTILS_H


# include <errno.h>
# include "../so_long.h"

# include "imgs.h"
# include "keys.h"
# include "display.h"

# define TRUE 1
# define SUCCESS 1
# define FALSE 0
# define FAILURE 0

# define PATH_SIZE 200

typedef struct s_img	t_img;
typedef struct s_game	t_game;

////// MEMORY_UTILS.C //////
void	ft_free_paths(char *path_l, char *path_r);
void	ft_clear_map(char **map, ssize_t height);
//------------------//

////// IMAGE_UTILS.C //////
void	ft_render_static(t_game *game);
int	ft_dynamic_render(t_game *game);
void	ft_render_walls(t_game *game);
void	ft_clear_imgs(t_game *game);
//------------------//

#endif
