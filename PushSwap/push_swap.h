/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   push_swap.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 16:21:17 by tibras            #+#    #+#             */
/*   Updated: 2025/12/02 17:35:58 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PUSH_SWAP_H
# define PUSH_SWAP_H

# include "libft.h"

////////// SUB .H //////// 
# include "utils/push_swap_utils.h"
# include "srcs/push_swap_srcs.h"

////////// STRUCTURES //////// 
typedef struct s_list {
	int					value;
	int					cost;
	int					index;
	struct s_list		*next;
	struct s_list		*previous;
}	t_list;

////////// PARSING.C //////// 
int	ft_init_str(s_list **stack, char *str),
int ft_parsing(s_list **stack, int argc, char **argv);

#endif 