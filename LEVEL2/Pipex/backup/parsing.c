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

// AFFECT PATH= TO PIPEX EQUIVALENT
void	ft_find_path_env(t_pipex *pipex)
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
	if (!pipex->paths)
		error_exit(pipex, 1, "Error, no PATH found\n");
}

// AFFECT THE PATH FOR EXECVE TO EACH SMD STRUCT
char	*ft_affect_path(t_pipex *pipex, char *cmd)
{
	int	i;
	char *full_cmd;

	i = 0;
	if (!access(cmd, F_OK | X_OK))
		return (cmd);
	while (pipex->paths[i])
	{
		full_cmd = ft_strdup_gc(pipex->paths[i], &pipex->gc);
		full_cmd = ft_strjoin_gc(full_cmd, "/", &pipex->gc);
		full_cmd = ft_strjoin_gc(full_cmd, cmd, &pipex->gc);
		if (!access(full_cmd, F_OK | X_OK))
			return (full_cmd);
		i++;
	}
	return (NULL);
}

// INITALISE LA STRUCTURE PIPEX
void	ft_default_pipex(t_pipex *pipex, char **envp, int argc, char **argv)
{
	int i = 0;

	pipex->gc = NULL;
	pipex->cmds = NULL;
	pipex->envs = envp;
	pipex->paths = NULL;
	pipex->infile = argv[1];
	pipex->outfile = argv[argc - 1];
	pipex->exit_code = 0;
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
	pipex->pid_arr = ft_calloc_gc(pipex->cmd_nbr, sizeof(pid_t), &pipex->gc);
	if (!pipex->pid_arr)
		error_exit(pipex, ERR_MALLOC, "Error with malloc pid_t arr\n");
	pipex->pipe_arr = ft_calloc_gc(pipex->cmd_nbr - 1, sizeof(int *), &pipex->gc);
	if (!pipex->pipe_arr)
		error_exit(pipex, ERR_MALLOC, "Error with malloc pipe arr\n");
	while (i++ < pipex->cmd_nbr - 1)	
		pipex->pipe_arr[i - 1] = ft_calloc_gc(2, sizeof(int), &pipex->gc);
	if (!pipex->is_heredoc)
	{
		pipex->infile_fd = open(pipex->infile, O_RDONLY);
		if (pipex->infile_fd == -1)
			perror(pipex->infile);
	}
	pipex->outfile_fd = open(pipex->outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (pipex->outfile_fd == -1)
		perror(pipex->outfile);
}

// ALLOCATE AND FILLS THE CMD STRUCTURE CONTENT
t_cmd	*ft_create_cmd_struct(char *argv, t_pipex *pipex)
{
	t_cmd	*new_cmd;

	new_cmd = ft_calloc_gc(1, sizeof(t_cmd), &pipex->gc);
	if (!new_cmd)
		error_exit(pipex, ERR_MALLOC, "Error with malloc struct when intializing cmd\n");
	new_cmd->args = ft_split_charset_gc(argv, SPACE, &pipex->gc);
	if (!new_cmd->args)
		error_exit(pipex, ERR_MALLOC, "Error with split args when intializing cmd\n");
	new_cmd->path = ft_affect_path(pipex, new_cmd->args[0]);
	return (new_cmd);
}

// LOADS CMD ARR INTO PIPEX STRUCT
void	ft_load_cmds(t_pipex *pipex, char **argv)
{
	int	i;

	i = 0;
	pipex->cmds = ft_calloc_gc(pipex->cmd_nbr + 1, sizeof(t_cmd *), &pipex->gc);
	if (!pipex->cmds)
		error_exit(pipex, ERR_MALLOC, "Error with malloc when intializing **cmds\n");
	while (i < pipex->cmd_nbr)
	{
		if (pipex->is_heredoc)
			pipex->cmds[i] = ft_create_cmd_struct(argv[3 + i], pipex);
		else
			pipex->cmds[i] = ft_create_cmd_struct(argv[2 + i], pipex);
		i++;
	}
	pipex->cmds[i] = NULL;
}