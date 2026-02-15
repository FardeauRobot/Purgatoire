/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_errors.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/15 16:26:30 by fardeau           #+#    #+#             */
/*   Updated: 2026/02/15 17:32:02 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_ERRORS_H
# define PIPEX_ERRORS_H

typedef enum e_errors {
	STANDARD_ERROR = 0,
	ERRN_ARGS,
	ERRN_OPEN_IN,
	ERRN_OPEN_OUT,
	ERRN_PATH,
	ERRN_INIT,
	ERRN_MALLOC,
}	t_errors;

# define ERRS_ARGS "Wrong number of arguments\n"
# define ERRS_OPEN_IN "Error opening infile\n"
# define ERRS_OPEN_OUT "Error opening outfile\n"
# define ERRS_PATH "Error finding path\n"

typedef struct s_pipex t_pipex;

// ERRORS.C //
int ft_error(int error, char *error_str);
int	ft_exit(t_pipex *pipex, int error, char *error_str);
//////

#endif