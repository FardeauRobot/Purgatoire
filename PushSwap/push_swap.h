/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   push_swap.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 16:21:17 by tibras            #+#    #+#             */
/*   Updated: 2025/12/03 18:16:50 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PUSH_SWAP_H
# define PUSH_SWAP_H

# include "libft.h"

////////// SUB .H //////// 
# include "utils/push_swap_utils.h"
# include "srcs/push_swap_srcs.h"

# define SPACE = " \t\n\r\f\v"

////////// STRUCTURES //////// 
typedef struct s_node {
	int					value;
	int					cost;
	int					index;
}	t_node;

////////// PARSING.C //////// 
int	ft_init_str(t_list **stack, char *str);

int ft_parsing(t_list **stack, int argc, char **argv);


#endif 