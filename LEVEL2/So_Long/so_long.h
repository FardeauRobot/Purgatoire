/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   so_long.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/22 12:12:07 by tibras            #+#    #+#             */
/*   Updated: 2026/01/06 18:51:31 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SO_LONG_H
# define SO_LONG_H

# include "libft.h"
# include "mlx.h"
# include "utils/so_long_utils.h"

// STANDARD DEFINES FOR READABILITY
# define TRUE 1
# define FALSE 0

typedef struct s_menu {
	t_img	character1[4];
	t_img	character2[4];
	t_img	character3[4];
}	t_menu;

// STRUCT FOR THE GAME
typedef struct s_game {
	void	*mlx;
	void	*win;
	int		framerate;
	t_menu	menu;
}	t_game;

#endif