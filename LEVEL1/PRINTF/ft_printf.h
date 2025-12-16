/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 18:28:33 by tibras            #+#    #+#             */
/*   Updated: 2025/11/27 11:25:22 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PRINTF_H
# define FT_PRINTF_H

# include <unistd.h>
# include <stdlib.h>
# include <stdarg.h>

# define BASE_10 "0123456789"
# define BASE_UPPER "0123456789ABCDEF"
# define BASE_LOWER "0123456789abcdef"

//  PRINTF.C
int	ft_cputchar(int c);
int	ft_cputstr(char *str);
int	ft_strlen(char *str);
int	ft_check_print(va_list args, char c);
int	ft_printf(const char *s, ...);

// PRINTF_NBR.C
int	ft_print_nbr(long long nb, int l_base, int mod);

#endif