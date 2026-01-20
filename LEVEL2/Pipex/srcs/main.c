/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/20 13:36:13 by tibras            #+#    #+#             */
/*   Updated: 2026/01/20 16:39:43 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex_srcs.h"

char *ft_find_path(char *cmd, char **envp)
{
	char *path;
	char **path_arr;

	return (path);
}

int ft_strlen(char *str)
{
	size_t i;

	i = 0;
	while (str[i])
		i++;
	return (i);
}

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

int main (int argc, char **argv, char **envp)
{
	t_pipe	pipe_test;
	int		in_fd;
	int		out_fd;

	(void)argc;
	(void)argv;
	in_fd = open("tests/infile", O_RDONLY);
	out_fd = open("tests/outfile", O_RDWR | O_CREAT, 0644);
	pipe(pipe_test.pipe_fd);
	int i = 0;
	while (envp[i])
	{
		printf("%s\n", envp[i]);
		i++;
	}
	ft_putstr_fd("COUCOU\n", out_fd);
	ft_print_fd(in_fd, out_fd);
	ft_close_fd(in_fd, out_fd);
}
