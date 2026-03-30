/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/02 00:00:00 by tibras            #+#    #+#             */
/*   Updated: 2026/03/12 13:12:37 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERRORS_H
# define ERRORS_H

typedef enum e_err
{
	SUCCESS,
	STANDARD_ERROR,
	ERRN_NONE,
	ERRN_MALLOC,
	ERRN_PARSING,
	ERRN_OPEN,
	ERRN_THREADS,
	ERRN_MUTEX,
	ERRN_UNKNOWN
}	t_err;

/* ============== ERROR MESSAGES ============================ */

# define EXPECTED_ARGS		"Expects: NB_PHILO D_TIME E_TIME S_TIME (NB_MEALS)"
# define INVALID_PARAM		"Argument is not a valid number : "
# define ERR_MSG_ARGS		"Wrong number of arguments."
# define ERR_PARSING		"Error parsing : "
# define ERR_MAX_CAP		"Expects at most 200 guests"
# define ERR_MIN_TIME		"Expects time values to be at least 60 ms"
# define ERR_MAX_MEALS		"Can eat at most 10 000 meals, seems enough"
# define ERR_GUESTS			"Not enough room at the table."
# define ERR_TIME			"Duration exceeds the limit of the program : "
# define ERR_MEALS			"Philosophers can't eat that much ... "
# define ERR_THREAD			"Error threads : "
# define ERR_INIT			"Initialization didn't work."
# define ERR_MUTEX			"Error mutexes init : "
# define ERR_MSG_MALLOC		"memory allocation failed"

/* ============== ERROR FUNCTIONS ========================== */

/* src/utils/error.c */
int		ft_error(char *context, char *detail, int error);
int		check_args(int argc, int expected, char *usage);

/* ============== MEMORY HELPERS =========================== */

/* src/utils/memory.c */
void	*safe_malloc(size_t size);
void	free_split(char **tab);

#endif
