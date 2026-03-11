/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 00:00:00 by author            #+#    #+#             */
/*   Updated: 2026/03/11 17:34:15 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

// THIS IS THE MAIN

// FUNCTION USED TO INITIALIZE THE MAIN STRUCTURE AND START THE PROGRAM
int	main(int argc, char **argv)
{
	t_cub data;

	ft_bzero(&data, sizeof(t_cub));
	// ft_cub_init(&data);
	ft_parsing(&data, argv, argc);
	ft_cub_print(&data);
	ft_game(&data);
	return (EXIT_SUCCESS);
}
