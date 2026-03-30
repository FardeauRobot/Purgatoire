/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamjada <alamjada@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/25 20:03:04 by salman            #+#    #+#             */
/*   Updated: 2026/02/28 13:10:50 by alamjada         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEBUG_H
# define DEBUG_H

# include "struct.h"

// UTILS/DEBUG.C
void	ft_cmd_print(t_cmd *head);
void	ft_tokens_print(t_token *head);
void	ft_type_print(t_token *token);

#endif