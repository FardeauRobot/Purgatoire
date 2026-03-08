/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   defines.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/02 00:00:00 by tibras            #+#    #+#             */
/*   Updated: 2026/03/08 19:10:01 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CUB3D_DEFINES_H
# define CUB3D_DEFINES_H

# include "structures.h"

/* ============== RETURN CODES ============================= */
# define SUCCESS 0
# define FAILURE 1

/* ============== LIMITS =================================== */
/*
** Add project-specific limits here, e.g.:
** # define MAX_PATH 4096
** # define MAX_ARGS 256
*/

// PARSING/PARSING.C
int	ft_parsing(t_cub *data, char **argv, int argc);

// UTILS/OUTPUT.C
void	ft_matrix_print(char **matrix);
void	ft_cub_print(t_cub *data);

// UTILS/PARSING_UTILS.C
int	ft_format_check(char *filepath);

// UTILS/ERROR.C
void	ft_data_clean(t_cub *data);

#endif
