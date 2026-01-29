/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/20 13:36:13 by tibras            #+#    #+#             */
/*   Updated: 2026/01/28 17:21:53 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex_srcs.h"

// CLOSE.C
void	ft_close_files(t_pipex *pipex)
{
	if (pipex->infile_fd != -1)
		close(pipex->infile_fd);
	if (pipex->outfile_fd != -1)
		close(pipex->outfile_fd);
}

void	ft_close_pipes(int *pipe)
{
	if (pipe[READ] != -1)
		close(pipe[READ]);
	if (pipe[WRITE] != -1)
		close(pipe[WRITE]);
}

//////////////////////////////////////

// INIT.C
// Affect the pipex.paths to splitter envp of PATH=
void	ft_find_path_env(t_pipex *pipex)
{
	char *path;
	size_t i;

	i = 0;
	while (pipex->envs[i++])
	{
		if (ft_strncmp(pipex->envs[i - 1], "PATH=", 5) == 0)
		{
			path = pipex->envs[i - 1] + 5;
			break;
		}
	}
	pipex->paths = ft_split_sep_gc(path, ':', &pipex->gc);
	if (!pipex->paths)
		error_exit(pipex, 1, "Error, no PATH found\n");
}

// Set default values to pipex structs
void	ft_default_pipex(t_pipex *pipex, char **envp, int argc, char **argv)
{
	pipex->gc = NULL;
	pipex->cmds = NULL;
	pipex->envs = envp;
	pipex->paths = NULL;
	pipex->infile = argv[1];
	pipex->outfile = argv[argc - 1];
	pipex->exit_code = 0;
	if (ft_strcmp(pipex->infile, "here_doc"))
	{
		pipex->is_heredoc = 0;
		pipex->cmd_nbr = argc - 3;
	}
	else
	{
		pipex->is_heredoc = 1;
		pipex->limiter = argv[2];
		pipex->cmd_nbr = argc - 4;
	}
	pipex->pid_arr = ft_calloc_gc(pipex->cmd_nbr, sizeof(pid_t),   &pipex->gc);
	if (!pipex->pid_arr)
		error_exit(pipex, ERR_MALLOC, "Error with malloc pid_t arr\n");
	pipex->pipe_arr = ft_calloc_gc(pipex->cmd_nbr - 1, sizeof(int *), &pipex->gc);
	if (!pipex->pipe_arr)
		error_exit(pipex, ERR_MALLOC, "Error with malloc pipe arr\n");
	int i = 0;
	while (i++ < pipex->cmd_nbr - 1)	
		pipex->pipe_arr[i - 1] = ft_calloc_gc(2, sizeof(int), &pipex->gc);
	if (!pipex->is_heredoc)
	{
		pipex->infile_fd = open(pipex->infile, O_RDONLY);
		if (pipex->infile_fd == -1)
			perror(pipex->infile);
	}
	pipex->outfile_fd = open(pipex->outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (pipex->outfile_fd == -1)
		perror(pipex->outfile);
}

char	*ft_affect_path(t_pipex *pipex, char *cmd)
{
	int	i;
	char *full_cmd;

	i = 0;
	if (!access(cmd, F_OK | X_OK))
		return (cmd);
	while (pipex->paths[i])
	{
		full_cmd = ft_strdup_gc(pipex->paths[i], &pipex->gc);
		full_cmd = ft_strjoin_gc(full_cmd, "/", &pipex->gc);
		full_cmd = ft_strjoin_gc(full_cmd, cmd, &pipex->gc);
		if (!access(full_cmd, F_OK | X_OK))
			return (full_cmd);
		i++;
	}
	return (NULL);
}

// Allocate and fills the cmd struct
t_cmd	*ft_create_cmd_struct(char *argv, t_pipex *pipex)
{
	t_cmd	*new_cmd;

	new_cmd = ft_calloc_gc(1, sizeof(t_cmd), &pipex->gc);
	if (!new_cmd)
		error_exit(pipex, ERR_MALLOC, "Error with malloc struct when intializing cmd\n");
	new_cmd->args = ft_split_charset_gc(argv, SPACE, &pipex->gc);
	if (!new_cmd->args)
		error_exit(pipex, ERR_MALLOC, "Error with split args when intializing cmd\n");
	new_cmd->path = ft_affect_path(pipex, new_cmd->args[0]);
	return (new_cmd);
}

// Loads the cmd structs in the pipex struct
void	ft_load_cmds(t_pipex *pipex, char **argv)
{
	int	i;

	i = 0;
	pipex->cmds = ft_calloc_gc(pipex->cmd_nbr + 1, sizeof(t_cmd *), &pipex->gc);
	if (!pipex->cmds)
		error_exit(pipex, ERR_MALLOC, "Error with malloc when intializing **cmds\n");
	while (i < pipex->cmd_nbr)
	{
		if (pipex->is_heredoc)
			pipex->cmds[i] = ft_create_cmd_struct(argv[3 + i], pipex);
		else
			pipex->cmds[i] = ft_create_cmd_struct(argv[2 + i], pipex);
		i++;
	}
	pipex->cmds[i] = NULL;
}

// Initialize the different variables, and fills in necessary for execution
void	ft_init_pipex(t_pipex *pipex, char **envp, char **argv, int argc)
{
	ft_default_pipex(pipex, envp, argc, argv);
	ft_find_path_env(pipex);
	ft_load_cmds(pipex, argv);
}

void	ft_exec_child(t_pipex *pipex, int i)
{
	if (i == 0)
	{
		if (pipex->infile_fd == -1)
			error_exit(pipex, 1, "No such file or directory\n");
		dup2(pipex->infile_fd, STDIN_FILENO);
		close(pipex->infile_fd);
	}
	else
		dup2(pipex->pipe_arr[i - 1][READ], STDIN_FILENO);
	if (i == pipex->cmd_nbr - 1)
	{
		if (pipex->outfile_fd == -1)
			error_exit(pipex, 1, "No such file or directory\n");
		dup2(pipex->outfile_fd, STDOUT_FILENO);
		close(pipex->outfile_fd);
	}
	else
		dup2(pipex->pipe_arr[i][WRITE], STDOUT_FILENO);
	// ft_close_files(pipex);
}

void	ft_open_pipes(t_pipex *pipex)
{
	int i;

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
	int i;

	ft_open_pipes(pipex);
	i = 0;
	while (i < pipex->cmd_nbr)
	{
		pipex->pid_arr[i] = fork();
		if (pipex->pid_arr[i] == 0)
		{
			ft_exec_child(pipex, i);
			if (access(pipex->cmds[i]->path, F_OK) || !pipex->cmds[i]->path)
				error_exit(pipex, ERR_EXEC, "Command not found\n");
			execve(pipex->cmds[i]->path, pipex->cmds[i]->args, pipex->envs);
			error_exit(pipex, ERR_EXEC, "Cmd not executed properly \n");
		}
		i++;
	}
	ft_close_files(pipex);
	i = 0;
	while (i++ < pipex->cmd_nbr - 1)
		ft_close_pipes(pipex->pipe_arr[i - 1]);
	i = 0;
	while (i++ < pipex->cmd_nbr)
	{
		waitpid(0, &pipex->exit_code, 0);
		if (WIFEXITED(pipex->exit_code))
			pipex->exit_code = WEXITSTATUS(pipex->exit_code);
		else if (WIFSIGNALED(pipex->exit_code))
			pipex->exit_code = 128 + WTERMSIG(pipex->exit_code);
		i++;
	}
}

// MAIN
int main (int argc, char **argv, char **envp)
{
	t_pipex pipex;
	
	// if (argc != 5)
	// 	return (1);
	pipex.envs = envp;
	ft_init_pipex(&pipex, envp, argv, argc);
	ft_exec_pipes(&pipex);
	error_exit(&pipex, pipex.exit_code, NULL);
}
