/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstmap.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/12 15:54:50 by tibras            #+#    #+#             */
/*   Updated: 2025/11/18 13:52:51 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

t_list	*ft_lstmap(t_list *lst, void *(*f)(void *), void (*del)(void *))
{
	t_list	*head;
	t_list	*tmp;
	t_list	*current;
	void	*new_content;

	if (!f)
		return (NULL);
	head = NULL;
	current = lst;
	while (current)
	{
		new_content = f(current->content);
		tmp = ft_lstnew(new_content);
		if (!tmp)
		{
			if (del)
				del(new_content);
			if (del)
				ft_lstclear(&head, del);
			return (NULL);
		}
		ft_lstadd_back(&head, tmp);
		current = current->next;
	}
	return (head);
}
