/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/08 18:21:27 by tibras            #+#    #+#             */
/*   Updated: 2026/01/08 18:37:01 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "so_long_srcs.h"

int ft_parsing (char *path_map)
{
	int fd;
	int len;
	int max;
	int height;
	char *line;

	if (path_map != ".ber")
		return (1) // ERREUR FORMAT
	fd = open(path_map, O_RDONLY);
	if (fd == -1)
		return (2); // ERREUR OPEN
	height = 0;
	line = get_next_line(fd);
	max = ft_strlen(line);
	while (line)
	{
		height++;
		len = ft_strlen(line);
		free(line);
		if (len != max)
			return (3); // ERREUR DIMENSIONS
		line = get_next_line(fd);
	}
}
