/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/15 16:19:43 by fardeau           #+#    #+#             */
/*   Updated: 2026/02/15 17:32:47 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_H
# define PIPEX_H

# include "libft.h"
# include "pipex_errors.h"

#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

typedef enum e_filetype {
	INFILE,
	OUTFILE,
}	t_filetype;

typedef enum e_state {
	DEFAULT,
	IN_QUOTE,
	IN_DQUOTE,
	WAITING,
}	t_state;

typedef struct s_cmd_args {
	char *content;
	struct s_cmd_args *next;
}	t_cmd_args;

typedef struct s_cmd {
	char *path;
	char **args;
	char **envp;
	int 	infd;
	int		outfd;
	t_cmd_args *args_lst;
}	t_cmd;

typedef struct s_pipex {
	t_cmd	cmds[2];
	int 	pid[2];
	int		pipefd[2];
	int		error_code;
	t_state	state;
	char	*path;
	t_list	*gc;
}	t_pipex;

// INIT.C //
int		ft_open(char *path, t_filetype typefd);
char	*ft_path_find(t_pipex *pipex, char **envp);
int		ft_pipex_init(char **argv, char **envp, t_pipex *pipex);
//-------/

// PARSING.C //
void	ft_state_detect (t_pipex *pipex, char c);
int		ft_cmd_init(t_pipex *pipex, t_cmd *cmd, char *str);
//-------/

#endif 