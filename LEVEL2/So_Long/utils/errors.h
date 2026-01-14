/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/09 14:07:36 by tibras            #+#    #+#             */
/*   Updated: 2026/01/13 18:55:44 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERRORS_H
# define ERRORS_H

# include <stdlib.h>

# define WIN 0
# define ERRN_ARGS 1
# define ERRN_BER 2
# define ERRN_OPEN 3
# define ERRN_OPEN_MAP 4
# define ERRN_EMPTY 5
# define ERRN_MAP_CHARS 6
# define ERRN_MAP_SHAPE 7
# define ERRN_MAP_WALLS 8
# define ERRN_MAP_COLLECT 9
# define ERRN_MAP_EXIT 10
# define ERRN_MAP_PLAYER 11
# define ERRN_LOAD_ASSETS 12
# define ERRN_MALLOC 99
# define ERRN_MLX 100

# define ERRS_ARGS "Wrong number of arguments || Prototyped ./so_long path_map\n"
# define ERRS_BER "File not ending with .ber\n"
# define ERRS_OPEN "Can't open file\n"
# define ERRS_OPEN_MAP "Can't open file to fill the map\n"
# define ERRS_EMPTY "File is empty\n"
# define ERRS_MALLOC "Malloc failed\n"
# define ERRS_MALLOC_MAP "Fail malloc map[]\n"
# define ERRS_MAP_CHARS "Incorrect characters in the map\n"
# define ERRS_MAP_SHAPE "Invalid shape of the map\n"
# define ERRS_MAP_WALLS "Borders of the map are not only walls\n"
# define ERRS_MAP_COLLECT "Not enough collectibles\n"
# define ERRS_MAP_EXIT "No exit found\n"
# define ERRS_MAP_PLAYER "Invalid player count\n"

void	error_measure_map(t_game *game, int fd, char *line, int err_nb);
void	error_exit(t_game *game, char *msg, int err_nb);
void	ft_clear_game(t_game *game);
void	ft_printerror(char *msg);

#endif