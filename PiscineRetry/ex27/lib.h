/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lib.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 16:59:47 by tibras            #+#    #+#             */
/*   Updated: 2025/11/05 18:22:07 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIB_H
# define LIB_H

// LIBS
# include <unistd.h>
# include <fcntl.h>

//	DEFS UTILS
//	utils.c
void	ft_putchar_fd(char c, int fd);
void	ft_putstr_fd(char *str, int fd);
void	ft_putstrl(char *str, int len);

// DEFS EXEC
// exec.c
void	ft_display_file(char *filepath);

#endif