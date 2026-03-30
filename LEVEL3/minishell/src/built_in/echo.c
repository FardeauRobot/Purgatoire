/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/13 19:33:53 by alamjada          #+#    #+#             */
/*   Updated: 2026/02/27 20:45:48 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	ft_is_flag(char *str)
{
	int	j;

	j = 0;
	if (str[j] && str[j] == '-')
		j++;
	else
		return (0);
	while (str[j] && str[j] == 'n')
		j++;
	if (!str[j])
		return (1);
	return (0);
}

static void	ft_write_safe(char *str, int len)
{
	if (write(STDOUT_FILENO, str, len) == -1)
		ft_error(NULL, errno, ERR_WRITE, NULL);
}

static int	have_flag_first(char *str)
{
	if (str[0] == '-' && str[1] == 'n')
		return (1);
	return (0);
}

void	ft_echo(char **args)
{
	int	i;

	if (!args || !*args || !args[1])
		return (ft_write_safe("\n", 1));
	if (args[1][0] == '-' && !args[1][1])
		return (ft_write_safe("-", 1), ft_write_safe("\n", 1));
	i = 1;
	if (have_flag_first(args[1]))
	{
		while (args[i])
		{
			if (!ft_is_flag(args[i]))
				break ;
			i++;
		}
	}
	while (args[i])
	{
		ft_write_safe(args[i], ft_strlen(args[i]));
		if (args[i] && args[i + 1])
			ft_write_safe(" ", 1);
		i++;
	}
	if (!have_flag_first(args[1]))
		ft_write_safe("\n", 1);
}
