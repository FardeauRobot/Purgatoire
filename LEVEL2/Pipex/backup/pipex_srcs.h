/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_srcs.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/20 13:35:44 by tibras            #+#    #+#             */
/*   Updated: 2026/01/30 12:10:35 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_SRCS_H
# define PIPEX_SRCS_H

// A DELETE
# include <stdio.h>
//////////// //////////// ////////////

# include "../pipex.h"


// # define BUFFER_SIZE 4096
// # define CMDS_NBR 64
// # define ENVS_CMD_NBR 128
// # define PATH_SIZE 200
// # define ARGS_SIZE 128
// # define ARGS_NBR 128

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
	char	**args;
	char	*path;
	int		error;
}	t_cmd;

typedef struct s_pipex {
	t_cmd	**cmds;
	char	**envs;
	char	**paths;
	char	*infile;
	char	*outfile;
	int		cmd_nbr;
	int		infile_fd;
	int		outfile_fd;
	int		exit_code;
	int		is_heredoc;
	int		**pipe_arr;
	pid_t	*pid_arr;
	char	*limiter;
	t_list	*gc;
} t_pipex;

////// CLOSE.C //////
void	ft_close_files(t_pipex *pipex);
void	ft_close_pipes(t_pipex *pipex);
//-----------------/

////// MAIN.C //////
//-----------------/

////// PARSING.C //////
void	ft_find_path_env(t_pipex *pipex);
void	ft_default_pipex(t_pipex *pipex, char **envp, int argc, char **argv);
void	ft_load_cmds(t_pipex *pipex, char **argv);
//-----------------/

////// ERRORS.C //////
void	error_exit(t_pipex *pipex, int error, char *err_msg);
//-----------------/
#endif