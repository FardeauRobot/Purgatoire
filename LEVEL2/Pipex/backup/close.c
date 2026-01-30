/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   close.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/29 14:13:13 by tibras            #+#    #+#             */
/*   Updated: 2026/01/29 20:34:47 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex_srcs.h"

void	ft_close_files(t_pipex *pipex)
{
	if (pipex->infile_fd != -1)
		close(pipex->infile_fd);
	if (pipex->outfile_fd != -1)
		close(pipex->outfile_fd);
}

void	ft_close_pipes(t_pipex *pipex)
{
	int	i;

	i = 0;
	while (i < pipex->cmd_nbr - 1)
	{
		if (pipex->pipe_arr[i][READ] != - 1)
			close(pipex->pipe_arr[i][READ]);
		if (pipex->pipe_arr[i][WRITE] != - 1)
			close(pipex->pipe_arr[i][WRITE]);
		i++;
	}
}
