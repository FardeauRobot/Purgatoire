/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   push_swap_utils.h                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 17:25:49 by tibras            #+#    #+#             */
/*   Updated: 2025/12/03 15:56:12 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PUSH_SWAP_UTILS_H
# define PUSH_SWAP_UTILS_H

#include "../push_swap.h"

////////// PARSING.C //////// 
int ft_parsing(t_list **stack, int argc, char **argv);

////////// PARSING_UTILS.C //////// 
static t_node	*ft_node_init(int nb)

////////// PARSING_UTILS.C //////// 

////////// FREE_UTILS.C //////// 
static void	ft_node_del(void *content)

#endif
