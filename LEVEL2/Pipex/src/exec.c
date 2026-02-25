/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/15 18:00:00 by fardeau           #+#    #+#             */
/*   Updated: 2026/02/23 12:15:58 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/pipex.h"

// REDIRIGE LES FD DU CHILD EN FONCTION DE SA POSITION
static void	ft_child_redirect(t_pipex *pipex, t_position pos)
{
	int	in_fd;
	int	out_fd;

	if (pos == CHILD_FIRST)
	{
		in_fd = pipex->cmd1.infd;
		out_fd = pipex->pipefd[1];
	}
	else
	{
		in_fd = pipex->pipefd[0];
		out_fd = pipex->cmd2.outfd;
	}
	if (in_fd < 0 || out_fd < 0)
	{
		ft_fds_close_all(pipex);
		ft_gc_free_all(&pipex->gc);
		exit(STANDARD_ERROR);
	}
	dup2(in_fd, STDIN_FILENO);
	dup2(out_fd, STDOUT_FILENO);
	ft_fds_close_all(pipex);
}

// EXECUTE UN CHILD EN FONCTION DE SA POSITION (CHILD_FIRST ou CHILD_SECOND)
static void	ft_child_exec(t_pipex *pipex, t_position pos)
{
	t_cmd	*cmd;

	if (pos == CHILD_FIRST)
		cmd = &pipex->cmd1;
	else
		cmd = &pipex->cmd2;
	ft_child_redirect(pipex, pos);
	if (!cmd->path)
	{
		ft_gc_free_all(&pipex->gc);
		exit(CMD_NOT_FOUND);
	}
	execve(cmd->path, cmd->args, pipex->envp);
	perror(cmd->args[0]);
	ft_gc_free_all(&pipex->gc);
	if (errno == EACCES)
		exit(PERM_DENIED);
	exit(CMD_NOT_FOUND);
}

static void	ft_child_wait(int pid2, int *status)
{
	int		reaped;
	int		tmp_status;
	pid_t	wpid;

	reaped = 0;
	while (reaped < 2)
	{
		wpid = waitpid(-1, &tmp_status, 0);
		if (wpid == pid2)
			*status = tmp_status;
		reaped++;
	}
}

// LANCE LE PIPELINE : pipe + 2 forks (meme logique que ft_exec mais avec pipe)
int	ft_pipeline_exec(t_pipex *pipex)
{
	pid_t	pid1;
	pid_t	pid2;
	int		status;

	if (pipe(pipex->pipefd) == -1)
	{
		perror("pipe");
		return (1);
	}
	pid1 = fork();
	if (pid1 == -1)
		return (perror("fork"), 1);
	if (pid1 == 0)
		ft_child_exec(pipex, CHILD_FIRST);
	pid2 = fork();
	if (pid2 == -1)
		return (perror("fork"), 1);
	if (pid2 == 0)
		ft_child_exec(pipex, CHILD_SECOND);
	ft_fds_close_all(pipex);
	ft_child_wait(pid2, &status);
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	return (1);
}
