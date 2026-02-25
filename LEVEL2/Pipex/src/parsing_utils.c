/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/21 18:00:00 by tibras            #+#    #+#             */
/*   Updated: 2026/02/21 17:42:32 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/pipex.h"

void	ft_cmd_not_found(char *arg)
{
	ft_putstr_fd("pipex: command not found: ", STDERR_FILENO);
	ft_putendl_fd(arg, STDERR_FILENO);
}

static void	ft_infile_open(t_pipex *pipex, char *filename)
{
	pipex->cmd1.infd = open(filename, O_RDONLY);
	if (pipex->cmd1.infd == -1)
		perror(filename);
}

static void	ft_outfile_open(t_pipex *pipex, char *filename)
{
	pipex->cmd2.outfd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (pipex->cmd2.outfd == -1)
		perror(filename);
}

void	ft_args_parse(t_pipex *pipex, char **argv)
{
	ft_infile_open(pipex, argv[1]);
	ft_cmd_parse(pipex, &pipex->cmd1, argv[2]);
	ft_cmd_parse(pipex, &pipex->cmd2, argv[3]);
	ft_outfile_open(pipex, argv[4]);
}
