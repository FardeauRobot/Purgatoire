/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/02 00:00:00 by tibras            #+#    #+#             */
/*   Updated: 2026/03/02 14:42:51 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERRORS_H
# define ERRORS_H

typedef enum e_err
{
	ERR_NONE,
	ERR_MALLOC,
	ERR_ARGS,
	ERR_OPEN,
	ERR_UNKNOWN
}	t_err;

/* ============== ERROR MESSAGES ============================ */
/*
** Centralise your error strings here for consistency.
** Usage:  print_error("open", ERR_MSG_PERM);
*/
# define ERR_MSG_MALLOC		"memory allocation failed"
# define ERR_MSG_OPEN		"cannot open file"
# define ERR_MSG_PERM		"permission denied"
# define EXPECTED_ARGS		"Expects : NB_PHILO DYING_TIME EATING_TIM SLEEP_TIME (NB_OF_MEALS)"
# define ERR_MSG_ARGS		"Wrong number of arguments."
# define ERR_MSG_FD			"bad file descriptor"

/* ============== ERROR FUNCTIONS ========================== */
/* src/utils/error.c */
int		ft_error(char *context, char *detail, int error);
void	exit_error(t_philo *philo, char *context, char *detail, int error);
int		check_args(int argc, int expected, char *usage);

/* ============== MEMORY HELPERS =========================== */
/* src/utils/memory.c */
void	*safe_malloc(size_t size);
void	free_split(char **tab);

#endif
