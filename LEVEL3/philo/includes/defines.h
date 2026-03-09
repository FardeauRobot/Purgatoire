/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   defines.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/02 13:01:12 by tibras            #+#    #+#             */
/*   Updated: 2026/03/09 09:13:48 by tibras           ###   ########.fr       */
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

# define MAX_TIME 1000000
# define MAX_GUESTS 200
# define MAX_MEALS 10000

/* ============== MACROS =================================== */

# define STR_EAT "is eating\n"
# define STR_SLEEPING "is sleeping\n"
# define STR_FORK "has taken a fork\n"
# define STR_THINKING "is thinking\n"
# define STR_DEAD "died\n"

/* ============== ENUMS ==================================== */

typedef enum e_state
{
	FORK,
	EATING,
	THINKING,
	SLEEPING,
	DEAD,
	ALIVE,
	ALONE,
	STANDARD
}			t_state;

typedef enum e_fork
{
	LEFT,
	RIGHT,
	BOTH
}			t_fork;

typedef enum e_scales
{
	SECONDS,
	MILLISECONDS,
	MICROSECONDS,
}			t_scales;

typedef struct s_philo	t_philo;
typedef struct s_guest	t_guest;

// START.C
int	ft_meal(t_philo *philo);
void	ft_start(t_philo *philo);

// PARSING_UILS.C
int ft_isnum(char *str);
int	ft_atoll_safe(char *str, long long *nb);

// PARSING.C
int	ft_parsing(t_philo *philo, int argc, char **argv);

// OUTPUT.C
void	ft_action_print(t_guest *guest);

// ACTIONS.C
void	ft_status_change(t_guest *guest, t_state status);
int		ft_dead_check(t_philo *philo);
int	ft_eat(t_guest *guest);
int	ft_sleep(t_guest *guest);
int	ft_think(t_guest *guest);

// TIME.C
long long	ft_think_time(t_guest *guest);
long long	ft_get_time(t_scales scale);
void	ft_precise_sleep(t_guest *guest);

// TIME.C
void	ft_exec_finished(t_philo *philo);

// ERROR.C
void	ft_philo_clean(t_philo *philo);

// UTILS.C
void	ft_status_change(t_guest *guest, t_state status);
void	ft_meal_update(t_guest *guest);
long long	ft_meal_read(t_guest *guest);
int		ft_dead_check(t_philo *philo);

// REAPER.C
void	*ft_reaper(void *ptr);

#endif
