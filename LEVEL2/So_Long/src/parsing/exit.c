/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/30 10:54:41 by tibras            #+#    #+#             */
/*   Updated: 2026/01/30 11:23:15 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "so_long_srcs.h"

char	**ft_create_map_ff(t_game *game)
{
	char	**map_cpy;
	size_t	i;

	i = 0;
	map_cpy = malloc(sizeof(char *) * game->map_height);
	if (!map_cpy)
		error_exit(game, "Error malloc flood-fill\n", ERRN_MALLOC);
	ft_bzero(map_cpy, game->map_height * sizeof(char *));
	while (i < game->map_height)
	{
		map_cpy[i] = ft_strdup(game->map[i]);
		if (!map_cpy[i])
		{
			ft_clear_map(map_cpy, i);
			error_exit(game, "Error malloc flood-fill\n", ERRN_MALLOC);
		}
		i++;
	}
	return (map_cpy);
}

int	ft_flood_fill(char **map, int y, int x, int *gold_count)
{
	int	exit_found;

	exit_found = 0;
	if (map[y][x] == '1' || map[y][x] == 'F')
		return (FAILURE);
	if (map[y][x] == 'C')
		(*gold_count)--;
	if (map[y][x] == 'E')
		exit_found = 1;
	map[y][x] = 'F';
	if (ft_flood_fill(map, y - 1, x, gold_count))
		exit_found = 1;
	if (ft_flood_fill(map, y + 1, x, gold_count))
		exit_found = 1;
	if (ft_flood_fill(map, y, x + 1, gold_count))
		exit_found = 1;
	if (ft_flood_fill(map, y, x - 1, gold_count))
		exit_found = 1;
	return (SUCCESS);
}

void	ft_check_exit(t_game *game)
{
	char	**map_ff;
	int		gold_count;

	gold_count = game->collectibles;
	map_ff = ft_create_map_ff(game);
	if (!map_ff)
		error_exit(game, "Error malloc flood-fill\n", ERRN_MALLOC);
	if (!ft_flood_fill(map_ff, game->player_pos[1], game->player_pos[0],
			&gold_count) || gold_count != 0)
	{
		ft_clear_map(map_ff, game->map_height);
		error_exit(game, ERRS_MAP_EXIT, ERRN_MAP_EXIT);
	}
	ft_clear_map(map_ff, game->map_height);
}
