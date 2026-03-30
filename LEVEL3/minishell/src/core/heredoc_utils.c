/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamjada <alamjada@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/23 12:16:51 by alamjada          #+#    #+#             */
/*   Updated: 2026/02/28 13:10:50 by alamjada         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	ft_signal_heredoc_handler(int sig)
{
	g_signal_glob = sig;
	write(1, "\n", 1);
	close(STDIN_FILENO);
}

int	ft_signal_here_ctrld(int pipefd[2], t_minishell *minishell, t_token *token)
{
	ft_fds_close(pipefd[0], pipefd[1]);
	close_all_fds(minishell);
	ft_putstr_fd("minishell: warning: here-document ", 2);
	ft_putstr_fd("delimited by end-of-file (wanted `", 2);
	ft_putstr_fd(token->str, 2);
	ft_putstr_fd("')\n", 2);
	return (SUCCESS);
}

int	ft_signal_here_ctrlc(int pipefd[2], t_minishell *minishell, int saved_stdin,
	char *line)
{
	ft_fds_close(pipefd[0], pipefd[1]);
	dup2(saved_stdin, STDIN_FILENO);
	close(saved_stdin);
	close_all_fds(minishell);
	if (line)
		free(line);
	return (SUCCESS);
}

int	ft_here_pipe(int pipefd[2])
{
	if (pipe(pipefd) == -1)
	{
		ft_error(NULL, GENERAL_ERROR, ERR_HEREDOC, ERR_PIPE_FAIL);
		return (GENERAL_ERROR);
	}
	return (SUCCESS);
}

void	ft_heredoc_end(int pipefd[2], t_minishell *minishell, int mod,
		t_cmd *cmd)
{
	close(pipefd[1]);
	if (g_signal_glob == SIGINT)
	{
		close(pipefd[0]);
		minishell->exit_status = 130;
	}
	if (mod == 1)
		ft_redirection_exec(pipefd[0], &cmd->infd);
	else
		close(pipefd[0]);
	setup_signal();
}
