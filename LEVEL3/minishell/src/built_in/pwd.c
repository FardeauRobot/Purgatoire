/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/13 19:02:48 by alamjada          #+#    #+#             */
/*   Updated: 2026/02/20 18:03:30 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ft_pwd(t_minishell *minishell)
{
	char	*buff;

	buff = getcwd(NULL, 0);
	if (buff)
	{
		ft_printf("%s\n", buff);
		free(buff);
		return (SUCCESS);
	}
	return (ft_error(minishell, errno, strerror(errno), NULL));
}
