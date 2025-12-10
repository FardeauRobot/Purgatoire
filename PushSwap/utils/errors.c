/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/04 18:49:23 by tibras            #+#    #+#             */
/*   Updated: 2025/12/10 12:15:39 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap_utils.h"



int	ft_err_print(int error)
{

	if (error == -1)
		ft_putstr_fd(ERR_ARGS, 2);
	if (error == 1)		
		ft_putstr_fd(ERR_PARS, 2);
	return (error);
}