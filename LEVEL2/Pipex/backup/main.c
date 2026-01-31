/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/20 13:36:13 by tibras            #+#    #+#             */
/*   Updated: 2026/01/30 12:33:08 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex_srcs.h"

void	ft_init_pipex(t_pipex *pipex, char **envp, char **argv, int argc)
{
	ft_default_pipex(pipex, envp, argc, argv);
	ft_find_path_env(pipex);
	ft_load_cmds(pipex, argv);
}

void	ft_exec_child(t_pipex *pipex, int i)
{
	if (i == 0)
		dup2(pipex->infile_fd, STDIN_FILENO);
	else
		dup2(pipex->pipe_arr[i - 1][READ], STDIN_FILENO);
	if (i == pipex->cmd_nbr - 1)
		dup2(pipex->outfile_fd, STDOUT_FILENO);
	else
		dup2(pipex->pipe_arr[i][WRITE], STDOUT_FILENO);
	ft_close_pipes(pipex);
	ft_close_files(pipex);
	if (!pipex->cmds[i]->path || access(pipex->cmds[i]->path, F_OK | X_OK))
		error_exit(pipex, ERR_CMD_NOT_FOUND, "Command not found\n");
	execve(pipex->cmds[i]->path, pipex->cmds[i]->args, pipex->envs);
	error_exit(pipex, ERR_EXEC, "Cmd not executed properly\n");
}

void	ft_open_pipes(t_pipex *pipex)
{
	int	i;

	i = 0;
	while (i < pipex->cmd_nbr - 1)
	{
		if (pipe(pipex->pipe_arr[i]) == -1)
			error_exit(pipex, ERR_PIPE, "Error while creating pipe\n");
		i++;
	}
}

void	ft_exec_pipes(t_pipex *pipex)
{
	int	i;

	ft_open_pipes(pipex);
	i = 0;
	while (i < pipex->cmd_nbr - 1)
	{
		pipex->pid_arr[i] = fork();
		if (pipex->pid_arr[i] == -1)
			error_exit(pipex, ERR_PIPE, "Fork failed\n");
		if (pipex->pid_arr[i] == 0)
			ft_exec_child(pipex, i);
		i++;
	}
	ft_close_pipes(pipex);
	ft_close_files(pipex);
	i = 0;
	while (i++ < pipex->cmd_nbr)
		waitpid(pipex->pid_arr[i - 1], &pipex->exit_code, 0);
	if (WIFEXITED(pipex->exit_code))
		pipex->exit_code = WEXITSTATUS(pipex->exit_code);
	else if (WIFSIGNALED(pipex->exit_code))
		pipex->exit_code = ERR_SIGKILL + WTERMSIG(pipex->exit_code);
}

int	main(int argc, char **argv, char **envp)
{
	t_pipex	pipex;

	if (argc < 5)
		return (ft_putstr_fd("Usage: ./pipex file1 cmd1 cmd2 file2\n", 2), 1);
	pipex.envs = envp;
	ft_init_pipex(&pipex, envp, argv, argc);
	ft_exec_pipes(&pipex);
	error_exit(&pipex, pipex.exit_code, NULL);
}
