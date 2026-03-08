/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 11:59:48 by fardeau           #+#    #+#             */
/*   Updated: 2026/03/08 12:00:00 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

/*
** FT_FORMAT_CHECK - VALIDATES THAT THE FILE EXTENSION IS .CUB
** RETURNS SUCCESS IF VALID, FAILURE OTHERWISE
*/
int	ft_format_check(char *filepath)
{
	int	len_path;

	len_path = ft_strlen(filepath);
	if (ft_strncmp(&filepath[len_path - 4], ".cub", 4))
		return (FAILURE);
	return (SUCCESS);
}