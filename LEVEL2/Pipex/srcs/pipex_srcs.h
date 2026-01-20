/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_srcs.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/20 13:35:44 by tibras            #+#    #+#             */
/*   Updated: 2026/01/20 15:31:44 by tibras           ###   ########.fr       */
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

typedef enum e_ends {
	READ,
	WRITE, 
}	t_ends;

typedef struct s_pipe {
	int pipe_fd[2];
	char *cmd;
	char *path;
}	t_pipe;

#endif