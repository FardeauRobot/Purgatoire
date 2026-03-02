/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/02 00:00:00 by tibras            #+#    #+#             */
/*   Updated: 2026/03/02 08:34:34 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERRORS_H
# define ERRORS_H

typedef enum e_err
{
	ERR_NONE,
	ERR_MALLOC,
	ERR_OPEN,
	ERR_ARGS,
	ERR_UNKNOWN
}	t_err;

/* ============== ERROR MESSAGES ============================ */
/*
** Centralise your error strings here for consistency.
** Usage:  print_error("open", ERR_MSG_PERM);
*/
# define ERR_MSG_MALLOC	"memory allocation failed"
# define ERR_MSG_OPEN	"cannot open file"
# define ERR_MSG_PERM	"permission denied"
# define ERR_MSG_ARGS	"wrong number of arguments"
# define ERR_MSG_FD		"bad file descriptor"

/* ============== ERROR FUNCTIONS ========================== */
/* src/utils/error.c */
void	print_error(char *context, char *detail);
void	exit_error(t_data *data, char *context, char *detail);
int		check_args(int argc, int expected, char *usage);

/* ============== MEMORY HELPERS =========================== */
/* src/utils/memory.c */
void	*safe_malloc(size_t size);
void	free_split(char **tab);
void	free_data(t_data *data);

#endif
