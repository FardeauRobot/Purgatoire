/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   push_swap_utils.h                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 17:25:49 by tibras            #+#    #+#             */
/*   Updated: 2025/12/11 14:57:03 by tibras           ###   ########.fr       */
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
	Parsing.c = Fonctions du parsing
*/

////////// DIVERSE.C //////// 
void	ft_lstprint_nbr(t_list *lst);
void	ft_lstprint_both(t_list *la, t_list *lb);
//----------------------------//

////////// ERROR_UTILS.C //////// 
int		ft_error(t_list **s_a, t_list **s_b);
//----------------------------//

////////// FREE_UTILS.C //////// 
void	ft_clear_all(t_list **s_a, t_list **s_b);
//----------------------------//

////////// NODE_UTILS.C //////// 
t_node	*ft_lstmin(t_list *lst);
t_node	*ft_lstmax(t_list *lst);
t_node	*ft_node_init(long nb);
//----------------------------//

////////// PARSING_UTILS.C //////// 
int		ft_is_number(char *nbr);
int		ft_free_parse(char **arr, t_list **stack);
//----------------------------//

////////// PARSING.C //////// 
int		ft_init_str(t_list **stack, char *str);
int		ft_no_double(t_list *stack);
int		ft_is_sorted(t_list *lst);
int		ft_parsing(t_list **stack, int argc, char **argv);
//----------------------------//

#endif
