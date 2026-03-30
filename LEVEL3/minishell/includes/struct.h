/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   struct.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/19 20:49:28 by alamjada          #+#    #+#             */
/*   Updated: 2026/02/27 11:17:20 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STRUCT_H
# define STRUCT_H
# include "../libft/libft.h"

// ENUM TYPES ELEMENTS
typedef enum e_type
{
	DEFAULT = 0,
	PIPE,
	IN_CHEVRON,
	IN_DCHEVRON,
	OUT_CHEVRON,
	OUT_DCHEVRON,
	WORD,
	CMD,
	GARBAGE,
	FLAG,
	NBR_TYPES,
}					t_types;

// ENUMS MACHINE STATES
typedef enum e_state
{
	NORMAL,
	IN_QUOTE,
	IN_DQUOTE,
	WAITING,
	NBR_STATE,
}					t_state;

// STRUCTURE POUR CMD
typedef struct s_cmd
{
	t_list			node;
	char			*path;
	char			**args;
	char			**envp;
	int				infd;
	int				outfd;
	int				error_file;
}					t_cmd;

// STRUCTURE POUR UN ELEMENT
typedef struct s_token
{
	t_list			node;
	char			*str;
	t_types			type;
	char			*path;
	int				should_split;
}					t_token;

typedef struct s_env
{
	t_list			node;
	char			*str;
	char			*name;
	char			*content;
}					t_env;

// STRUCTURE GLOBALE POUR LE MINISHELL
typedef struct s_minishell
{
	t_token			*head_token;
	t_cmd			*head_cmd;
	t_env			*head_env;
	t_list			*gc_global;
	t_list			*gc_line;
	t_list			*gc_tmp;
	char			*line;
	t_state			state;
	int				exit_status;
	int				cached_status;
}					t_minishell;

// STRUCTURE POUR EXEC CHILD
typedef struct s_child
{
	int				index;
	int				prev_pipe;
	int				size_cmd;
	int				pipe_fd[2];
}					t_child;

#endif
