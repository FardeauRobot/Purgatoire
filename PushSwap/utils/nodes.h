/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nodes.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/05 20:26:48 by tibras            #+#    #+#             */
/*   Updated: 2025/12/11 13:48:09 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef NODES_H
# define NODES_H

////////// STRUCTURES //////// 
typedef struct s_node {
	int					value;
	int					index;
	int					to_top;
	int					above;
	struct s_node		*target;
}	t_node;

#endif