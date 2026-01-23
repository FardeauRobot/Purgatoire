/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_srcs.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/20 13:35:44 by tibras            #+#    #+#             */
/*   Updated: 2026/01/23 17:09:52 by tibras           ###   ########.fr       */
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
}	t_ends;

typedef struct s_cmd {
	char **args;
	char *path;
	int		fd_in;
	int		fd_out;
	pid_t	pid;
	struct s_cmd *next;
}	t_cmd;

typedef struct s_pipex {
	t_cmd	*cmds;
	char	**envs;
	int exit_code;
	int is_heredoc;
	char *limiter;
} t_pipex;

////// PRINT_TESTS.C //////
void	ft_print_char_arr(char **arr);
//-----------------/

#endif