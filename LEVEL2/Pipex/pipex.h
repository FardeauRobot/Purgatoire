/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tibras <tibras@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 16:02:09 by tibras            #+#    #+#             */
/*   Updated: 2026/01/20 13:48:49 by tibras           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_H
# define PIPEX_H

# include <unistd.h> 
# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <fcntl.h>
# include <errno.h>
# include <signal.h>

# include "srcs/pipex_srcs.h"

// MACROS
/*
	<unistd.h>
	F_OK    // Check if file exists
	R_OK    // Check if readable
	W_OK    // Check if writable
	X_OK    // Check if executable

	<fcntl.h>
	O_RDONLY    // Read only
	O_WRONLY    // Write only
	O_RDWR      // Read and write

	O_CREAT     // Create file if it doesn't exist
	O_TRUNC     // Truncate (empty) file if it exists
	O_APPEND    // Append to end of file
}
*/
#endif