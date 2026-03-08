/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 11:03:04 by fardeau           #+#    #+#             */
/*   Updated: 2026/03/08 19:12:45 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

int	ft_texture_dispatch(t_cub *data, char *id, char *path)
{
	if (ft_strncmp(id, "NO", 3) == 0)
	{
		data->textures.north = ft_strdup_gc(path, &data->gc_global);
		if (!data->textures.north)
			return (ft_error(ERR_MSG_PARSING, ERR_MSG_MALLOC, ERRN_MALLOC));
	}
	if (ft_strncmp(id, "SO", 3) == 0)
	{
		data->textures.south = ft_strdup_gc(path, &data->gc_global);
		if (!data->textures.south)
			return (ft_error(ERR_MSG_PARSING, ERR_MSG_MALLOC, ERRN_MALLOC));
	}
	if (ft_strncmp(id, "WE", 3) == 0)
	{
		data->textures.west = ft_strdup_gc(path, &data->gc_global);
		if (!data->textures.west)
			return (ft_error(ERR_MSG_PARSING, ERR_MSG_MALLOC, ERRN_MALLOC));
	}
	if (ft_strncmp(id, "EA", 3) == 0)
	{
		data->textures.east = ft_strdup_gc(path, &data->gc_global);
		if (!data->textures.east)
			return (ft_error(ERR_MSG_PARSING, ERR_MSG_MALLOC, ERRN_MALLOC));
	}
	if (ft_strncmp(id, "F", 2) == 0)
	{
		data->textures.floor = ft_strdup_gc(path, &data->gc_global);
		if (!data->textures.floor)
			return (ft_error(ERR_MSG_PARSING, ERR_MSG_MALLOC, ERRN_MALLOC));
	}
	if (ft_strncmp(id, "C", 2) == 0)
	{
		data->textures.ceiling = ft_strdup_gc(path, &data->gc_global);
		if (!data->textures.ceiling)
			return (ft_error(ERR_MSG_PARSING, ERR_MSG_MALLOC, ERRN_MALLOC));
	}
	return (SUCCESS);
}

int ft_textures_detect(char *id)
{
	if (ft_strncmp(id, "NO", 3) != 0
		&& ft_strncmp(id, "SO", 3) != 0
		&& ft_strncmp(id, "EA", 3) != 0
		&& ft_strncmp(id, "WE", 3) != 0
		&& ft_strncmp(id, "F", 2) != 0
		&& ft_strncmp(id, "C", 2) != 0)
		return (FAILURE);
	return (SUCCESS);
}

int	ft_textures_fill(t_cub *data, char *line)
{
	char **arr;

	arr = ft_split_charset_gc((const char *)line, " \t\n", &data->gc_tmp);
	if (!arr)
		return (ft_error(ERR_MSG_PARSING, ERR_MSG_MALLOC, ERRN_MALLOC));
	if (!arr[0])
		return (FAILURE);

	if (ft_textures_detect(arr[0]))
		return (FAILURE);

	if (ft_strlen(arr[0]) > 2 || ft_matrix_len(arr) > 2)
		return (ft_error(ERR_MSG_INVALID_ID, arr[0], ERRN_MALLOC));

	if (ft_texture_dispatch(data, arr[0], arr[1]) != SUCCESS)
		return (ft_error(ERR_MSG_INVALID_ID, arr[0], ERRN_MALLOC));
	return (SUCCESS);
}

int	ft_file_store(t_cub *data)
{
	char	*line;
	int		i;
	t_list 	*node;
	t_list	*lst;

	i = 0;
	lst = NULL;
	line = get_next_line(data->fd_map);
	while (line)
	{
		ft_gc_add_node(&data->gc_tmp, line);
		node = ft_lstnew_gc(line, &data->gc_tmp);
		if (!node)
			return (ft_error(ERR_MSG_PARSING, ERR_MSG_MALLOC, ERRN_MALLOC));
		ft_lstadd_back(&lst, node);
		i++;
		line = get_next_line(data->fd_map);
	}
	data->file = ft_calloc_gc(i + 1, sizeof(char *), &data->gc_global);
	if (!data->file)
		return (ft_error(ERR_MSG_PARSING, ERR_MSG_MALLOC, ERRN_MALLOC));
	i = 0;
	while (lst)
	{
		data->file[i++] = ft_strdup_gc(lst->content, &data->gc_global);
		lst = lst->next;
	}
	data->file[i] = NULL;
	return (SUCCESS);
}

int	ft_textures_parsing(t_cub *data)
{
	int i;

	i = -1;
	while (data->file[++i])
		ft_textures_fill(data, data->file[i]);
	return (SUCCESS);
}

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