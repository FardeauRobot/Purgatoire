/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/15 18:00:00 by fardeau           #+#    #+#             */
/*   Updated: 2026/02/15 19:02:14 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/pipex.h"

int	ft_error_print(char *str)
{
	ft_putstr_fd(str, STDERR_FILENO);
	return (1);
}

void	ft_pipex_exit(t_pipex *pipex, int error, char *str)
{
	if (str)
		ft_error_print(str);
	ft_gc_free_all(&pipex->gc);
	exit(error);
}

void	ft_child_exit(t_pipex *pipex, int error)
{
	ft_gc_free_all(&pipex->gc);
	exit(error);
}
