/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   push_swap.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 16:21:17 by tibras            #+#    #+#             */
/*   Updated: 2025/12/05 20:28:00 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PUSH_SWAP_H
# define PUSH_SWAP_H

# include "libft.h"
# include "utils/nodes.h"
# include "utils/errors.h"
# include "srcs/push_swap_srcs.h"
# include "utils/push_swap_utils.h"

////////// MACRO //////// 
# define SPACE " \t\n\r\f\v"

////////// PARSING.C //////// 
int	ft_init_str(t_list **stack, char *str);

int	ft_parsing(t_list **stack, int argc, char **argv);

#endif 