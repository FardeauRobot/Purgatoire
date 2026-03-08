/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/02 19:00:52 by fardeau           #+#    #+#             */
/*   Updated: 2026/03/07 22:35:50 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_H
# define UTILS_H

// UTILS.C
void	ft_status_change(t_guest *guest, t_state status);
void	ft_meal_update(t_guest *guest);
int		ft_end_check(t_philo *philo);

// OUTPUT.C
void	ft_bzero(void *ptr, int size);
void	ft_putstr_fd(char *str, int fd);
void	ft_putendl_fd(char *str, int fd);
void	ft_print_main_philo(t_philo *philo);
void	ft_diverse_print(t_guest *guest, char *str);
void	ft_action_print(t_guest *guest);

// PARSING_UTILS.C
int ft_isnum(char *str);
int	ft_atoll_safe(char *str, long long *nb);

// TIME.C
void ft_update_last_meal(t_guest *guest);
long long	ft_get_time(t_scales scale);
void	ft_precise_sleep(t_guest *guest);

#endif