/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_print.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 13:40:37 by tibras            #+#    #+#             */
/*   Updated: 2026/02/28 10:08:11 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	ft_env_print(t_env *head, int fd)
{
	while (head)
	{
		if (head->content)
		{
			ft_putstr_fd(head->name, fd);
			ft_putstr_fd("=", fd);
			ft_putstr_fd(head->content, fd);
			ft_putstr_fd("\n", fd);
		}
		head = (t_env *)head->node.next;
	}
}

void	ft_export_print(t_env **arr, int env_size, int fd)
{
	int	i;

	i = 0;
	while (i < env_size)
	{
		ft_putstr_fd("export ", fd);
		ft_putstr_fd(arr[i]->name, fd);
		ft_putstr_fd("=", fd);
		ft_putchar_fd('\"', fd);
		if (arr[i]->content)
			ft_putstr_fd(arr[i]->content, fd);
		ft_putchar_fd('\"', fd);
		ft_putstr_fd("\n", fd);
		i++;
	}
}
