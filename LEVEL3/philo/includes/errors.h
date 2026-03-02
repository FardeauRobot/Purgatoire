/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/02 00:00:00 by tibras            #+#    #+#             */
/*   Updated: 2026/03/02 22:06:03 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERRORS_H
# define ERRORS_H

typedef enum e_err
{
	SUCCESS,
	STANDARD_ERROR,
	ERR_NONE,
	ERR_MALLOC,
	ERRN_PARSING,
	ERR_OPEN,
	ERR_UNKNOWN
}	t_err;

/* ============== ERROR MESSAGES ============================ */
/*
** Centralise your error strings here for consistency.
** Usage:  print_error("open", ERR_MSG_PERM);
*/
# define ERR_MSG_MALLOC		"memory allocation failed"
# define EXPECTED_ARGS		"Expects : NB_PHILO DYING_TIME EATING_TIM SLEEP_TIME (NB_OF_MEALS)"
# define ERR_MSG_ARGS		"Wrong number of arguments."
# define ERR_PARSING		"Error parsing : "
# define INVALID_PARAM		"Argument is not a number : "
# define OVERINT			"Argument doesn't fit in an it"

/* ============== ERROR FUNCTIONS ========================== */
/* src/utils/error.c */
int		ft_error(char *context, char *detail, int error);
void	ft_exit(t_philo *philo, char *context, char *detail, int error);
int		check_args(int argc, int expected, char *usage);

/* ============== MEMORY HELPERS =========================== */
/* src/utils/memory.c */
void	*safe_malloc(size_t size);
void	free_split(char **tab);

#endif
