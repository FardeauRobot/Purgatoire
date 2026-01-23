/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/20 13:36:13 by tibras            #+#    #+#             */
/*   Updated: 2026/01/23 18:32:19 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex_srcs.h"


/* 
	ON INITIALISE LA STRUCT (pipex, argv, envp)
		On affecte envs avec envp
		exit_code set a 0
		Check si heredoc
			Si heredoc -> on recupere le limiter
		ON CREE LA LISTE CHAINEE 
	ON TRAITE LES CMDS
		Création du noeud cmd
			args = argv[i + 2]
			path
			fd_in = -1
			fd_out = -1
			pid = -1
			next = NULL

*/
int main (int argc, char **argv, char **envp)
{
	t_pipex	pipex;
	t_cmd	test;
	
	(void)argc;
	(void)argv;
	ft_print_char_arr(pipex.envs);
}