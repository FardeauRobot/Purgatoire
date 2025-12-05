/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/04 18:49:23 by tibras            #+#    #+#             */
/*   Updated: 2025/12/05 15:47:49 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap_utils.h"

int	ft_err_print(int error)
{

	if (error == -1)
		write(2, ERR_ARGS, ft_strlen(ERR_ARGS));
	if (error == 1)		
		write(2, ERR_PARS, ft_strlen(ERR_PARS));
	return (error);
}