/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/15 18:00:00 by fardeau           #+#    #+#             */
/*   Updated: 2026/02/23 12:16:38 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/pipex.h"

static void	ft_pipex_init(t_pipex *pipex, char **envp)
{
	ft_bzero(pipex, sizeof(t_pipex));
	pipex->envp = envp;
	pipex->cmd1.infd = -1;
	pipex->cmd1.outfd = -1;
	pipex->cmd2.infd = -1;
	pipex->cmd2.outfd = -1;
	pipex->pipefd[0] = -1;
	pipex->pipefd[1] = -1;
}

int	main(int argc, char **argv, char **envp)
{
	t_pipex	pipex;
	int		ret;

	if (argc != 5)
	{
		ft_putstr_fd("Expected ./pipex file1 cmd1 cmd2 file2\n", STDERR_FILENO);
		return (STANDARD_ERROR);
	}
	ft_pipex_init(&pipex, envp);
	ft_args_parse(&pipex, argv);
	ret = ft_pipeline_exec(&pipex);
	ft_gc_free_all(&pipex.gc);
	return (ret);
}
