/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   push_swap_srcs.h                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/03 12:39:39 by tibras            #+#    #+#             */
/*   Updated: 2025/12/15 13:48:26 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PUSH_SWAP_SRCS_H
# define PUSH_SWAP_SRCS_H

# include "../push_swap.h"

////////// PARSING.C //////// 
int		ft_init_str(t_list **stack, char *str);
int		ft_no_double(t_list *stack);
int	ft_is_sorted(t_list **lst);
int		ft_parsing(t_list **stack, int argc, char **argv);
//----------------------------//

////////// SORTING.C //////// 
void	ft_move(t_list *to_place, t_list **stack_a, t_list **stack_b);
int	ft_sorting(t_list **la, t_list **lb);
//----------------------------//

t_list	*ft_find_smallest(t_list *stack_a);
////////// INIT.C //////// 
void	ft_init(t_list **stack_a, t_list **stack_b);
//----------------------------//


#endif
