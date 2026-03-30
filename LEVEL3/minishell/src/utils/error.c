/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/05 16:28:43 by tibras            #+#    #+#             */
/*   Updated: 2026/02/28 11:50:00 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <asm-generic/errno-base.h>

// ERROR + MSG SUR SORTIE ERREUR
// PENSER  A PRENDRE EN COMPTRE STRERRRNO PLUS SIMPLE POUR LA GESTION
int	ft_error(t_minishell *minishell, int error, char *str1, char *str2)
{
	if (minishell)
		minishell->exit_status = error;
	if (str1 || str2)
	{
		ft_putstr_fd(ERR_SHELL_PFX, 2);
		if (str1)
			ft_putstr_fd(str1, STDERR_FILENO);
		if (str2)
			ft_putstr_fd(str2, STDERR_FILENO);
		ft_putchar_fd('\n', STDERR_FILENO);
	}
	else if (errno != 0)
	{
		perror(ERR_SHELL_PFX);
		if (errno == EACCES)
			return (126);
	}
	return (error);
}

// CLOSE TOUS LES FDS
void	close_all_fds(t_minishell *minishell)
{
	t_cmd	*cmd;

	cmd = minishell->head_cmd;
	while (cmd)
	{
		if (cmd->infd > 2)
		{
			close(cmd->infd);
			cmd->infd = STDIN_FILENO;
		}
		if (cmd->outfd > 2)
		{
			close(cmd->outfd);
			cmd->outfd = STDOUT_FILENO;
		}
		cmd = (t_cmd *)cmd->node.next;
	}
}

// FONCTION DE FIN DE PROGRAMME, EXIT AVEC VALEUR CODE ERROR
void	ft_exit(t_minishell *minishell, int error, char *str)
{
	close_all_fds(minishell);
	ft_gc_free_all(&minishell->gc_line);
	ft_gc_free_all(&minishell->gc_tmp);
	ft_gc_free_all(&minishell->gc_global);
	rl_clear_history();
	minishell->exit_status = error;
	if (str)
		ft_putendl_fd(str, STDERR_FILENO);
	exit(error);
}

void	ft_exit_cmd(t_minishell *ms, int err, char *cmd, char *msg)
{
	ft_putstr_fd(ERR_SHELL_PFX, STDERR_FILENO);
	if (cmd)
		ft_putstr_fd(cmd, STDERR_FILENO);
	if (msg)
		ft_putendl_fd(msg, STDERR_FILENO);
	ft_exit(ms, err, NULL);
}
