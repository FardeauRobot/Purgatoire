/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/21 19:04:22 by alamjada          #+#    #+#             */
/*   Updated: 2026/02/27 19:31:34 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// AFFECTE L'ETAT A MINISHELL POUR
void	ft_state_detect(char c, t_minishell *minishell)
{
	if (c == '"')
	{
		if (minishell->state == NORMAL || minishell->state == WAITING)
			minishell->state = IN_DQUOTE;
		else if (minishell->state == IN_DQUOTE)
			minishell->state = NORMAL;
	}
	else if (c == '\'')
	{
		if (minishell->state == NORMAL || minishell->state == WAITING)
			minishell->state = IN_QUOTE;
		else if (minishell->state == IN_QUOTE)
			minishell->state = NORMAL;
	}
	else if (!ft_ischarset(c, SPACES) && minishell->state == WAITING)
		minishell->state = NORMAL;
}

// INTERPRETE LE CARACTERE EN FONCTION DE L'ETAT
static int	ft_state_interpret(char *line, int *index, char *buffer,
		t_minishell *minishell)
{
	if (ft_ischarset(line[*index], SPACES) && minishell->state == NORMAL)
		return (ft_state_inter_space_normal(minishell, buffer));
	else if (ft_ischarset(line[*index], OPERATORS)
		&& minishell->state == NORMAL)
		return (ft_state_inter_ope_normal(minishell, buffer, line, *index));
	else if (minishell->state != WAITING)
		return (ft_state_inter_not_waiting(minishell, buffer, line, *index));
	return (SUCCESS);
}

// CREE LA LISTE DES TOKENS A UTILSER POUR LE PARSING
static int	ft_token_lst_create(t_minishell *minishell)
{
	char	*buffer;
	int		line_len;
	int		i;

	line_len = ft_strlen(minishell->line);
	buffer = ft_calloc_gc(line_len + 1, sizeof(char), &minishell->gc_line);
	if (!buffer)
		return (ft_error(minishell, MALLOC_FAIL, ERR_MALLOC_BUF_INTERP, NULL));
	i = -1;
	while (++i < line_len)
	{
		ft_state_detect(minishell->line[i], minishell);
		if (ft_state_interpret(minishell->line, &i, buffer, minishell))
			return (1);
	}
	minishell->line[i] = '\0';
	if (minishell->state == IN_QUOTE || minishell->state == IN_DQUOTE)
		return (ft_error(minishell, PARSING_FAIL, ERR_QUOTES, NULL));
	if (ft_strlen(buffer) > 0 && ft_token_add(minishell,
			ft_token_create(minishell, buffer)))
		return (ft_error(minishell, MALLOC_FAIL, ERR_MALLOC_INTERP, NULL));
	return (0);
}

static void	ft_type_affect(t_minishell *minishell)
{
	t_token	*current;

	current = minishell->head_token;
	while (current)
	{
		if (current->str[0] == '|')
			current->type = PIPE;
		else if (current->str[0] == '<')
		{
			current->type = IN_CHEVRON;
			if (current->str[1])
				current->type = IN_DCHEVRON;
		}
		else if (current->str[0] == '>')
		{
			current->type = OUT_CHEVRON;
			if (current->str[1])
				current->type = OUT_DCHEVRON;
		}
		else
			current->type = WORD;
		current = (t_token *)current->node.next;
	}
}

// On récupere la ligne, on traite pour avoir des types de mots
// On les récupere ensuite pour créer des phrases
int	ft_tokenize(t_minishell *minishell)
{
	if (ft_token_lst_create(minishell))
		return (1);
	ft_type_affect(minishell);
	return (0);
}
