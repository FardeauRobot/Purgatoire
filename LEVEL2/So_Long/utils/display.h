/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   display.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/06 17:59:00 by fardeau           #+#    #+#             */
/*   Updated: 2026/01/06 18:50:34 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DISPLAY_H
# define DISPLAY_H

# include "so_long_utils.h"

// MENU SIZE
# define MENU_WIDTH 800
# define MENU_HEIGHT 600

// MENU CENTER MACROS
# define MENU_CENTER_X (MENU_WIDTH / 2)
# define MENU_CENTER_OFF_X(off) (MENU_CENTER_X + (off))
# define MENU_CENTER_Y (MENU_HEIGHT / 2)
# define MENU_CENTER_OFF_Y(off) (MENU_CENTER_Y + (off))

#endif