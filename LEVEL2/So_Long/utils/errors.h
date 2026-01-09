/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/09 14:07:36 by tibras            #+#    #+#             */
/*   Updated: 2026/01/09 19:29:31 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERRORS_H
# define ERRORS_H

# define ERR_BER 1
# define ERR_OPEN 2
# define ERR_LEN 3

# define EXIT_SUCCESS 0
# define EXIT_FAILURE 1

# define FAILURE 0
# define SUCCESS 1

void error_measure_map(t_game *game, int fd, char *line, char *msg);
void error_exit(t_game *game, char *msg);

#endif