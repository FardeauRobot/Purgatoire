/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 00:00:00 by author            #+#    #+#             */
/*   Updated: 2026/03/08 19:12:40 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

/*
** MAIN - ENTRY POINT, INITS DATA, PARSES, PRINTS AND CLEANS UP
*/
int	main(int argc, char **argv)
{
	t_cub data;

	ft_bzero(&data, sizeof(t_cub));
	ft_parsing(&data, argv, argc);
	ft_cub_print(&data);
	ft_data_clean(&data);
	return (EXIT_SUCCESS);
}
