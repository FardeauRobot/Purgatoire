/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_srcs.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/20 13:35:44 by tibras            #+#    #+#             */
/*   Updated: 2026/01/27 14:49:59 by tibras           ###   ########.fr       */
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
	char **args;
	char *path;
	int	error;
	struct s_cmd *next;
}	t_cmd;

typedef struct s_pipex {
	t_cmd	*cmds;
	char	**envs;
	char	**paths;
	int		cmd_nbr;
	char	*infile;
	char	*outfile;
	int		infile_fd;
	int		outfile_fd;
	int		exit_code;
	int		is_heredoc;
	pid_t	*pid;
	char	*limiter;
	t_list	*gc;
} t_pipex;

////// MAIN.C //////
//-----------------/

////// PARSING.C //////
//-----------------/

#endif