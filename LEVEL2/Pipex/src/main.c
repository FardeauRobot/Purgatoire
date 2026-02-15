/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/15 16:19:31 by fardeau           #+#    #+#             */
/*   Updated: 2026/02/15 17:15:24 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

int main (int argc, char **argv, char **envp)
{
	t_pipex	pipex;

	// ON DEFINIT
	if (argc != 5)
		return (ft_error(ERRN_ARGS, ERRS_ARGS));
	ft_bzero(&pipex, sizeof(pipex));
	pipex.error_code = ft_pipex_init(argv, envp, &pipex);
	if (pipex.error_code)
		ft_exit(&pipex, pipex.error_code , NULL);
	ft_printf("%s\n", pipex.path);
	// ARGV[1] == infile
	// ARGV[2] == cmd1
	// ARGV[3] == cmd2
	// ARGV[4] == outfile
	ft_printf("Coucou\n");
	return (ft_exit(&pipex, 0, NULL));
}