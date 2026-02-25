/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/15 18:00:00 by fardeau           #+#    #+#             */
/*   Updated: 2026/02/23 11:59:02 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_H
# define PIPEX_H

# include "../libft/libft.h"
# include <errno.h>
# include <fcntl.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/wait.h>
# include <unistd.h>

typedef enum e_error
{
	SUCCESS = 0,
	STANDARD_ERROR = 1,
	PERM_DENIED = 126,
	CMD_NOT_FOUND = 127,
}			t_error;

typedef enum e_state
{
	NORMAL,
	IN_QUOTE,
	IN_DQUOTE,
	WAITING,
}			t_state;

typedef enum e_position
{
	CHILD_FIRST,
	CHILD_SECOND,
}			t_position;

typedef struct s_cmd
{
	char	*path;
	char	**args;
	int		infd;
	int		outfd;
}			t_cmd;

typedef struct s_pipex
{
	t_cmd	cmd1;
	t_cmd	cmd2;
	int		pipefd[2];
	char	**envp;
	t_list	*gc;
	t_state	state;
}			t_pipex;

void		ft_state_detect(char c, t_pipex *pipex);
void		ft_buffer_add(char *buffer, char c);
void		ft_args_parse(t_pipex *pipex, char **argv);
void		ft_cmd_not_found(char *arg);
void		ft_cmd_parse(t_pipex *pipex, t_cmd *cmd, char *arg);

char		*ft_path_resolve(t_pipex *pipex, char *cmd);

int			ft_pipeline_exec(t_pipex *pipex);

void		ft_fd_close(int *fd);
void		ft_fds_close_all(t_pipex *pipex);
int			ft_error_print(char *str);
void		ft_pipex_exit(t_pipex *pipex, int error, char *str);
void		ft_child_exit(t_pipex *pipex, int error);

#endif
