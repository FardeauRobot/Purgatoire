/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/02 00:00:00 by tibras            #+#    #+#             */
/*   Updated: 2026/03/03 09:32:49 by tibras           ###   ########.fr       */
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
# define ERR_MSG_ARGS		"Wrong number of arguments. "
# define ERR_PARSING		"Error parsing : "
# define ERR_GUESTS			"Not enough room at the table."
# define ERR_MAX_CAP		"Expects at most 250 guests"
# define ERR_TIME			"Duration exceeds the limit of the program : "
# define ERR_MEALS			"Philosophers can't eat that much ... "
# define ERR_MAX_MEALS		"Can eat at most 10 000 meals, seems enough"
# define INVALID_PARAM		"Argument is not a valid number : "

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
