/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstat_index.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/11 17:01:42 by tibras            #+#    #+#             */
/*   Updated: 2025/12/20 18:06:07 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

t_list	*ft_lstat_index(t_list *lst, int index)
{
	t_list	*current;

	current = lst;
	while (current)
	{
		if (current->index == index)
			return (current);
		if (current->index > index)
			return (NULL);
		current = current->next;
	}
	return (NULL);
}
