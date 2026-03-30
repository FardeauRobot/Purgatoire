/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fardeau <fardeau@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/05 16:31:44 by tibras            #+#    #+#             */
/*   Updated: 2026/02/27 22:08:58 by fardeau          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERRORS_H
# define ERRORS_H

typedef enum e_errors
{
	SUCCESS = 0,
	GENERAL_ERROR = 1,
	ERR_SYNTAX = 2,
	NO_SUCH_FILE_O_DIR = 3,
	ERR_CHECK_TOKEN = 4,
	ERR_HANDLE_PIPE = 5,
	ERR_REDIRECTION = 6,
	ERR_OPEN = 7,
	PARSING_FAIL = 50,
	BUFFER_FAIL = 98,
	MALLOC_FAIL = 99,
	PERMISSION_DENIED = 126,
	IS_DIRECTORY = 126,
	CMD_NOT_FOUND = 127,
	SIGNAL = 128,
}	t_errors;

/* ---- Shell prefix ---- */
# define ERR_SHELL_PFX "bash : "
# define OVERFLOW_ERROR 2

/* ---- Syntax errors ---- */
# define ERRS_SYNT_NEAR "Syntax error near unexpected token "
# define ERR_QUOTES "Syntax error: unclosed quotes"
# define ERR_SYNT_NEWLINE "Syntax error near unexpected token 'newline'"
# define ERR_TOK_NEWLINE "`newline'"

/* ---- Heredoc errors ---- */
# define ERR_HEREDOC_WARN "minishell: warning: here-document"
# define ERR_HEREDOC_EOF "delimited by end-of-file (wanted `"
# define ERR_HEREDOC_CLOSE "')\n"
# define ERR_HEREDOC "Heredoc error :"
# define ERR_PIPE_FAIL "Pipe creation failed"

/* ---- Buffer / memory errors ---- */
# define ERR_BUF_SIZE "Insufficient buffer size"
# define ERR_BUF_SMALL "Buffer too small"
# define ERR_MALLOC_EXPAND "Error malloc expands"
# define ERR_MALLOC_QUOTES "Error malloc end of quotes handling"
# define ERR_MALLOC_INTERP "Fail Malloc Interpreter"
# define ERR_MALLOC_BUF_INTERP "Fail Malloc Buffer Interpreter"
# define ERR_MALLOC_ENV "Error malloc setup env"
# define ERR_SETUP_ENV "Error setting up env"
# define ERR_MALLOC_EXPORT "Error malloc with export"
# define ERR_ADD_EXPORT "Error adding to export"

/* ---- Exec errors ---- */
# define ERR_PERM_DENIED ": Permission denied"
# define ERR_CMD_NOT_FOUND ": command not found"
# define ERR_NO_FILE_OR_DIR ": No such file or directory"
# define ERR_IS_DIR ": Is a directory"

/* ---- Signal errors ---- */
# define ERR_SIG_INT "signal error SIGINT"
# define ERR_SIG_IGN_QUIT "signal error ignore SIGQUIT"
# define ERR_SIG_DFL_QUIT "signal error default SIGQUIT"
# define ERR_SIG_DFL_INT "signal error default SIGINT"

/* ---- Built-in: cd / pwd ---- */
# define ERR_CD "cd"
# define ERR_CD_PFX "cd: "
# define ERR_NOT_A_DIR ": Not a directory"
# define ERR_TOO_MANY_ARGS "too many arguments"
# define ERR_HOME_NOT_SET "HOME not set"
# define ERR_PWD_NOT_FOUND "PWD not found"
# define ERR_PWD "pwd"

/* ---- Built-in: echo ---- */
# define ERR_WRITE "write"

/* ---- Built-in: exit ---- */
# define ERR_EXIT_PFX "exit: "
# define ERR_NUMERIC_ARG ": numeric argument required"

/* ---- Built-in: export ---- */
# define ERR_EXPORT "export:"
# define ERR_EXPORT_PFX "export: "
# define ERR_EMPTY_ID "`': not a valid identifier"
# define ERR_INVALID_ID ": not a valid identifier"

# define ERR_UNSET_PFX "unset: "
# define ERR_INVALID_OPT ": invalid option"

#endif
