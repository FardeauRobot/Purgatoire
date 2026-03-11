/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_textures.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 19:19:24 by fardeau           #+#    #+#             */
/*   Updated: 2026/03/10 19:51:57 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

/*
** FT_TEXTURE_DISPATCH - ASSIGNS A TEXTURE PATH TO THE MATCHING FIELD
** MATCHES ID (NO, SO, WE, EA, F, C) AND STORES PATH IN DATA->TEXTURES
*/
static int	ft_texture_dispatch(t_cub *data, char *id, char *path)
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

/*
** FT_TEXTURES_DETECT - CHECKS IF ID IS A VALID TEXTURE/COLOR IDENTIFIER
** RETURNS SUCCESS IF ID IS NO, SO, EA, WE, F OR C, FAILURE OTHERWISE
*/
static int ft_textures_detect(char *id)
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

/*
** FT_TEXTURES_FILL - SPLITS A LINE AND FILLS TEXTURE DATA
** RETURNS SUCCESS IF LINE IS A VALID TEXTURE, FAILURE IF NOT
*/
static int	ft_textures_fill(t_cub *data, char *line)
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

/*
** FT_FILE_STORE - READS THE .CUB FILE AND STORES EACH LINE IN DATA->FILE
** USES GET_NEXT_LINE TO READ, THEN COPIES INTO A GC-MANAGED CHAR **
*/
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
		if (ft_gc_add_node(&data->gc_tmp, line))
			return (ft_error(ERR_MSG_PARSING, ERR_MSG_MALLOC, ERRN_MALLOC));
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

/*
** FT_TEXTURES_PARSING - ITERATES FILE LINES TO PARSE TEXTURES AND COLORS
** STOPS WHEN IT HITS THE FIRST MAP LINE AND SETS DATA->INDEX_MAP_START
*/
int	ft_textures_parsing(t_cub *data)
{
	int	i;
	int	ret;

	i = -1;
	while (data->file[++i])
	{
		ret = ft_textures_fill(data, data->file[i]);
		if (ret != SUCCESS && ret != FAILURE)
			return (ret);
		if (ret == FAILURE && !ft_is_only(data->file[i], ft_isspace))
		{
			data->map.index_map_start = i;
			return (SUCCESS);
		}
	}
	return (SUCCESS);
}
