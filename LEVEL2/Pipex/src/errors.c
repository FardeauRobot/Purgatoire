/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/15 16:23:57 by fardeau           #+#    #+#             */
/*   Updated: 2026/02/15 16:57:40 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

int ft_error(int error, char *error_str)
{
	if (error_str)
		ft_putstr_fd(error_str, STDERR_FILENO);
	return (error);
}

int	ft_exit(t_pipex *pipex, int error, char *error_str)
{
	if (error_str)
		ft_putstr_fd(error_str, STDERR_FILENO);
	if (pipex->cmds[0].infd > 2)
		close(pipex->cmds[0].infd);
	if (pipex->cmds[1].outfd > 2)
		close(pipex->cmds[1].outfd);
	ft_gc_free_all(&pipex->gc);
	exit(error);
}