/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstindex.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/11 11:49:25 by tibras            #+#    #+#             */
/*   Updated: 2025/12/20 18:05:57 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_lstindex(t_list **lst)
{
	int		i;
	t_list	*current;

	i = 0;
	current = *lst;
	while (current)
	{
		current->index = i;
		i++;
		current = current->next;
	}
}
