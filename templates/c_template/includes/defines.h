/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   defines.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/02 00:00:00 by tibras            #+#    #+#             */
/*   Updated: 2026/03/02 08:26:12 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEFINES_H
# define DEFINES_H

/* ============== RETURN CODES ============================= */
# define SUCCESS 0
# define FAILURE 1

/* ============== LIMITS =================================== */
/*
** Add project-specific limits here, e.g.:
** # define MAX_PATH 4096
** # define MAX_ARGS 256
*/

/* ============== ENUMS ==================================== */

typedef enum e_err
{
	ERR_NONE,
	ERR_MALLOC,
	ERR_OPEN,
	ERR_ARGS,
	ERR_UNKNOWN
}	t_err;

/*
** Add project-specific enums here, e.g.:
** typedef enum e_token_type { ... } t_token_type;
*/

#endif
