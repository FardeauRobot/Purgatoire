/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmds.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/09 17:34:00 by tibras            #+#    #+#             */
/*   Updated: 2026/02/28 11:54:05 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "errors.h"
#include "minishell.h"

static int	ft_token_word_count(t_token *current)
{
	int	count;

	count = 1;
	while (current)
	{
		if (current->type == PIPE)
			return (count);
		else if (current->type != GARBAGE)
			count++;
		current = (t_token *)current->node.next;
	}
	return (count);
}

static t_cmd	*ft_cmd_init(t_minishell *minishell, t_token *token)
{
	t_cmd	*cmd;
	int		count;

	count = ft_token_word_count(token);
	cmd = ft_cmd_new(minishell);
	if (!cmd)
	{
		ft_error(minishell, MALLOC_FAIL, "Error malloc", NULL);
		return (NULL);
	}
	cmd->args = ft_calloc_gc(count + 1, sizeof(char *), &minishell->gc_line);
	if (!cmd->args)
	{
		ft_error(minishell, MALLOC_FAIL, "Error malloc", NULL);
		return (NULL);
	}
	return (cmd);
}

static int	ft_cmd_fill(t_minishell *minishell, t_cmd *cmd, t_token **token)
{
	int	i;

	i = 1;
	while (*token && (*token)->type != PIPE)
	{
		if (ft_token_affect(minishell, cmd, token, &i))
		{
			ft_fds_close(cmd->infd, cmd->outfd);
			return (GENERAL_ERROR);
		}
		if (*token)
			*token = (t_token *)(*token)->node.next;
	}
	cmd->args[i] = NULL;
	if (!cmd->path)
		cmd->path = cmd->args[0];
	if (cmd->args[0] && cmd->args[0][0])
		ft_cmd_add(minishell, cmd);
	else
		ft_fds_close(cmd->infd, cmd->outfd);
	if (*token)
		*token = (t_token *)(*token)->node.next;
	return (SUCCESS);
}

int	ft_cmd_lst_create(t_minishell *minishell)
{
	t_token	*tok_current;
	t_cmd	*cmd_new;

	tok_current = minishell->head_token;
	while (tok_current)
	{
		cmd_new = ft_cmd_init(minishell, tok_current);
		if (!cmd_new)
			return (GENERAL_ERROR);
		if (ft_cmd_fill(minishell, cmd_new, &tok_current))
			return (GENERAL_ERROR);
	}
	ft_cmd_add(minishell, NULL);
	return (SUCCESS);
}
