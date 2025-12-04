/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   push_swap_utils.h                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 17:25:49 by tibras            #+#    #+#             */
/*   Updated: 2025/12/04 12:48:46 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PUSH_SWAP_UTILS_H
# define PUSH_SWAP_UTILS_H

# include "../push_swap.h"

////////// PARSING_UTILS.C //////// 
t_node	*ft_node_init(int nb);

////////// PARSING_UTILS.C //////// 

////////// FREE_UTILS.C //////// 
int		ft_full_free(char **arr, t_list **stack, void (*del)(void *));
void	ft_node_del(void *content);

////////// PARSING.C //////// 
int		ft_parsing(t_list **stack, int argc, char **argv);

#endif
