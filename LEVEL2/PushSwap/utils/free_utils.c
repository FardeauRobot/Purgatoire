/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/03 15:45:25 by tibras            #+#    #+#             */
/*   Updated: 2025/12/11 14:58:56 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap_utils.h"

void	ft_clear_all(t_list **s_a, t_list **s_b)
{
	if (s_a)
		ft_lstclear(s_a, free);
	if (s_b)
		ft_lstclear(s_b, free);
}
