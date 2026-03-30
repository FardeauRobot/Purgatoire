/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handler.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/13 18:43:32 by alamjada          #+#    #+#             */
/*   Updated: 2026/02/19 11:13:34 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	handler_signal_child(void)
{
	if (signal(SIGQUIT, SIG_DFL) == SIG_ERR)
		perror(ERR_SIG_DFL_QUIT);
	if (signal(SIGINT, SIG_DFL) == SIG_ERR)
		perror(ERR_SIG_DFL_INT);
}

void	handler_first_cmd(int infd, int outfd, int size_cmd, int pipe_fd)
{
	if (infd != STDIN_FILENO)
	{
		dup2(infd, STDIN_FILENO);
		close(infd);
	}
	if (outfd != STDOUT_FILENO)
	{
		dup2(outfd, STDOUT_FILENO);
		close(outfd);
	}
	else if (size_cmd > 1)
		dup2(pipe_fd, STDOUT_FILENO);
}

void	handler_last_cmd(int infd, int prev_pipe, int outfd)
{
	if (infd != STDIN_FILENO)
	{
		dup2(infd, STDIN_FILENO);
		close(infd);
	}
	else
		dup2(prev_pipe, STDIN_FILENO);
	close(prev_pipe);
	if (outfd != STDOUT_FILENO)
	{
		dup2(outfd, STDOUT_FILENO);
		close(outfd);
	}
}

void	handler_mid_cmd(int infd, int prev_pipe, int outfd, int pipe_fd)
{
	if (infd != STDIN_FILENO)
	{
		dup2(infd, STDIN_FILENO);
		close(infd);
	}
	else
		dup2(prev_pipe, STDIN_FILENO);
	close(prev_pipe);
	if (outfd != STDOUT_FILENO)
	{
		dup2(outfd, STDOUT_FILENO);
		close(outfd);
	}
	else
		dup2(pipe_fd, STDOUT_FILENO);
}
