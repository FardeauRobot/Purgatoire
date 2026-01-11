/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   so_long_utils.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/06 18:00:21 by fardeau           #+#    #+#             */
/*   Updated: 2026/01/11 18:04:59 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SO_LONG_UTILS_H
# define SO_LONG_UTILS_H

# include "../so_long.h"

# include "imgs.h"
# include "keys.h"
# include "display.h"

# define TRUE 1
# define FALSE 0

# define PATH_SIZE 200

typedef struct s_img	t_img;
typedef struct s_game	t_game;
typedef struct s_menu	t_menu;

void	ft_clear_map(char **map, ssize_t height);
void	ft_clear_imgs(t_game *game, t_menu *menu);

#endif
