/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/21 16:11:38 by tibras            #+#    #+#             */
/*   Updated: 2026/01/26 18:23:36 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex_srcs.h"


void ft_get_path_line(t_pipex *pipex)
{
	size_t	i;

	if (!pipex->envs || !*pipex->envs)
	{
		ft_printf("SUCE\n");
		exit (1);
	}
	i = 0;
	pipex->path_line = NULL;
	while (pipex->envs[i])
		if (ft_strncmp(pipex->envs[i++], "PATH=", 5) == 0)
			pipex->path_line = pipex->envs[i - 1] + 5;
}

void ft_valid_path(t_pipex *pipex, t_cmd *cmd)
{
	char buffer[BUFFER_SIZE];
	char *argv[ARGS_NBR + 1];
	size_t start;
	size_t end;
	size_t i;

	if (!pipex->path_line)
		exit (1);
	start = 0;
	i = 0;
	while (i < ARGS_NBR && cmd->args[i][0])
	{
		argv[i] = cmd->args[i];
		i++;
	}
	argv[i] = NULL;
	while (pipex->path_line[start])
	{
		ft_bzero(buffer, BUFFER_SIZE);
		end = start;
		i = 0;
		while ( pipex->path_line[end] != '\0' && pipex->path_line[end] != ':')
			buffer[i++] = pipex->path_line[end++];
		if (ft_strlcat(buffer, "/", BUFFER_SIZE) > BUFFER_SIZE ||
			ft_strlcat(buffer, cmd->args[0], BUFFER_SIZE) > BUFFER_SIZE ||
			ft_strlcpy(cmd->path, buffer, BUFFER_SIZE) > BUFFER_SIZE)
		ft_printf("Cmd->path = %s\n", cmd->path);
		if (!access(cmd->path, F_OK))
		{
			execve(cmd->path, argv, pipex->envs);
			break;
		}
		start = end + 1;
	}
}
