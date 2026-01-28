/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   macros.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/26 13:37:42 by tibras            #+#    #+#             */
/*   Updated: 2026/01/28 14:38:18 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MACROS_H
# define MACROS_H

# define FAILURE 0
# define SUCCESS 1

# define EXIT_ERROR 1
# define EXIT_SUCCESS 0

# define SPACE " \a\b\t\n\v\f\r"

typedef enum e_errors {
	ERR_GEN = 1,
	ERR_ARGS = 2,
	ERR_MALLOC = 99,
	ERR_PIPE = 100,
	ERR_FD = 101,
	ERR_EXEC = 126,
	ERR_CMD_NOT_FOUND = 127,
	ERR_SIGKILL = 128,
}	t_errors;

typedef enum e_select {
	FIRST = 1,
	SECOND = 2,
	BOTH = 3,
}	t_select;

# endif