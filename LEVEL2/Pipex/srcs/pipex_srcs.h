/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_srcs.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/20 13:35:44 by tibras            #+#    #+#             */
/*   Updated: 2026/01/26 17:24:36 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_SRCS_H
# define PIPEX_SRCS_H

// A DELETE
# include <stdio.h>
//////////// //////////// ////////////

# include "../pipex.h"

# define FAILURE 0
# define SUCCESS 1

# define EXIT_ERROR 1
# define EXIT_SUCCESS 0

# define BUFFER_SIZE 4096
# define CMDS_NBR 256
# define ENVS_CMD_NBR 512
# define PATH_SIZE 200
# define ARGS_SIZE 200
# define ARGS_NBR 128

# define SPACE " \a\b\t\n\v\f\r"

typedef enum e_errors {
	ERR_GEN = 1,
	ERR_ARGS = 2,
	ERR_EXEC = 126,
	ERR_CMD_NOT_FOUND = 127,
	ERR_SIGKILL = 128,
}	t_errors;

typedef enum e_open {
	INFILE,
	OUTFILE,
	HERE_DOC,
}	t_open;

typedef enum e_ends {
	READ,
	WRITE, 
	NB_ENDS,
}	t_ends;

typedef struct s_cmd {
	char args[ARGS_NBR][ARGS_SIZE];
	char path[PATH_SIZE];
	int fd[NB_ENDS];
	pid_t	pid;
	struct s_cmd *next;
}	t_cmd;

typedef struct s_pipex {
	t_cmd	cmds[CMDS_NBR];
	char	**envs;
	char	*path_line;
	int		infile_fd;
	int		outfile_fd;
	int exit_code;
	int is_heredoc;
	char *limiter;
} t_pipex;

////// PARSING.C //////
// void ft_get_path(char **envp, t_pipex pipex);
void ft_get_path(t_pipex *pipex);
void ft_get_path_line(t_pipex *pipex);
void ft_valid_path(t_pipex *pipex, t_cmd *cmd);
//-----------------/

#endif