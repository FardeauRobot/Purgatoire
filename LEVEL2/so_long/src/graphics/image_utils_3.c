/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   image_utils_3.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/14 17:26:37 by tibras            #+#    #+#             */
/*   Updated: 2026/02/14 17:46:38 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "so_long.h"
#include "so_long/imgs.h"

void	ft_img_load_error(t_game *game, char **path, int err, char *err_str)
{
	if (*path)
		free(*path);
	error_exit(game, err_str, err);
}
