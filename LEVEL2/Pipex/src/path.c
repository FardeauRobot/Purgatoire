/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/15 18:00:00 by fardeau           #+#    #+#             */
/*   Updated: 2026/02/15 19:13:22 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/pipex.h"

// RECUPERE LES PATHS DEPUIS L'ENV (meme logique que ft_get_path de check2.c)
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

// TESTE CHAQUE PATH (meme logique que ft_test_path de check2.c)
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

// RESOUT LE CHEMIN COMPLET D'UNE COMMANDE (meme logique que ft_check_cmd)
char	*ft_path_resolve(t_pipex *pipex, char *cmd)
{
	char		**paths;

	if (!cmd || !*cmd)
		return (NULL);
	if (cmd[0] == '/' && !cmd[1])
		return (NULL);
	if (access(cmd, X_OK) == 0)
		return (cmd);
	paths = ft_path_get(pipex);
	if (!paths)
		return (NULL);
	return (ft_path_test(pipex, paths, cmd));
}
