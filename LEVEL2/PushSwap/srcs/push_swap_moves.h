/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   push_swap_moves.h                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/11 12:10:28 by tibras            #+#    #+#             */
/*   Updated: 2025/12/19 15:24:10 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PUSH_SWAP_MOVES_H
# define PUSH_SWAP_MOVES_H

#include "../push_swap.h"

////////// ROTATE.C //////// 
void	ft_rotate(t_list **lst);
void	ft_ra(t_list **lst, int check);
void	ft_rb(t_list **lst, int check);
void	ft_rr(t_list **la, t_list **lb, int check);

////////// REVERSE_ROTATE.C //////// 
void	ft_reverse_rotate(t_list **lst);
void	ft_rra(t_list **lst, int check);
void	ft_rrb(t_list **lst, int check);
void	ft_rrr(t_list **la, t_list **lb, int check);

////////// PUSH.C //////// 
void	ft_push(t_list **stack_src, t_list **stack_dest);
void	ft_pa(t_list **stack_b, t_list **stack_a, int check);
void	ft_pb(t_list **stack_a, t_list **stack_b, int check);

#endif