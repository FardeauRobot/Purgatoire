/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamjada <alamjada@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/19 20:30:32 by alamjada          #+#    #+#             */
/*   Updated: 2026/02/28 13:17:12 by alamjada         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int			g_signal_glob = 0;

static void	ft_minishell_reset(t_minishell *minishell)
{
	ft_gc_free_all(&minishell->gc_line);
	ft_gc_free_all(&minishell->gc_tmp);
	minishell->head_token = NULL;
	minishell->head_cmd = NULL;
	minishell->state = NORMAL;
	minishell->cached_status = minishell->exit_status;
	minishell->exit_status = 0;
}

static int	ft_run_shell(t_minishell *minishell)
{
	ft_gc_add_node(&minishell->gc_line, minishell->line);
	if (*minishell->line)
		add_history(minishell->line);
	if (ft_tokenize(minishell) == GENERAL_ERROR)
		return (GENERAL_ERROR);
	if (checker_token(minishell) == ERR_SYNTAX)
		return (GENERAL_ERROR);
	if (ft_cmd_lst_create(minishell) == GENERAL_ERROR)
		return (GENERAL_ERROR);
	if (minishell->head_cmd)
		ft_exec(minishell);
	return (SUCCESS);
}

int	main(int argc, char **argv, char **envp)
{
	t_minishell	minishell;

	ft_ignore((void *)&argc, (void *)argv);
	ft_bzero(&minishell, sizeof(t_minishell));
	ft_env_setup(&minishell, envp);
	while (1)
	{
		setup_signal();
		ft_minishell_reset(&minishell);
		minishell.line = readline("Minishell > ");
		if (!minishell.line)
			ft_exit(&minishell, minishell.exit_status, NULL);
		if (!*minishell.line)
		{
			free(minishell.line);
			continue ;
		}
		if (ft_run_shell(&minishell) == GENERAL_ERROR)
		{
			close_all_fds(&minishell);
			continue ;
		}
		close_all_fds(&minishell);
	}
	ft_exit(&minishell, minishell.cached_status, NULL);
}
