/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstnew_gc.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 10:00:00 by tibras            #+#    #+#             */
/*   Updated: 2026/01/27 09:50:29 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

t_list	*ft_lstnew_gc(void *content, t_list **gc_head)
{
	t_list	*element;

	element = ft_calloc_gc(1, sizeof(t_list), gc_head);
	if (!element)
		return (NULL);
	element->content = content;
	element->next = NULL;
	element->previous = NULL;
	return (element);
}
