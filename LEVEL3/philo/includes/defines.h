/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   defines.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/02 13:01:12 by tibras            #+#    #+#             */
/*   Updated: 2026/03/03 15:13:09 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEFINES_H
# define DEFINES_H

# include "structures.h"

/* ============== LIMITS =================================== */
/*
** Add project-specific limits here, e.g.:
** # define MAX_PATH 4096
** # define MAX_ARGS 256
*/

/* ============== ENUMS ==================================== */

/* ============== MACROS ==================================== */

# define MAX_TIME 100000
# define MAX_GUESTS 250
# define MAX_MEALS 10000
 
/*
** Add project-specific enums here, e.g.:
** typedef enum e_token_type { ... } t_token_type;
*/

// PARSING.C
int	ft_parsing(t_philo *philo, int argc, char **argv);

// START.C
void	ft_start(t_philo *philo);

#endif
