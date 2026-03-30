/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_dispatch.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamjada <alamjada@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/24 15:38:18 by tibras            #+#    #+#             */
/*   Updated: 2026/02/28 13:16:21 by alamjada         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	ft_dchevron_treat(t_minishell *minishell, t_token *token,
	t_cmd *cmd, t_token **token_ptr)
{
	if (!token->node.next || !((t_token *)token->node.next)->str)
		return (ft_error(minishell, ERR_SYNTAX, ERR_SYNT_NEWLINE, NULL));
	else
	{
		if (token->node.next)
			((t_token *)token->node.next)->type = WORD;
		ft_heredoc_handle(minishell, cmd, token);
		*token_ptr = (t_token *)token->node.next;
	}
	return (SUCCESS);
}

static int	ft_simple_redirection(t_minishell *minishell, t_token **token_ptr,
		t_cmd *cmd)
{
	t_token	*next;
	t_token	*token;

	token = *token_ptr;
	next = (t_token *)token->node.next;
	if (!next || !next->str || !next->str[0])
		return (ft_error(minishell, ERR_SYNTAX, ERR_SYNT_NEWLINE, NULL));
	ft_quotes_handle(minishell, next);
	ft_redirection_handler(minishell, cmd, token);
	if (minishell->exit_status != ERR_OPEN && minishell->exit_status != 0)
		return (GENERAL_ERROR);
	if (token->node.next)
		*token_ptr = (t_token *)token->node.next;
	return (SUCCESS);
}

int	ft_token_affect(t_minishell *minishell, t_cmd *cmd, t_token **token_ptr,
		int *i)
{
	t_token	*token;

	token = *token_ptr;
	if (token->type == WORD || token->type == FLAG)
		cmd->args[(*i)++] = token->str;
	else if (token->type == CMD)
	{
		cmd->path = token->path;
		cmd->args[0] = token->str;
	}
	else if (token->type == OUT_CHEVRON || token->type == OUT_DCHEVRON
		|| token->type == IN_CHEVRON)
	{
		return (ft_simple_redirection(minishell, token_ptr, cmd));
	}
	else if (token->type == IN_DCHEVRON)
		if (ft_dchevron_treat(minishell, token, cmd, token_ptr) != SUCCESS)
			return (ERR_SYNTAX);
	return (SUCCESS);
}
