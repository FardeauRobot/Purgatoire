/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/09 17:35:57 by tibras            #+#    #+#             */
/*   Updated: 2026/01/13 15:16:30 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "so_long_utils.h"

void	ft_free_paths(char *path_l, char *path_r)
{
	if (path_l)
		free(path_l);
	if (path_r)
		free(path_r);
}

void	ft_clear_map(char **map, ssize_t height)
{
	ssize_t i;

	i = 0;
	while (i < height)
	{
		if (map[i])
			free(map[i]);
		i++;
	}
	free(map);
}