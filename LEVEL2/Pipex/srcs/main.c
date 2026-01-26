/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/20 13:36:13 by tibras            #+#    #+#             */
/*   Updated: 2026/01/26 18:43:30 by tibras           ###   ########.fr       */
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

void	ft_fill_argv(t_cmd *cmd, char *cmd_str)
{
	int start;
	int end;
	int index;

	start = 0;
	index = 0;
	while (cmd_str[start])
	{
		ft_bzero(cmd->args[index], ARGS_SIZE);
		end = start;
		while (!ft_ischarset(cmd_str[end], SPACE) && cmd_str[end] != '\0')
			end++;
		ft_strlcpy(cmd->args[index], cmd_str + start, end - start + 1);
		index++;
		if (!cmd_str[end])
			break;
		start = end + 1;
	}
}

void	ft_init_cmds(t_pipex *pipex, char **argv, int argc)
{
	int i;
	int j;
	int start;
	char *cmd_argv[ARGS_NBR + 1];
	
	i = 0;
	start = 2;
	while (i + start < argc - 1)
	{
		ft_bzero(pipex->cmds[i].args, ARGS_SIZE);
		ft_fill_argv(&pipex->cmds[i], argv[i + start]);
		j = 0;
		while (j < ARGS_NBR && pipex->cmds[i].args[j][0])
		{
			cmd_argv[j] = pipex->cmds[i].args[j];
			printf("CMD_ARGV[%d] = %s\n", j, cmd_argv[j]);
			j++;
		}
		cmd_argv[j] = NULL;
		i++;
	}
}

int main (int argc, char **argv, char **envp)
{
	t_pipex	pipex;

	// 1ERE COMMANDE  = ARGV[2]
	// DERNIERE COMMANDE  = ARGV[argc - 1]

	char *cmd = "ls";
	char *flags = "-la";
	
	pipex.envs = envp;
	ft_printf("INFILE = %s || OUTFILE = %s\n", argv[1], argv[argc -1]);
	ft_get_path_line(&pipex);
	pipex.infile_fd = open(argv[1], O_RDONLY, 0644);
	pipex.outfile_fd = open(argv[argc - 1], O_RDONLY | O_CREAT, 0644);
	ft_printf("%d\n", pipex.infile_fd);
	ft_printf("%d\n", pipex.outfile_fd);
	ft_init_cmds(&pipex, argv, argc);
	ft_valid_path(&pipex, &pipex.cmds[0]);
	ft_strlcpy(pipex.cmds[0].args[0], cmd, ft_strlen(cmd) + 1);
	ft_strlcpy(pipex.cmds[0].args[1], flags, ft_strlen(flags) + 1);
	ft_close_fds(&pipex);
}