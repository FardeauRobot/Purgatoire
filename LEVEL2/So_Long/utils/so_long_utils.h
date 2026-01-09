/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   so_long_utils.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/06 18:00:21 by fardeau           #+#    #+#             */
/*   Updated: 2026/01/09 19:44:09 by tibras           ###   ########.fr       */
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


void	ft_clear_map(char **map, ssize_t height);

#endif
