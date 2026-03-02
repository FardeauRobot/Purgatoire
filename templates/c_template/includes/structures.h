/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   structures.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/02 00:00:00 by tibras            #+#    #+#             */
/*   Updated: 2026/03/02 08:27:21 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STRUCTURES_H
# define STRUCTURES_H

# include "defines.h"
# include "libft.h"

/* ============== STRUCTURES =============================== */

/*
** t_data - main project context
** Centralises all runtime data so it can be passed to functions cleanly.
** Add your fields here as the project grows.
*/
typedef struct s_data
{
	int		argc;
	char	**argv;
	char	**envp;
	int		exit_code;
}	t_data;

/*
** Add project-specific structures here, e.g.:
** typedef struct s_token { ... } t_token;
** typedef struct s_cmd   { ... } t_cmd;
*/

#endif
