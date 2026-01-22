/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_srcs.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/20 13:35:44 by tibras            #+#    #+#             */
/*   Updated: 2026/01/22 14:16:32 by tibras           ###   ########.fr       */
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


# define ERR_GEN 1
# define ERR_ARGS 2
# define ERR_EXEC 126
# define ERR_CMD_NOT_FOUND 127
# define ERR_SIGKIL 128

# define SPACE " \a\b\t\n\v\f\r"

typedef enum e_ends {
	READ,
	WRITE, 
}	t_ends;

typedef struct s_pipe {
	int pipe_fd[2];
	int status;
	char *cmd;
	char *path;
	char **arg_pipe;
}	t_pipe;

typedef struct s_data {
	int		count_fctn;
	int		fd_infile;
	int		fd_outfile;
	t_pipe	**arr_pipe;
}	t_data;

////// PARSING.C //////
char *ft_find_path(char *str);
char *ft_get_path(char **envp);
char *ft_correct_path(char **envp, char *cmd);
//---------------/

////// FREE_UTILS.C //////
void	ft_free_arr_pipe(t_pipe **arr_pipe, int size);
//---------------/
#endif