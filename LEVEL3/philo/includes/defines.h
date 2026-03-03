/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   defines.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/02 13:01:12 by tibras            #+#    #+#             */
/*   Updated: 2026/03/03 19:50:30 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEFINES_H
# define DEFINES_H

/* ============== COLORS =================================== */

# define RED "\033[0;31m"
# define GREEN "\033[0;32m"
# define YELLOW "\033[0;33m"
# define BLUE "\033[0;34m"
# define MAGENTA "\033[0;35m"
# define CYAN "\033[0;36m"
# define WHITE "\033[0;37m"
# define RESET "\033[0m"

/* ============== LIMITS =================================== */
/*
** Add project-specific limits here, e.g.:
** # define MAX_PATH 4096
** # define MAX_ARGS 256
*/

# define MAX_TIME 1000000
# define MAX_GUESTS 250
# define MAX_MEALS 10000

// PARSING.C
int	ft_parsing(t_philo *philo, int argc, char **argv);

// START.C
void	ft_start(t_philo *philo);

// OUTPUT.C
void	ft_action_print(int t_id, t_state state);
#endif
