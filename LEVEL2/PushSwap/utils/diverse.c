/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   diverse.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/04 17:29:13 by tibras            #+#    #+#             */
/*   Updated: 2025/12/20 17:07:25 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap_utils.h"

t_node	*ft_get_content(t_list *lst)
{
	if (lst)
		return (lst->content);
	return (NULL);
}

t_list	*ft_get_target(t_node *node)
{
	if (node)
		return (node->target);
	return (NULL);
}

t_node	*ft_get_node(t_list *lst)
{
	if (lst)
		return (lst->content);
	return (NULL);
}
