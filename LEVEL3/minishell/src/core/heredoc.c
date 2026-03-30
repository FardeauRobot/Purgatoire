/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamjada <alamjada@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/13 15:04:28 by tibras            #+#    #+#             */
/*   Updated: 2026/02/28 13:13:53 by alamjada         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	ft_heredoc_loop(t_minishell *minishell, t_token *token, int *pipefd,
	int saved_stdin)
{
	char	*line;

	while (1)
	{
		line = readline("> ");
		if (g_signal_glob == SIGINT)
		{
			if (!ft_signal_here_ctrlc(pipefd, minishell, saved_stdin, line))
				break ;
		}
		else if (!line)
		{
			if (!ft_signal_here_ctrld(pipefd, minishell, token))
				break ;
		}
		ft_gc_add_node(&minishell->gc_line, line);
		if (!ft_strncmp(line, token->str, ft_strlen(token->str))
			&& ft_strlen(line) == ft_strlen(token->str))
			break ;
		ft_putendl_fd(line, pipefd[1]);
		line = NULL;
	}
}

static void	ft_heredoc(t_minishell *minishell, t_cmd *cmd, t_token *token,
	int mod)
{
	int		pipefd[2];
	int		saved_stdin;

	if (ft_here_pipe(pipefd) == GENERAL_ERROR)
		return ;
	saved_stdin = dup(STDIN_FILENO);
	signal(SIGINT, ft_signal_heredoc_handler);
	ft_heredoc_loop(minishell, token, pipefd, saved_stdin);
	close(saved_stdin);
	ft_heredoc_end(pipefd, minishell, mod, cmd);
}

t_token	*ft_heredoc_find_last(t_token *token)
{
	t_token	*last;

	last = NULL;
	while (token && token->type != PIPE)
	{
		if (token->type == IN_DCHEVRON)
			last = token;
		token = (t_token *)token->node.next;
	}
	return (last);
}

void	ft_heredoc_handle(t_minishell *minishell, t_cmd *cmd, t_token *token)
{
	t_token	*last;

	last = ft_heredoc_find_last(token);
	if (token == last)
		ft_heredoc(minishell, cmd, (t_token *)token->node.next, 1);
	else
		ft_heredoc(minishell, cmd, (t_token *)token->node.next, 0);
}
