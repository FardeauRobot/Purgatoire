/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_gc_add_node.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 10:00:00 by tibras            #+#    #+#             */
/*   Updated: 2026/05/11 10:37:41 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_gc.h"

int	ft_gc_add_node(t_list **gc_head, void *ptr)
{
	t_list	*new_node;

	new_node = malloc(sizeof(t_list));
	if (!new_node)
		return (1);
	new_node->content = ptr;
	new_node->next = *gc_head;
	// new_node->previous = NULL;
	// new_node->index = 0;
	*gc_head = new_node;
	return (0);
}
