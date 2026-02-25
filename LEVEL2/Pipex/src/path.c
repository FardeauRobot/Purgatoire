/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/15 18:00:00 by fardeau           #+#    #+#             */
/*   Updated: 2026/02/23 10:37:04 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/pipex.h"

static char	**ft_path_get(t_pipex *pipex)
{
	char	*env;
	char	**paths;

	env = getenv("PATH");
	if (!env)
		return (NULL);
	paths = ft_split_sep_gc(env, ':', &pipex->gc);
	if (!paths)
		return (NULL);
	return (paths);
}

static char	*ft_path_test(t_pipex *pipex, char **paths, char *cmd)
{
	char		*tmp;
	char		*cur_path;
	int			i;

	i = 0;
	while (paths[i])
	{
		tmp = ft_strjoin_gc(paths[i], "/", &pipex->gc);
		cur_path = ft_strjoin_gc(tmp, cmd, &pipex->gc);
		if (access(cur_path, X_OK) == 0)
			return (cur_path);
		i++;
	}
	return (NULL);
}

char	*ft_path_resolve(t_pipex *pipex, char *cmd)
{
	char		**paths;

	if (!cmd || !*cmd)
		return (NULL);
	if (cmd[0] == '/' && !cmd[1])
		return (NULL);
	if (access(cmd, X_OK) == 0)
		return (cmd);
	if (ft_strchr(cmd, '/'))
	{
		if (access(cmd, F_OK) == 0)
		{
			perror(cmd);
			ft_child_exit(pipex, PERM_DENIED);
		}
		perror(cmd);
		return (NULL);
	}
	paths = ft_path_get(pipex);
	if (!paths)
		return (NULL);
	return (ft_path_test(pipex, paths, cmd));
}
