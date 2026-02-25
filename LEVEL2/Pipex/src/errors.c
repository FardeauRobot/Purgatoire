/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/15 18:00:00 by fardeau           #+#    #+#             */
/*   Updated: 2026/02/23 10:48:37 by tibras           ###   ########.fr       */
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

// FERME UN FD SI VALIDE
void	ft_fd_close(int *fd)
{
	if (*fd > 2)
	{
		close(*fd);
		*fd = -1;
	}
}

// FERME TOUS LES FD OUVERTS
void	ft_fds_close_all(t_pipex *pipex)
{
	ft_fd_close(&pipex->pipefd[0]);
	ft_fd_close(&pipex->pipefd[1]);
	ft_fd_close(&pipex->cmd1.infd);
	ft_fd_close(&pipex->cmd2.outfd);
}
