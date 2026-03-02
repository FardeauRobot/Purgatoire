/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_gnl.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/19 12:00:00 by tibras            #+#    #+#             */
/*   Updated: 2026/02/24 09:06:40 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_GNL_H
# define FT_GNL_H

# include <stdlib.h>

// Get Next Line configuration
# ifndef BUFFER_SIZE_GNL
#  define BUFFER_SIZE_GNL 42
# endif

// Get Next Line main function
char	*get_next_line(int fd);

// Get Next Line utility functions
size_t	ft_gnl_strlen(const char *str);
char	*ft_gnl_strjoin(const char *s1, const char *s2);
int		ft_gnl_strchr(const char *s, int c);
char	*ft_gnl_substr(const char *s, int max);
void	ft_gnl_free_all(char **arr);

#endif
