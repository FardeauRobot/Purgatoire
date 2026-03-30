/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_setup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/12 19:32:38 by fardeau           #+#    #+#             */
/*   Updated: 2026/02/28 10:09:19 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "errors.h"
#include "minishell.h"

static int	ft_fill_env(t_minishell *minishell, t_env *new, char *str)
{
	char	*separator;

	new->str = ft_strdup_gc(str, &minishell->gc_global);
	if (!new->str)
		return (ft_error(minishell, MALLOC_FAIL, ERR_MALLOC_ENV, NULL));
	separator = ft_strchr(str, '=');
	if (separator)
	{
		new->name = ft_substr_gc(str, 0, separator - str,
				&minishell->gc_global);
		new->content = ft_strdup_gc(separator + 1, &minishell->gc_global);
	}
	else
	{
		new->name = ft_strdup_gc(str, &minishell->gc_global);
		new->content = NULL;
	}
	if (!new->name || (separator && !new->content))
		return (ft_error(minishell, MALLOC_FAIL, ERR_MALLOC_ENV, NULL));
	return (SUCCESS);
}

t_env	*ft_env_new(t_minishell *minishell, char *str)
{
	t_env	*new;

	new = ft_calloc_gc(1, sizeof(t_env), &minishell->gc_global);
	if (!new)
	{
		ft_error(NULL, MALLOC_FAIL, ERR_MALLOC_ENV, NULL);
		return (NULL);
	}
	if (ft_fill_env(minishell, new, str) == GENERAL_ERROR)
	{
		ft_error(NULL, MALLOC_FAIL, ERR_MALLOC_ENV, NULL);
		return (NULL);
	}
	return (new);
}

// TROUVE UN ENV A PARTIR DU NOM
t_env	*ft_env_find(t_env *head_env, char *to_find)
{
	while (head_env)
	{
		if (ft_strcmp(head_env->name, to_find) == 0)
			return (head_env);
		head_env = (t_env *)head_env->node.next;
	}
	return (NULL);
}

static void	ft_env_ensure(t_minishell *minishell, char *name, char *value)
{
	if (ft_env_find(minishell->head_env, name) == NULL)
		ft_env_add(minishell, ft_env_new(minishell, value));
}

int	ft_env_setup(t_minishell *minishell, char **envp)
{
	int		i;
	t_env	*new;
	char	*cwd;
	char	*pwd_str;

	i = -1;
	while (envp[++i])
	{
		new = ft_env_new(minishell, envp[i]);
		if (!new || ft_env_add(minishell, new))
			ft_exit(minishell, GENERAL_ERROR, ERR_SETUP_ENV);
	}
	ft_env_ensure(minishell, "_", "_=/usr/bin/env");
	if (ft_env_find(minishell->head_env, "PWD") == NULL)
	{
		cwd = getcwd(NULL, 0);
		pwd_str = ft_strjoin_gc("PWD=", cwd, &minishell->gc_global);
		free(cwd);
		if (pwd_str)
			ft_env_add(minishell, ft_env_new(minishell, pwd_str));
	}
	ft_env_ensure(minishell, "OLDPWD", "OLDPWD");
	ft_env_ensure(minishell, "SHLVL", "SHLVL=1");
	return (SUCCESS);
}
