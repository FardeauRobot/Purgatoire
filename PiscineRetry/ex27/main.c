/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/04 15:32:45 by tibras            #+#    #+#             */
/*   Updated: 2025/11/06 13:23:01 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lib.h"

int	main(int argc, char **argv)
{
	if (argc < 2)
	{
		ft_putstr_fd("File name missing.\n", 2);
		return (1);
	}
	else if (argc > 2)
	{
		ft_putstr_fd("Too many arguments.\n", 2);
		return (2);
	}
	else
		ft_display_file(argv[1]);
	return (0);
}
