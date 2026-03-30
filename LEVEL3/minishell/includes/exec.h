/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamjada <alamjada@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/19 13:20:21 by alamjada          #+#    #+#             */
/*   Updated: 2026/02/28 13:10:50 by alamjada         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXEC_H
# define EXEC_H

# include "struct.h"

// ── EXEC ────────────────────────────────────────────

// CORE/EXEC.C
void	ft_exec(t_minishell *minishell);

// CORE/CHILD.C
void	child_process(t_minishell *minishell, t_cmd *cmd, t_child *child,
			int pipe_fd[2]);

// CORE/CHILD_UTILS.C
t_child	*ft_child_new(t_minishell *minishell, int size_cmd);

// CORE/PIPE_HANDLER.C
void	handler_signal_child(void);
void	handler_first_cmd(int infd, int outfd, int size_cmd, int pipe_fd);
void	handler_last_cmd(int infd, int prev_pipe, int outfd);
void	handler_mid_cmd(int infd, int prev_pipe, int outfd, int pipe_fd);

// CORE/WAIT.C
void	ft_wait_subprocess(t_minishell *minishell, int size_cmd, int *pids);

// CORE/REDIR.C
void	ft_redirection_exec(int new_fd, int *old_fd);
int		ft_redirection_handler(t_minishell *minishell, t_cmd *cmd,
			t_token *token);

// CORE/HEREDOC.C
void	ft_heredoc_handle(t_minishell *minishell, t_cmd *cmd, t_token *token);

// CORE/HEREDOC_UTILS.C
void	ft_signal_heredoc_handler(int sig);
int		ft_signal_here_ctrld(int pipefd[2], t_minishell *minishell, \
	t_token *token);
int		ft_signal_here_ctrlc(int pipefd[2], t_minishell *minishell, \
	int saved_stdin, char *line);
int		ft_here_pipe(int pipefd[2]);
void	ft_heredoc_end(int pipefd[2], t_minishell *minishell, int mod,
			t_cmd *cmd);

#endif
