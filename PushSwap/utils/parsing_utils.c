/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 18:30:19 by tibras            #+#    #+#             */
/*   Updated: 2025/12/02 17:02:39 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap_utils.h"

int	ft_full_free(char **arr, s_list **stack)
{
	ft_free_array(arr);
	ft_lstclear(stack);
	return (-1);
}