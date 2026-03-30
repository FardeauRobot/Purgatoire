/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamjada <alamjada@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 17:17:28 by tibras            #+#    #+#             */
/*   Updated: 2026/02/28 13:13:43 by alamjada         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	parent_process(int *prev_pipe, int pipe_fd[2])
{
	signal(SIGINT, SIG_IGN);
	close(pipe_fd[1]);
	if (*prev_pipe != -1)
		close(*prev_pipe);
	*prev_pipe = pipe_fd[0];
}

static void	ft_exec_only_built_in(t_minishell *minishell, t_cmd *cmd)
{
	int	saved_stdout;
	int	saved_stdin;

	saved_stdout = dup(STDOUT_FILENO);
	saved_stdin = dup(STDIN_FILENO);
	if (cmd->infd != STDIN_FILENO)
	{
		dup2(cmd->infd, STDIN_FILENO);
		close(cmd->infd);
		cmd->infd = STDIN_FILENO;
	}
	if (cmd->outfd != STDOUT_FILENO)
	{
		dup2(cmd->outfd, STDOUT_FILENO);
		close(cmd->outfd);
		cmd->outfd = STDOUT_FILENO;
	}
	if (ft_strcmp(cmd->args[0], "exit") == 0)
		ft_fds_close(saved_stdin, saved_stdout);
	run_built_in(cmd, minishell);
	dup2(saved_stdout, STDOUT_FILENO);
	dup2(saved_stdin, STDIN_FILENO);
	ft_fds_close(saved_stdin, saved_stdout);
}

static void	ft_ex(t_minishell *minishell, int size_cmd, int pipe_fd[2],
	int *pids)
{
	t_cmd	*cmd;
	t_child	*child;

	child = ft_child_new(minishell, size_cmd);
	if (!child)
		return ;
	cmd = minishell->head_cmd;
	while (++child->index < size_cmd)
	{
		cmd->envp = ft_envp_build(minishell);
		if (is_built_in(cmd) && size_cmd == 1)
			ft_exec_only_built_in(minishell, cmd);
		else if (pipe(pipe_fd) != -1)
		{
			pids[child->index] = fork();
			if (pids[child->index] < 0)
				ft_exit(minishell, errno, strerror(errno));
			if (pids[child->index] == 0)
				child_process(minishell, cmd, child, pipe_fd);
			parent_process(&child->prev_pipe, pipe_fd);
		}
		cmd = (t_cmd *)cmd->node.next;
	}
	if (child->prev_pipe != -1)
		close(child->prev_pipe);
}

void	ft_exec(t_minishell *minishell)
{
	int	*pids;
	int	pipe_fd[2];
	int	size_cmd;

	size_cmd = ft_lstsize((t_list *)(minishell->head_cmd));
	if (!size_cmd)
		return ;
	pids = ft_gc_malloc(sizeof(int) * size_cmd, &minishell->gc_line);
	if (!pids)
		return ;
	ft_ex(minishell, size_cmd, pipe_fd, pids);
	if (is_built_in(minishell->head_cmd) && size_cmd == 1)
		return ;
	ft_wait_subprocess(minishell, size_cmd, pids);
}
