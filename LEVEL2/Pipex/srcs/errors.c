/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/23 17:11:41 by tibras            #+#    #+#             */
/*   Updated: 2026/01/28 09:39:19 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex_srcs.h"

void	error_exit(t_pipex *pipex, int error, char *err_msg)
{
	if (err_msg)
		ft_putstr_fd(err_msg, 2);
	ft_gc_free_all(&pipex->gc);
	exit(error);
}