/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   push_swap_srcs.h                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/03 12:39:39 by tibras            #+#    #+#             */
/*   Updated: 2025/12/20 17:44:02 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PUSH_SWAP_SRCS_H
# define PUSH_SWAP_SRCS_H

# include "../push_swap.h"

////////// LIS.C ////////
int		*ft_lis_init(int len, int mod);
void	ft_compute_lis(int *lis, int *parent, int *arr, int s_len);
int		ft_free_lis(int *lis, int *par, int *s_arr);
void	ft_save_lis(t_list **stack_a, int *lis_len, int **lis);
int		ft_is_in_lis(int *lis, int lis_len, int value);
//----------------------------//

////////// PARSING.C ////////
void	ft_init_str(t_list **stack, char *str);
int		ft_no_double(t_list *stack);
int		ft_is_sorted(t_list **lst);
int		ft_parsing(t_list **stack, int argc, char **argv);
//----------------------------//

////////// SORTING.C ////////

void	ft_affect_target(t_list **stack_a, t_list **stack_b);
t_list	*ft_find_smallest(t_list *stack_a);
void	ft_move(t_list *to_place, t_list **stack_a, t_list **stack_b);
int		ft_sorting(t_list **la, t_list **lb);
//----------------------------//

////////// INIT.C ////////
int		*ft_stack_to_arr(t_list *stack, int size);
void	ft_dispatch(t_list **stack_a, t_list **stack_b, int *lis, int lis_len);
void	ft_init(t_list **stack_a, t_list **stack_b);
//----------------------------//

////////// MOVES.C ////////
void	ft_move(t_list *to_place, t_list **stack_a, t_list **stack_b);
//----------------------------//

////////// COST.C ////////
void	ft_affect_cost(t_list **stack_a, t_list **stack_b);
void	ft_check_above(t_list **stack);
t_list	*ft_find_cheapest(t_list *stack);
void	ft_get_cost(t_list **stack);
void	ft_compute_cost(t_list **stack_b);
//----------------------------//

////////// COST.C ////////
void	ft_mini_sorting(t_list **stack_a, t_list **stack_b);
//----------------------------//
#endif
