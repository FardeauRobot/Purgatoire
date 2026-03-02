/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   structures.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/02 08:27:21 by tibras            #+#    #+#             */
/*   Updated: 2026/03/02 14:35:10 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STRUCTURES_H
# define STRUCTURES_H

/* ============== STRUCTURES =============================== */

/*
** t_data - main project context
** Centralises all runtime data so it can be passed to functions cleanly.
** Add your fields here as the project grows.
*/

typedef enum e_state
{
	ONE_FORK,
	EATING,
	THINKING,
	SLEEPING
}	t_state;

typedef struct s_guest
{
	int		nb_meals;
	int		time_last_meal;
}	t_guest;

typedef struct s_philo
{
	// int		argc;
	// char	**argv;
	t_guest	*guests;
	int		nb_philo;
	int		nb_forks;
	int		time_to_die;
	int		time_to_eat;
	int		time_to_sleep;
	int		need_meals;
	int		exit_code;
}	t_philo;

/*
** Add project-specific structures here, e.g.:
** typedef struct s_token { ... } t_token;
** typedef struct s_cmd   { ... } t_cmd;
*/

#endif
