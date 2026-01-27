/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_gnl.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/19 12:00:00 by tibras            #+#    #+#             */
/*   Updated: 2025/12/19 12:00:00 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_GNL_H
# define FT_GNL_H

# include <stdlib.h>

// Get Next Line configuration
# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 42
# endif

// Get Next Line main function
char	*get_next_line(int fd);

// Get Next Line utility functions
size_t	ft_gnl_strlen(const char *str);
char	*ft_gnl_strjoin(const char *s1, const char *s2);
int		ft_gnl_strchr(const char *s, int c);
char	*ft_gnl_substr(const char *s, int max);

#endif
