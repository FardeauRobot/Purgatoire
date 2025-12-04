/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/04 18:49:23 by tibras            #+#    #+#             */
/*   Updated: 2025/12/04 18:58:07 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap_utils.h"

int	ft_err_print(int error)
{

	if (error == -1)
		write(2, ERRORARGS, ft_strlen(ERRORARGS));
	if (error == 1)		
		write(2, ERRORPARS, ft_strlen(ERRORPARS));
	return (error);
}