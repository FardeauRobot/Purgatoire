/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/09 14:10:09 by tibras            #+#    #+#             */
/*   Updated: 2026/01/19 16:12:36 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "so_long_utils.h"

void	ft_printerror(char *msg)
{
	if (!msg)
		return;
	write(2, msg, ft_strlen(msg));
}

void	error_exit(t_game *game, char *msg, int err_nb)
{
	if (err_nb != 0)
		ft_printerror("Error\n");
	ft_printerror(msg);
	get_next_line(-1);
	if (game)
		ft_clear_game(game);
	exit(err_nb);
}