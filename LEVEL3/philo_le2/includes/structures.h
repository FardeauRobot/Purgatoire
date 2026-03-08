/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   structures.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/02 08:27:21 by tibras            #+#    #+#             */
/*   Updated: 2026/03/07 22:31:37 by tibras           ###   ########.fr       */
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
	pthread_t		thread;
	int				t_id;

	// TODO : INCLURE LE NBRE MAX DE REPAS DANS END_CHECK ET UPDATE LAST MEAL
	pthread_mutex_t m_nb_meal;
	int				nb_meals;
	long long		time_last_meal;
	t_state			status;
	pthread_mutex_t m_status;
	pthread_mutex_t	m_last_meal;
	int				held_fork[2];
	pthread_mutex_t	*forks[2];
	pthread_mutex_t	*print;
	t_philo			*data;
}	t_guest;

typedef struct s_philo
{
	t_guest	guests[MAX_GUESTS];
	int		nb_philo;
	int		needed_meals;
	long long	start_time;
	pthread_mutex_t *m_fork;
	int		nb_forks;
	pthread_mutex_t m_full;
	int		full;
	pthread_mutex_t m_ended;
	int		ended;
	time_t	time_to_die;
	time_t	time_to_eat;
	time_t	time_to_sleep;
	pthread_mutex_t m_print;
}	t_philo;

#endif
