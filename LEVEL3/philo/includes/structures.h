/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   structures.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/02 08:27:21 by tibras            #+#    #+#             */
/*   Updated: 2026/03/06 09:21:16 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STRUCTURES_H
# define STRUCTURES_H

# include "defines.h"
# include <pthread.h>
# include <time.h>

/* ============== STRUCTURES =============================== */

typedef struct s_philo	t_philo;

typedef struct s_guest
{
	int			t_id;
	pthread_t	thread;
	int		    nb_meals;
	int		    time_last_meal;
	t_state		status;
	pthread_mutex_t *forks[2];
	pthread_mutex_t *print;
	t_philo		*data;
}	t_guest;

typedef struct s_philo
{
	t_guest	guests[MAX_GUESTS];
	int		nb_philo;
	int		nb_forks;
	int		needed_meals;
	int		full;
	long long	start_time;
	time_t	time_to_die;
	time_t	time_to_eat;
	time_t	time_to_sleep;
	t_state	is_dead;
	pthread_mutex_t *m_fork;
	pthread_mutex_t m_lock_eat;
	pthread_mutex_t m_is_dead;
	pthread_mutex_t m_print;
	pthread_mutex_t m_full;
}	t_philo;

#endif
