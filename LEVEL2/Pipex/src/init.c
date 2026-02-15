/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/15 16:59:28 by fardeau           #+#    #+#             */
/*   Updated: 2026/02/15 17:07:49 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

int		ft_open(char *path, t_filetype typefd)
{
	int fd;

	fd = -1;
	if (typefd == INFILE)
		fd = open(path, O_RDONLY);
	if (typefd == OUTFILE)
		fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	return (fd);
}

char	*ft_path_find(t_pipex *pipex, char **envp)
{
	int i;

	i = 0;
	if (!envp)
		return (NULL);
	while (envp[i])
	{
		if (!ft_strncmp(envp[i], "PATH=", 5))
			return (ft_strdup_gc(envp[i], &pipex->gc));
		i++;
	}
	return (NULL);
}


int	ft_pipex_init(char **argv, char **envp, t_pipex *pipex)
{
	pipex->state = DEFAULT;
	pipex->cmds[0].infd = ft_open(argv[1], INFILE);
	if (pipex->cmds[0].infd == -1)
		return (ft_error(ERRN_OPEN_IN, ERRS_OPEN_IN));
	pipex->cmds[1].outfd = ft_open(argv[4], OUTFILE);
	if (pipex->cmds[1].outfd == -1)
		return (ft_error(ERRN_OPEN_OUT, ERRS_OPEN_OUT));
	pipex->path = ft_path_find(pipex, envp);
	if (!pipex->path)
		return (ft_error(ERRN_PATH, ERRS_PATH));
	return (0);
}


