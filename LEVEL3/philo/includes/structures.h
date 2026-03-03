/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   structures.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/02 08:27:21 by tibras            #+#    #+#             */
/*   Updated: 2026/03/03 19:44:01 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STRUCTURES_H
# define STRUCTURES_H

# include <pthread.h>
# include <time.h>

/* ============== ENUMS ==================================== */

typedef enum e_state
{
	FORK_L,
	FORK_R,
	EATING,
	THINKING,
	SLEEPING,
	DEAD
}			t_state;

typedef enum e_scales
{
	SECONDS,
	MILLISECONDS,
	MICROSECONDS,
}			t_scales;

/* ============== STRUCTURES =============================== */

/*
** t_data - main project context
** Centralises all runtime data so it can be passed to functions cleanly.
** Add your fields here as the project grows.
*/

typedef struct s_guest
{
	int			t_id;
	pthread_t	thread;
	int		    nb_meals;
	int		    time_last_meal;
	t_state		status;
}	t_guest;

typedef struct s_philo
{
	t_guest	guests[250];
	int		nb_philo;
	int		nb_forks;
	time_t	time_to_die;
	time_t	time_to_eat;
	time_t	time_to_sleep;
	int		needed_meals;
}	t_philo;

/*
** Add project-specific structures here, e.g.:
** typedef struct s_token { ... } t_token;
** typedef struct s_cmd   { ... } t_cmd;
*/

#endif
