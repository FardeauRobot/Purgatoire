/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/13 19:44:40 by alamjada          #+#    #+#             */
/*   Updated: 2026/02/28 10:01:34 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static long long	ft_compute(char *nptr, long long sign, long long *res)
{
	unsigned long long	nbr;
	int					digit;

	nbr = 0;
	while (ft_isdigit((unsigned char)*nptr))
	{
		digit = *nptr - '0';
		if (nbr > LLONG_MAX / 10)
			return (OVERFLOW_ERROR);
		nbr = nbr * 10;
		if (sign == 1 && nbr > (unsigned long long)LLONG_MAX - digit)
			return (OVERFLOW_ERROR);
		if (sign == -1 && nbr > (unsigned long long)-(LLONG_MIN + 1) + 1
			- digit)
			return (OVERFLOW_ERROR);
		nbr += digit;
		nptr++;
	}
	*res = nbr * sign;
	(void)res;
	return (nbr);
}

static int	ft_atoi_safe(const char *nptr, long long *res)
{
	long long	sign;

	if (!nptr)
		return (GENERAL_ERROR);
	while (ft_isspace((unsigned char)*nptr))
		nptr++;
	sign = 1;
	if (*nptr == '-' || *nptr == '+')
	{
		if (*nptr == '-')
			sign = -1;
		nptr++;
	}
	if (!ft_isdigit(*nptr))
		return (GENERAL_ERROR);
	if (ft_compute((char *)nptr, sign, res) == OVERFLOW_ERROR)
		return (GENERAL_ERROR);
	return (SUCCESS);
}

void	ft_buildin_exit(t_minishell *minishell, char **args)
{
	long long	code;
	int			i;

	i = 0;
	while (args[i])
		i++;
	close_all_fds(minishell);
	ft_putendl_fd("exit", STDIN_FILENO);
	if (ft_atoi_safe(args[1], &code) == GENERAL_ERROR)
	{
		ft_error(minishell, ERR_SYNTAX, args[1], ERR_NUMERIC_ARG);
		ft_exit(minishell, ERR_SYNTAX, NULL);
	}
	else if (i > 2)
	{
		ft_error(minishell, GENERAL_ERROR, ERR_EXIT_PFX, ERR_TOO_MANY_ARGS);
		return ;
	}
	else if (i == 1)
		code = minishell->exit_status;
	ft_exit(minishell, code % 256, NULL);
}
