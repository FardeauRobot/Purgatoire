/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   struct_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/13 12:36:28 by tibras            #+#    #+#             */
/*   Updated: 2026/03/13 12:42:40 by tibras           ###   ########.fr       */
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