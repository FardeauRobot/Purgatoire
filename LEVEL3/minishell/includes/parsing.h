/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamjada <alamjada@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/21 17:22:02 by alamjada          #+#    #+#             */
/*   Updated: 2026/02/28 13:10:50 by alamjada         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSING_H
# define PARSING_H

# include "struct.h"

// ── PARSING ─────────────────────────────────────────

// PARSING/LEXER.C
void	ft_state_detect(char c, t_minishell *minishell);
int		ft_tokenize(t_minishell *minishell);

// PARSING/LEXER_UTILS.C
int		ft_buffer_add(char *buffer, char c);
int		ft_state_inter_space_normal(t_minishell *minishell, char *buffer);
int		ft_state_inter_ope_normal(t_minishell *minishell, char *buffer,
			char *line, int index);
int		ft_state_inter_not_waiting(t_minishell *minishell, char *buffer,
			char *line, int index);

// PARSING/PARSER.C
int		checker_token(t_minishell *minishell);

// PARSING/SYNTAX_CHECK.C
int		ft_check_flags(char *str);
int		ft_check_redirection(char *str);

// PARSING/PATH_RESOLVE.C
int		ft_cmd_find_path(t_minishell *minishell, t_token *token);
int		is_redirection(t_token *token);

// PARSING/EXPANDER.C
void	ft_quotes_utils(t_token *token, t_minishell *minishell, \
	char *usable_str);

// PARSING/TOKEN_DISPATCH.C
int		ft_token_affect(t_minishell *minishell, t_cmd *cmd, t_token **token,
			int *i);

// PARSING/CMDS.C
int		ft_cmd_lst_create(t_minishell *minishell);

#endif
