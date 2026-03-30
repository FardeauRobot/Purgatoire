/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamjada <alamjada@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/19 13:20:04 by alamjada          #+#    #+#             */
/*   Updated: 2026/02/28 13:10:50 by alamjada         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_H
# define UTILS_H

# include "struct.h"

// ── UTILS ───────────────────────────────────────────

// UTILS/TOKEN.C
int		ft_tokens_count(t_token *head);
t_token	*ft_token_create(t_minishell *minishell, char *buffer);
int		ft_token_add(t_minishell *minishell, t_token *to_add);
int		ft_find_token(t_types type, t_token *head);

// UTILS/CMD_LIST.C
int		ft_cmd_add(t_minishell *minishell, t_cmd *to_add);
t_cmd	*ft_cmd_new(t_minishell *minishell);

// UTILS/ENV_SETUP.C
t_env	*ft_env_new(t_minishell *minishell, char *str);
t_env	*ft_env_find(t_env *head_env, char *to_find);
int		ft_env_setup(t_minishell *minishell, char **envp);

// UTILS/ENV_PRINT.C
void	ft_env_print(t_env *head, int fd);
void	ft_export_print(t_env **arr, int env_size, int fd);

// UTILS/ERROR.C
int		ft_error(t_minishell *minishell, int error, char *str1, char *str2);
void	ft_exit(t_minishell *minishell, int error, char *str);
void	ft_exit_cmd(t_minishell *ms, int err, char *cmd, char *msg);

void	close_all_fds(t_minishell *minishell);

// CORE/SIGNAL.C
void	setup_signal(void);

// UTILS/EXPORT_UTILS.C
int		ft_env_add(t_minishell *minishell, t_env *new_env);
t_env	**ft_export_duplicate(t_minishell *minishell, int env_size);
void	ft_export_null(t_minishell *minishell, int fd);

// UTILS/CONVERT.C
char	**ft_envp_build(t_minishell *minishell);
void	ft_ignore(void *ptr1, void *ptr2);

// UTILS/HANDLERS_UTILS.C
int		handle_pipe(t_minishell *minishell, t_token *token, int *cmd_find);
void	handle_word(t_token *token, t_minishell *minishell, int *cmd_find);
void	ft_quotes_handle(t_minishell *minishell, t_token *token);

// UTILS/EXEC_UTILS.C
void	ft_fds_close(int infd, int outfd);

#endif
