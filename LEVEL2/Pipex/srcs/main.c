/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/20 13:36:13 by tibras            #+#    #+#             */
/*   Updated: 2026/01/22 16:10:08 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex_srcs.h"

// char *ft_find_path(char *cmd, char **envp)
// {
// 	char *path;
// 	char **path_arr;

// 	// if (access(path, ))
// 	return (path);
// }

// int ft_strlen(char *str)
// {
// 	size_t i;

// 	i = 0;
// 	while (str[i])
// 		i++;
// 	return (i);
// }

void	ft_putstr_fd(char *str, int fd)
{
	write(fd, str, ft_strlen(str));
}

void ft_print_fd(int in_fd, int out_fd)
{
	if (in_fd > -2)
		printf("in_fd = %d\n", in_fd);
	if (out_fd > -2)
		printf("out_fd = %d\n", out_fd);
}

void	ft_close_fd(int in_fd, int out_fd)
{
	if (in_fd >= 0)
		close(in_fd);
	if (out_fd >= 0) 
		close(out_fd);
}

void	ft_init_arr_pipe(char **argv, t_pipe *arr_pipe, int size, char **envp)
{
	int i;
	
	i = 0;
	(void)envp;
	// (void)argv;
	// (void)arr_pipe;
	while (i < size)
	{
		arr_pipe[i].arg_pipe = ft_split_charset(argv[i + 2], SPACE);
		// arr_pipe[i].cmd = NULL;
		// arr_pipe[i].path = NULL;
		// arr_pipe[i].arg_pipe = ft_split_charset(argv[i + 2], SPACE);
		i++;
	}
}

int main (int argc, char **argv, char **envp)
{
	t_data pipex;

	// INITIALISATION
	pipex.arr_pipe = NULL;
	pipex.count_fctn = argc - 3;
	// printf("%d\n", pipex.count_fctn);
	pipex.arr_pipe = ft_calloc(sizeof(t_pipe *) ,pipex.count_fctn);
	if (!pipex.arr_pipe)
		return (-1);
	int i = 0;
	while (i < pipex.count_fctn)
	{
		pipex.arr_pipe[i] = ft_calloc(sizeof(t_pipe), 1);
		pipex.arr_pipe[i]->arg_pipe = ft_split_charset(argv[i + 2], SPACE);
		if (pipex.arr_pipe[i]->arg_pipe == NULL)
			return (-1);
		pipex.arr_pipe[i]->cmd = pipex.arr_pipe[i]->arg_pipe[0];
			// ERROR_EXIT PARSING
		i++;
	}
	int j;
	i = 0;
	while (pipex.arr_pipe[i])
	{
		j = 0;
		while (pipex.arr_pipe[i]->arg_pipe[j])
		{
			// ft_printf("%s\n", pipex.arr_pipe[i]->arg_pipe[j]);
			j++;
		}
		i++;
	}

	// ON A RECUPERE LES ARGUMENTS
	// ON PASSE AU TEST POUR VOIR SI CA FONCTIONNE
	i = 0;
	int pid;
	while (i < pipex.count_fctn)
	{
		if (i < pipex.count_fctn - 1)
		{
			if (pipe(pipex.arr_pipe[i]->pipe_fd))
				exit(-1);
		}
		pid = fork();
		if (pid == 0)
		{
			pipex.arr_pipe[i]->path = ft_correct_path(envp, pipex.arr_pipe[i]->cmd);
			if (pipex.arr_pipe[i]->path == NULL)
				return (-1);
			if (i == 0)
			{
				pipex.fd_infile = open(argv[1], O_RDONLY);
				if (pipex.fd_infile < 0)
					exit(-1);

				dup2(pipex.fd_infile, STDIN_FILENO);
				close(pipex.fd_infile);

				dup2(pipex.arr_pipe[i]->pipe_fd[WRITE], STDOUT_FILENO);

				close(pipex.arr_pipe[i]->pipe_fd[READ]);
				close(pipex.arr_pipe[i]->pipe_fd[WRITE]);
			}
			if (i == pipex.count_fctn - 1)
			{
				dup2(pipex.arr_pipe[i - 1]->pipe_fd[READ], STDIN_FILENO);
				close(pipex.arr_pipe[i - 1]->pipe_fd[READ]);
				close(pipex.arr_pipe[i - 1]->pipe_fd[WRITE]);
				pipex.fd_outfile = open(argv[argc - 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
				if (pipex.fd_outfile < 0)
					exit(-1);
				dup2(pipex.fd_outfile, STDOUT_FILENO);
				close(pipex.fd_outfile);
			}
			if (execve(pipex.arr_pipe[i]->path, pipex.arr_pipe[i]->arg_pipe, envp))
				// ERROR_EXIT CHILD
				exit(-1);
		}
		if (i > 0)
		{
			close(pipex.arr_pipe[i - 1]->pipe_fd[READ]);
			close(pipex.arr_pipe[i - 1]->pipe_fd[WRITE]);
		}
		i++;
	}
	i = 0;
	while (i < pipex.count_fctn)
	{
		waitpid(-1, NULL, 0);
		i++;
	}
return (0);
}
