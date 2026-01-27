/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/20 13:36:13 by tibras            #+#    #+#             */
/*   Updated: 2026/01/27 14:58:00 by tibras           ###   ########.fr       */
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

void	ft_close_fds(t_pipex *pipex)
{
	if (pipex->infile_fd != -1)
		close(pipex->infile_fd);
	if (pipex->outfile_fd != -1)
		close(pipex->outfile_fd);
}

// Affect the pipex.paths to splitter envp of PATH=
void	ft_find_path(t_pipex *pipex)
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
}

// Set default values to pipex structs
void	ft_default_pipex(t_pipex *pipex, char **envp, int argc, char **argv)
{
	pipex->paths = NULL;
	pipex->pid = NULL;
	pipex->gc = NULL;
	pipex->envs = envp;
	pipex->infile = argv[1];
	pipex->outfile = argv[argc - 1];
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
}

void	ft_load_cmds(t_pipex *pipex)
{
	int	i;

}

// Initialize the different variables, and fills in necessary for execution
void	ft_init_pipex(t_pipex *pipex, char **envp, char **argv, int argc)
{

	if (!argv)
		return;
	ft_default_pipex(pipex, envp, argc, argv);
	ft_find_path(pipex);
	ft_create_cmds(pipex);

	// int i = 0;
	// while (pipex->paths[i])
	// {
	// 	ft_printf("%s\n", pipex->paths[i]);
	// 	i++;
	// }
	// printf("%d\n", pipex->cmd_nbr);
}

// void	ft_init_pipex(t_pipex *pipex)
// {
	
// }

// MAIN
int main (int argc, char **argv, char **envp)
{
	t_pipex pipex;
	
	ft_init_pipex(&pipex, envp, argv, argc);
	pipex.envs = envp;
	ft_find_path(&pipex);
	ft_gc_free_all(&pipex.gc);
}

/*
	On initialise : 
		Envs = envp;
		paths = split(PATH=);
*/