/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_conversion.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/19 12:00:00 by tibras            #+#    #+#             */
/*   Updated: 2025/12/19 12:00:00 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_CONVERSION_H
# define FT_CONVERSION_H

// String to number conversions
int		ft_atoi(const char *nptr);
long	ft_atol(const char *nptr);
int		ft_overint(long n);

// Number to string conversions
char	*ft_itoa(int n);

// Integer comparison and min/max functions
int		ft_min_int(int a, int b);
int		ft_get_max(int *arr, int arr_len);

#endif
