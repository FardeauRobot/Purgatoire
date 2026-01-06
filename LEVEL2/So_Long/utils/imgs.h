/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   imgs.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/31 11:39:29 by fardeau           #+#    #+#             */
/*   Updated: 2026/01/06 19:01:18 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IMGS_H
# define IMGS_H

# include "so_long_utils.h"

// PATHS TO IMGS
# define IMG_PATH "imgs/"

// MACROS FOR READABILITY
# define PUT_IMG(g, img, x, y) \
    mlx_put_image_to_window((g)->mlx, (g)->win, (img), (x), (y))

typedef struct s_img
{
	void	*img;
	int		width;
	int		height;
}	t_img;

#endif