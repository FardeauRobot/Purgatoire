/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handlers_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 11:32:38 by tibras            #+#    #+#             */
/*   Updated: 2026/02/28 11:37:48 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// FONCTION PRINCIPALE POUR GERE LES QUOTES
void	ft_quotes_handle(t_minishell *minishell, t_token *token)
{
	char	usable_str[BUFFER_SIZE];

	minishell->state = NORMAL;
	token->should_split = 0;
	ft_bzero(usable_str, BUFFER_SIZE);
	ft_quotes_utils(token, minishell, usable_str);
	token->str = ft_strdup_gc(usable_str, &minishell->gc_line);
	if (!token->str)
		ft_error(minishell, MALLOC_FAIL, ERR_MALLOC_QUOTES, NULL);
}

int	handle_pipe(t_minishell *minishell, t_token *token, int *cmd_find)
{
	if (*cmd_find == 0 || !token->node.next
		|| ((t_token *)token->node.next)->type == PIPE)
		return (ft_error(minishell, ERR_SYNTAX, ERRS_SYNT_NEAR, token->str));
	*cmd_find = 0;
	return (SUCCESS);
}

void	handle_word(t_token *token, t_minishell *minishell, int *cmd_find)
{
	if (*cmd_find == 0)
	{
		ft_cmd_find_path(minishell, token);
		token->type = CMD;
		*cmd_find = 1;
	}
	else if (ft_check_flags(token->str) == 1)
		token->type = FLAG;
}
