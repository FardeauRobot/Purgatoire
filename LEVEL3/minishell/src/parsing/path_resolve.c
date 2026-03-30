/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path_resolve.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamjada <alamjada@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/11 12:00:44 by alamjada          #+#    #+#             */
/*   Updated: 2026/02/28 13:10:50 by alamjada         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "errors.h"
#include "minishell.h"

static char	**ft_get_path(t_minishell *minishell)
{
	t_env	*env;
	char	**envp;

	env = ft_env_find(minishell->head_env, "PATH");
	if (!env)
		return (NULL);
	envp = ft_split_sep_gc(env->content, ':', &minishell->gc_line);
	if (!envp)
		return (NULL);
	return (envp);
}

static int	ft_test_path(t_minishell *minishell, char **envp, t_token *token)
{
	char		*tmp;
	char		*cur_path;
	int			i;
	struct stat	stat_file;

	i = 0;
	while (envp[i])
	{
		tmp = ft_strjoin_gc(envp[i], "/", &minishell->gc_line);
		if (!tmp)
			return (GENERAL_ERROR);
		cur_path = ft_strjoin_gc(tmp, token->str, &minishell->gc_line);
		if (!cur_path)
			return (GENERAL_ERROR);
		if (stat(cur_path, &stat_file) == 0 && S_ISREG(stat_file.st_mode))
		{
			if (access(cur_path, X_OK) == 0)
			{
				token->path = cur_path;
				return (SUCCESS);
			}
		}
		i++;
	}
	return (GENERAL_ERROR);
}

int	ft_cmd_find_path(t_minishell *minishell, t_token *token)
{
	char		**envp;
	struct stat	stat_file;

	if (!token || !token->str || !*token->str)
		return (GENERAL_ERROR);
	if (token->str[0] == '/' && !token->str[1])
		return (GENERAL_ERROR);
	if (ft_strchr(token->str, '/'))
	{
		if (stat(token->str, &stat_file) == 0 && S_ISREG(stat_file.st_mode))
		{
			token->path = token->str;
			return (SUCCESS);
		}
		return (GENERAL_ERROR);
	}
	envp = ft_get_path(minishell);
	if (!envp)
		return (GENERAL_ERROR);
	return (ft_test_path(minishell, envp, token));
}

int	is_redirection(t_token *token)
{
	if (token->type == IN_CHEVRON || token->type == OUT_CHEVRON
		|| token->type == IN_DCHEVRON || token->type == OUT_DCHEVRON)
		return (1);
	return (0);
}
