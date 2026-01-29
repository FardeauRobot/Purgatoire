/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_output.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/19 12:00:00 by tibras            #+#    #+#             */
/*   Updated: 2026/01/26 13:08:22 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_OUTPUT_H
# define FT_OUTPUT_H

#include "libft.h"

// Base definitions for printf
# define BASE_10 "0123456789"
# define BASE_UPPER "0123456789ABCDEF"
# define BASE_LOWER "0123456789abcdef"

// File descriptor output functions
void	ft_putchar_fd(char c, int fd);
void	ft_putstr_fd(char *s, int fd);
void	ft_putendl_fd(char *s, int fd);
void	ft_putnbr_fd(int n, int fd);
void	ft_putnbr_base(int nbr, char *base);

// Array printing functions
void	ft_print_int_arr(int *nb, int size);

// Printf function
int		ft_printf(const char *s, ...);

#endif
