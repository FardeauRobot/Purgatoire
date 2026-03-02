/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_gc_free_all.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 10:00:00 by tibras            #+#    #+#             */
/*   Updated: 2026/01/27 09:53:53 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_gc.h"

void	ft_gc_free_all(t_list **gc_head)
{
	t_list	*current;
	t_list	*next;

	current = *gc_head;
	while (current)
	{
		next = current->next;
		if (current->content)
			free(current->content);
		free(current);
		current = next;
	}
	*gc_head = NULL;
}
