/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   built_ins.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alamjada <alamjada@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/18 09:56:55 by tibras            #+#    #+#             */
/*   Updated: 2026/02/28 13:10:50 by alamjada         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BUILT_INS_H
# define BUILT_INS_H

# include "struct.h"

// ── BUILT-INS ───────────────────────────────────────

// BUILT_IN/BUILTIN.C
int		is_built_in(t_cmd *cmd);
void	run_built_in(t_cmd *cmd, t_minishell *minishell);

// BUILT_IN/ECHO.C
void	ft_echo(char **args);

// BUILT_IN/CD.C
int		ft_cd(t_minishell *minishell, char **args);

// BUILT_IN/CD_UTILS.C
char	*ft_no_path(t_env *head_env);
void	update_pwd(t_env **head_env, t_minishell *minishell);
void	update_old_pwd(t_env **head_env, char *old_pwd, t_minishell *minishell);
char	*save_pwd(void);
char	*ft_expand_home(t_minishell *minishell, char *path);

// BUILT_IN/PWD.C
int		ft_pwd(t_minishell *minishell);

// BUILT_IN/EXPORT.C
int		ft_env_format_check(char *str);
int		ft_export(t_minishell *minishell, int fd, char **args);

// BUILT_IN/UNSET.C
void	ft_unset(t_minishell *minishell, char **args);
// void	ft_unset(t_env **head_env, char **args);

// BUILT_IN/ENV.C
void	ft_env(t_minishell *minishell);

// BUILT_IN/EXIT.C
void	ft_buildin_exit(t_minishell *minishell, char **args);

#endif
