/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamjada <alamjada@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/25 20:01:29 by salman            #+#    #+#             */
/*   Updated: 2026/02/28 13:10:50 by alamjada         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// IMPRIME TOUTES LES DATAS STRUCT CMD
void	ft_cmd_print(t_cmd *head)
{
	t_cmd	*current;
	int		i;

	current = head;
	while (current)
	{
		i = 0;
		if (current->path)
			ft_printf("|{PATH = %s}|\n", current->path);
		else
			ft_printf("|{NO PATH YET}|");
		while (current->args[i++])
			ft_printf("[ARGV[%d] = %s]\n", i - 1, current->args[i - 1]);
		ft_printf("|[OUTFILE = %d || INFILE = %d]|\n", current->outfd,
			current->infd);
		current = (t_cmd *)current->node.next;
	}
}

// IMPRIME TOUTES LES DATAS STRUCT TOKEN
void	ft_tokens_print(t_token *head)
{
	t_token	*current;

	current = head;
	while (current)
	{
		ft_printf("CONTENT TOKEN = %s\n", current->str);
		ft_type_print(current);
		current = (t_token *)current->node.next;
	}
}

// IMPRIME LE TYPE EN FONCTION DE L'ENUM
void	ft_type_print(t_token *token)
{
	if (token->type == DEFAULT)
		ft_printf("TYPE = DEFAULT\n");
	else if (token->type == PIPE)
		ft_printf("TYPE = PIPE\n");
	else if (token->type == IN_CHEVRON)
		ft_printf("TYPE = IN_CHEVRON\n");
	else if (token->type == IN_DCHEVRON)
		ft_printf("TYPE = IN_DCHEVRON\n");
	else if (token->type == OUT_CHEVRON)
		ft_printf("TYPE = OUT_CHEVRON\n");
	else if (token->type == OUT_DCHEVRON)
		ft_printf("TYPE = OUT_DCHEVRON\n");
	else if (token->type == WORD)
		ft_printf("TYPE = WORD\n");
}
