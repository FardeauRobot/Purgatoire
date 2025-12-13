/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   push_swap_utils.h                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 17:25:49 by tibras            #+#    #+#             */
/*   Updated: 2025/12/13 18:47:44 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PUSH_SWAP_UTILS_H
# define PUSH_SWAP_UTILS_H

# include "../push_swap.h"

/*
	FICHIERS UTILITAIRES
	Errors_utils.c = Gestion erreurs
	Free_utils.c = Gestion malloc
	Node_utils.c = Fonction sur noeuds
	Parsing_utils.c = Fonctions complémentaires pour le parsing
*/

////////// ERROR_UTILS.C //////// 
int	ft_error_parse(char **arr, t_list **stack);
int	ft_error_stacks(t_list **s_a, t_list **s_b);
//----------------------------//

////////// FREE_UTILS.C //////// 
void	ft_clear_all(t_list **s_a, t_list **s_b);
//----------------------------//

////////// NODE_UTILS.C //////// 
t_node	*ft_node_min(t_list *lst);
t_node	*ft_lstmax(t_list *lst);
t_node	*ft_node_init(long nb);
//----------------------------//

////////// PARSING_UTILS.C //////// 
int		ft_is_number(char *nbr);
int		ft_free_parse(char **arr, t_list **stack);
//----------------------------//

////////// DIVERSE.C //////// 
t_node	*ft_get_content(t_list *lst);
t_node	*ft_get_node(t_list *lst);
t_list	*ft_get_target(t_node *node);
void	ft_lstprint_nbr(t_list *lst);
void	ft_lstprint_both(t_list *la, t_list *lb);
//----------------------------//

#endif
