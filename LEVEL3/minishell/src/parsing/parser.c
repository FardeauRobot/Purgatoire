/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 18:26:24 by alamjada          #+#    #+#             */
/*   Updated: 2026/02/28 11:43:41 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static t_token	*ft_split_new_token(t_minishell *minishell, char *str)
{
	t_token	*new_token;

	new_token = ft_token_create(minishell, str);
	if (!new_token)
	{
		ft_error(minishell, MALLOC_FAIL, ERR_MALLOC_INTERP, NULL);
		return (NULL);
	}
	new_token->type = WORD;
	return (new_token);
}

static int	ft_split_append_parts(t_minishell *minishell,
	t_token *token, char **parts)
{
	t_token	*prev;
	t_token	*next;
	t_token	*new_token;
	int		i;

	prev = token;
	next = (t_token *)token->node.next;
	i = 1;
	while (parts[i])
	{
		new_token = ft_split_new_token(minishell, parts[i++]);
		if (!new_token)
			return (GENERAL_ERROR);
		prev->node.next = (t_list *)new_token;
		new_token->node.previous = (t_list *)prev;
		prev = new_token;
	}
	prev->node.next = (t_list *)next;
	if (next)
		next->node.previous = (t_list *)prev;
	return (SUCCESS);
}

static int	ft_split_token_words(t_minishell *minishell, t_token *token)
{
	char	**parts;

	if (token->type != WORD || !token->should_split)
		return (SUCCESS);
	parts = ft_split_charset_gc(token->str, SPACES, &minishell->gc_line);
	if (!parts)
		return (ft_error(minishell, MALLOC_FAIL, ERR_MALLOC_INTERP, NULL));
	token->str = parts[0];
	token->should_split = 0;
	if (!token->str)
		return (SUCCESS);
	return (ft_split_append_parts(minishell, token, parts));
}

static int	handle_redirection(t_minishell *minishell, t_token *token)
{
	if (ft_check_redirection(token->str) == 0)
		return (ft_error(minishell, ERR_SYNTAX, ERRS_SYNT_NEAR, token->str));
	if (!token->node.next)
		return (ft_error(minishell, ERR_SYNTAX, ERRS_SYNT_NEAR,
				ERR_TOK_NEWLINE));
	if (is_redirection((t_token *)token->node.next)
		|| ((t_token *)token->node.next)->type == PIPE)
		return (ft_error(minishell, ERR_SYNTAX, ERRS_SYNT_NEAR,
				((t_token *)token->node.next)->str));
	return (SUCCESS);
}

int	checker_token(t_minishell *minishell)
{
	t_token	*token;
	int		cmd_find;

	cmd_find = 0;
	token = minishell->head_token;
	while (token)
	{
		ft_quotes_handle(minishell, token);
		if (ft_split_token_words(minishell, token))
			return (GENERAL_ERROR);
		if (!token->str || (!token->str[0] && !cmd_find))
			token->type = GARBAGE;
		if (is_redirection(token))
		{
			if (handle_redirection(minishell, token))
				return (ERR_SYNTAX);
		}
		else if (token->type == WORD)
			handle_word(token, minishell, &cmd_find);
		else if (token->type == PIPE)
			if (handle_pipe(minishell, token, &cmd_find))
				return (ERR_SYNTAX);
		token = (t_token *)token->node.next;
	}
	return (SUCCESS);
}
