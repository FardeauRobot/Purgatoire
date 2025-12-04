/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   node_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/03 15:50:06 by tibras            #+#    #+#             */
/*   Updated: 2025/12/04 12:18:16 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap_utils.h"

// CREER UN NOUVEAU NOEUD
t_node	*ft_node_init(int nb)
{
	t_node	*new;	

	new = malloc(sizeof(t_node));
	if (!new)
		return (NULL);
	new->value = nb;
	return (new);
}

