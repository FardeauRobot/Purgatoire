/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   push_swap_utils.h                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 17:25:49 by tibras            #+#    #+#             */
/*   Updated: 2025/12/11 14:36:00 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PUSH_SWAP_UTILS_H
# define PUSH_SWAP_UTILS_H

# include "../push_swap.h"

////////// NODE_UTILS.C //////// 
void	ft_lstprint_nbr(t_list *lst);
t_node	*ft_lstmin(t_list *lst);
t_node	*ft_lstmax(t_list *lst);
t_node	*ft_node_init(long nb);

////////// FREE_UTILS.C //////// 
void	ft_clear_all(t_list **s_a, t_list **s_b);
void	ft_node_del(void *content);

////////// PARSING.C //////// 
int	ft_init_str(t_list **stack, char *str);
int	ft_free_parse(char **arr, t_list **stack);
int	ft_no_double(t_list *stack);
int	ft_is_sorted(t_list *lst);
int		ft_parsing(t_list **stack, int argc, char **argv);

////////// ERRORS.C //////// 
int		ft_print_error(void);
int	ft_error(t_list **s_a, t_list **s_b);

////////// PARSING_UTILS.C //////// 
int		ft_is_number(char *nbr);

#endif
