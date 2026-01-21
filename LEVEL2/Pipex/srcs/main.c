/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/20 13:36:13 by tibras            #+#    #+#             */
/*   Updated: 2026/01/21 18:46:28 by tibras           ###   ########.fr       */
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
	while (i < size)
	{

		arr_pipe[i].arg_pipe = NULL;
		arr_pipe[i].cmd = NULL;
		arr_pipe[i].path = NULL;
		arr_pipe[i].arg_pipe = ft_split_charset(argv[i + 2], SPACE);
		// if (!arr_pipe[i]->arg_pipe)
		// {
		// }
		i++;
	}
}

int main (int argc, char **argv, char **envp)
{
	t_data	pipex;

	pipex.count_fctn = argc - 3;
	pipex.arr_pipe = ft_calloc(sizeof(t_pipe),pipex.count_fctn);
	if (!pipex.arr_pipe)
		exit(1);
	ft_init_arr_pipe(argv, *pipex.arr_pipe, pipex.count_fctn, envp);
	ft_free_arr_pipe(pipex.arr_pipe, pipex.count_fctn);
}


// buf[600];
// bzero
// buf = cat (split[i], buf);
// buf = cat ('/', buf);
// buf = (cmd, buf);
// if (access)
// 	return (path);
