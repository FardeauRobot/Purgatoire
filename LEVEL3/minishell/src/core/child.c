/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   child.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamjada <alamjada@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/13 18:31:47 by alamjada          #+#    #+#             */
/*   Updated: 2026/02/28 13:13:29 by alamjada         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	ft_exec_built_in(t_cmd *cmd, t_minishell *minishell)
{
	run_built_in(cmd, minishell);
	ft_exit(minishell, SUCCESS, NULL);
}

static void	ft_exec_not_xok(t_cmd *cmd, t_minishell *minishell)
{
	if (errno == EACCES)
	{
		if (ft_strchr(cmd->path, '/'))
			ft_exit_cmd(minishell, PERMISSION_DENIED, cmd->args[0],
				ERR_PERM_DENIED);
		else
			ft_exit_cmd(minishell, CMD_NOT_FOUND, cmd->args[0],
				ERR_CMD_NOT_FOUND);
	}
	else if (!ft_strchr(cmd->path, '/'))
		ft_exit_cmd(minishell, CMD_NOT_FOUND, cmd->args[0],
			ERR_CMD_NOT_FOUND);
	else
		ft_exit_cmd(minishell, CMD_NOT_FOUND, cmd->args[0],
			ERR_NO_FILE_OR_DIR);
}

static void	close_pipe_and_exec(t_cmd *cmd, t_minishell *minishell,
	int pipe_fd[2])
{
	struct stat	path_stat;

	ft_fds_close(pipe_fd[0], pipe_fd[1]);
	if (cmd->outfd == -1 || cmd->infd == -1 || cmd->error_file == 1)
		ft_exit(minishell, GENERAL_ERROR, NULL);
	if (is_built_in(cmd))
		ft_exec_built_in(cmd, minishell);
	if (!cmd->path)
		ft_exit_cmd(minishell, CMD_NOT_FOUND, cmd->args[0],
			ERR_CMD_NOT_FOUND);
	if (access(cmd->path, X_OK) == -1)
		ft_exec_not_xok(cmd, minishell);
	if (stat(cmd->path, &path_stat) == 0 && S_ISDIR(path_stat.st_mode))
	{
		if (ft_strchr(cmd->path, '/'))
			ft_exit_cmd(minishell, IS_DIRECTORY, cmd->args[0], ERR_IS_DIR);
		else
			ft_exit_cmd(minishell, CMD_NOT_FOUND, cmd->args[0],
				ERR_CMD_NOT_FOUND);
	}
	if (execve(cmd->path, cmd->args, cmd->envp) == -1)
		ft_exit(minishell, errno, strerror(errno));
}

void	child_process(t_minishell *minishell, t_cmd *cmd, t_child *child,
		int pipe_fd[2])
{
	handler_signal_child();
	if (child->index == 0)
		handler_first_cmd(cmd->infd, cmd->outfd, child->size_cmd, pipe_fd[1]);
	else if (child->index == child->size_cmd - 1)
		handler_last_cmd(cmd->infd, child->prev_pipe, cmd->outfd);
	else
		handler_mid_cmd(cmd->infd, child->prev_pipe, cmd->outfd, pipe_fd[1]);
	if (cmd->infd > 2)
		cmd->infd = STDIN_FILENO;
	if (cmd->outfd > 2)
		cmd->outfd = STDOUT_FILENO;
	close_pipe_and_exec(cmd, minishell, pipe_fd);
}
