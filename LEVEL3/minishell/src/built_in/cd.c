/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/13 19:49:15 by alamjada          #+#    #+#             */
/*   Updated: 2026/02/28 10:00:30 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	ft_cd_back(t_minishell *minishell, char *old_pwd)
{
	t_env	*tmp;

	tmp = ft_env_find(minishell->head_env, "OLDPWD");
	if (!tmp)
		return (GENERAL_ERROR);
	if (chdir(tmp->content) == -1)
	{
		free(old_pwd);
		ft_error(minishell, errno, ERR_CD, NULL);
		return (errno);
	}
	update_old_pwd(&minishell->head_env, old_pwd, minishell);
	update_pwd(&minishell->head_env, minishell);
	free(old_pwd);
	ft_pwd(minishell);
	return (SUCCESS);
}

char	*ft_define_path(t_minishell *minishell, char **args)
{
	char	*path;

	if (!args[1] || ft_strcmp(args[1], "~") == 0)
		path = ft_no_path(minishell->head_env);
	else if (ft_strchr(args[1], '~') && args[1][1])
		path = ft_expand_home(minishell, args[1]);
	else
		path = args[1];
	return (path);
}

static int	ft_try_cd(char *path, t_minishell *minishell)
{
	struct stat	stat_file;

	if (stat(path, &stat_file) != 0)
		return (ft_error(minishell, GENERAL_ERROR, strerror(errno), NULL));
	if (!S_ISDIR(stat_file.st_mode))
		return (ft_error(minishell, ENOTDIR, ERR_CD_PFX, ft_strjoin_gc(path,
					ERR_NOT_A_DIR, &minishell->gc_tmp)));
	if (chdir(path) == -1)
		return (ft_error(minishell, errno, strerror(errno), NULL));
	return (SUCCESS);
}

static int	ft_cd2(t_minishell *minishell, char *path)
{
	int		res;
	char	*old_pwd;

	old_pwd = save_pwd();
	res = ft_try_cd(path, minishell);
	if (res != SUCCESS)
	{
		free(old_pwd);
		return (res);
	}
	update_old_pwd(&minishell->head_env, old_pwd, minishell);
	free(old_pwd);
	update_pwd(&minishell->head_env, minishell);
	return (SUCCESS);
}

int	ft_cd(t_minishell *minishell, char **args)
{
	char	*path;
	char	*old_pwd;

	if (args[2])
		return (ft_error(minishell, GENERAL_ERROR, ERR_CD_PFX,
				ERR_TOO_MANY_ARGS));
	if (args[1] && ft_strcmp(args[1], "-") == 0)
	{
		old_pwd = save_pwd();
		return (ft_cd_back(minishell, old_pwd));
	}
	else
		path = ft_define_path(minishell, args);
	if (!path)
		return (ft_error(minishell, GENERAL_ERROR, ERR_CD_PFX,
				ERR_HOME_NOT_SET));
	return (ft_cd2(minishell, path));
}
