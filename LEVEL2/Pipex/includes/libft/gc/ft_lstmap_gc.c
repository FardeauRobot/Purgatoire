/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstmap_gc.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 10:00:00 by tibras            #+#    #+#             */
/*   Updated: 2026/01/27 09:50:30 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

/*
** Map function that builds a new list by applying f to each content of lst.
*/
t_list	*ft_lstmap_gc(t_list *lst, void *(*f)(void *), void (*del)(void *), \
	t_list **gc_head)
{
	t_list	*head;
	t_list	*tmp;

	if (!lst || !f)
		return (NULL);
	(void)del;
	head = NULL;
	while (lst)
	{
		tmp = ft_lstnew_gc(f(lst->content), gc_head);
		if (!tmp)
			return (NULL);
		if (!head)
			head = tmp;
		else
			ft_lstadd_back(&head, tmp);
		lst = lst->next;
	}
	return (head);
}
