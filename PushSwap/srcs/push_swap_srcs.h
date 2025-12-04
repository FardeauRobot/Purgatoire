/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   push_swap_srcs.h                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/03 12:39:39 by tibras            #+#    #+#             */
/*   Updated: 2025/12/04 17:14:30 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PUSH_SWAP_SRCS_H
# define PUSH_SWAP_SRCS_H

#include "../push_swap.h"


////////// ROTATE.C //////// 
int ft_rotate(t_list **lst);
int	ft_ra(t_list **lst, int check);
int	ft_rb(t_list **lst, int check);
int ft_rr(t_list **la, t_list **lb, int check);

////////// REVERSE_ROTATE.C //////// 
int ft_reverse_rotate(t_list **lst);
int	ft_rra(t_list **lst, int check);
int	ft_rrb(t_list **lst, int check);
int ft_rrr(t_list **la, t_list **lb, int check);

#endif

