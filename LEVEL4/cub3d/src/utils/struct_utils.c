/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   struct_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/13 12:36:28 by tibras            #+#    #+#             */
/*   Updated: 2026/03/13 17:43:28 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	ft_struct_affect(t_cub *data, t_p_structs *p_structs)
{
	p_structs->p_data = data;
	p_structs->p_map = &data->map;
	p_structs->p_minimap = &data->minimap;
	p_structs->p_player = &data->player;
}

void	*ft_get_struct(void *ptr, int struct_id)
{
	t_p_structs	*p_structs;

	if (!ptr)
		return (NULL);
	p_structs = (t_p_structs *)ptr;
	if (struct_id == S_DATA)
		return (p_structs->p_data);
	if (struct_id == S_MAP)
		return (p_structs->p_map);
	if (struct_id == S_MINIMAP)
		return (p_structs->p_minimap);
	if (struct_id == S_PLAYER)
		return (p_structs->p_player);
	return (NULL);
}

void	ft_init_structs(t_cub *data)
{
	ft_struct_affect(data, &data->p_structs);
	ft_struct_affect(data, &data->map.p_structs);
	ft_struct_affect(data, &data->minimap.p_structs);
	ft_struct_affect(data, &data->player.p_structs);
	ft_struct_affect(data, &data->textures.p_structs);
}
