/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/21 16:11:38 by tibras            #+#    #+#             */
/*   Updated: 2026/01/21 17:44:15 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex_srcs.h"

char *ft_find_path(char *str)
{
	int i;
	char *env_path;

	i = 0;
	env_path = "PATH=";
	while (str[i] == env_path[i])
		i++;
	if (!env_path[i])
		return (str + i);
	return (NULL);
}

char *ft_get_path(char **envp)
{
	int i;
	char *path;

	i = 0;
	while (envp[i])
	{
		path = ft_find_path(envp[i]);
		if (path)
			return (path);
		i++;
	}
	return (NULL);
}

static void ft_free_arr_path(char **arr_path)
{
	int i;

	i = 0;
	while (arr_path[i])
	{
		free(arr_path[i]);
		i++;
	}
	free(arr_path);
}

char *ft_correct_path(char **envp, char *cmd)
{
	int i;
	size_t ttl_len;
	char path[500];
	char *correct_path;
	char **arr_path;

	correct_path = ft_get_path(envp);
	arr_path = ft_split_sep(correct_path, ':');
	if (!arr_path)
		exit(1);
	i = 0;
	while (arr_path[i])
	{
		ft_bzero(path, 500);
		ttl_len = ft_strlcat(path, (const char *)arr_path[i], ft_strlen(arr_path[i]) + 1);
		ttl_len = ft_strlcat(path, "/", ttl_len + 2);
		ttl_len = ft_strlcat(path, cmd, ttl_len + ft_strlen(cmd) + 1);
		if (!access(path, X_OK))
		{
			ft_free_arr_path(arr_path);
			return (ft_strdup(path));
		}
		i++;
	}
	ft_free_array(arr_path);
	return (NULL);
}

// int ft_parsing(char **argv, t_pipe )