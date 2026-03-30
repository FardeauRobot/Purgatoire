/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cntrl.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 15:49:08 by tibras            #+#    #+#             */
/*   Updated: 2026/02/26 10:11:21 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	ft_redirection_exec(int new_fd, int *old_fd)
{
	if (*old_fd > 2)
		close(*old_fd);
	*old_fd = new_fd;
}

static int	ft_open(char *path, t_types mod)
{
	int	fd;

	fd = -1;
	if (mod == IN_CHEVRON)
		fd = open(path, O_RDONLY);
	else if (mod == OUT_CHEVRON)
		fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	else if (mod == OUT_DCHEVRON)
		fd = open(path, O_WRONLY | O_CREAT | O_APPEND, 0644);
	return (fd);
}

static void	ft_open_error(t_token *token, t_cmd *cmd, t_minishell *minishell)
{
	cmd->error_file = 1;
	ft_putstr_fd(ERR_SHELL_PFX, STDERR_FILENO);
	perror(((t_token *)token->node.next)->str);
	if (!ft_find_token(PIPE, token))
		minishell->exit_status = GENERAL_ERROR;
}

int	ft_redirection_handler(t_minishell *minishell, t_cmd *cmd, t_token *token)
{
	int	fd;

	if (token->node.next == NULL)
		return (GENERAL_ERROR);
	if (cmd->error_file)
		return (SUCCESS);
	if (token->type == IN_DCHEVRON)
		return (ft_heredoc_handle(minishell, cmd, token), 0);
	fd = ft_open(((t_token *)token->node.next)->str, token->type);
	if (fd == -1)
		ft_open_error(token, cmd, minishell);
	if (fd > 2 && token->type == IN_CHEVRON)
		ft_redirection_exec(fd, &cmd->infd);
	else if (fd > 2 && (token->type == OUT_CHEVRON
			|| token->type == OUT_DCHEVRON))
		ft_redirection_exec(fd, &cmd->outfd);
	return (SUCCESS);
}
