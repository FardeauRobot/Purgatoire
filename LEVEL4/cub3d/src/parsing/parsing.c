/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 11:03:04 by fardeau           #+#    #+#             */
/*   Updated: 2026/03/08 19:28:52 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

/*
** FT_PARSING - MAIN PARSING ENTRY POINT
** VALIDATES ARGS, FILE FORMAT, STORES FILE, PARSES TEXTURES AND MAP
*/
int	ft_parsing(t_cub *data, char **argv, int argc)
{
	if (argc != 2)
		ft_exit(data, ERRN_PARSING, ERR_MSG_ARGS, ERR_MSG_ARGC);

	// CHECK FORMAT
	if (ft_format_check(argv[1]) != SUCCESS)
		ft_exit(data, ERRN_PARSING, ERR_MSG_OPEN, ERR_MSG_FORMAT);

	// CHECK OPEN
	data->fd_map = open(argv[1], O_RDONLY);
	if (data->fd_map == -1)
		ft_exit(data, ERRN_OPEN, ERR_MSG_OPEN, argv[1]);

	// STORE THE FILE
	if (ft_file_store(data) != SUCCESS)
		ft_exit(data, ERRN_PARSING, NULL, NULL);

	// CHECK TEXTURES
	if (ft_textures_parsing(data) != SUCCESS)
		ft_exit(data, ERRN_PARSING, ERR_MSG_PARSING, ERR_MSG_TEXTURES);

	// CHECK MAP
	ft_gc_free_all(&data->gc_tmp);
	return (SUCCESS);
}
