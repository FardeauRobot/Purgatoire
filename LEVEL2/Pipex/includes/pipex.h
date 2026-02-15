/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/15 18:00:00 by fardeau           #+#    #+#             */
/*   Updated: 2026/02/15 19:04:53 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_H
# define PIPEX_H

# include "../libft/libft.h"
# include <fcntl.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/wait.h>
# include <unistd.h>

// ENUMS ETATS MACHINE STATE (meme logique que minishell)
typedef enum e_state
{
	NORMAL,
	IN_QUOTE,
	IN_DQUOTE,
	WAITING,
}					t_state;

// ENUM POSITION POUR IDENTIFIER LE CHILD
typedef enum e_position
{
	CHILD_FIRST,
	CHILD_SECOND,
}					t_position;

// STRUCTURE POUR UNE COMMANDE (meme logique que t_cmd de minishell)
typedef struct s_cmd
{
	char			*path;
	char			**args;
	int				infd;
	int				outfd;
}					t_cmd;

// STRUCTURE GLOBALE POUR PIPEX
typedef struct s_pipex
{
	t_cmd			cmd1;
	t_cmd			cmd2;
	int				pipefd[2];
	char			**envp;
	t_list			*gc;
	t_state			state;
}					t_pipex;

// SRC/PARSING.C – MACHINE STATE
void				ft_state_detect(char c, t_pipex *pipex);
void				ft_buffer_add(char *buffer, char c);
void				ft_args_parse(t_pipex *pipex, char **argv);

// SRC/PATH.C
char				*ft_path_resolve(t_pipex *pipex, char *cmd);

// SRC/EXEC.C
int					ft_pipeline_exec(t_pipex *pipex);

// SRC/ERRORS.C
int					ft_error_print(char *str);
void				ft_pipex_exit(t_pipex *pipex, int error, char *str);
void				ft_child_exit(t_pipex *pipex, int error);

#endif
