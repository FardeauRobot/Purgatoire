/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/22 15:59:42 by tibras            #+#    #+#             */
/*   Updated: 2025/11/25 12:10:37 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 42
# endif

// LIBS
// A DELETE
# include <stdio.h>
# include <fcntl.h>

// A GARDER
# include <stdlib.h>
# include <unistd.h>

// PROTOTYPES FONCTIONS
// UTILS
void	ft_bzero(void *s, size_t n);
size_t	ft_strlen(char *str);
int		ft_strchr(const char *s, int c);
char	*ft_strjoin(char *s1, char *s2);
char	*ft_mini_substr(char *s, int max);

// MAIN
char	*get_next_line(int fd);

#endif