/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   checking.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/30 10:52:30 by tibras            #+#    #+#             */
/*   Updated: 2026/01/30 11:21:57 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "so_long_srcs.h"

int	ft_char_check(char *str)
{
	int		i;
	char	*accepted;

	accepted = "01CEP\n";
	i = 0;
	while (str[i])
	{
		if (!ft_ischarset(str[i], accepted))
			return (FAILURE);
		i++;
	}
	return (SUCCESS);
}

int	ft_format_check(char *filepath)
{
	int	len_path;

	len_path = ft_strlen(filepath);
	if (ft_strncmp(&filepath[len_path - 4], ".ber", 4))
		return (FAILURE);
	return (SUCCESS);
}

int	ft_check_walls(t_game *game, char *row_map, size_t row)
{
	size_t	i;

	i = 0;
	if (row == 0 || row == game->map_height - 1)
	{
		while (row_map[i] && row_map[i] != '\n')
		{
			if (row_map[i] != '1')
				return (FAILURE);
			i++;
		}
	}
	else
		if (row_map[0] != '1' || row_map[game->map_width - 1] != '1')
			return (FAILURE);
	return (SUCCESS);
}

void	ft_check_assets(t_game *game)
{
	if (game->collectibles < 1)
		error_exit(game, ERRS_MAP_COLLECT, ERRN_MAP_COLLECT);
	if (game->exit != 1)
		error_exit(game, ERRS_MAP_EXIT, ERRN_MAP_EXIT);
	if (game->nb_player != 1)
	{
		if (game->nb_player > 1)
			error_exit(game, "Too much players detected\n", ERRN_MAP_PLAYER);
		error_exit(game, ERRS_MAP_PLAYER, ERRN_MAP_PLAYER);
	}
}
