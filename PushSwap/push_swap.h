/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   push_swap.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 16:21:17 by tibras            #+#    #+#             */
/*   Updated: 2025/11/29 18:00:18 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PUSH_SWAP_H
# define PUSH_SWAP_H

#include "libft.h"

////////// SUB .H //////// 
# include "utils/push_swap_utils.h"
# include "srcs/push_swap_srcs.h"

////////// STRUCTURES //////// 
typedef struct s_node {
	int					value;
	struct s_node		*next;
	struct s_node		*previous;
}	t_node;

////////// PARSING.C //////// 

#endif 